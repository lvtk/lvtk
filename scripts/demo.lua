import = require ('lvtk.import')
exec = import.exec

local bundle = import ('lvtk.lv2')
assert (type(bundle) == 'table', "Failed to import bundle")
exec (bundle, 'lvtk.Demo')
