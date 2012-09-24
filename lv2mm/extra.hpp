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
   This covers extensions that aren't in the LV2 Specification officially
*/

#ifndef LV2_XXX_HPP
#define LV2_XXX_HPP

#include <lv2mm/types.hpp>

namespace LV2 {

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

#endif /* LV2_XXX_HPP */
