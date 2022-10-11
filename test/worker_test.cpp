// Copyright 2019 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#include "tests.hpp"

#include <boost/test/unit_test.hpp>

#include "lvtk/ext/worker.hpp"
#include "lvtk/lvtk.hpp"
#include "lvtk/plugin.hpp"

#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>

#include <cstdint>
#include <cstring>
#include <vector>

// dummy plugin with worker interface
struct WorkerPlug : lvtk::Plugin<WorkerPlug, lvtk::Worker> {
    WorkerPlug (const lvtk::Args& args) : Plugin (args) {}

    static constexpr const uint32_t work_value            = 1111;
    static constexpr const uint32_t work_value_multiplier = 5;
    static constexpr const uint32_t work_size             = sizeof (uint32_t);

    bool work_called          = false;
    bool work_response_called = false;
    bool end_run_called       = false;

    void request_work() {
        work_called          = false;
        work_response_called = false;
        end_run_called       = false;

        auto value = work_value;
        schedule_work (work_size, &value);
    }

    lvtk::WorkerStatus work (lvtk::WorkerRespond& respond, uint32_t size, const void* data) {
        work_called = true;
        // BOOST_REQUIRE(false == respond); // not testing response yet
        return LV2_WORKER_SUCCESS;
    }

    lvtk::WorkerStatus work_response (uint32_t size, const void* data) {
        work_response_called = true;
        return LV2_WORKER_SUCCESS;
    }

    lvtk::WorkerStatus end_run() {
        end_run_called = true;
        return LV2_WORKER_SUCCESS;
    }
};

struct WorkerTest {
    void integration() {
        lvtk::Descriptor<WorkerPlug> reg (LVTK_TEST_PLUGIN_URI);
        const auto& desc = lvtk::descriptors().back();
        BOOST_REQUIRE (strcmp (desc.URI, LVTK_TEST_PLUGIN_URI) == 0);

        LV2_Worker_Schedule schedule = {
            (void*) this, _schedule_work
        };

        LV2_Feature feature = {
            LV2_WORKER__schedule, (void*) &schedule
        };

        const LV2_Feature* features[] = { &feature, nullptr };
        auto handle                   = desc.instantiate (&desc, 44100.0, "/fake/path", features);
        auto plugin                   = static_cast<WorkerPlug*> (handle);

        plugin->request_work();
        BOOST_REQUIRE (work_was_requested);
        BOOST_REQUIRE (work_size == WorkerPlug::work_size);
        BOOST_REQUIRE (work_data == WorkerPlug::work_value);

        auto iface = (const LV2_Worker_Interface*) desc.extension_data (LV2_WORKER__interface);
        BOOST_REQUIRE (iface != nullptr);
        if (iface) {
            iface->work (handle, nullptr, nullptr, work_size, &work_data);

            BOOST_REQUIRE (plugin->work_called);
            iface->work_response (handle, WorkerPlug::work_size, &work_data);
            BOOST_REQUIRE (plugin->work_response_called);
            iface->end_run (handle);
            BOOST_REQUIRE (plugin->end_run_called);
        }

        desc.cleanup (handle);
        lvtk::descriptors().pop_back(); // needed so descriptor count test doesn't fail
    }

private:
    bool work_was_requested = false;
    uint32_t work_data = 0, work_size = 0;
    static LV2_Worker_Status _schedule_work (LV2_Worker_Schedule_Handle handle, uint32_t size, const void* data) {
        auto self                = static_cast<WorkerTest*> (handle);
        self->work_size          = size;
        self->work_data          = *(uint32_t*) data;
        self->work_was_requested = true;
        return LV2_WORKER_SUCCESS;
    }
};

BOOST_AUTO_TEST_SUITE (Worker)

BOOST_AUTO_TEST_CASE (integration) {
    WorkerTest().integration();
}

BOOST_AUTO_TEST_SUITE_END()
