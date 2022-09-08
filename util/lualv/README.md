## LV2 Lua Console

Here's the basic idea I have - MRF.
- [] Follow the same EXE naming/install scheme as LuaJIT. luajit <=> lualv
     This really just means, make sure default search paths are correct
     when the console boots up.
- [] Move to own project (maybe). It makes sense to also keep it in 
    `lvtk/util` since the console need the modules to work.
- [x] Do not depend or link with LVTK directly. Load dynamic LVTK lua modules instead.
- [] Automatically import from lvtk additional language functions like
    `promise`, `import`, `include`, `new`, `class`, `filesystem`,
    Probably don't want to get carried way here though
- [] Improve the CLI options if needed.
- [] Add some minimal branding above or below the normal lua.exe banner

### Biggest challenge:
Should install some kind of hook system in the console so that lvtk
Gui's and console can run simulatneously without blocking each other.
