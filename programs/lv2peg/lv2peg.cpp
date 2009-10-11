/****************************************************************************
    
    LV2PEG - LV2 Port Enum Generator

    This program reads an RDF file (in the Turtle syntax) describing
    an LV2 plugin and generates a C header file with data structures
    that contain information about the plugin.
    
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

#include <cfloat>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <redland.h>
//#include "turtleparser.hpp"
//#include "query.hpp"
//#include "namespaces.hpp"

#ifndef VERSION
#define VERSION "UNKNOWNVERSION"
#endif


using namespace std;
//using namespace PAQ;


struct PortInfo {
  PortInfo() 
    : min(-FLT_MAX), 
      max(FLT_MAX), 
      default_value(min),
      toggled(false),
      integer(false),
      logarithmic(false) { }
  string name;
  float min;
  float max;
  float default_value;
  bool toggled;
  bool integer;
  bool logarithmic;
};


int main(int argc, char** argv) {
  
  for (int i = 1; i < argc; ++i) {
    if (string(argv[i]) == "-V" || string(argv[i]) == "--version") {
      cout<<"lv2peg "<<VERSION<<" by Lars Luthman <lars.luthman@gmail.com>"
          <<endl;
      return 0;
    }
  }
  
  if (argc < 3) {
    cerr<<"Use like this: lv2peg <input file> <output file> "<<endl;
    return -1;
  }
  
  // initialise librdf
  librdf_world* world = librdf_new_world();
  if (!world) {
    cerr<<"Failed to initialise librdf."<<endl;
    return -1;
  }
  
  // parse turtle file
  librdf_parser* parser = librdf_new_parser(world, "turtle", 0, 0);
  if (!parser) {
    cerr<<"Failed to initialise Turtle parser."<<endl;
    return -1;
  }
  librdf_storage* storage = librdf_new_storage(world, "memory", "storage", 0);
  if (!storage)  {
    cerr<<"Failed to initialise RDF storage."<<endl;
    return -1;
  }
  librdf_model* model = librdf_new_model(world, storage, 0);
  if (!model) {
    cerr<<"Failed to initialise RDF data model."<<endl;
    return -1;
  }
  librdf_uri* file_uri = librdf_new_uri_from_filename(world, argv[1]);
  if (!file_uri) {
    cerr<<"Failed to create URI from filename."<<endl;
    return -1;
  }
  if (librdf_parser_parse_into_model(parser, file_uri, 0, model)) {
    cerr<<"Failed to parse the input file."<<endl;
    return -1;
  }
  
  // find all plugins in the file
  librdf_query* plugins_query = 
    librdf_new_query(world, "sparql", 0,
		     (const unsigned char*) // this is stupid
		     "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
		     "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
		     "SELECT DISTINCT ?plugin, ?pegname WHERE { \n"
		     "?plugin a :Plugin. \n"
		     "?plugin ll:pegName ?pegname. }",
		     0);
  if (!plugins_query) {
    cerr<<"Failed to initialise plugin query."<<endl;
    return -1;
  }
  librdf_query_results* plugins_results =
    librdf_query_execute(plugins_query, model);
  if (!plugins_results) {
    cerr<<"Failed to execute plugin query."<<endl;
    return -1;
  }
  map<string, string> plugins;
  while (!librdf_query_results_finished(plugins_results)) {
    plugins[(char*)librdf_uri_as_string(librdf_node_get_uri(librdf_query_results_get_binding_value(plugins_results, 0)))] = 
      (char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(plugins_results, 1));
    librdf_query_results_next(plugins_results);
  }
  librdf_free_query_results(plugins_results);
  librdf_free_query(plugins_query);
  
  // iterate over all plugins
  map<string, map<int, PortInfo> > info;
  map<string, string>::const_iterator plug_iter;
  for (plug_iter = plugins.begin(); plug_iter != plugins.end(); ++plug_iter) {
    
    // query the plugin ports
    librdf_uri* plugin_uri = librdf_new_uri(world, (const unsigned char*)plug_iter->first.c_str());
    librdf_query* ports_query = 
      librdf_new_query(world, "sparql", 0,
		       (const unsigned char*) // this is stupid
		       "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
		       "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
		       "SELECT ?index, ?symbol WHERE { \n"
		       "<>        :port       ?port. \n"
		       "?port     :index      ?index; \n"
		       "          :symbol     ?symbol. }",
		       plugin_uri);
    if (!ports_query) {
      cerr<<"Failed to initialise port query."<<endl;
      return -1;
    }
    librdf_query_results* ports_results =
      librdf_query_execute(ports_query, model);
    if (!ports_results) {
      cerr<<"Failed to execute port query."<<endl;
      return -1;
    }
    map<int, PortInfo> ports;
    while (!librdf_query_results_finished(ports_results)) {
      int port_index = atoi((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(ports_results, 0)));
      if (ports.find(port_index) != ports.end()) {
	cerr<<"Index "<<port_index<<" is used for more than one port"<<endl;
	return -1;
      }
      ports[port_index].name = (char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(ports_results, 1));
      librdf_query_results_next(ports_results);
    }
    librdf_free_query_results(ports_results);
    librdf_free_query(ports_query);
    
    // check that the port indices are OK
    map<int, PortInfo>::const_iterator iter;
    int next = 0;
    for (iter = ports.begin(); iter != ports.end(); ++iter) {
      if (iter->first != next) {
        cerr<<"There was no description of port "<<next
            <<" in plugin "<<plug_iter->first<<endl;
        return -1;
      }
      ++next;
    }
    
    // get min values
    librdf_query* min_query = 
      librdf_new_query(world, "sparql", 0,
		       (const unsigned char*) // this is stupid
		       "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
		       "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
		       "SELECT ?index, ?min WHERE { \n"
		       "<>        :port       ?port. \n"
		       "?port     :index      ?index; \n"
		       "          :minimum    ?min. }",
		       plugin_uri);
    if (!min_query) {
      cerr<<"Failed to initialise minimum value query."<<endl;
      return -1;
    }
    librdf_query_results* min_results = librdf_query_execute(min_query, model);
    if (!min_results) {
      cerr<<"Failed to execute minimum value query."<<endl;
      return -1;
    }
    while (!librdf_query_results_finished(min_results)) {
      int port_index = atof((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(min_results, 0)));
      ports[port_index].min = atof((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(min_results, 1)));
      librdf_query_results_next(min_results);
    }
    librdf_free_query_results(min_results);
    librdf_free_query(min_query);
    
    // get max values
    librdf_query* max_query = 
      librdf_new_query(world, "sparql", 0,
		       (const unsigned char*) // this is stupid
		       "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
		       "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
		       "SELECT ?index, ?max WHERE { \n"
		       "<>        :port       ?port. \n"
		       "?port     :index      ?index; \n"
		       "          :maximum    ?max. }",
		       plugin_uri);
    if (!max_query) {
      cerr<<"Failed to initialise maximum value query."<<endl;
      return -1;
    }
    librdf_query_results* max_results = librdf_query_execute(max_query, model);
    if (!max_results) {
      cerr<<"Failed to execute maximum value query."<<endl;
      return -1;
    }
    while (!librdf_query_results_finished(max_results)) {
      int port_index = atof((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(max_results, 0)));
      ports[port_index].max = atof((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(max_results, 1)));
      librdf_query_results_next(max_results);
    }
    librdf_free_query_results(max_results);
    librdf_free_query(max_query);
    
    // get default values
    librdf_query* default_query = 
      librdf_new_query(world, "sparql", 0,
		       (const unsigned char*) // this is stupid
		       "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
		       "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
		       "SELECT ?index, ?default WHERE { \n"
		       "<>        :port       ?port. \n"
		       "?port     :index      ?index; \n"
		       "          :default    ?default. }",
		       plugin_uri);
    if (!default_query) {
      cerr<<"Failed to initialise default value query."<<endl;
      return -1;
    }
    librdf_query_results* default_results = librdf_query_execute(default_query, model);
    if (!default_results) {
      cerr<<"Failed to execute default value query."<<endl;
      return -1;
    }
    while (!librdf_query_results_finished(default_results)) {
      int port_index = atof((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(default_results, 0)));
      ports[port_index].default_value = atof((char*)librdf_node_get_literal_value(librdf_query_results_get_binding_value(default_results, 1)));
      librdf_query_results_next(default_results);
    }
    librdf_free_query_results(default_results);
    librdf_free_query(default_query);
    
    // get port hints
    librdf_query* hints_query = 
      librdf_new_query(world, "sparql", 0,
		       (const unsigned char*) // this is stupid
		       "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
		       "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
		       "SELECT ?index, ?hint WHERE { \n"
		       "<>        :port       ?port. \n"
		       "?port     :index      ?index. \n"
		       "?port     :portHint   ?hint. }",
		       plugin_uri);
    if (!hints_query) {
      cerr<<"Failed to initialise port hint query."<<endl;
      return -1;
    }
    librdf_query_results* hints_results = 
      librdf_query_execute(hints_query, model);
    if (!hints_results) {
      cerr<<"Failed to execute port hints query."<<endl;
      return -1;
    }
    while (!librdf_query_results_finished(hints_results)) {
      librdf_node* n = librdf_query_results_get_binding_value(hints_results, 0);
      int port_index = atof((char*)librdf_node_get_literal_value(n));
      string hint = (char*)librdf_uri_as_string(librdf_node_get_uri(librdf_query_results_get_binding_value(hints_results, 1)));
      if (hint == "<http://lv2plug.in/ns#toggled")
        ports[port_index].toggled = true;
      if (hint == "<http://lv2plug.in/ns#integer")
        ports[port_index].integer = true;
      if (hint == "<http://lv2plug.in/ns#logarithmic")
        ports[port_index].logarithmic = true;
      librdf_query_results_next(hints_results);
    }
    librdf_free_query_results(hints_results);
    librdf_free_query(hints_query);
    
    info[plug_iter->first] = ports;
  }
  
  // write the header file
  string header_guard = argv[2];
  for (size_t i = 0; i < header_guard.size(); ++i) {
    char& c = header_guard[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')))
      header_guard[i] = '_';
  }
  ofstream fout(argv[2]);
  fout<<"#ifndef "<<header_guard<<endl
      <<"#define "<<header_guard<<endl<<endl<<endl;
  
  // define data structure
  fout<<"#ifndef PEG_STRUCT"<<endl
      <<"#define PEG_STRUCT"<<endl
      <<"typedef struct {"<<endl
      <<"  float min;"<<endl
      <<"  float max;"<<endl
      <<"  float default_value;"<<endl
      <<"  char toggled;"<<endl
      <<"  char integer;"<<endl
      <<"  char logarithmic;"<<endl
      <<"} peg_data_t;"<<endl
      <<"#endif"<<endl<<endl;
    
  map<string, map<int, PortInfo> >::const_iterator piter;
  for (piter = info.begin(); piter != info.end(); ++piter) {
    fout<<"/* "<<piter->first<<" */"<<endl<<endl;
    
    // write the URI
    fout<<"static const char "<<plugins[piter->first]<<"_uri[] = \""
        <<piter->first<<"\";"<<endl<<endl;
    
    // write port labels
    fout<<"enum "<<plugins[piter->first]<<"_port_enum {"<<endl;
    map<int, PortInfo>::const_iterator iter;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter)
      fout<<"  "<<plugins[piter->first]<<"_"<<iter->second.name<<","<<endl;
    fout<<"  "<<plugins[piter->first]<<"_n_ports"<<endl
        <<"};"<<endl<<endl;
    
    // write port info
    fout<<"static const peg_data_t "
        <<plugins[piter->first]<<"_ports[] = {"<<endl;
    for (iter = piter->second.begin(); iter != piter->second.end(); ++iter) {
      fout<<"  { "
          <<iter->second.min<<", "
          <<iter->second.max<<", "
          <<iter->second.default_value<<", "
          <<(iter->second.toggled ? "1" : "0")<<", "
          <<(iter->second.integer ? "1" : "0")<<", "
          <<(iter->second.logarithmic ? "1" : "0")<<" }, "<<endl;
    }
    fout<<"};"<<endl<<endl<<endl;
    
  }
  
  fout<<"#endif /* "<<header_guard<<" */"<<endl;
  
  return 0;
}
