#include "server.h"
#include "client_http.hpp"
#include "server_http.hpp"
#include "utility.hpp"
#include "context.h"
#include "logger.h"

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
	Config& config = Context::get_config();
	m_ip = config["host"];
	m_port = config["port"];
	m_url = config["url"];
	string client_id = config["client_id"];
	m_url.replace( m_url.find("@") , m_url.length() - m_url.find("@"), client_id);
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
	    else
	    {
		Logger::log()->critical("Server failed to process data, return code : {}", string(r2->status_code) );
	    }
  	}
	catch(const SimpleWeb::system_error &e) {
	    Logger::log()->critical("Client request error : {}", e.what() );
	    return false;
  	}
	
	return false;	
}
