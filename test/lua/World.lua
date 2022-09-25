local world = require ('lvtk.World')

local w = world.new()
world.load_all (w)

local nuris = 0
local nplus = 0

for k,v in ipairs (world.plugin_uris (w)) do
    nuris = nuris + 1
    print (v)
end

for k,v in ipairs (world.plugins (w)) do
    nplus = nplus + 1
end

local LVTK_PLUGINS__Volume = "https://lvtk.org/plugins/volume"
local plugin = world.instantiate (w, LVTK_PLUGINS__Volume)
if nplus > 0 then
    assert (plugin ~= nil);
end

assert (nuris == nplus)

plugin = world.instantiate (w, "fake plugin")
assert (plugin == nil)
os.exit(0)
