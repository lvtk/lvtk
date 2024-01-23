#!/bin/bash
# Copyright 2022 Michael Fisher <mfisher@lvtk.org>
# SPDX-License-Identifier: ISC

from subprocess import Popen
from subprocess import PIPE
import glob
import sys

def include_what_you_use (file):
    iwyu = ['include-what-you-use', 
            '-Xiwyu', '--mapping_file=scripts/iwyu.imp',
            # '-Xiwyu', '--transitive_includes_only',
            '-Iinclude', '-Isrc',
            '-Isubprojects/lv2/include',
            '-std=c++17' ]
    cmd = iwyu
    cmd.append (file)
    process = Popen(cmd, stdout=PIPE)
    (output, err) = process.communicate()
    if err: 
        print("ERROR:", err)
        exit (99999)
    process.wait()
    return process.returncode

files = []
if len(sys.argv) > 1:
    files = [sys.argv[1]]
else:
    # files  = ['include/lvtk/lvtk.h']
    files += glob.glob ('include/lvtk/*.hpp')
    files += glob.glob ('include/lvtk/ext/*.hpp')
    files += glob.glob ('include/lvtk/host/*.hpp')
    files += glob.glob ('include/lvtk/ui/*.hpp')

    for ext in 'cpp hpp'.split():
        files += glob.glob ('src/*.%s' % ext)
        files += glob.glob ('src/host/*.%s')
        files += glob.glob ('src/ui/*.%s')
        files += glob.glob ('src/ui/details/*.%s')

for f in files:
    include_what_you_use (f)

exit(0)
