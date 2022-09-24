local World = require ('lvtk.World')
local w = World.new()
w:load_all()

local nuris = 0
local nplus = 0

for k,v in ipairs (w:plugin_uris()) do
    nuris = nuris + 1
end

for k,v in ipairs (w:plugins()) do
    nplus = nplus + 1
end

local LVTK_PLUGINS__Volume = "https://lvtk.org/plugins/volume"
local plugin = w:instantiate (LVTK_PLUGINS__Volume)

assert (nuris == nplus and nuris > 0 and plugin ~= nil)

plugin = w:instantiate ("fake plugin")
assert (plugin == nil)
os.exit(0)
