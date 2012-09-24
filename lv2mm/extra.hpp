/**
    extra.hpp - Support file for writing LV2 plugins in C++

    Copyright (C) 2012 Michael Fisher <mfisher31@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA
*/
/**
   @extra.hpp
   C++ convenience header for the LV2 extra extensions.
   This covers extensions that aren't in the LV2 Specification officially,
   Many of these are more than likely broken.
*/

#ifndef LV2_EXTRA_HPP
#define LV2_EXTRA_HPP

#if defined (LV2MM_EXTRA_ENABLED)

#include <lv2mm/types.hpp>

namespace LV2 {

   /** @internal
       A mixin that allows easy sending of OSC from GUI to plugin.

       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.

       Do NOT use this. It may change in the future.
       @ingroup guimixins
   */
    LV2MM_MIXIN_CLASS WriteOSC {

     /** This is the type that your plugin or GUI class will inherit when you
         use the WriteOSC mixin. The public and protected members defined
         here will be available in your plugin class.
     */
     LV2MM_MIXIN_DERIVED {

        I() : m_osc_type(0) {
         m_buffer = lv2_event_buffer_new(sizeof(LV2_Event) + 256, 0);
       }

       bool check_ok() {
         Derived* d = static_cast<Derived*>(this);
         m_osc_type = d->
           uri_to_id(LV2_EVENT_URI, "http://lv2plug.in/ns/ext/osc#OscEvent");
         m_event_buffer_format = d->
           uri_to_id(LV2_UI_URI, "http://lv2plug.in/ns/extensions/ui#Events");
         return !Required || (m_osc_type && m_event_buffer_format);
       }

     protected:

       bool write_osc(uint32_t port, const char* path, const char* types, ...) {
         if (m_osc_type == 0)
           return false;
         // XXX handle all sizes here - this is dangerous
         lv2_event_buffer_reset(m_buffer, 0, m_buffer->data);
         LV2_Event_Iterator iter;
         lv2_event_begin(&iter, m_buffer);
         va_list ap;
         va_start(ap, types);
         uint32_t size = lv2_osc_event_vsize(path, types, ap);
         va_end(ap);
         if (!size)
           return false;
         va_start(ap, types);
         bool success = lv2_osc_buffer_vappend(&iter, 0, 0, m_osc_type,
                                               path, types, size, ap);
         va_end(ap);
         if (success) {
           static_cast<Derived*>(this)->
             write(port, m_buffer->header_size + m_buffer->capacity,
                   m_event_buffer_format, m_buffer);
           return true;
         }
         return false;
       }

       uint32_t m_osc_type;
       uint32_t m_event_buffer_format;
       LV2_Event_Buffer* m_buffer;

     };

   };

