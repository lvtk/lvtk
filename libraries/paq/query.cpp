/****************************************************************************
    
    query.cpp - Query engine for RDF data
    
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

#include <algorithm>

#include "query.hpp"


using namespace std;


namespace PAQ {
  

  RDFTerm* QueryResult::operator[](size_t i) {
    return m_data[i];
  }


  RDFTerm* QueryResult::operator[](Variable& var) {
    return m_data[m_variables[&var]];
  }
  
  
  LangMatches::LangMatches(Variable& var, const std::string& lang)
    : m_var(&var),
      m_lang(lang) {

  }
  
  
  void LangMatches::set_variable_indices(Query& query) {
    RDFTerm* t = query.data.add_variable(m_var);
    m_index = t->index;
    m_var = 0;
  }  
  
  bool LangMatches::execute(QueryEngine& engine) {
    RDFTerm* t = engine.N1[engine.mapping[m_index]];
    return (t->lang == m_lang);
  }
  
  
  Filter* lang_matches(Variable& var, const std::string& lang) {
    return new LangMatches(var, lang);
  }


  TermEquality::TermEquality(Variable& var1, Variable& var2)
    : m_var1(&var1),
      m_var2(&var2) {
    
  }
  
  
  void TermEquality::set_variable_indices(Query& query) {
    RDFTerm* t = query.data.add_variable(m_var1);
    m_index1 = t->index;
    m_var1 = 0;
    t = query.data.add_variable(m_var2);
    m_index2 = t->index;
    m_var2 = 0;
  }
  
  
  bool TermEquality::execute(QueryEngine& engine) {
    return (engine.mapping[m_index1] == engine.mapping[m_index2]);
  }
  

  Filter* operator==(Variable& var1, Variable& var2) {
    return new TermEquality(var1, var2);
  }


  UnaryFilter::UnaryFilter(Variable& var) 
    : m_var(&var) {

  }
  
  
  void UnaryFilter::set_variable_indices(Query& query) {
    RDFTerm* t = query.data.add_variable(m_var);
    m_index = t->index;
    m_var = 0;
  }


  bool IsURI::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->type == RDFTerm::URIRef);
  }

  
  Filter* is_uriref(Variable& var) {
    return new IsURI(var);
  }
  
  
  bool IsLiteral::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->type == RDFTerm::Literal);
  }

  
  Filter* is_literal(Variable& var) {
    return new IsLiteral(var);
  }


  bool IsNumeric::execute(QueryEngine& engine) {
    RDFTerm* t = engine.N1[engine.mapping[m_index]];
    if (t->type == RDFTerm::Literal &&
        t->datatype &&
        (t->datatype->name == "<http://www.w3.org/2001/XMLSchema#integer>" ||
         t->datatype->name == "<http://www.w3.org/2001/XMLSchema#decimal>" ||
         t->datatype->name == "<http://www.w3.org/2001/XMLSchema#double>"))
      return true;
    return false;
  }

  
  Filter* is_numeric(Variable& var) {
    return new IsNumeric(var);
  }


  bool IsInteger::execute(QueryEngine& engine) {
    RDFTerm* t = engine.N1[engine.mapping[m_index]];
    if (t->type == RDFTerm::Literal &&
        t->datatype &&
        t->datatype->name == "<http://www.w3.org/2001/XMLSchema#integer>")
      return true;
    return false;
  }

  
  Filter* is_integer(Variable& var) {
    return new IsInteger(var);
  }


  Negation::Negation(Filter* f) 
    : m_filter(f) { 
  
  }
  
  
  Negation::~Negation() { 
    delete m_filter; 
  }
  
  
  void Negation::set_variable_indices(Query& q) { 
    m_filter->set_variable_indices(q); 
  }
  
  
  bool Negation::execute(QueryEngine& engine) { 
    return !m_filter->execute(engine); 
  }


  Filter* no(Filter* filter) { 
    return new Negation(filter); 
  }
  
  
  NumericCompare::NumericCompare(Variable& var, double value) 
    : m_var(&var), 
      m_value(value) { 
  
  }
  
  
  void NumericCompare::set_variable_indices(Query& query) {
    RDFTerm* t = query.data.add_variable(m_var);
    m_index = t->index;
    m_var = 0;
  }

  
  bool NumericLess::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->get_numeric_value() < m_value);
  }


  Filter* operator<(Variable& var, double value) {
    return new NumericLess(var, value);
  }


  Filter* operator>=(double value, Variable& var) {
    return new NumericLess(var, value);
  }


  bool NumericLessEqual::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->get_numeric_value() <= m_value);
  }


  Filter* operator<=(Variable& var, double value) {
    return new NumericLessEqual(var, value);
  }


  Filter* operator>(double value, Variable& var) {
    return new NumericLessEqual(var, value);
  }


  bool NumericGreater::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->get_numeric_value() > m_value);
  }

  
  Filter* operator>(Variable& var, double value) {
    return new NumericGreater(var, value);
  }
  
  
  Filter* operator<=(double value, Variable& var) {
    return new NumericGreater(var, value);
  }
  
  
  bool NumericGreaterEqual::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->get_numeric_value() >= m_value);
  }

  
  Filter* operator>=(Variable& var, double value) {
    return new NumericGreaterEqual(var, value);
  }
  
  
  Filter* operator<(double value, Variable& var) {
    return new NumericGreaterEqual(var, value);
  }
  
  
  bool NumericEquality::execute(QueryEngine& engine) {
    return (engine.N1[engine.mapping[m_index]]->get_numeric_value() == m_value);
  }


  Filter* operator==(Variable& var, double value) {
    return new NumericEquality(var, value);
  }


  Filter* operator==(double value, Variable& var) {
    return new NumericEquality(var, value);
  }


  Filter* operator!=(Variable& var, double value) {
    return no(new NumericEquality(var, value));
  }


  Filter* operator!=(double value, Variable& var) {
    return no(new NumericEquality(var, value));
  }
  
  
  BinaryFilter::BinaryFilter(Variable& var1, Variable& var2) 
    : m_var1(&var1),
      m_var2(&var2) {

  }
  
  
  void BinaryFilter::set_variable_indices(Query& query) {
    RDFTerm* t = query.data.add_variable(m_var1);
    m_index1 = t->index;
    m_var1 = 0;
    t = query.data.add_variable(m_var2);
    m_index2 = t->index;
    m_var2 = 0;
  }


  Query::Query() {

  }
  
  
  Query& Query::where(Variable& s, Variable& p, Variable& o) {
    data.add_triple(data.add_variable(&s), 
                    data.add_variable(&p),
                    data.add_variable(&o));
    return *this;
  }


  Query& Query::where(const std::string& s, Variable& p, Variable& o) {
    data.add_triple(add_term(s),
                    data.add_variable(&p),
                    data.add_variable(&o));
    return *this;
  }


  Query& Query::where(Variable& s, const std::string& p, Variable& o) {
    data.add_triple(data.add_variable(&s),
                    add_term(p),
                    data.add_variable(&o));
    return *this;
  }


  Query& Query::where(Variable& s, Variable& p, const std::string& o) {
    data.add_triple(data.add_variable(&s),
                    data.add_variable(&p), 
                    add_term(o));
    return *this;
  }


  Query& Query::where(const std::string& s, const std::string& p, Variable& o) {
    data.add_triple(add_term(s), 
                    add_term(p), 
                    data.add_variable(&o));
    return *this;
  }


  Query& Query::where(Variable& s, const std::string& p, const std::string& o) {
    data.add_triple(data.add_variable(&s), 
                    add_term(p), 
                    add_term(o));
    return *this;
  }


  Query& Query::where(const std::string& s, Variable& p, const std::string& o) {
    data.add_triple(add_term(s), 
                    data.add_variable(&p), 
                    add_term(o));
    return *this;
  }


  Query& Query::where(const std::string& s,const std::string& p,
                      const std::string& o) {
    data.add_triple(add_term(s), 
                    add_term(p),
                    add_term(o));
    return *this;
  }


  Query& Query::filter(Filter* f) {
    f->set_variable_indices(*this);
    filters.push_back(f);
    return *this;
  }
  
  
  vector<QueryResult> Query::run(RDFData& data) {
    QueryEngine qe(*this, data);
    return qe.run();
  }


  Query::Query(Variable& var1, Variable& var2, Variable& var3, Variable& var4) {
    if (&var1 != &Variable::nil) {
      RDFTerm* var = data.add_variable(&var1);
      selected[&var1] = var->index;
      if (&var2 != &Variable::nil) {
        var = data.add_variable(&var2);
        selected[&var2] = var->index;
        if (&var3 != &Variable::nil) {
          var = data.add_variable(&var3);
          selected[&var3] = var->index;
          if (&var4 != &Variable::nil) {
            var = data.add_variable(&var4);
            selected[&var4] = var->index;
          }
        }
      }
    }
  }


  RDFTerm* Query::add_term(const std::string& str) {
    if (str.size() > 0 && str[0] == '<')
      return data.add_uriref(str);
    if (str.substr(0, 3) == "__:")
      return data.add_bnode(str);
    return data.add_literal(str);
  }


  Query select(Variable& var1, Variable& var2, Variable& var3, Variable& var4) {
    return Query(var1, var2, var3, var4);
  }
  
  
  QueryEngine::QueryEngine(Query& q, RDFData& d) 
    : N1(d.get_terms()),
      N2(q.data.get_terms()),
      T1(d.triples),
      T2(q.data.triples),
      query(q) {
    
    NULL_NODE = N1.size();
    NULL_NODE = N2.size() > NULL_NODE ? N2.size() : NULL_NODE;
    
    mapping.resize(N2.size(), NULL_NODE);
  }
  
  
  std::vector<QueryResult> QueryEngine::run() {
    match(0);
    return result;
  }
  
  
  void QueryEngine::match(size_t depth) {
    
    //cerr<<endl<<"depth = "<<depth<<endl<<endl;
    
    // found a match, check it and add it to the result vector
    if (depth == T2.size()) {
      
      for (size_t i = 0; i < query.filters.size(); ++i) {
        if (!query.filters[i]->execute(*this))
          return;
      }
      
      //cerr<<"***** ***** ****** ***** ***** ***** ***** ***** *****"<<endl;
      //cerr<<"***** ***** RESULT ***** ***** ***** ***** ***** *****"<<endl;
      //cerr<<"***** ***** ****** ***** ***** ***** ***** ***** *****"<<endl;
      
      QueryResult qr;
      for (size_t i = 0; i < query.selected.size(); ++i) {
        qr.m_data.push_back(N1[mapping[i]]);
        /*
          cerr<<"   "<<N2[i]->name<<" ("<<i<<")"<<endl
              <<"      -> "<<N1[mapping[i]]->name<<" ("<<mapping[i]<<")"<<endl;
        */
      }
      qr.m_variables = query.selected;
      //cerr<<"size: "<<result.size()<<endl;
      result.push_back(qr);
      return;
    }
    
    // iterate over all triples in the target to see if any of them
    // match the current triplet in the pattern
    size_t b1, b2, b3;
    for (size_t i = 0; i < T1.size(); ++i) {
      
      /*
      cerr<<endl<<"Trying to match "<<endl<<"("
          <<T2[depth]->subject->name<<" ("<<T2[depth]->subject->index<<"),"<<endl
          <<T2[depth]->predicate->name<<" ("<<T2[depth]->predicate->index<<"),"<<endl
          <<T2[depth]->object->name<<" ("<<T2[depth]->object->index<<")"<<") "
          <<endl<<"to "<<endl<<"("
          <<T1[i]->subject->name<<" ("<<T1[i]->subject->index<<"),"<<endl
          <<T1[i]->predicate->name<<" ("<<T1[i]->predicate->index<<"),"<<endl
          <<T1[i]->object->name<<" ("<<T1[i]->object->index<<")"<<") "<<endl<<endl;
      */
      b1 = NULL_NODE;
      b2 = NULL_NODE;
      b3 = NULL_NODE;
      
      if (match_node(T2[depth]->subject, T1[i]->subject, b1)) {
        if (match_node(T2[depth]->predicate, T1[i]->predicate, b2)) {
          if (match_node(T2[depth]->object, T1[i]->object, b3)) {
            match(depth + 1);
          }
        }
      }
      
      if (b1 != NULL_NODE)
        mapping[b1] = NULL_NODE;
      if (b2 != NULL_NODE)
        mapping[b2] = NULL_NODE;
      if (b3 != NULL_NODE)
        mapping[b3] = NULL_NODE;
      
    }
    
  }
  
  
  bool QueryEngine::match_node(RDFTerm* p, RDFTerm* t, size_t& bound) {
    
    // if the pattern node is mapped already, just check if it's mapped to
    // the target node
    if (mapping[p->index] == t->index) {
      //cerr<<p->name<<" ("<<p->index<<") is already mapped to "<<t->name<<endl;
      return true;
    }
    
    // if it's unmapped and is a variable, map it to the target node
    // without checking anything
    if (p->type == RDFTerm::Variable && mapping[p->index] == NULL_NODE) {
      //cerr<<"mapping variable "<<p->index<<" to "<<t->name<<endl;
      mapping[p->index] = t->index;
      bound = p->index;
      return true;
    }
    
    // map the pattern node to the target node if they are equivalent
    if ((p->type == t->type) && (p->name == t->name)) {
      //cerr<<"mapping "<<p->name<<" ("<<p->index<<") to "<<t->name<<endl;
      mapping[p->index] = t->index;
      bound = p->index;
      return true;
    }
    
    //cerr<<"no match"<<endl;
    return false;
  }

  
}
