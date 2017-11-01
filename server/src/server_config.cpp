#include "server_config.h"

#include <string>
#include <istream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

using boost::property_tree::ptree;

ServerConfig::ServerConfig( string config_json )
{
	if ( ! initialize_server_config( config_json ) )
	{	
		// throw
		// log
	}
}

bool ServerConfig::initialize_server_config(string json_config)
{
	ptree pt;

	ifstream file( json_config );
	if( ! file.is_open())
	{
	   return false;
	} 
	boost::property_tree::read_json(file, pt);	

	string client_id;

	for( auto& it : pt )
	{
		if( it.first == "host" )
		{
			m_ip = it.second.data();
		}
		else if( it.first == "port" )
		{
			m_port =  it.second.data();
		}
	}
	file.close();
	return true;
}

