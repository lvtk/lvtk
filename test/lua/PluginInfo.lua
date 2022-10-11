-- Copyright 2022 Michael Fisher <mfisher@lvtk.org>
-- SPDX-License-Identifier: ISC

local PluginInfo = require ('lvtk.PluginInfo');
local info = PluginInfo.new ({
    URI = "http://fakeuri.dev",
    name = "Fake Plugin"
})
assert (info.name == "Fake Plugin")
assert (info.URI == "http://fakeuri.dev")
os.exit (0)
