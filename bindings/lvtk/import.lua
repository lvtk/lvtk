--- Import things from lv2 bundles or plain lua files
-- @module lvtk.import

-- Copyright 2022 Michael Fisher <mfisher@lvtk.org>
-- SPDX-License-Identifier: ISC

local M = {}

local function load_bundle (path, env)
    -- for testing
    return loadfile (path .. '/import.lua', 'bt', env or _ENV)
end

local function invoke_bundle (path, env, ...)
    local invoke, err = load_bundle (path, env)
    if err then return err end
    return invoke (...)
end

local function exec_bundle (bundle, package, ...)
    for k,v in pairs(bundle.packages) do
        if (v.ID == package) then return v.exec (...) end
    end
    return -1
end

local function make_import_table (name)
    return {
        name = name,
        packages = {}
    }
end

--- Create's a boiler plate import table
M.define = make_import_table

---
-- Run an app contained in the bundle.
M.exec = exec_bundle

setmetatable (M, {
    __call = function (_, ...)
        return invoke_bundle (...)
    end
})

return M
