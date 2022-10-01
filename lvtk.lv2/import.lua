--------------------------------------------
local lvtk = require ('lvtk')
---- add some C++ like operators ----
local class = lvtk.object
local new = class.new

-- shortcuts
local define = lvtk.import.define

---

local I = define ('LVTK')

--###########################################################333333333333333333
-- LVTK Gui Demo
-- define a new Widget type with attributes.
-- class types in lua are really just a table
-- So this "Box" table is a table, but it can also be used
-- as a type identifier
local Box = class (lvtk.Widget, {
    color = {
        set = function (self, value)
            rawset (self, '_color', value)
            -- self:repaint() not there yet :)
        end,
        get = function (self)
            return rawget (self, '_color') or 0x444444ff
        end
    }
})

-- "constructor" is required right now whether you want one or not ...
function Box:init()
    -- ... because the parent class method needs called for
    -- it all to work.
    lvtk.Widget.init (self)
end

--- g is a lvtk.Graphics
function Box:paint (g)
    g:set_color (self.color)
    g:fill_rect (self.bounds:at(0, 0))
end

function Box:resized()
    local r = self.bounds
    -- anything that implements tostring
    -- can be printed
    print (self, r)
end

function Box:obstructed (x, y)
    return true
end

function Box:presed (ev)
    local evs = tostring (ev)
    print ("pressed: " .. evs)
end

function Box:released (ev)
    local evs = tostring (ev)
    print ("released: " .. evs)
end

-- a sub class of the sub class
local RedBox = class (Box)
function RedBox:init()
    -- Invoke the parent class' init method.
    -- skipping the init step causes all kinds of
    -- confusing problems.
    Box.init (self)
    self.color = 0xff0000ff    -- set the parent class property
    self.visible = true        -- make it visible
    self:set_size (360, 240)   -- need a size
    -- TODO  self.size = { x, y }
end

local lvtk_demo_run = function()
    -- `new` can also instantiate an object from a lua module name
    -- if it implements a `new` factory function
    local main = new ('lvtk.Main')
    -- or call new on an imported type table

    -- Elevate a red box to View status e.g. an OS window
    main:elevate (new (RedBox))

    -- drive the GUI
    local running = true
    while (running) do
        main:loop (-1.0)
        running = main:running()
    end

    return main.exit_code()
end

-- register the demo for importing
local table = require ('table')
table.insert (I.packages, {
    -- type of package
    type    = 'app',

    -- It's name
    name    = "LVTK Demo",

    -- Package identifier: use lua-style namespacing please
    ID      = 'lvtk.Demo',                  

    -- Will be used in the future for package resolving
    URI     = 'https://lvtk.org/apps/lvtk-demo', 

    -- Requireed property by an 'app' type
    exec    = lvtk_demo_run
})

-- keep adding packages here

return I
