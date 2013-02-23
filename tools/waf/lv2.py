#!/usr/bin/env python
# encoding: utf-8
# Michael Fisher 2012

"""
LV2 and LV2 Bundle Related Tools
------------------------
WARNING: At the time of writing, this file assumes you have called
autowaf.configure(conf)  in your configure target. TODO: Fix That

Written for waf-1.7.x . Have not tested previous versions.
"""

import os, shutil, sys, platform
from waflib import TaskGen, Task, Build, Options, Utils, Errors
from waflib.TaskGen import taskgen_method, feature, after_method, before_method
from waflib.Configure import conf

import autowaf

@conf
def check_lv2 (self, min_version="1.0.0"):
	autowaf.check_pkg(self, "lv2", uselib_store="LV2", atleast_version=min_version)

def bundle_name_for_output(out):
	name = out.name
	k = name.rfind('.')
	if k >= 0:
		name = name[:k] + '.lv2'
	else:
		name = name + '.lv2'
	return name


def configure(conf):
	pat = conf.env['cshlib_PATTERN']
	if not pat:
		pat = conf.env['cxxshlib_PATTERN']
	if pat.startswith('lib'):
		pat = pat[3:]
	conf.env['pluginlib_PATTERN'] = pat
	conf.env['pluginlib_EXT']  = pat[pat.rfind('.'):]

@taskgen_method
def create_bundle_dir(self, name, out):
        bld = self.bld
        dir = out.parent.find_or_declare(name)
        dir.mkdir()
        return dir


@feature('cshlib','cxxshlib')
@before_method('apply_link')
def fixup_soname(self):
	if not getattr(self, 'lv2_bundle', False): return
	
	self.env['cxxshlib_PATTERN'] = \
		self.env['cshlib_PATTERN'] = \
		self.env['pluginlib_PATTERN']



@feature('cshlib','cxxshlib')
@after_method('apply_link')
def create_task_lv2plugin(self):
	if not getattr(self, 'lv2_bundle', False): return
	
	out = self.link_task.outputs[0]
    
	''' If lv2_target is set the value there will be used for the
	bundle name '''
	name = getattr(self, 'lv2_target', bundle_name_for_output(out))

	dir = self.create_bundle_dir(name, out)
	plugin = dir.find_or_declare([out.name])
	
	self.apptask = self.create_task('lv2', self.link_task.outputs, plugin)
    
	inst_to = getattr(self, 'install_path', "${LV2DIR}") + '/%s/' % name
	self.bld.install_files(inst_to, plugin, chmod=Utils.O755)
	
	'''  Handle LV2 Resources: 
	Files with pattern *manifest.ttl get copied as manifest.ttl. 
	Files with pattern *manifest.ttl.in* get processed as a 'subst' feature
	WIP: TODO: Clean it up'''
	if getattr(self, 'lv2_resources', False):
		res_dir = plugin.parent
		# LV2 Bundle Resources
		for x in self.to_list(self.lv2_resources):
			node = self.path.find_node(x)
			if not node:
				raise Errors.WafError('Missing lv2_resource %r in %r' % (x, self.bld))
	
			parent = node.parent
			if os.path.isdir(node.abspath()):
				nodes = node.ant_glob('**')
			else:
				nodes = [node]
			for node in nodes:
				rel    = node.path_from(parent)
				relstr = str(rel)
				
				# Handle a Manifest file
				if "manifest.ttl" in relstr:
					if "ttl.in" in relstr:
						tsk = self.create_task('subst', node, res_dir.make_node("manifest.ttl"))
						self.bld.install_as(inst_to + '/%s' % "manifest.ttl", tsk.outputs[0])
					else:
						tsk = self.create_task('lv2', node, res_dir.make_node("manifest.ttl"))
						self.bld.install_as(inst_to + '/%s' % "manifest.ttl", node)
					continue
				
				if ".ttl.in" in relstr:
					turtle = os.path.splitext(relstr)[0]
					tsk = self.create_task('subst', node, res_dir.make_node(turtle))
					self.bld.install_as(inst_to + '/%s' % turtle, node)
					continue
				
				# All else verbatim copy with the lv2 task generator
				tsk = self.create_task('lv2', node, res_dir.make_node(rel))
				self.bld.install_as(inst_to + '/%s' % rel, node)
	
	if getattr(self.bld, 'is_install', None):
		# disable the normal binary installation
		self.install_task.hasrun = Task.SKIP_ME


class lv2(Task.Task):
	"""
	Create lv2 bundles
	"""
	color = 'PINK'
	def run(self):
		self.outputs[0].parent.mkdir()
		shutil.copy2(self.inputs[0].srcpath(), self.outputs[0].abspath())
