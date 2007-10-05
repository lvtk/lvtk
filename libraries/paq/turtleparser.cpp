/****************************************************************************
    
    turtleparser.cpp - A RDF/Turtle parser
    
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

#include <sstream>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <boost/spirit/dynamic.hpp>

#include "turtleparser.hpp"


namespace PAQ {

  
  std::string encode_utf8(const std::string& str) {
    unsigned int codepoint;
    istringstream iss(str);
    iss>>hex>>codepoint;
    string result;
    
    if (codepoint <= 0x7F)
      result += (unsigned char)codepoint;
    else if (codepoint <= 0x7FF) {
      result += (unsigned char)(192 + (codepoint >> 6));
      result += (unsigned char)(128 + (codepoint & 63));
    }
    else if (codepoint <= 0xFFFF) {
      result += (unsigned char)(224 + (codepoint >> 12));
      result += (unsigned char)(128 + ((codepoint >> 6) & 63));
      result += (unsigned char)(128 + (codepoint & 63));
    }
    else if (codepoint <= 0x10FFFF) {
      result += (unsigned char)(240 + (codepoint >> 18));
      result += (unsigned char)(128 + ((codepoint >> 12) & 63));
      result += (unsigned char)(128 + ((codepoint >> 6) & 63));
      result += (unsigned char)(128 + (codepoint & 63));
    }
    
    return result;
  }



  TurtleParser::TurtleParser() {
    m_rule_map[parser_id(&turtleDoc)] = "turtleDoc";
    m_rule_map[parser_id(&statement)] = "statement";
    m_rule_map[parser_id(&directive)] = "directive";
    m_rule_map[parser_id(&triples)] = "triples";
    m_rule_map[parser_id(&predicateObjectList)] = "predicateObjectList";
    m_rule_map[parser_id(&objectList)] = "objectList";
    m_rule_map[parser_id(&verb)] = "verb";
    m_rule_map[parser_id(&comment)] = "comment";
    m_rule_map[parser_id(&subject)] = "subject";
    m_rule_map[parser_id(&predicate)] = "predicate";
    m_rule_map[parser_id(&object)] = "object";
    m_rule_map[parser_id(&literal)] = "literal";
    m_rule_map[parser_id(&datatypeString)] = "datatypeString";
    m_rule_map[parser_id(&integer)] = "integer";
    m_rule_map[parser_id(&_double)] = "_double";
    m_rule_map[parser_id(&decimal)] = "decimal";
    m_rule_map[parser_id(&exponent)] = "exponent";
    m_rule_map[parser_id(&boolean)] = "boolean";
    m_rule_map[parser_id(&blank)] = "blank";
    m_rule_map[parser_id(&itemList)] = "itemList";
    m_rule_map[parser_id(&collection)] = "collection";
    m_rule_map[parser_id(&_ws)] = "_ws";
    m_rule_map[parser_id(&resource)] = "resource";
    m_rule_map[parser_id(&nodeID)] = "nodeID";
    m_rule_map[parser_id(&qname)] = "qname";
    m_rule_map[parser_id(&uriref)] = "uriref";
    m_rule_map[parser_id(&language)] = "language";
    m_rule_map[parser_id(&nameStartChar)] = "nameStartChar";
    m_rule_map[parser_id(&nameChar)] = "nameChar";
    m_rule_map[parser_id(&name)] = "name";
    m_rule_map[parser_id(&prefixName)] = "prefixName";
    m_rule_map[parser_id(&relativeURI)] = "relativeURI";
    m_rule_map[parser_id(&quotedString)] = "quotedString";
    m_rule_map[parser_id(&_string)] = "_string";
    m_rule_map[parser_id(&longString)] = "longString";
    m_rule_map[parser_id(&character)] = "character";
    m_rule_map[parser_id(&echaracter)] = "echaracter";
    m_rule_map[parser_id(&_hex)] = "_hex";
    m_rule_map[parser_id(&ucharacter)] = "ucharacter";
    m_rule_map[parser_id(&scharacter)] = "scharacter";
    m_rule_map[parser_id(&lcharacter)] = "lcharacter";
  

    // this EBNF grammar is taken from http://www.dajobe.org/2004/01/turtle/

    turtleDoc = *statement;
  
    statement = ((directive >> *_ws >> '.' >> *_ws) | 
                 (triples >> *_ws >> '.' >> *_ws) |
                 +_ws);

    directive = (str_p("@prefix") >> +_ws >> !prefixName >> 
                 ch_p(':') >> +_ws >> uriref);
      
    triples = subject >> +_ws >> predicateObjectList;
      
    predicateObjectList = verb >> +_ws >> objectList >> 
      *(*_ws >> ';' >> *_ws >> 
        verb >> +_ws >> objectList) >> !(*_ws >> ';');
      
    objectList = object >> *(*_ws >> ',' >> *_ws >> object);
      
    verb = predicate | ch_p('a');
      
    comment = ch_p('#') >> *(~chset<>("\x0A\x0D"));
      
    subject = resource | blank;
      
    predicate = resource;
      
    object = resource | blank | literal; 

      
    literal = ((quotedString >> !(ch_p('@') >> language) >> (eps_p - ch_p('^'))) | 
               datatypeString | _double | decimal | integer | boolean);
      
    datatypeString = quotedString >> str_p("^^") >> resource;
  
    integer = !(ch_p('-') | ch_p('+')) >> +range_p('0', '9');
  
    // hackity hack
    _double = (!(ch_p('-') | ch_p('+')) >> 
               ((+range_p('0', '9') >> ch_p('.')>>*range_p('0', '9')) |
                (ch_p('.') >> +range_p('0','9')) |
                (+range_p('0','9'))) >> exponent >>
               (eps_p - (*_ws >> (str_p("@prefix") | subject | end_p))));
             
    decimal = (!(ch_p('-') | ch_p('+')) >>
               ((+range_p('0', '9') >> ch_p('.') >> *range_p('0', '9')) |
                (ch_p('.') >> +range_p('0','9'))) >>
               (eps_p - (*_ws >> (str_p("@prefix") | subject | end_p))));
  
    exponent = ((ch_p('e') | ch_p('E')) >> !(ch_p('-') | ch_p('+')) >>
                +range_p('0','9'));
      
    boolean = str_p("true") | str_p("false");
      
    blank = (nodeID | str_p("[]") | 
             (ch_p('[') >> *_ws >> predicateObjectList >> *_ws >> ch_p(']')) |
             collection);
      
    itemList = object >> *(+_ws >> object);
      
    collection = (ch_p('(') >> *_ws >> !itemList >> 
                  *_ws >> ch_p(')'));
      
    _ws = ch_p('\x09') | ch_p('\x0A') | ch_p('\x0D') | ch_p('\x20') | comment;
      
    resource = uriref | qname;
      
    nodeID = str_p("_:") >> name;
      
    qname = (!prefixName) >> ch_p(':') >> !name;
      
    uriref = ch_p('<') >> relativeURI >> ch_p('>');
      
    language = +chset<>("a-z") >> *(ch_p('-') >> +chset<>("a-z0-9"));
      
    // XXX BUG, many more characters should be allowed here but I'm lazy
    // XXX Maybe this has been fixed now? I'm too lazy to check
    nameStartChar = (chset<>("A-Za-z_") | urange_p(0xC0, 0xD6) |
                     urange_p(0x00D8, 0x00F6) | urange_p(0x00F8, 0x02FF) |
                     urange_p(0x0370, 0x037D) | urange_p(0x037F, 0x1FFF) |
                     urange_p(0x200C, 0x200D) | urange_p(0x2070, 0x218F) |
                     urange_p(0x2C00, 0x2FEF) | urange_p(0x3001, 0xD7FF) |
                     urange_p(0xF900, 0xFDCF) | urange_p(0xFDF0, 0xFFFD) |
                     urange_p(0x10000, 0xEFFFF));
      
    nameChar = (nameStartChar | ch_p('-') | chset<>("0-9") | uch_p(0xB7) |
                urange_p(0x0300, 0x036F) | urange_p(0x203F, 0x2040));
      
    name = nameStartChar >> *nameChar;
      
    prefixName = (nameStartChar - uch_p('_')) >> *nameChar;
      
    relativeURI = *ucharacter;
    
    quotedString = longString | _string;
      
    _string = ch_p('"') >> *scharacter >> ch_p('"');
      
    longString = str_p("\"\"\"") >> *lcharacter >> str_p("\"\"\"");
  
    character = ((str_p("\\u") >> _hex >> _hex >> _hex >> _hex) |
                 (str_p("\\U") >> _hex >> _hex >> _hex >> _hex >> 
                  _hex >> _hex >> _hex >> _hex) |
                 str_p("\\\\") |
                 urange_p(0x20, 0x5B) |
                 urange_p(0x00005D, 0x10FFFF));
      
    echaracter = character | str_p("\\t") | str_p("\\n") | str_p("\\r");
      
    _hex = chset<>("0-9A-F");
      
    ucharacter = (character - ch_p('>')) | str_p("\\>");
      
    scharacter = (character - ch_p('"')) | str_p("\\\"");
  
    // is " allowed as an lcharacter?
    lcharacter = (echaracter | str_p("\\\"") |
                  uch_p(0x09) | uch_p(0x0A) | uch_p(0x0D)) - str_p("\"\"\"");

  }



  string TurtleParser::node_to_string(iter_t node) {
    string result;
    if (node->children.size() == 0) 
      copy(node->value.begin(), node->value.end(), back_inserter(result));
    else {
      for (iter_t i = node->children.begin(); i != node->children.end(); ++i)
        result += node_to_string(i);
    }
    return result;
  }



  void TurtleParser::do_directive(iter_t iter) {
    iter_t iter2;
    string prefix, uri;
    for (iter2 = iter->children.begin(); 
         iter2 != iter->children.end(); ++iter2) {
      if (iter2->value.id() == parser_id(&prefixName))
        prefix = node_to_string(iter2);
      if (iter2->value.id() == parser_id(&uriref)) {
        uri = node_to_string(iter2);
        ucharacter_escape(uri);
      }
    }
    m_prefix_map[prefix] = uri.substr(0, uri.size() - 1);
  }



  void TurtleParser::do_statement(iter_t iter) {
    iter_t child = iter->children.begin();
    if (child->value.id() == parser_id(&directive))
      do_directive(child);
    else if (child->value.id() == parser_id(&triples))
      do_triples(child);
  }



  void TurtleParser::do_triples(iter_t iter) {
    iter_t child;
    RDFTerm* sub = 0;
    multimap<RDFTerm*, RDFTerm*> polist;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&subject))
        sub = do_subject(child);
      else if (child->value.id() == parser_id(&predicateObjectList))
        polist = do_predicateObjectList(child);
    }
    assert(sub);
    multimap<RDFTerm*, RDFTerm*>::iterator i;
    for (i = polist.begin(); i != polist.end(); ++i)
      m_data->add_triple(sub, i->first, i->second);
  }



  RDFTerm* TurtleParser::do_subject(iter_t iter) {
    iter_t child = iter->children.begin();
    if (child->value.id() == parser_id(&resource))
      return do_resource(child);
    else if (child->value.id() == parser_id(&blank))
      return do_blank(child);
    assert(!"This should never happen");
  }



  RDFTerm* TurtleParser::do_resource(iter_t iter) {
    iter_t child = iter->children.begin();
    RDFTerm* result = 0;
    if (child->value.id() == parser_id(&uriref)) {
      result = do_uriref(child);
    }
    else if (child->value.id() == parser_id(&qname))
      result = do_qname(child);
    assert(result);
    return result;
  }



  RDFTerm* TurtleParser::do_blank(iter_t iter) {
    iter_t child;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&nodeID))
        return do_nodeID(child);
      else if (child->value.id() == parser_id(&predicateObjectList)) {
        RDFTerm* blank = m_data->add_bnode();
        multimap<RDFTerm*, RDFTerm*> polist = do_predicateObjectList(child);
        multimap<RDFTerm*, RDFTerm*>::iterator i;
        for (i = polist.begin(); i != polist.end(); ++i)
          m_data->add_triple(blank, i->first, i->second);
        return blank;
      }
      else if (child->value.id() == parser_id(&collection))
        return do_collection(child);
      else if (node_to_string(child) == "[]")
        return m_data->add_bnode();
    }
    assert(!"This should never happen");
    return 0;
  }



  RDFTerm* TurtleParser::do_uriref(iter_t iter) {
    iter_t second = iter->children.begin() + 1;
    if (node_to_string(second) == ">")
      return m_data->add_uriref(string("<") + absolutise("") + string(">"));
    return do_relativeURI(iter->children.begin() + 1);
  }
  
  
  RDFTerm* TurtleParser::do_relativeURI(iter_t iter) {
    string str = node_to_string(iter);
    ucharacter_escape(str);
    RDFTerm* t = m_data->add_uriref(string("<") + absolutise(str) + ">");    
    return t;
  }


  RDFTerm* TurtleParser::do_qname(iter_t iter) {
    iter_t child;
    string prefix;
    string name_s;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&prefixName))
        prefix = node_to_string(child);
      else if (child->value.id() == parser_id(&name))
        name_s = node_to_string(child);
    }
    string uri = m_prefix_map[prefix] + name_s + ">";
    return m_data->add_uriref(uri);
  }



  RDFTerm* TurtleParser::do_nodeID(iter_t iter) {
    return m_data->add_bnode(node_to_string(iter));
  }



  multimap<RDFTerm*,RDFTerm*> TurtleParser::do_predicateObjectList(iter_t iter) {
    multimap<RDFTerm*, RDFTerm*> result;
    iter_t child;
    RDFTerm* current_verb = 0;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&verb))
        current_verb = do_verb(child);
      else if (child->value.id() == parser_id(&objectList)) {
        vector<RDFTerm*> v = do_objectList(child);
        for (unsigned i = 0; i < v.size(); ++i) {
          assert(current_verb);
          result.insert(make_pair(current_verb, v[i]));
        }
      }
    }
    return result;
  }



  RDFTerm* TurtleParser::do_collection(iter_t iter) {
    RDFTerm* nil = m_data->
      add_uriref("<http://www.w3.org/1999/02/22-rdf-syntax-ns#nil>");
    RDFTerm* head = nil;
  
    iter_t child;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&itemList)) {
        vector<RDFTerm*> objects = do_itemList(child);
        head = m_data->add_bnode();
        RDFTerm* chead = head;
        RDFTerm* first = m_data->
          add_uriref("<http://www.w3.org/1999/02/22-rdf-syntax-ns#first>");
        RDFTerm* rest = m_data->
          add_uriref("<http://www.w3.org/1999/02/22-rdf-syntax-ns#rest>");
        for (unsigned i = 0; i < objects.size(); ++i) {
          m_data->add_triple(chead, first, objects[i]);
          if (i < objects.size() - 1) {
            RDFTerm* nhead = m_data->add_bnode();
            m_data->add_triple(chead, rest, nhead);
            chead = nhead;
          }
          else
            m_data->add_triple(chead, rest, nil);
        }
      }
    }
  
    return head;
  }



  RDFTerm* TurtleParser::do_verb(iter_t iter) {
    iter_t child = iter->children.begin();
    if (child->value.id() == parser_id(&predicate))
      return do_predicate(child);
    else
      return m_data->
        add_uriref("<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>");
  }



  vector<RDFTerm*> TurtleParser::do_objectList(iter_t iter) {
    vector<RDFTerm*> result;
    iter_t child;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&object))
        result.push_back(do_object(child));
    }
    return result;
  }



  RDFTerm* TurtleParser::do_predicate(iter_t iter) {
    return do_resource(iter->children.begin());
  }



  RDFTerm* TurtleParser::do_object(iter_t iter) {
    iter_t child = iter->children.begin();
    if (child->value.id() == parser_id(&resource))
      return do_resource(child);
    else if (child->value.id() == parser_id(&blank))
      return do_blank(child);
    else if (child->value.id() == parser_id(&literal))
      return do_literal(child);
    assert(!"This should never happen");
  }



  RDFTerm* TurtleParser::do_literal(iter_t iter) {
    RDFTerm* result = 0;
    iter_t child = iter->children.begin();
    string lang;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&quotedString))
        result = do_quotedString(child);
      else if (child->value.id() == parser_id(&language))
        lang = node_to_string(child);
      else if (child->value.id() == parser_id(&datatypeString))
        result = do_datatypeString(child);
      else if (child->value.id() == parser_id(&_double))
        result = do_double(child);
      else if (child->value.id() == parser_id(&decimal))
        result = do_decimal(child);
      else if (child->value.id() == parser_id(&integer))
        result = do_integer(child);
      else if (child->value.id() == parser_id(&boolean))
        result = do_boolean(child);
    }
    assert(result);
    result->lang = lang;
    return result;
  }



  vector<RDFTerm*> TurtleParser::do_itemList(iter_t iter) {
    vector<RDFTerm*> result;
    iter_t child;
    for (child = iter->children.begin(); 
         child != iter->children.end(); ++child) {
      if (child->value.id() == parser_id(&object))
        result.push_back(do_object(child));
    }
    return result;
  }



  RDFTerm* TurtleParser::do_quotedString(iter_t iter) {
    iter_t child = iter->children.begin();
    RDFTerm* result = 0;
    if (child->value.id() == parser_id(&_string))
      result = do_string(child);
    else if (child->value.id() == parser_id(&longString))
      result = do_longString(child);
    assert(result);
    return result;
  }



  RDFTerm* TurtleParser::do_string(iter_t iter) {
    string str = node_to_string(iter);
    scharacter_escape(str);
    return m_data->add_literal(str.substr(1, str.size() - 2));
  }



  RDFTerm* TurtleParser::do_longString(iter_t iter) {
    string str = node_to_string(iter);
    scharacter_escape(str);
    return m_data->add_literal(str.substr(3, str.size() - 6));
  }



  RDFTerm* TurtleParser::do_double(iter_t iter) {
    RDFTerm* double_type = 
      m_data->add_uriref("<http://www.w3.org/2001/XMLSchema#double>");
    return m_data->add_literal(node_to_string(iter), double_type); 
  }



  RDFTerm* TurtleParser::do_decimal(iter_t iter) {
    RDFTerm* decimal_type = 
      m_data->add_uriref("<http://www.w3.org/2001/XMLSchema#decimal>");
    return m_data->add_literal(node_to_string(iter), decimal_type); 

  }



  RDFTerm* TurtleParser::do_integer(iter_t iter) {
    RDFTerm* integer_type = 
      m_data->add_uriref("<http://www.w3.org/2001/XMLSchema#integer>");
    return m_data->add_literal(node_to_string(iter), integer_type); 

  }



  RDFTerm* TurtleParser::do_boolean(iter_t iter) {
    RDFTerm* boolean_type = 
      m_data->add_uriref("<http://www.w3.org/2001/XMLSchema#boolean>");
    return m_data->add_literal(node_to_string(iter), boolean_type); 
  }



  RDFTerm* TurtleParser::do_datatypeString(iter_t iter) {
    iter_t child = iter->children.begin();
    RDFTerm* data = 0;
    RDFTerm* type = 0;
    if (child->value.id() == parser_id(&quotedString))
      data = do_quotedString(child);
    else if (child->value.id() == parser_id(&resource))
      type = do_resource(child);
    assert(data);
    assert(type);
    data->datatype = type;
    return data;
  }


  bool TurtleParser::parse_ttl(const std::string& str, RDFData& data,
			       const std::string& base_uri) {
  
    m_data = &data;
    m_base_uri = base_uri;
    
    tree_parse_info<> info = pt_parse(str.c_str(), turtleDoc);
  
    if (info.full) {
      iter_t root = info.trees.begin();
      if (root->value.id() == parser_id(&turtleDoc)) {
        for (iter_t iter = root->children.begin(); 
             iter != root->children.end(); ++iter) {
          do_statement(iter);
        }
      }
    }
    return info.full;
  }

  
  bool TurtleParser::parse_ttl_file(const std::string& file, RDFData& data) {
    
    m_data = &data;
    
    // mmap the file
    int fd;
    if ((fd = open(file.c_str(), O_RDONLY)) == -1)
      return false;
    struct stat info;
    if (stat(file.c_str(), &info))
      return false;
    const char* text;
    if (!(text = (char*)mmap(0, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)))
      return false;
    close(fd);
    
    // build the base URI
    if (file[0] == '/')
      m_base_uri = string("file://") + file;
    else {
      static char buf[1024];
      m_base_uri = string("file://") + getcwd(buf, 1024) + "/" + file;
    }
    
    // parse
    tree_parse_info<> pinfo = pt_parse(text, text + info.st_size, turtleDoc);
    if (pinfo.full) {
      iter_t root = pinfo.trees.begin();
      if (root->value.id() == parser_id(&turtleDoc)) {
        for (iter_t iter = root->children.begin(); 
             iter != root->children.end(); ++iter) {
          do_statement(iter);
        }
      }
    }
    
    munmap((void*)text, info.st_size);
    
    return pinfo.full;
  }


  bool TurtleParser::parse_ttl_url(const std::string& url, RDFData& data) {
    if (url.substr(0, 8) != "<file://")
      return false;
    return parse_ttl_file(url.substr(8, url.size() - 9), data);
  }


  void TurtleParser::ucharacter_escape(string& str) {
    string::size_type i = 0;
    while ((i = str.find("\\u", i)) != string::npos)
      str.replace(i, 6, encode_utf8(str.substr(i + 2, 4)));
    i = 0;
    while ((i = str.find("\\U", i)) != string::npos)
      str.replace(i, 10, encode_utf8(str.substr(i + 2, 8)));
    i = 0;
    while ((i = str.find("\\>", i)) != string::npos)
      str.replace(i, 2, ">");
    i = 0;
    while ((i = str.find("\\\\", i)) != string::npos)
      str.replace(i, 2, "\\");
  }


  void TurtleParser::scharacter_escape(string& str) {
    string::size_type i = 0;
    while ((i = str.find("\\u", i)) != string::npos)
      str.replace(i, 6, encode_utf8(str.substr(i + 2, 4)));
    i = 0;
    while ((i = str.find("\\U", i)) != string::npos)
      str.replace(i, 10, encode_utf8(str.substr(i + 2, 8)));
    i = 0;
    while ((i = str.find("\\t", i)) != string::npos)
      str.replace(i, 2, "\t");
    i = 0;
    while ((i = str.find("\\n", i)) != string::npos)
      str.replace(i, 2, "\n");
    i = 0;
    while ((i = str.find("\\r", i)) != string::npos)
      str.replace(i, 2, "\r");
    i = 0;
    while ((i = str.find("\\\"", i)) != string::npos)
      str.replace(i, 2, "\"");
    i = 0;
    while ((i = str.find("\\\\", i)) != string::npos)
      str.replace(i, 2, "\\");
  }


  string TurtleParser::absolutise(const std::string& str) {
    
    // check if it's an absolute URI, if so return
    unsigned cpos = str.find(':');
    if (cpos != string::npos && cpos > 0) {
      if (str.size() > 0) {
	if ((str[0] >= 'a' && str[0] <= 'z') ||
	    (str[0] >= 'A' && str[0] <= 'Z')) {
	  unsigned int i;
	  for (i = 1; i < cpos; ++i) {
	    if (!((str[i] >= 'a' && str[i] <= 'z') ||
		  (str[i] >= 'A' && str[i] <= 'Z') ||
		  (str[i] >= '0' && str[i] <= '9')))
	      break;
	  }
	  if (i == cpos)
	    return str;
	}
      }
    }
    
    // it's a relative URI, but what type?
    if (str.size() == 0)
      return m_base_uri;
    else if (str.size() >= 2 && str.substr(0, 2) == "//")
      return m_base_uri.substr(m_base_uri.find(':')) + str;
    else if (str.size() >= 1 && str[0] == '/') {
      unsigned a = str.find("//");
      if (a == string::npos)
	return m_base_uri.substr(m_base_uri.find('/')) + str;
      else
	return m_base_uri.substr(m_base_uri.find('/', a + 2)) + str;
    }
    else
      return m_base_uri.substr(0, m_base_uri.rfind('/')) + "/" + str;
  }


}
