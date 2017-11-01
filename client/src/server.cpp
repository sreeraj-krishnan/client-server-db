#include "server.h"
#include "client_http.hpp"
#include "server_http.hpp"
#include "utility.hpp"
#include "context.h"

#include <string>
#include <istream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;

using boost::property_tree::ptree;

Server::Server( )
{
   #if 0
	if ( ! initialize_server_config( config_json ) )
	{	
		// throw
		// log
	}
   #endif
	Config& config = Context::get_config();
	m_ip = config["host"];
	m_port = config["port"];
	m_url = config["url"];
	string client_id = config["client_id"];
	m_url.replace( m_url.find("@") , m_url.length() - m_url.find("@"), client_id);
}

bool Server::initialize_server_config(string json_config)
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
		else if( it.first == "url")
		{
			m_url = it.second.data();
		}
		else if( it.first == "client_id")
		{
			client_id = it.second.data();
		}
	}
	m_url.replace( m_url.find("@") , m_url.length() - m_url.find("@"), client_id);
	file.close();
	return true;
}

bool Server::send_data( std::string& data )
{
	using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

	HttpClient client( m_ip + string(":") + m_port );
	
	try 
	{
	    SimpleWeb::CaseInsensitiveMultimap headers;
	    headers.insert( std::pair<string,string>("Content-type","binary")) ;
	    headers.insert( std::pair<string,string>("Accept","application/json")) ;
	    auto r2 = client.request("POST", m_url , data, headers);
	    if ( string(r2->status_code).substr(0,3) == "201" )
	    {
		return true;
	    }
  	}
	catch(const SimpleWeb::system_error &e) {
	    cerr << "Client request error: " << e.what() << endl;
	    return false;
  	}
	
	return false;	
}
