/****************************************************************************
    
    query.hpp - Query engine for RDF data
    
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

#ifndef QUERY_HPP
#define QUERY_HPP

#include <map>
#include <string>
#include <vector>

#include "rdf.hpp"


namespace PAQ {

  class Pattern;
  
  /** This class contains a query result, i.e. a single matching subgraph. */
  class QueryResult {
  public:
    
    /** Return the bound node for the variable mentioned in place @c var. */
    RDFTerm* operator[](size_t var);

    /** Return the bound node for the variable @c var. */
    RDFTerm* operator[](Variable& var);
    
  protected:
    
    friend class QueryEngine;
    
    std::map<Variable*, size_t> m_variables;
    std::vector<RDFTerm*> m_data;
  };
  
  
  class Query;
  class QueryEngine;
  
  
  /** A filter is a constraint on a bound node. If a candidate for binding
      does not match the constraint that particular result is discarded. 
      All member functions are for internal use only. */
  class Filter {
  public:
    virtual ~Filter() { }
    virtual void set_variable_indices(Query& query) = 0;
    virtual bool execute(QueryEngine& engine) = 0;
  };
  
  
  /** A filter that matches the language of a string literal against a 
      language code. */
  class LangMatches : public Filter {
  public:
    LangMatches(Variable& var, const std::string& lang);
    void set_variable_indices(Query& query);
    bool execute(QueryEngine& engine);
  protected:
    Variable* m_var;
    size_t m_index;
    std::string m_lang;
  };

  /** Creates a filter that matches the language of a string literal against a 
      language code. */
  Filter* lang_matches(Variable& var, const std::string& lang);
  
  
  /** A filter that matches the values of two bound nodes. */
  class TermEquality : public Filter {
  public:
    TermEquality(Variable& var1, Variable& var2);
    void set_variable_indices(Query& query);
    bool execute(QueryEngine& engine);
  protected:
    Variable* m_var1;
    Variable* m_var2;
    size_t m_index1;
    size_t m_index2;
  };

  /** Creates a filter that matches the values of two bound nodes. */
  Filter* operator==(Variable& var1, Variable& var2);
  
  
  /** Abstract base class for all unary filters. */
  class UnaryFilter : public Filter {
  public:
    UnaryFilter(Variable& var);
    virtual void set_variable_indices(Query& query);
  protected:
    Variable* m_var;
    size_t m_index;
  };
  
  
  /** A filter that checks if a bound node is an URI reference. */
  class IsURI : public UnaryFilter {
  public:
    IsURI(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if a bound node is an URI reference. */
  Filter* is_uriref(Variable& var);
  
  
  /** A filter that checks if a bound node is a literal. */
  class IsLiteral : public UnaryFilter {
  public:
    IsLiteral(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if a bound node is a literal. */
  Filter* is_literal(Variable& var);
  
  
  /** A filter that checks if a bound node is a numeric literal. */
  class IsNumeric : public UnaryFilter {
  public:
    IsNumeric(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if a bound node is a numeric literal. */
  Filter* is_numeric(Variable& var);
  
  
  /** A filter that checks if a bound node is an integer literal. */
  class IsInteger : public UnaryFilter {
  public:
    IsInteger(Variable& var) : UnaryFilter(var) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if a bound node is an integer literal. */
  Filter* is_integer(Variable& var);
  
  
  /** A filter that checks for the inverse of another filter. */
  class Negation : public Filter {
  public:
    Negation(Filter* f);
    ~Negation();
    void set_variable_indices(Query& q);
    bool execute(QueryEngine& engine);
  protected:
    Filter* m_filter;
  };

  /** Creates a filter that checks for the inverse of another filter. */
  Filter* no(Filter* filter);
  
  
  /** A filter that is built from two other filters. */
  class Aggregate : public Filter {
  public:
    Aggregate(Filter* f1, Filter* f2);
    ~Aggregate();
    void set_variable_indices(Query& q);
  protected:
    Filter* m_f1;
    Filter* m_f2;
  };

  
  /** A filter that checks if one filter OR another is true. */
  class Or : public Aggregate {
  public:
    Or(Filter* f1, Filter* f2);
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if at least one of two other 
      filters is true. */
  Filter* or_filter(Filter* f1, Filter* f2);
  
  
  /** A filter that checks if one filter AND another is true. */
  class And : public Aggregate {
  public:
    And(Filter* f1, Filter* f2);
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if two other filters is true. */
  Filter* and_filter(Filter* f1, Filter* f2);
  
  
  /** A filter that compares the numeric value of a node to a constant. */
  class NumericCompare : public Filter {
  public:
    NumericCompare(Variable& var, double value);
    virtual void set_variable_indices(Query& q);
  protected:
    Variable* m_var;
    double m_value;
    size_t m_index;
  };
  
  
  /** A filter that checks if the node's value is less than a constant. */
  class NumericLess : public NumericCompare {
  public:
    NumericLess(Variable& var, double value) : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if the node's value is less 
      than a constant. */
  Filter* operator<(Variable& var, double value);

  /** Creates a filter that checks if the node's value is less 
      than a constant. */
  Filter* operator>=(double value, Variable& var);
  
  
  /** A filter that checks if the node's value is less than or equal to
      a constant. */
  class NumericLessEqual : public NumericCompare {
  public:
    NumericLessEqual(Variable& var, double value) : NumericCompare(var, value) {}
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if the node's value is less than or equal to
      a constant. */
  Filter* operator<=(Variable& var, double value);

  /** Creates a filter that checks if the node's value is less than or equal to
      a constant. */
  Filter* operator>(double value, Variable& var);
  
  
  /** A filter that checks if the node's value is greater than a constant. */
  class NumericGreater : public NumericCompare {
  public:
    NumericGreater(Variable& var, double value) : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if the node's value is greater than
      a constant. */
  Filter* operator>(Variable& var, double value);

  /** Creates a filter that checks if the node's value is greater than 
      a constant. */
  Filter* operator<=(double value, Variable& var);
  

  /** A filter that checks if the node's value is greater than or equal to
      a constant. */
  class NumericGreaterEqual : public NumericCompare {
  public:
    NumericGreaterEqual(Variable& var, double value) 
      : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if the node's value is greater than 
      or equal to a constant. */
  Filter* operator>=(Variable& var, double value);

  /** Creates a filter that checks if the node's value is greater than 
      or equal to a constant. */
  Filter* operator<(double value, Variable& var);
  
  
  /** A filter that checks if the node's value is equal to a numeric 
      constant. */
  class NumericEquality : public NumericCompare {
  public:
    NumericEquality(Variable& var, double value) : NumericCompare(var, value) { }
    bool execute(QueryEngine& engine);
  };

  /** Creates a filter that checks if the node's value is equal to a constant.*/
  Filter* operator==(Variable& var, double value);

  /** Creates a filter that checks if the node's value is equal to a constant.*/
  Filter* operator==(double value, Variable& var);

  /** Creates a filter that checks if the node's value is different from
      a constant. */
  Filter* operator!=(Variable& var, double value);

  /** Creates a filter that checks if the node's value is different from
      a constant. */
  Filter* operator!=(double value, Variable& var);
  

  /** A filter that checks if the node's value is equal to a string 
      constant. */
  class StringEquality : public Filter {
  public:
    StringEquality(Variable& var, const std::string& value);
    virtual void set_variable_indices(Query& q);
    bool execute(QueryEngine& engine);
  protected:
    Variable* m_var;
    std::string m_value;
    size_t m_index;
  };
  
  /** Creates a filter that checks if the node's value is equal to a constant.*/
  Filter* operator==(Variable& var, const std::string& value);

  /** Creates a filter that checks if the node's value is equal to a constant.*/
  Filter* operator==(const std::string& value, Variable& var);

  /** Creates a filter that checks if the node's value is different from
      a constant. */
  Filter* operator!=(Variable& var, const std::string& value);

  /** Creates a filter that checks if the node's value is different from
      a constant. */
  Filter* operator!=(const std::string& value, Variable& var);  

  
  /** Abstract base class for all binary filters. */
  class BinaryFilter : public Filter {
  public:
    BinaryFilter(Variable& var1, Variable& var2);
    virtual void set_variable_indices(Query& query);
  protected:
    Variable* m_var1;
    Variable* m_var2;
    size_t m_index1;
    size_t m_index2;
  };
  
  
  /** A class that holds queries. A query can be executed many times on
      different data sets. */
  class Query {
  public:
    
    Query();
    
    /** Add a new triple pattern to the query. */
    Query& where(Variable& s, Variable& p, Variable& o);
    Query& where(const std::string& s, Variable& p, Variable& o);
    Query& where(Variable& s, const std::string& p, Variable& o);
    Query& where(Variable& s, Variable& p, const std::string& o);
    Query& where(Variable& s, const std::string& p, const std::string& o);
    Query& where(const std::string& s, Variable& p, const std::string& o);
    Query& where(const std::string& s, const std::string& p, Variable& o);
    Query& where(const std::string& s, const std::string& p,
                 const std::string& o);
    
    /** Add a filter to the query. */
    Query& filter(Filter* f); 
    
    /** Run the query on a dataset. */
    std::vector<QueryResult> run(RDFData& data);

    RDFData data;
    
  protected:
    
    friend class QueryEngine;
    
    friend Query select(Variable& var1, Variable& var2, 
                        Variable& var3, Variable& var4);
    
    Query(Variable& var1, Variable& var2, Variable& var3, Variable& var4);
    RDFTerm* add_term(const std::string& str);
    
    std::vector<Filter*> filters;
    
    std::map<Variable*, size_t> selected;
  };
  
  
  /** Create a new query with some selected variables. */
  Query select(Variable& var1, Variable& var2 = Variable::nil, 
               Variable& var3 = Variable::nil, Variable& var4 = Variable::nil);
  
  
  /** This is the class that does all the subgraph matching work. It is for
      internal use only. */
  class QueryEngine {
  public:
    
    
    QueryEngine(Query& q, RDFData& d);
    
    std::vector<QueryResult> run();
    
    std::vector<size_t> mapping;
    const std::vector<RDFTerm*>& N1;
    const std::vector<RDFTerm*>& N2;
    const std::vector<Triple*>& T1;
    const std::vector<Triple*>& T2;
    
  protected:
    
    const Query& query;
    
    void match(size_t depth);
    bool match_node(RDFTerm* p, RDFTerm* t, size_t& bound);
    
    size_t NULL_NODE;

    std::vector<QueryResult> result;
    
  };
  
}


#endif
