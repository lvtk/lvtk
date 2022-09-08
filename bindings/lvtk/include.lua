--- Include other scripts.
-- Functionality similar to PHP `include`
-- @module lvtk.include

-- Copyright 2022 Michael Fisher <mfisher@lvtk.org>
-- SPDX-License-Identifier: ISC

local M = {}

setmetatable (M, {
    __call = function (_, ...)
        return {}
    end
})

return M
