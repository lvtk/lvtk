/* 
    Copyright (c) 2019, Michael Fisher <mfisher@kushview.net>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include <lvtk/lvtk.hpp>
#include <lvtk/atom.hpp>
#include <lvtk/map.hpp>

namespace lvtk {

/** A template base class for LV2 plugin instances. Default implementations 
    exist for most methods, so you only have to implement @ref connect_port() 
    and @ref run().
    
    The signature of subclass must matches the one in the example code below, 
    otherwise it will not work with plugin registration. The host will use the 
    @c args parameter to pass @ref Args "details" about instantiation you can 
    use in your constructor.

    This is a template so that simulated dynamic binding can be used for
    the callbacks. This is not all that useful for simple plugins but it may
    come in handy when using @ref Extension "Extensions" and it doesn't add
    any additional vtable lookup and function call costs, like real dynamic
    binding would.

    <h3>Copy Audio Example</h3>
    @code
        #include <cstring>
        #include <lvtk/plugin.hpp>

        using namespace lvtk;

        class CopyAudio : public Instance<CopyAudio>
        {
        public:
            CopyAudio (const Args& args) : Instance (args) { }

            void connect_port (uint32_t port, void* data) {
                audio[port] = data;
            }

            void run (uint32_t sample_count) {
                std::memcpy (audio[1], audio[0], sample_count * sizeof (float));
            }
        
        private:
            float* audio[2];
        };

        // Register a descriptor for this instance type
        static Plugin<CopyAudio> copyaudio ("http://lvtoolkit.org/plugins/CopyAudio");

    @endcode

    If the above code is compiled into a shared module, it could form the binary 
    part of a fully functional LV2 plugin with one audio input port and one audio 
    output port that just copies the input to the output.

    You can extend your instance by passing @ref Extension "Extensions" as 
    template parameters to Instance (second template parameter and onwards).
    
    @see \ref BufSize, \ref Log, \ref Options, \ref ResizePort, \ref State, 
         \ref URID, \ref Worker,
    
    @headerfile lvtk/instance.hpp
    @ingroup plugin
 */
template<class S, template<class> class... E>
class Instance : public E<S>...
{
protected:
    /** Instance with Arguments.
      
        @param args  Arguments created during instantiation.  Your subclass
                     must pass the @ref Args here
     */
    explicit Instance (const Args& args) : E<S> (args.features)... {}

public:
    ~Instance() = default;

    /** Override this function if you need to do anything on activation.
        This is always called before the host starts using the @ref run() 
        function. You should reset your plugin to it's initial state here. 
     */
    void activate() {}

    /** Override this to connect to your own port buffers.

        Remember that if you want your plugin to be realtime safe this function
        may not block, allocate memory or otherwise take a long time to return.

        @param port The index of the port to connect.
        @param data The buffer to connect it to.
     */
    void connect_port (uint32_t port, void* data) {}

    /** This is the process callback which should fill all output port buffers.
        You most likely want to override it - the default implementation does
        nothing.
        
        Remember that if you want your plugin to be realtime safe, this function
        may not block, allocate memory or take more than `sample_count` time
        to execute.
        
        @param sample_count The number of audio frames to process.
     */
    void run (uint32_t sample_count) {}

    /** Override this function if you need to do anything on deactivation.
        The host calls this when it does not plan to make any more calls to
        @ref run() unless it calls @ref activate() again.
     */
    void deactivate() {}

    /** Override this to handle cleanup. Is called immediately before the 
        instance is deleted.  You only need to implement this if you'd like 
        to do something special before the destructor.
     */
    void cleanup() {}

private:
    friend class Plugin<S>; // so this can be private
    static void map_extension_data (ExtensionMap& em) {
        using pack_context = std::vector<int>;
        pack_context { (E<S>::map_extension_data (em), 0)... };
    }
};

}
