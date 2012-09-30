/****************************************************************************
    
    rdf.cpp - Data structures for RDF data
    
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

#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "rdf.hpp"


namespace PAQ {

  Variable Variable::nil;


  bool RDFTermCompare::operator()(const RDFTerm* a, const RDFTerm* b) {
    if (a == b)
      return false;
    return (a->type < b->type) ||
      ((a->type == b->type) && ((a->name < b->name) ||
                                ((a->name == b->name) && 
                                 (a->datatype < b->datatype))));
  }

  Namespace::Namespace(const std::string& str) 
    : m_prefix(str.substr(0, str.size() - 1)) {
  
  }


  std::string Namespace::operator()(const std::string& str) {
    return m_prefix + str + '>';
  }
  
  
  RDFTerm::RDFTerm(Type t, const std::string& n, RDFTerm* d)
    : type(t), 
      name(n), 
      datatype(d) {
  
  }


  RDFTerm::~RDFTerm() { 

  }
  
  
  double RDFTerm::get_numeric_value() const {
    return std::atof(name.c_str());
  }


  RDFVariable::RDFVariable() : RDFTerm(RDFTerm::Variable) { 

  }


  Triple::Triple(RDFTerm* s, RDFTerm* p, RDFTerm* o)
    : subject(s),
      predicate(p),
      object(o) {

  }

  
  RDFData::RDFData() 
    : bnode_counter(0) { 

  }


  RDFData::~RDFData() {
    for (unsigned i = 0; i < terms.size(); ++i)
      delete terms[i];
    for (unsigned i = 0; i < triples.size(); ++i)
      delete triples[i];
  }


  RDFTerm* RDFData::add_uriref(const std::string& name) {
    std::map<std::string, RDFTerm*>::const_iterator iter = urirefs.find(name);
    if (iter == urirefs.end()) {
      RDFTerm* term = new RDFTerm(RDFTerm::URIRef, name);
      iter = urirefs.insert(make_pair(name, term)).first;
      term->index = terms.size();
      terms.push_back(term);
    }
    return iter->second;
  }
  

  RDFTerm* RDFData::add_bnode(const std::string& name) {
    std::string realname;
    if (name == "") {
      std::ostringstream oss;
      oss<<"__:"<<(++bnode_counter);
      realname = oss.str();
    }
    else
      realname = name;
    std::map<std::string, RDFTerm*>::const_iterator iter = 
      bnodes.find(realname);
    if (iter == bnodes.end()) {
      RDFTerm* term = new RDFTerm(RDFTerm::BNode, realname);
      iter = bnodes.insert(make_pair(realname, term)).first;
      term->index = terms.size();
      terms.push_back(term);
    }
    return iter->second;
  }


  RDFTerm* RDFData::add_literal(const std::string& name, RDFTerm* datatype) {
    std::string realname = name;
    if (datatype) {
      realname += "^^";
      realname += datatype->name;
    }
    std::map<std::string, RDFTerm*>::const_iterator iter = 
      literals.find(realname);
    if (iter == literals.end()) {
      RDFTerm* term = new RDFTerm(RDFTerm::Literal, name, datatype);
      iter = literals.insert(make_pair(realname, term)).first;
      term->index = terms.size();
      terms.push_back(term);
    }
    return iter->second;
  }


  RDFTerm* RDFData::add_variable(PAQ::Variable* var) {
    std::map<PAQ::Variable*, RDFTerm*>::const_iterator iter = 
      variables.find(var);
    if (iter == variables.end()) {
      RDFTerm* term = new RDFVariable();
      iter = variables.insert(std::make_pair(var, term)).first;
      term->index = terms.size();
      terms.push_back(term);
    }
    return iter->second;
  }


  void RDFData::add_triple(RDFTerm* subject, RDFTerm* predicate, RDFTerm* object) {
    triples.push_back(new Triple(subject, predicate, object));
    subject->s_triples_[object].insert(predicate);
    object->o_triples_[subject].insert(predicate);
  }
  
 
  const std::vector<RDFTerm*>& RDFData::get_terms() const {
    return terms;
  }
  

}

