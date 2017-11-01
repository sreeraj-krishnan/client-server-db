#pragma once


#include <string>

using namespace std;

class Server
{
   public:
	
	Server();

	bool initialize_server_config(std::string json_config);

	bool send_data( std::string& data );

   private:
	string m_ip;
	string m_port;
	string m_url;
};
