#include "context.h"

#include <string>
#include <istream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using boost::property_tree::ptree;

Config Context::m_config;

Context::Context( string config_file )
{
  try
  {
	ptree pt;
	ifstream file( config_file );
	if( ! file.is_open())
	{
	   // log , throw
	} 
	boost::property_tree::read_json(file, pt);	
	for( auto& it : pt )
	{
		m_config.insert( std::pair<const string, const string>( it.first, it.second.data()) );
	}
	file.close();

  }catch(const std::exception& ex) {
	//log 
  }	
}

Config& Context::get_config()
{
	return m_config;
}


