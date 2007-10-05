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

#include "turtleparser.hpp"
#include "query.hpp"
#include "namespaces.hpp"

#ifndef VERSION
#define VERSION "UNKNOWNVERSION"
#endif


using namespace std;
using namespace PAQ;


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
  
  // parse turtle file
  TurtleParser tp;
  RDFData data;
  if (!tp.parse_ttl_file(argv[1], data)) {
    cerr<<"Could not parse input. Not valid Turtle syntax."<<endl;
    return -1;
  }
  
  // find all plugins in the file
  Namespace lv2("<http://lv2plug.in/ns/lv2core#>");
  Namespace ll("<http://ll-plugins.nongnu.org/lv2/namespace#>");
  Variable plugin, pegname;
  vector<QueryResult> qr =
    select(plugin, pegname)
    .where(plugin, ll("pegName"), pegname)
    .where(plugin, rdf("type"), lv2("Plugin"))
    .run(data);
  map<string, string> plugins;
  for (unsigned i = 0; i < qr.size(); ++i)
    plugins[qr[i][plugin]->name] = qr[i][pegname]->name;
  
  // iterate over all plugins
  map<string, map<int, PortInfo> > info;
  map<string, string>::const_iterator plug_iter;
  for (plug_iter = plugins.begin(); plug_iter != plugins.end(); ++plug_iter) {
    // query the ports
    Variable port, index, symbol;
    qr = select(index, symbol)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("symbol"), symbol)
      .run(data);
  
    // put ports in a map so they get sorted
    map<int, PortInfo> ports;
    for (size_t i = 0; i < qr.size(); ++i) {
      int port_index = atoi(qr[i][index]->name.c_str());
      ports[port_index].name = qr[i][symbol]->name;
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
    
    // get min values
    Variable value;
    qr = select(index, value)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("minimum"), value)
      .run(data);
    for (unsigned i = 0; i < qr.size(); ++i)
      ports[atoi(qr[i][index]->name.c_str())].min = 
        atof(qr[i][value]->name.c_str());

    // get max values
    qr = select(index, value)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("maximum"), value)
      .run(data);
    for (unsigned i = 0; i < qr.size(); ++i)
      ports[atoi(qr[i][index]->name.c_str())].max = 
        atof(qr[i][value]->name.c_str());
    
    // get default values
    qr = select(index, value)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("default"), value)
      .run(data);
    for (unsigned i = 0; i < qr.size(); ++i)
      ports[atoi(qr[i][index]->name.c_str())].default_value = 
        atof(qr[i][value]->name.c_str());
    
    // get port hints
    Variable hint;
    qr = select(index, hint)
      .where(plug_iter->first, lv2("port"), port)
      .where(port, lv2("index"), index)
      .where(port, lv2("portHint"), hint)
      .run(data);
    for (unsigned i = 0; i < qr.size(); ++i) {
      if (qr[i][hint]->name == lv2("toggled"))
        ports[atoi(qr[i][index]->name.c_str())].toggled = true;
      if (qr[i][hint]->name == lv2("integer"))
        ports[atoi(qr[i][index]->name.c_str())].integer = true;
      if (qr[i][hint]->name == lv2("logarithmic"))
        ports[atoi(qr[i][index]->name.c_str())].logarithmic = true;
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
        <<piter->first.substr(1, piter->first.size() - 2)<<"\";"<<endl<<endl;
    
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
