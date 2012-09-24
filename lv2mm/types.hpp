/****************************************************************************
    
    types.hpp - support file for writing LV2 plugins in C++
    
    Copyright (C) 2006-2008 Lars Luthman <lars.luthman@gmail.com>

    The first version of the URIMap class was written by Dave Robillard.
    It has since been modified.

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

****************************************************************************/

#ifndef LV2_TYPES_HPP
#define LV2_TYPES_HPP

#include <map>
#include <string>

#include "private/debug.hpp"

/** TODO: Put macros somewhere else that make sense */
#define LV2MM_PLUGIN_CLASS     Plugin<D, Ext1, Ext2, Ext3, Ext4, Ext5, Ext6, Ext7, Ext8, Ext9>
#define LV2MM_MIXIN_CLASS      template <bool Required = true> struct
#define LV2MM_MIXIN_DERIVED    template <class Derived> struct I : Extension<Required>

namespace LV2 {
   /** Convenience typedef */
   typedef LV2_Handle Handle;
}

#include <lv2mm/feature.hpp>

namespace LV2 {

   /** @internal This class is used by the End class */
   struct Empty {};
  
   /** @internal
      This class is used to terminate the recursive inheritance trees
      created by MixinTree. */
   struct End {
    typedef Empty C;
   };
  

  /** @internal
      This template class creates an inheritance tree of extension templates
      from a parameter list. It is inherited by the Plugin class to make
      it possible to add overridable extension functions to the class.
      The first template parameter will be used as the first template
      parameter of @c E1, and also be passed as the first parameter of the
      next level of the inheritance tree. Each @c bool parameter will be used
      as the second parameter to the template directly preceding it. */
  template <class A,
	    class E1 = End, 
	    class E2 = End, 
	    class E3 = End, 
	    class E4 = End, 
	    class E5 = End, 
	    class E6 = End, 
	    class E7 = End, 
	    class E8 = End, 
	    class E9 = End>
  struct MixinTree 
    : E1::template I<A>, MixinTree<A, E2, E3, E4, E5, E6, E7, E8, E9> {
    
    typedef MixinTree<A, E2, E3, E4, E5, E6, E7, E8, E9> Parent;
    
    /** @internal
	Add feature handlers to @c hmap for the feature URIs. */
    static void
    map_feature_handlers(FeatureHandlerMap& hmap) {
      E1::template I<A>::map_feature_handlers(hmap);
      Parent::map_feature_handlers(hmap);
    }
    
    /** Check if the features are OK with the plugin initialisation. */
    bool
    check_ok() {
      return E1::template I<A>::check_ok() && Parent::check_ok();
    }
    
    /** Return any extension data. */
    static const void*
    extension_data(const char* uri) {
      const void* result = E1::template I<A>::extension_data(uri);
      if (result)
	return result;
      return Parent::extension_data(uri);
    }
    
  };


  /** @internal
      This is a specialisation of the inheritance tree template that terminates
      the recursion. */
  template <class A>
  struct MixinTree<A, End, End, End, End, End, End, End, End, End> {
    static void map_feature_handlers(FeatureHandlerMap& hmap) { }
    bool check_ok() const { return true; }
    static const void* extension_data(const char* uri) { return 0; }
  };


  /** @internal
      Base class for extensions. Extension mixin classes don't have to 
      inherit from this class, but it's convenient.
  */
  template <bool Required>
  struct Extension {
    
    /** @internal 
     */
    Extension() : m_ok(!Required) { }
    
    /** @internal
	Default implementation does nothing - no handlers added. 
    */
    static void map_feature_handlers(FeatureHandlerMap& hmap) { }
    
    /** @internal
	Return @c true if the plugin instance is OK, @c false if it isn't. 
    */
    bool check_ok() { return m_ok; }
  
    /** @internal
	Return a data pointer corresponding to the URI if this extension 
	has one. 
    */
    static const void* extension_data(const char* uri) { return 0; }
  
  protected:
  
    bool m_ok;
  
  };

}

#endif
