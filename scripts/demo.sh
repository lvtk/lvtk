#!/bin/bash
# Copyright 2022 Michael Fisher <mfisher@lvtk.org>
# SPDX-License-Identifier: ISC

export LUA_PATH="bindings/?.lua"
export LUA_CPATH="build/bindings/?.so"
lua scripts/demo.lua
exit $?
