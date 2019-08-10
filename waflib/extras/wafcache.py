#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2019 (ita)

"""
Filesystem-based cache system to share and re-use build artifacts

The following environment variables may be set:
* WAFCACHE: absolute path of the waf cache (/tmp/wafcache_user,
            where `user` represents the currently logged-in user)
* WAFCACHE_TRIM_MAX_FOLDER: maximum amount of tasks to cache (1M)
* WAFCACHE_EVICT_MAX_BYTES: maximum amount of cache size in bytes (10GB)
* WAFCACHE_EVICT_INTERVAL_MINUTES: minimum time interval to try
                                   and trim the cache (3 minutess)

Cache access operations (copy to and from) are delegated to pre-forked
subprocesses. Though these processes perform atomic copies, they
are unaware of other processes running on the system

The files are copied using hard links by default; if the cache is located
onto another partition, the system switches to file copies instead.

Usage::

	def build(bld):
		bld.load('wafcache')
		...
"""

import atexit, base64, errno, fcntl, getpass, os, shutil, sys, threading, time
try:
	import subprocess32 as subprocess
except ImportError:
	import subprocess

CACHE_DIR = os.environ.get('WAFCACHE', '/tmp/wafcache_' + getpass.getuser())
TRIM_MAX_FOLDERS = int(os.environ.get('WAFCACHE_TRIM_MAX_FOLDER', 1000000))
EVICT_INTERVAL_MINUTES = int(os.environ.get('WAFCACHE_EVICT_INTERVAL_MINUTES', 3))
EVICT_MAX_BYTES = int(os.environ.get('WAFCACHE_EVICT_MAX_BYTES', 10**10))
OK = "ok"

try:
	import cPickle
except ImportError:
	import pickle as cPickle

if __name__ != '__main__':
	from waflib import Task, Logs, Utils, Build

def can_retrieve_cache(self):
	"""
	New method for waf Task classes
	"""
	if not self.outputs:
		return False

	self.cached = False

	sig = self.signature()
	ssig = Utils.to_hex(self.uid() + sig)

	files_to = [node.abspath() for node in self.outputs]
	err = cache_command(ssig, [], files_to)
	if not err.startswith(OK):
		if Logs.verbose:
			Logs.debug('wafcache: error getting from cache %s', err)
		return False

	self.cached = True
	return True

def put_files_cache(self):
	"""
	New method for waf Task classes
	"""
	if not self.outputs:
		return

	if getattr(self, 'cached', None):
		return

	bld = self.generator.bld
	sig = self.signature()
	ssig = Utils.to_hex(self.uid() + sig)

	files_from = [node.abspath() for node in self.outputs]
	err = cache_command(ssig, files_from, [])

	if not err.startswith(OK):
		if Logs.verbose:
			Logs.debug('wafcache: error caching %s', err)

	bld.task_sigs[self.uid()] = self.cache_sig

def hash_env_vars(self, env, vars_lst):
	"""
	Reimplement BuildContext.hash_env_vars so that the resulting hash does not depend on local paths
	"""
	if not env.table:
		env = env.parent
		if not env:
			return Utils.SIG_NIL

	idx = str(id(env)) + str(vars_lst)
	try:
		cache = self.cache_env
	except AttributeError:
		cache = self.cache_env = {}
	else:
		try:
			return self.cache_env[idx]
		except KeyError:
			pass

	v = str([env[a] for a in vars_lst])
	v = v.replace(self.srcnode.abspath().__repr__()[:-1], '')
	m = Utils.md5()
	m.update(v.encode())
	ret = m.digest()

	Logs.debug('envhash: %r %r', ret, v)

	cache[idx] = ret

	return ret

def uid(self):
	"""
	Reimplement Task.uid() so that the signature does not depend on local paths
	"""
	try:
		return self.uid_
	except AttributeError:
		m = Utils.md5()
		src = self.generator.bld.srcnode
		up = m.update
		up(self.__class__.__name__.encode())
		for x in self.inputs + self.outputs:
			up(x.path_from(src).encode())
		self.uid_ = m.digest()
		return self.uid_


