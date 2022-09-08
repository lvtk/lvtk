#!/bin/bash
export LUA_PATH="bindings/?.lua"
export LUA_CPATH="build/bindings/?.so"
build/lualv scripts/demo.lua
exit $?