   /** Preset GUI extension - the host will tell the GUI what presets are
       available and which one of them is currently active, the GUI can request
       saving and using presets.

       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.
       @ingroup guimixins
   */
    LV2MM_MIXIN_CLASS Presets {

     /** This is the type that your plugin or GUI class will inherit when you
         use the Presets mixin. The public and protected members defined
         here will be available in your plugin class.
     */
     LV2MM_MIXIN_DERIVED {

         I() : m_hdesc(0), m_host_support(false) { }

         /** @internal */
         static void map_feature_handlers(FeatureHandlerMap& hmap) {
           hmap[LV2_UI_PRESETS_URI] = &I<Derived>::handle_feature;
         }

         /** @internal */
         static void handle_feature(void* instance, void* data) {
           Derived* d = reinterpret_cast<Derived*>(instance);
           I<Derived>* e = static_cast<I<Derived>*>(d);
           e->m_hdesc = static_cast<LV2UI_Presets_Feature*>(data);
           e->m_ok = (e->m_hdesc != 0);
           e->m_host_support = (e->m_hdesc != 0);
         }

         bool check_ok() {
           if (LV2MM_DEBUG) {
             std::clog<<"    [LV2::Presets] Validation "
                      <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
           }
           return this->m_ok;
         }

         /** This is called by the host to let the GUI know that a new
             preset has been added or renamed.
             @param number The number of the added preset.
             @param name The name of the added preset.
         */
         void preset_added(uint32_t    number,
                            char const* name) {

         }

         /** This is called by the host to let the GUI know that a previously
             existing preset has been removed.
             @param number The number of the removed preset.
         */
         void preset_removed(uint32_t number) {

         }

         /** This is called by the host to let the GUI know that all previously
             existing presets have been removed.
         */
         void presets_cleared() {

         }

         /** This is called by the host to let the GUI know that the current
             preset has changed. If the number is equal to
             @c LV2_UI_PRESETS_NOPRESET there is no current preset.
             @param number The number of the active preset, or
                           LV2_UI_PRESETS_NOPRESET if there is no active preset.
         */
         void current_preset_changed(uint32_t number) {

         }

         /** @internal
             Returns the plugin descriptor for this extension.
         */
         static void const* extension_data(char const* uri) {
           static LV2UI_Presets_GDesc desc = { &_preset_added,
                                               &_preset_removed,
                                               &_presets_cleared,
                                               &_current_preset_changed };
           if (!std::strcmp(uri, LV2_UI_PRESETS_URI))
             return &desc;
           return 0;
         }

     protected:

         /** You can call this to request that the host changes the current
             preset to @c preset. */
         void change_preset(uint32_t preset) {
           if (m_hdesc) {
             if (LV2MM_DEBUG) {
               std::clog<<"[LV2::Presets] change_preset("<<preset<<")"
                        <<std::endl;
             }
             m_hdesc->change_preset(static_cast<Derived*>(this)->controller(),
                                    preset);
           }
           else if (LV2MM_DEBUG) {
             std::clog<<"[LV2::Presets] change_preset("<<preset<<")"
                      <<" --- Function not provided by host!"<<std::endl;
           }
         }

         /** You can call this to request that the host saves the current state
             of the plugin instance to a preset with the given number and name.
         */
         void save_preset(uint32_t preset, char const* name) {
           if (m_hdesc) {
             if (LV2MM_DEBUG) {
               std::clog<<"[LV2::Presets] save_preset("<<preset<<", \""
                        <<name<<"\")"<<std::endl;
             }
             m_hdesc->save_preset(static_cast<Derived*>(this)->controller(),
                                  preset, name);
           }
           else if (LV2MM_DEBUG) {
             std::clog<<"[LV2::Presets] save_preset("<<preset<<", \""
                      <<name<<"\")"
                      <<" --- Function not provided by host!"<<std::endl;
           }
         }

         /** Returns @c true if the host supports the Preset feature, @c false
             if it does not. */
         bool host_supports_presets() const {
           return m_host_support;
         }

     private:

         static void _preset_added(LV2UI_Handle gui,
                                    uint32_t     number,
                                    char const*  name) {
           if (LV2MM_DEBUG) {
             std::clog<<"[LV2::Presets] Host called preset_added("
                      <<number<<", \""<<name<<"\")."<<std::endl;
           }
           static_cast<Derived*>(gui)->preset_added(number, name);
         }

         static void _preset_removed(LV2UI_Handle gui,
                                      uint32_t     number) {
           if (LV2MM_DEBUG) {
             std::clog<<"[LV2::Presets] Host called preset_removed("
                      <<number<<")."<<std::endl;
           }
           static_cast<Derived*>(gui)->preset_removed(number);
         }

         static void _presets_cleared(LV2UI_Handle gui) {
           if (LV2MM_DEBUG) {
             std::clog<<"[LV2::Presets] Host called presets_cleared()."
                      <<std::endl;
           }
           static_cast<Derived*>(gui)->presets_cleared();
         }

         static void _current_preset_changed(LV2UI_Handle gui,
                                              uint32_t     number) {
           if (LV2MM_DEBUG) {
             std::clog<<"[LV2::Presets] Host called current_preset_changed("
                      <<number<<")."<<std::endl;
           }
           static_cast<Derived*>(gui)->current_preset_changed(number);
         }

         LV2UI_Presets_Feature* m_hdesc;
         bool m_host_support;

     };

   };

