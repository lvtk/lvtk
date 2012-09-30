/****************************************************************************
    
    rdf.hpp - Data structures for RDF data
    
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

#ifndef RDF_HPP
#define RDF_HPP

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>


namespace PAQ {


  struct Triple;

  struct RDFTerm;

  /** A Variable is an object that can be bound to an RDF resource. */
  struct Variable {
    static Variable nil;
  };

  
  /** A Namespace is a class that works as a Turtle URI prefix. */
  class Namespace {
  public:
    
    /** Create a namespace for the prefix @c str. */
    Namespace(const std::string& str);
    
    /** Return the expanded URI for the qname @c PREFIX:str where @c PREFIX
        is the constructor parameter for this Namespace object. */
    std::string operator()(const std::string& str);

  protected:
    std::string m_prefix;
  };
  
  
  /** Compare two RDFTerm objects for equality. */
  struct RDFTermCompare {
    bool operator()(const RDFTerm* a, const RDFTerm* b);
  };

  
  /** An RDFTerm is a node or edge in an RDF graph. */
  struct RDFTerm {
    enum Type {
      URIRef,
      Literal,
      BNode,
      Variable
    };
    
    /** Create a new RDFTerm with the type @c t, the name @c n, and the
        RDF datatype @c d. */
    RDFTerm(Type t, const std::string& n = "", RDFTerm* d = 0);
    virtual ~RDFTerm();
    
    /** Return the numeric value of this term. */
    double get_numeric_value() const;
    
    Type type;
    std::string name;
    RDFTerm* datatype;
    std::string lang;
    
    typedef std::map<RDFTerm*, std::set<RDFTerm*> > tmap;
  
    tmap s_triples_;
    tmap o_triples_;
  
    size_t index;
  };

  
  /** A RDFVariable is a term type that can be used for variables in query 
      patterns. */
  struct RDFVariable : public RDFTerm {
    RDFVariable();
  };

  
  /** This class represents a single RDF triple. */
  struct Triple {
    Triple(RDFTerm* s, RDFTerm* p, RDFTerm* o);
    RDFTerm* subject;
    RDFTerm* predicate;
    RDFTerm* object;
  };

  
  /** This class holds a RDF dataset. */
  struct RDFData {
    
    /** Create a new emtpy dataset. */
    RDFData();
    
    ~RDFData();
    
    /** Add an URI reference to the dataset. */
    RDFTerm* add_uriref(const std::string& name);

    /** Add a blank node to the dataset. */
    RDFTerm* add_bnode(const std::string& name = "");

    /** Add a literal to the dataset. */
    RDFTerm* add_literal(const std::string& name, RDFTerm* datatype = 0);

    /** Add an unbound variable node to the dataset. */
    RDFTerm* add_variable(PAQ::Variable* var);

    /** Add a new triple to the dataset. */
    void add_triple(RDFTerm* subject, RDFTerm* predicate, RDFTerm* object);
  
    const std::vector<RDFTerm*>& get_terms() const;
  
    std::map<std::string, RDFTerm*> urirefs;
    std::map<std::string, RDFTerm*> literals;
    std::map<std::string, RDFTerm*> bnodes;
    std::map<PAQ::Variable*, RDFTerm*> variables;
  
    std::vector<RDFTerm*> terms;
    std::vector<Triple*> triples;
  
    unsigned long bnode_counter;
  };


}


#endif
