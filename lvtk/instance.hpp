
#pragma once

#include <lvtk/lvtk.hpp>
#include <lvtk/feature.hpp>
#include <lvtk/ext/bufsize.hpp>
#include <lvtk/ext/log.hpp>
#include <lvtk/ext/state.hpp>
#include <lvtk/ext/worker.hpp>

namespace lvtk {

class Instance
{
protected:
    Instance () = delete;

public:
    Instance (double sample_rate, const String& bundle_path, const FeatureList& features)
        : m_sample_rate (sample_rate), m_bundle_path (bundle_path)
    {
        for (const auto& f : features)
        {
            if (strcmp (f->URI, LV2_URID__map) == 0 || 
                strcmp (f->URI, LV2_URID__unmap) == 0)
            {
                dictionary.set_feature (*f);
            }

            if (strcmp (f->URI, LV2_LOG__log))
                logger.set_feature (*f);
            
            if (strcmp (f->URI, LV2_WORKER__schedule) == 0)
                schedule_work_impl.set_feature (*f);

           #if 0
            if (strcmp (LV2_BUF_SIZE__boundedBlockLength, f->URI) == 0 ||
                strcmp (LV2_BUF_SIZE__powerOf2BlockLength, f->URI) == 0 ||
                strcmp (LV2_BUF_SIZE__fixedBlockLength, f->URI) == 0)
            {
                bufsize.set_feature (*f);
            }
           #endif
        }
    }
    
    virtual ~Instance() = default;

    void activate() {}
    void connect_port (uint32_t, void*) {};
    void run (uint32_t) {}
    void deactivate() {}
    void cleanup() { }

    //=========================================================================
    const Dictionary& get_dictionary() const { return dictionary; }
    URID map (const String& uri) const { return dictionary.map (uri); }
    String unmap (URID urid) const { return dictionary.unmap (urid); }

    //=========================================================================
    StateStatus save (StateStore &store, uint32_t flags, const FeatureList &features) {  return STATE_SUCCESS; }
    StateStatus restore (StateRetrieve &retrieve, uint32_t flags, const FeatureList &features) {  return STATE_SUCCESS; }

    //=========================================================================
    uint32_t schedule_work (uint32_t size, const void* data) { return schedule_work_impl (size, data); }
    WorkerStatus work (WorkerRespond &respond, uint32_t size, const void* data) { return WORKER_SUCCESS; }
    WorkerStatus work_response (uint32_t size, const void* body) { return WORKER_SUCCESS; }
    WorkerStatus end_run() { return WORKER_SUCCESS; }

    //=========================================================================
    const Log& get_log() const { return logger; }

protected:
    double sample_rate() const         { return m_sample_rate; }
    const String& bundle_path() const  { return m_bundle_path; }

private:
    double m_sample_rate;
    String m_bundle_path;
    Dictionary dictionary;
    WorkerSchedule schedule_work_impl;
    Log logger;
};

}