   /** The fixed buffer size extension. A host that supports this will always
       call the plugin's run() function with the same @c sample_count parameter,
       which will be equal to the value returned by I::get_buffer_size().

       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.
       @ingroup pluginmixins
   */
   LV2MM_MIXIN_CLASS FixedBufSize {

     /** This is the type that your plugin class will inherit when you use the
         FixedBufSize mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     LV2MM_MIXIN_DERIVED {

        I() : m_buffer_size(0) { }

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {
         hmap["http://tapas.affenbande.org/lv2/ext/fixed-buffersize"] =
           &I<Derived>::handle_feature;
       }

       /** @internal */
       static void handle_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);
         fe->m_buffer_size = *reinterpret_cast<uint32_t*>(data);
         if (LV2MM_DEBUG) {
           std::clog<<"    [LV2::FixedBufSize] Host set buffer size to "
                    <<fe->m_buffer_size<<std::endl;
         }
         fe->m_ok = true;
       }

       bool check_ok() {
         if (LV2MM_DEBUG) {
           std::clog<<"    [LV2::FixedBufSize] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

     protected:

       /** This returns the buffer size that the host has promised to use.
           If the host does not support this extension this function will
           return 0. */
       uint32_t get_buffer_size() const { return m_buffer_size; }

       uint32_t m_buffer_size;

     };
   };

   /** The fixed power-of-2 buffer size extension. This works just like
       FixedBufSize with the additional requirement that the buffer size must
       be a power of 2.

       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.
       @ingroup pluginmixins
   */
   LV2MM_MIXIN_CLASS FixedP2BufSize {

     /** This is the type that your plugin class will inherit when you use the
         FixedP2BufSize mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     LV2MM_MIXIN_DERIVED {

        I() : m_buffer_size(0) { }

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {
         hmap["http://tapas.affenbande.org/lv2/ext/power-of-two-buffersize"] =
           &I<Derived>::handle_feature;
       }

       /** @internal */
       static void handle_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);
         fe->m_buffer_size = *reinterpret_cast<uint32_t*>(data);
         if (LV2MM_DEBUG) {
           std::clog<<"    [LV2::FixedP2BufSize] Host set buffer size to "
                    <<fe->m_buffer_size<<std::endl;
         }
         fe->m_ok = true;
       }

       bool check_ok() {
         if (LV2MM_DEBUG) {
           std::clog<<"    [LV2::FixedP2BufSize] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

     protected:

       /** This returns the buffer size that the host has promised to use.
           If the host does not support this extension this function will
           return 0. */
       uint32_t get_buffer_size() const { return m_buffer_size; }

       uint32_t m_buffer_size;

     };

   };


   /** The save/restore extension.

       The actual type that your plugin class will
       inherit when you use this mixin is the internal struct template I.
       @ingroup pluginmixins
   */
   LV2MM_MIXIN_CLASS SaveRestore {

     /** This is the type that your plugin class will inherit when you use the
         SaveRestore mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     LV2MM_MIXIN_DERIVED {

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {
         hmap[LV2_SAVERESTORE_URI] = &I<Derived>::handle_feature;
       }

       /** @internal */
       static void handle_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);
         fe->m_ok = true;
       }

       bool check_ok() {
         if (LV2MM_DEBUG) {
           std::clog<<"    [LV2::SaveRestore] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

       /** @internal */
       static const void* extension_data(const char* uri) {
         if (!std::strcmp(uri, LV2_SAVERESTORE_URI)) {
           static LV2SR_Descriptor srdesc = { &I<Derived>::_save,
                                              &I<Derived>::_restore };
           return &srdesc;
         }
         return 0;
       }

       /** This function is called by the host when it wants to save the
           current state of the plugin. You should override it.
           @param directory A filesystem path to a directory where the plugin
                            should write any files it creates while saving.
           @param files A pointer to a NULL-terminated array of @c LV2SR_File
                        pointers. The plugin should set @c *files to point to
                        the first element in a dynamically allocated array of
                        @c LV2SR_File pointers to (also dynamically allocated)
                        @c LV2SR_File objects, listing the files to which the
                        internal state of the plugin instance has been saved.
                        These objects, and the array, will be freed by the host.
       */
       char* save(const char* directory, LV2SR_File*** files) { return 0; }

       /** This function is called by the host when it wants to restore
           the plugin to a previous state. You should override it.
           @param files An array of pointers to @c LV2SR_File objects, listing
                        the files from which the internal state of the plugin
                        instance should be restored.
       */
       char* restore(const LV2SR_File** files) { return 0; }

     protected:

       /** @internal
           Static callback wrapper. */
       static char* _save(LV2_Handle h,
                          const char* directory, LV2SR_File*** files) {
         if (LV2MM_DEBUG) {
           std::clog<<"[LV2::SaveRestore] Host called save().\n"
                    <<"  directory: \""<<directory<<"\""<<std::endl;
         }
         return reinterpret_cast<Derived*>(h)->save(directory, files);
       }

       /** @internal
           Static callback wrapper. */
       static char* _restore(LV2_Handle h, const LV2SR_File** files) {
         if (LV2MM_DEBUG) {
           std::clog<<"[LV2::SaveRestore] Host called restore().\n"
                    <<"  Files:\n";
           for (LV2SR_File const** f = files; (*f) != 0; ++f)
             std::clog<<"  \""<<(*f)->name<<"\" -> \""<<(*f)->path<<"\"\n";
           std::clog<<std::flush;
         }
         return reinterpret_cast<Derived*>(h)->restore(files);
       }

     };
   };


   /** @internal
       The message context extension. Experimental and unsupported. Don't use it.

       The actual type that your plugin class will inherit when you use
       this mixin is the internal struct template I.
       @ingroup pluginmixins
   */
   LV2MM_MIXIN_CLASS MsgContext {

     /** This is the type that your plugin class will inherit when you use the
         MsgContext mixin. The public and protected members defined here
         will be available in your plugin class.
     */
     LV2MM_MIXIN_DERIVED {

       /** @internal */
       static void map_feature_handlers(FeatureHandlerMap& hmap) {
         hmap[LV2_CONTEXT_MESSAGE] = &I<Derived>::handle_feature;
       }

       /** @internal */
       static void handle_feature(void* instance, void* data) {
         Derived* d = reinterpret_cast<Derived*>(instance);
         I<Derived>* fe = static_cast<I<Derived>*>(d);
         fe->m_ok = true;
       }

       bool check_ok() {
         if (LV2MM_DEBUG) {
           std::clog<<"    [LV2::MsgContext] Validation "
                    <<(this->m_ok ? "succeeded" : "failed")<<"."<<std::endl;
         }
         return this->m_ok;
       }

       /** @internal */
       static const void* extension_data(const char* uri) {
         if (!std::strcmp(uri, LV2_CONTEXT_MESSAGE)) {
           static LV2_Blocking_Context desc = { &I<Derived>::_blocking_run,
                                                &I<Derived>::_connect_port };
           return &desc;
         }
         return 0;
       }

       /** @internal
           This is called by the host when the plugin's message context is
           executed. Experimental and unsupported. Don't use it.
       */
       bool blocking_run(uint8_t* outputs_written) { return false; }

     protected:

       /** @internal
           Static callback wrapper. */
       static bool _blocking_run(LV2_Handle h, uint8_t* outputs_written) {
         if (LV2MM_DEBUG)
           std::clog<<"[LV2::MsgContext] Host called blocking_run()."<<std::endl;
         return reinterpret_cast<Derived*>(h)->blocking_run(outputs_written);
       }

       /** @internal
           Static callback wrapper. */
       static void _connect_port(LV2_Handle h, uint32_t port, void* buffer) {
         reinterpret_cast<Derived*>(h)->connect_port(port, buffer);
       }

     };
   };

} /* namespace LV2 */


#endif

#endif /* LV2_EXTRA_HPP */