def make_cached(cls):
	"""
	Enable the waf cache for a given task class
	"""
	if getattr(cls, 'nocache', None) or getattr(cls, 'has_cache', False):
		return

	m1 = getattr(cls, 'run', None)
	def run(self):
		if getattr(self, 'nocache', False):
			return m1(self)
		if self.can_retrieve_cache():
			return 0
		return m1(self)
	cls.run = run

	m2 = getattr(cls, 'post_run', None)
	def post_run(self):
		if getattr(self, 'nocache', False):
			return m2(self)
		ret = m2(self)
		self.put_files_cache()
		if hasattr(self, 'chmod'):
			for node in self.outputs:
				os.chmod(node.abspath(), self.chmod)
		return ret
	cls.post_run = post_run
	cls.has_cache = True

process_pool = []
def get_process():
	"""
	Returns a worker process that can process waf cache commands
	The worker process is assumed to be returned to the process pool when unused
	"""
	try:
		return process_pool.pop()
	except IndexError:
		filepath = os.path.dirname(os.path.abspath(__file__)) + os.sep + 'wafcache.py'
		cmd = [sys.executable, '-c', Utils.readf(filepath)]
		return subprocess.Popen(cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE, bufsize=0)

def atexit_pool():
	for k in process_pool:
		try:
			os.kill(k.pid, 9)
		except OSError:
			pass
		else:
			k.wait()
atexit.register(atexit_pool)

def build(bld):
	"""
	Called during the build process to enable file caching
	"""
	if process_pool:
		# already called once
		return

	for x in range(bld.jobs):
		process_pool.append(get_process())

	Task.Task.can_retrieve_cache = can_retrieve_cache
	Task.Task.put_files_cache = put_files_cache
	Task.Task.uid = uid
	Build.BuildContext.hash_env_vars = hash_env_vars
	for x in Task.classes.values():
		make_cached(x)

def cache_command(sig, files_from, files_to):
	"""
	Create a command for cache worker processes, returns a pickled
	base64-encoded tuple containing the task signature, a list of files to
	cache and a list of files files to get from cache (one of the lists
	is assumed to be empty)
	"""
	proc = get_process()

	obj = base64.b64encode(cPickle.dumps([sig, files_from, files_to]))
	proc.stdin.write(obj)
	proc.stdin.write('\n'.encode())
	proc.stdin.flush()
	obj = proc.stdout.readline()
	if not obj:
		raise OSError('Preforked sub-process %r died' % proc.pid)
	process_pool.append(proc)
	return cPickle.loads(base64.b64decode(obj))

try:
	copyfun = os.link
except NameError:
	copyfun = shutil.copy2

def atomic_copy(orig, dest):
	"""
	Copy files to the cache, the operation is atomic for a given file
	"""
	global copyfun
	tmp = dest + '.tmp'
	up = os.path.dirname(dest)
	try:
		os.makedirs(up)
	except OSError:
		pass

	try:
		copyfun(orig, tmp)
	except OSError as e:
		if e.errno == errno.EXDEV:
			copyfun = shutil.copy2
			copyfun(orig, tmp)
		else:
			raise
	os.rename(tmp, dest)

