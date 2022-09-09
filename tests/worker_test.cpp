
#include "tests.hpp"

#include "lvtk/ext/worker.hpp"
#include "lvtk/lvtk.hpp"
#include "lvtk/plugin.hpp"

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>
#include <lv2/core/lv2.h>
#include <lv2/worker/worker.h>

#include <cstdint>
#include <cstring>
#include <vector>

// dummy plugin with worker interface
struct WorkerPlug : lvtk::Plugin<WorkerPlug, lvtk::Worker> {
    WorkerPlug (const lvtk::Args& args) : Plugin (args) {}

    static constexpr const uint32_t work_value = 1111;
    static constexpr const uint32_t work_value_multiplier = 5;
    static constexpr const uint32_t work_size = sizeof (uint32_t);

    bool work_called = false;
    bool work_response_called = false;
    bool end_run_called = false;

    void request_work() {
        work_called = false;
        work_response_called = false;
        end_run_called = false;

        auto value = work_value;
        schedule_work (work_size, &value);
    }

    lvtk::WorkerStatus work (lvtk::WorkerRespond& respond, uint32_t size, const void* data) {
        work_called = true;
        // CPPUNIT_ASSERT(false == respond); // not testing response yet
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

class Worker : public TestFixutre {
    CPPUNIT_TEST_SUITE (Worker);
    CPPUNIT_TEST (integration);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

protected:
    void integration() {
        lvtk::Descriptor<WorkerPlug> reg (LVTK_TEST_PLUGIN_URI);
        const auto& desc = lvtk::descriptors().back();
        CPPUNIT_ASSERT (strcmp (desc.URI, LVTK_TEST_PLUGIN_URI) == 0);

        LV2_Worker_Schedule schedule = {
            .handle = this,
            .schedule_work = _schedule_work
        };

        LV2_Feature feature = {
            .URI = LV2_WORKER__schedule,
            .data = &schedule
        };

        const LV2_Feature* features[] = { &feature, nullptr };
        auto handle = desc.instantiate (&desc, 44100.0, "/fake/path", features);
        auto plugin = static_cast<WorkerPlug*> (handle);

        plugin->request_work();
        CPPUNIT_ASSERT (work_was_requested);
        CPPUNIT_ASSERT (work_size == WorkerPlug::work_size);
        CPPUNIT_ASSERT (work_data == WorkerPlug::work_value);

        auto iface = (const LV2_Worker_Interface*) desc.extension_data (LV2_WORKER__interface);
        CPPUNIT_ASSERT (iface != nullptr);
        if (iface) {
            iface->work (handle, nullptr, nullptr, work_size, &work_data);

            CPPUNIT_ASSERT (plugin->work_called);
            iface->work_response (handle, WorkerPlug::work_size, &work_data);
            CPPUNIT_ASSERT (plugin->work_response_called);
            iface->end_run (handle);
            CPPUNIT_ASSERT (plugin->end_run_called);
        }

        desc.cleanup (handle);
        lvtk::descriptors().pop_back(); // needed so descriptor count test doesn't fail
    }

private:
    bool work_was_requested = false;
    uint32_t work_data = 0, work_size = 0;
    static LV2_Worker_Status _schedule_work (LV2_Worker_Schedule_Handle handle, uint32_t size, const void* data) {
        auto self = static_cast<Worker*> (handle);
        self->work_size = size;
        self->work_data = *(uint32_t*) data;
        self->work_was_requested = true;
        return LV2_WORKER_SUCCESS;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION (Worker);
