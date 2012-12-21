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
/** @headerfile lvtk/types.hpp */

#ifndef LVTK_LV2_TYPES_HPP
#define LVTK_LV2_TYPES_HPP

#include <map>
#include <string>

#include <lvtk/feature.hpp>

#include "debug.hpp"

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

/** @namespace lvtk */
namespace lvtk {
   /** Convenience typedefs */
   typedef LV2_Handle Handle;

   namespace ui {
       typedef LV2UI_Handle Handle;
   }
}


namespace lvtk {

   /** @internal This class is used by the end class */
   struct empty {};
  
   /** @internal
      This class is used to terminate the recursive inheritance trees
      created by MixinTree. */
   struct end {
    typedef empty C;
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
	    class E1 = end,
	    class E2 = end,
	    class E3 = end,
	    class E4 = end,
	    class E5 = end,
	    class E6 = end,
	    class E7 = end,
	    class E8 = end,
	    class E9 = end>
  struct MixinTree
    : E1::template I<A>, MixinTree<A, E2, E3, E4, E5, E6, E7, E8, E9>
  {
    
    typedef MixinTree<A, E2, E3, E4, E5, E6, E7, E8, E9> Parent;
    
    /** @internal
	Add feature handlers to @c hmap for the feature URIs. */
    static void
    map_feature_handlers(feature_handler_map& hmap)
    {
      E1::template I<A>::map_feature_handlers(hmap);
      Parent::map_feature_handlers(hmap);
    }
    
    /** Check if the features are OK with the plugin initialisation. */
    bool
    check_ok()
    {
      return E1::template I<A>::check_ok() && Parent::check_ok();
    }
    
    /** Return any extension data. */
    static const void*
    extension_data(const char* uri)
    {
		const void* result = E1::template I<A>::extension_data (uri);
		if (result)
		{
			return result;
		}
		return Parent::extension_data (uri);
    }
    
  };


  /** @internal
      This is a specialization of the inheritance tree template that terminates
      the recursion. */
  template <class A>
  struct MixinTree<A, end, end, end, end, end, end, end, end, end> {
    static void map_feature_handlers(feature_handler_map& hmap) { }
    bool check_ok() const { return true; }
    static const void* extension_data(const char* uri) { return 0; }
  };


  /** @internal
      Base class for extensions. extension mixin classes don't have to
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
    static void
    map_feature_handlers(feature_handler_map& hmap) { }
    
    /** @internal
	Return @c true if the plugin instance is OK, @c false if it isn't. 
    */
    bool check_ok() { return m_ok; }
  
    /** @internal
	Return a data pointer corresponding to the URI if this extension 
	has one. 
    */
    static const void*
    extension_data(const char* uri) { return 0; }
  
  protected:
  
    bool m_ok;
  
  };


  template <bool Required = true>
  struct Behavior : Extension<Required>
  {

  };

}

#endif
