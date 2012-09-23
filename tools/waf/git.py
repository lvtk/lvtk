#!/usr/bin/env python
# C
# D

from subprocess import call

def tag_version(version, msg, prefix="v", suffix="",editor="nano"):
    version_string = prefix + version + suffix
    call(["git","tag","-a",version_string,"-m",msg])