def lru_trim():
	"""
	the cache folders take the form:
	`CACHE_DIR/0b/0b180f82246d726ece37c8ccd0fb1cde2650d7bfcf122ec1f169079a3bfc0ab9`
	they are listed in order of last access, and then removed
	until the amount of folders is within TRIM_MAX_FOLDERS and the total space
	taken by files is less than EVICT_MAX_BYTES
	"""
	lst = []
	for up in os.listdir(CACHE_DIR):
		if len(up) == 2:
			sub = os.path.join(CACHE_DIR, up)
			for hval in os.listdir(sub):
				path = os.path.join(sub, hval)

				size = 0
				for fname in os.listdir(path):
					size += os.lstat(os.path.join(path, fname)).st_size
				lst.append((os.stat(path).st_mtime, size, path))

	lst.sort(key=lambda x: x[0])
	lst.reverse()

	tot = sum(x[1] for x in lst)
	while tot > EVICT_MAX_BYTES or len(lst) > TRIM_MAX_FOLDERS:
		_, tmp_size, path = lst.pop()
		tot -= tmp_size

		tmp = path + '.tmp'
		try:
			shutil.rmtree(tmp)
		except OSError:
			pass
		try:
			os.rename(path, tmp)
		except OSError:
			sys.stderr.write('Could not rename %r to %r' % (path, tmp))
		else:
			try:
				shutil.rmtree(tmp)
			except OSError:
				sys.stderr.write('Could not remove %r' % tmp)
	sys.stderr.write("Cache trimmed: %r bytes in %r folders left\n" % (tot, len(lst)))


def lru_evict():
	"""
	Reduce the cache size
	"""
	lockfile = os.path.join(CACHE_DIR, 'all.lock')
	try:
		st = os.stat(lockfile)
	except EnvironmentError as e:
		if e.errno == errno.ENOENT:
			with open(lockfile, 'w') as f:
				f.write(''.encode())
			return
		else:
			raise

	if st.st_mtime < time.time() - EVICT_INTERVAL_MINUTES * 60:
		# check every EVICT_INTERVAL_MINUTES minutes if the cache is too big
		# OCLOEXEC is unnecessary because no processes are spawned
		fd = os.open(lockfile, os.O_RDWR | os.O_CREAT, 0o755)
		try:
			try:
				fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
			except EnvironmentError:
				sys.stderr.write('another process is running!\n')
				pass
			else:
				# now dow the actual cleanup
				lru_trim()
				os.utime(lockfile, None)
		finally:
			os.close(fd)

def copy_to_cache(sig, files_from, files_to):
	"""
	Copy files to the cache, existing files are overwritten,
	and the copy is atomic only for a given file, not for all files
	that belong to a given task object
	"""
	try:
		for i, x in enumerate(files_from):
			dest = os.path.join(CACHE_DIR, sig[:2], sig, str(i))
			atomic_copy(x, dest)
	except EnvironmentError:
		# no errors should be raised
		pass
	else:
		# attempt trimming if caching was successful:
		# we may have things to trim!
		lru_evict()

def copy_from_cache(sig, files_from, files_to):
	"""
	Copy files from the cache
	"""
	try:
		for i, x in enumerate(files_to):
			orig = os.path.join(CACHE_DIR, sig[:2], sig, str(i))
			atomic_copy(orig, x)

		# success! update the cache time
		os.utime(os.path.join(CACHE_DIR, sig[:2], sig), None)
	except EnvironmentError as e:
		return "Failed to copy %r to %r: %s" % (orig, x, e)
	return OK

def loop():
	"""
	This function is run when this file is run as a standalone python script,
	it assumes a parent process that will communicate the commands to it
	as pickled-encoded tuples (one line per command)

	The commands are to copy files to the cache or copy files from the
	cache to a target destination
	"""
	# one operation is performed at a single time by a single process
	# therefore stdin never has more than one line
	txt = sys.stdin.readline().strip()
	if not txt:
		# parent process probably ended
		sys.exit(1)
	ret = OK

	[sig, files_from, files_to] = cPickle.loads(base64.b64decode(txt))
	if files_from:
		# pushing to cache is done without any wait
		th = threading.Thread(target=copy_to_cache, args=(sig, files_from, files_to))
		th.setDaemon(True)
		th.start()
	elif files_to:
		# the build process waits for workers to (possibly) obtain files from the cache
		ret = copy_from_cache(sig, files_from, files_to)
	else:
		ret = "Invalid command"

	obj = base64.b64encode(cPickle.dumps(ret))
	sys.stdout.write(obj.decode())
	sys.stdout.write('\n')
	sys.stdout.flush()

if __name__ == '__main__':
	while 1:
		try:
			loop()
		except KeyboardInterrupt:
			break

