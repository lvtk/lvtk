#!/bin/bash
export LUA_PATH="bindings/?.lua"
export LUA_CPATH="build/bindings/?.so"
lua scripts/demo.lua
exit $?
