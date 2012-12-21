#!/usr/bin/env python
# Copyright (C) 2012  Michael Fisher <mfisher31@gmail.com>

from subprocess import call

def tag_version(version, msg, prefix="", suffix="", editor="nano"):
	tag_string = prefix + version + suffix
	call(["git","tag","-a",tag_string,"-m",msg])
	return tag_string
