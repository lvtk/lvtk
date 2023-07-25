# Copyright 2022 Michael Fisher <mfisher@lvtk.org>
# SPDX-License-Identifier: ISC

from subprocess import Popen, PIPE
import sys

def call_lua():
    prog = ''
    if len (sys.argv) > 0:
        prog = sys.argv[1]
    if len(prog) <= 0:
        prog = 'lua'
    process = Popen([prog, '-v'], stderr=PIPE, stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()
    if err: return '0.0.0'
    output = output.decode ('utf-8')
    v = output.strip().split(' ')[1].split('.')
    return '%s.%s' % (v[0], v[1])

vers = call_lua()
print (vers)
