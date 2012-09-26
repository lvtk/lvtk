/****************************************************************************
    
    ttl2peg - LV2 Port Enum Generator

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
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <redland.h>

#ifndef VERSION
#define VERSION "UNKNOWNVERSION"
#endif


using namespace std;


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


/** A template wrapper to handle all the annoying memory management for 
    librdf types. */
template <typename T, void(*F)(T*)>
class RDFPtr {
public:
  /** In the constructor, just take ownership of a T pointer. */
  RDFPtr(T* t) : m_t(t) { }
  /** In the destructor, call the deallocation function. */
  ~RDFPtr() { if (m_t) F(m_t); }
  /** Get the underlying pointer, to pass to librdf functions. */
  T* get() { return m_t; }
  /** Bool conversion operator. */
  operator bool() { return m_t != 0; }
private:
  /** Copying is not allowed, that could cause multiple deletions. */
  RDFPtr(RDFPtr const&) { }
  /** Assignment is not allowed, that could cause multiple deletions. */
  RDFPtr& operator=(RDFPtr const&) { return *this; }
  
  T* m_t;
};


/** A wrapper class for librdf_query_results. */

class RDFResults : public RDFPtr<librdf_query_results, 
                                 &librdf_free_query_results> {
public:
  typedef RDFPtr<librdf_query_results, &librdf_free_query_results> Parent;
  RDFResults(librdf_query_results* r) : Parent(r) { }
  
  /** Returns true if there are no more bindings. */
  bool finished() { return librdf_query_results_finished(get()); }
  
  /** Returns the literal value of a bound node. */
  char* get_literal(int i) { return reinterpret_cast<char*>(librdf_node_get_literal_value(librdf_query_results_get_binding_value(get(), i))); }

  /** Returns the URI of a bound node as a C string. */
  char* get_uri(int i) { return reinterpret_cast<char*>(librdf_uri_as_string(librdf_node_get_uri(librdf_query_results_get_binding_value(get(), i)))); }
  
  /** Goes to the next binding. */
  void next() { librdf_query_results_next(get()); }
};


/** Convenient typedef for a RDFPtr instance wrapping librdf_uri. */
typedef RDFPtr<librdf_uri, &librdf_free_uri> RDFUri;

/** Convenient typedef for a RDFPtr instance wrapping librdf_query. */
typedef RDFPtr<librdf_query, &librdf_free_query> RDFQuery;

/** Convenient typedef for a RDFPtr instance wrapping librdf_world. */
typedef RDFPtr<librdf_world, &librdf_free_world> RDFWorld;

/** Convenient typedef for a RDFPtr instance wrapping librdf_model. */
typedef RDFPtr<librdf_model, &librdf_free_model> RDFModel;

/** Convenient typedef for a RDFPtr instance wrapping librdf_parser. */
typedef RDFPtr<librdf_parser, &librdf_free_parser> RDFParser;

/** Convenient typedef for a RDFPtr instance wrapping librdf_storage. */
typedef RDFPtr<librdf_storage, &librdf_free_storage> RDFStorage;

/** Convenience function for parsing a Turtle file without all the librdf
    nastyness. */
librdf_model* parse_file(RDFWorld& world, string const& filename) {
  
  RDFParser parser(librdf_new_parser(world.get(), "turtle", 0, 0));
  if (!parser)
    throw runtime_error("Failed to initialise Turtle parser.");
  
  RDFStorage storage(librdf_new_storage(world.get(), "memory", "storage", 0));
  if (!storage)
    throw runtime_error("Failed to initialise RDF storage.");
  
  librdf_model* model = librdf_new_model(world.get(), storage.get(), 0);
  if (!model)
    throw runtime_error("Failed to initialise RDF data model.");
  
  RDFUri file_uri(librdf_new_uri_from_filename(world.get(), filename.c_str()));
  if (!file_uri) {
    librdf_free_model(model);
    throw runtime_error("Failed to create URI from filename.");
  }

  if (librdf_parser_parse_into_model(parser.get(), file_uri.get(), 0, model)) {
    librdf_free_model(model);
    throw runtime_error("Failed to parse the input file.");
  }
  
  return model;
}


/** Convenience function for running a SPARQL query on a model without all the
    librdf nastyness. */
librdf_query_results* run_query(RDFModel& model, RDFWorld& world,
                                string const& query, string const& base = "") {
  
  librdf_uri* base_uri_p = 0;
  if (base != "")
    base_uri_p = 
      librdf_new_uri(world.get(), 
                     reinterpret_cast<unsigned char const*>(base.c_str()));
  RDFUri base_uri(base_uri_p);
  RDFQuery rdf_query(librdf_new_query(world.get(), "sparql", 0,
                                      reinterpret_cast<unsigned char const*>(query.c_str()),
                                      base_uri.get()));
  if (!rdf_query)
    throw runtime_error("Failed to initialise SPARQL query.");
  librdf_query_results* rdf_results =
    librdf_query_execute(rdf_query.get(), model.get());
  if (!rdf_results)
    throw runtime_error("Failed to execute plugin query.");
  
  return rdf_results;
}


