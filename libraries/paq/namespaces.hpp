/****************************************************************************
    
    namespaces.hpp - Predefined URI prefixes for a RDF/Turtle parser
    
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

#ifndef NAMESPACES_HPP
#define NAMESPACES_HPP

#include <string>


namespace PAQ {


  std::string rdf(const std::string& str) {
    return std::string("<http://www.w3.org/1999/02/22-rdf-syntax-ns#") + 
      str + '>';
  }


  std::string rdfs(const std::string& str) {
    return std::string("<http://www.w3.org/2000/01/rdf-schema#") + 
      str + '>';
  }

  std::string doap(const std::string& str) {
    return std::string("<http://usefulinc.com/ns/doap#") + str + '>';
  }


  std::string foaf(const std::string& str) {
    return std::string("<http://xmlns.com/foaf/0.1/") + str + '>';
  }


}


#endif
