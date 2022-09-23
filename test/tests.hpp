#pragma once

#include <lvtk/ext/idle.hpp>
#include <lvtk/ext/parent.hpp>
#include <lvtk/ext/port_map.hpp>
#include <lvtk/ext/port_subscribe.hpp>
#include <lvtk/ext/resize.hpp>
#include <lvtk/ext/show.hpp>
#include <lvtk/ext/touch.hpp>

#include <lvtk/ext/atom.hpp>
#include <lvtk/ext/bufsize.hpp>
#include <lvtk/ext/data_access.hpp>
#include <lvtk/ext/instance_access.hpp>
#include <lvtk/ext/log.hpp>
#include <lvtk/ext/options.hpp>
#include <lvtk/ext/resize_port.hpp>
#include <lvtk/ext/state.hpp>
#include <lvtk/ext/urid.hpp>
#include <lvtk/ext/worker.hpp>

#include <lvtk/lvtk.hpp>
#include <lvtk/optional.hpp>
#include <lvtk/options.hpp>
#include <lvtk/plugin.hpp>
#include <lvtk/symbols.hpp>
#include <lvtk/ui.hpp>

#ifndef LVTK_VOLUME_URI
#    define LVTK_VOLUME_URI "http://lvtk.org/plugins/volume"
#endif

#define LVTK_TEST_PLUGIN_URI "http://lvtk.org/plugins/test"
#define LVTK_TEST_UI_URI     "http://lvtk.org/plugins/test#ui"
