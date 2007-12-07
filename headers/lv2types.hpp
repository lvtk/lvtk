/****************************************************************************
    
    lv2types.hpp - support file for writing LV2 plugins in C++
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
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

#ifndef LV2TYPES_HPP
#define LV2TYPES_HPP

namespace LV2 {
  
  /** Typedef for the LV2_Feature type so we get it into the LV2 namespace. */
  typedef LV2_Feature Feature;
  
  /** Convenient typedef for the feature handler function type. */
  typedef void(*FeatureHandler)(void*, void*);
  
  /** Convenient typedef for the feature handler map type. */
  typedef std::map<std::string, FeatureHandler> FeatureHandlerMap;
  
  

  /** @internal
      This template class is used to terminate the recursive inheritance trees
      created by InheritanceTree. */
  template <class A, bool B>
  struct End {
    
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
	    template <class, bool> class E1 = End, bool B1 = false,
	    template <class, bool> class E2 = End, bool B2 = false,
	    template <class, bool> class E3 = End, bool B3 = false,
	    template <class, bool> class E4 = End, bool B4 = false,
	    template <class, bool> class E5 = End, bool B5 = false,
	    template <class, bool> class E6 = End, bool B6 = false,
	    template <class, bool> class E7 = End, bool B7 = false,
	    template <class, bool> class E8 = End, bool B8 = false,
	    template <class, bool> class E9 = End, bool B9 = false>
  struct InheritanceTree 
    : E1<A, B1>, InheritanceTree<A, E2, B2, E3, B3, E4, B4, E5, B5, 
				 E6, B6, E7, B7, E8, B8, E9, B9> {
    
    typedef InheritanceTree<A, E2, B2, E3, B3, E4, B4, E5, B5, 
			    E6, B6, E7, B7, E8, B8, E9, B9> Parent;
    
    /** @internal
	Add feature handlers to @c hmap for the feature URIs. */
    static void map_feature_handlers(FeatureHandlerMap& hmap) {
      E1<A, B1>::map_feature_handlers(hmap);
      Parent::map_feature_handlers(hmap);
    }
    
    /** Check if the features are OK with the plugin initialisation. */
    bool check_ok() const { 
      return E1<A, B1>::check_ok() && Parent::check_ok();
    }
    
    /** Return any extension data. */
    static const void* extension_data(const char* uri) {
      const void* result = E1<A, B1>::extension_data(uri);
      if (result)
	return result;
      return Parent::extension_data(uri);
    }
    
  };


  /** @internal
      This is a specialisation of the inheritance tree template that terminates
      the recursion. */
  template <class A>
  struct InheritanceTree<A, 
			 End, false, End, false, End, false, 
			 End, false, End, false, End, false, 
			 End, false, End, false, End, false> {
    static void map_feature_handlers(FeatureHandlerMap& hmap) { }
    bool check_ok() const { return true; }
    static const void* extension_data(const char* uri) { return 0; }
  };

  
  /** Base class for extensions. Extension mixin classes don't have to 
      inherit from this class, but it's convenient. */
  template <bool Required>
  struct Extension {
    
    Extension() : m_ok(!Required) { }
    
    /** Default implementation does nothing - no handlers added. */
    static void map_feature_handlers(FeatureHandlerMap& hmap) { }
    
    /** Return @c true if the plugin instance is OK, @c false if it isn't. */
    bool check_ok() const { return m_ok; }
  
    /** Return a data pointer corresponding to the URI if this extension 
	has one. */
    static const void* extension_data(const char* uri) { return 0; }
  
  protected:
  
    bool m_ok;
  
  };

  
}


#endif
