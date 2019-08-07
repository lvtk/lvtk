
#pragma once

#include <map>
#include <lvtk/lvtk.hpp>
#include <lvtk/dictionary.hpp>

namespace lvtk {

using PluginDescriptors = DesctriptorList<LV2_Descriptor>;

static PluginDescriptors& descriptors() {
    static PluginDescriptors s_descriptors;
    return s_descriptors;
}

template<class InstanceType>
class Plugin
{
public:
    Plugin (const char* uri, const StringArray& required = StringArray()) {
        LV2_Descriptor desc;
        desc.URI = strdup (uri);
        desc.instantiate    = _instantiate;
        desc.connect_port   = _connect_port;
        desc.activate       = _activate;
        desc.run            = _run;
        desc.deactivate     = _deactivate;
        desc.cleanup        = _cleanup;
        desc.extension_data = _extension_data;
        descriptors().push_back (desc);

        for (const auto& req : required)
            s_required.push_back (req);
    }

    ~Plugin() = default;

    static void register_extension (const std::string& uri, const void* data) {
        std::clog << "reg: " << uri << std::endl;
        s_extensions[uri] = data;
    }

private:
    static ExtensionMap s_extensions;
    static StringArray  s_required;

    LV2_Worker_Schedule worker;

    static LV2_Handle _instantiate (const struct _LV2_Descriptor * descriptor,
	                                double                         sample_rate,
	                                const char *                   bundle_path,
	                                const LV2_Feature *const *     features)
    {
        const FeatureList feature_list (features);
        auto instance = std::unique_ptr<InstanceType> (
            new InstanceType (sample_rate, bundle_path, feature_list));

        std::clog << "[lvtk] checking required features" << std::endl;
        for (const auto& rq : s_required)
        {
            std::clog << "[lvtk] " << rq << ": ";
            bool provided = false;
            for (const auto& f : feature_list)
                if (strcmp (f->URI, rq.c_str()) == 0)
                    { provided = true; break; }
            
            if (! provided) {
                std::clog << "error - not provided by host\n";
                return nullptr;
            }
            else 
            {
                std::clog << "ok";
            }
        }

        return static_cast<LV2_Handle> (instance.release());
    }

    static void _connect_port (LV2_Handle handle, uint32_t port, void* data) {
        (static_cast<InstanceType*> (handle))->connect_port (port, data);
    }

    static void _activate (LV2_Handle handle) { 
        (static_cast<InstanceType*> (handle))->activate();
    }

    static void _run (LV2_Handle handle, uint32_t sample_count) {
        (static_cast<InstanceType*> (handle))->run (sample_count);
    }
    
    inline static void _deactivate (LV2_Handle handle) {
        (static_cast<InstanceType*> (handle))->deactivate();
    }

    inline static void _cleanup (LV2_Handle handle) {
        (static_cast<InstanceType*> (handle))->cleanup();
        delete static_cast<InstanceType*> (handle);
    }

    inline static const void* _extension_data (const char* uri) {
        auto e = s_extensions.find (uri);
        return e != s_extensions.end() ? e->second : nullptr;
    }
};

template<class PluginType> ExtensionMap Plugin<PluginType>::s_extensions = {};
template<class PluginType> StringArray Plugin<PluginType>::s_required;

}

#include <lvtk/instance.hpp>


extern "C" {

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    return (index < lvtk::descriptors().size())
        ? &lvtk::descriptors()[index] : NULL;
}

}