int main(int argc, char** argv) {
  
  // print the version info if requested
  for (int i = 1; i < argc; ++i) {
    if (string(argv[i]) == "-V" || string(argv[i]) == "--version") {
      cout<<"ttl2peg "<<VERSION<<" by Lars Luthman <lars.luthman@gmail.com>"
          <<endl;
      return 0;
    }
  }
  
  try {
    
    // print usage info if the parameters are invalid
    if (argc < 3)
      throw runtime_error("Usage: ttl2c <input file> <output file>");
    
    // initialise librdf
    RDFWorld world (librdf_new_world());
    if (!world)
      throw runtime_error("Failed to initialise librdf.");
    
    // parse turtle file
    RDFModel model(parse_file(world, argv[1]));
    
    // find all plugins in the file
    RDFResults 
      results(run_query(model, world,
                        "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
                        "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
                        "SELECT DISTINCT ?plugin, ?pegname WHERE { \n"
                        "?plugin a :Plugin. \n"
                        "?plugin ll:pegName ?pegname. }"));
    map<string, string> plugins;
    while (!results.finished()) {
      plugins[results.get_uri(0)] = results.get_literal(1);
      results.next();
    }
    
    // iterate over all plugins
    map<string, map<int, PortInfo> > info;
    map<string, string>::const_iterator plug_iter;
    
    for (plug_iter = plugins.begin(); plug_iter != plugins.end(); ++plug_iter) {
      
      // query the plugin ports
      map<int, PortInfo> ports;
      {
        RDFResults results(run_query(model, world,
                                     "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
                                     "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
                                     "SELECT ?index, ?symbol WHERE { \n"
                                     "<>        :port       ?port. \n"
                                     "?port     :index      ?index; \n"
                                     "          :symbol     ?symbol. }",
                                     plug_iter->first));
        while (!results.finished()) {
          int port_index = atoi(results.get_literal(0));
          if (ports.find(port_index) != ports.end()) {
            cerr<<"Index "<<port_index<<" is used for more than one port"<<endl;
            return -1;
          }
          ports[port_index].name = results.get_literal(1);
          results.next();
        }
      
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
      }
      
      // get min values
      {
        RDFResults results(run_query(model, world,
                                     "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
                                     "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
                                     "SELECT ?index, ?min WHERE { \n"
                                     "<>        :port       ?port. \n"
                                     "?port     :index      ?index; \n"
                                     "          :minimum    ?min. }",
                                     plug_iter->first));
        while (!results.finished()) {
          int port_index = atoi(results.get_literal(0));
          ports[port_index].min = atof(results.get_literal(1));
          results.next();
        }
      }
      
      // get max values
      {
        RDFResults results(run_query(model, world,
                                     "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
                                     "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
                                     "SELECT ?index, ?max WHERE { \n"
                                     "<>        :port       ?port. \n"
                                     "?port     :index      ?index; \n"
                                     "          :maximum    ?max. }",
                                     plug_iter->first));
        while (!results.finished()) {
          int port_index = atoi(results.get_literal(0));
          ports[port_index].max = atof(results.get_literal(1));
          results.next();
        }
      }
      
      // get default values
      {
        RDFResults results(run_query(model, world,
                                     "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
                                     "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
                                     "SELECT ?index, ?min WHERE { \n"
                                     "<>        :port       ?port. \n"
                                     "?port     :index      ?index; \n"
                                     "          :default    ?min. }",
                                     plug_iter->first));
        while (!results.finished()) {
          int port_index = atoi(results.get_literal(0));
          ports[port_index].default_value = atof(results.get_literal(1));
          results.next();
        }
      }
      
      // get port hints
      {
        RDFResults results(run_query(model, world,
                                     "PREFIX : <http://lv2plug.in/ns/lv2core#>\n"
                                     "PREFIX ll: <http://ll-plugins.nongnu.org/lv2/namespace#>\n"
                                     "SELECT ?index, ?hint WHERE { \n"
                                     "<>        :port       ?port. \n"
                                     "?port     :index      ?index. \n"
                                     "?port     :portHint   ?hint. }",
                                     plug_iter->first));
        while (!results.finished()) {
          int port_index = atoi(results.get_literal(0));
          string hint = results.get_literal(1);
          if (hint == "<http://lv2plug.in/ns#toggled")
            ports[port_index].toggled = true;
          if (hint == "<http://lv2plug.in/ns#integer")
            ports[port_index].integer = true;
          if (hint == "<http://lv2plug.in/ns#logarithmic")
            ports[port_index].logarithmic = true;
          results.next();
        }
      }
      
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
  
  }
  
  catch (runtime_error& e) {
    cerr<<e.what()<<endl;
    return -1;
  }

  return 0;
}
