#pragma once


#include <string>

using namespace std;

class ServerConfig
{
   public:
	
	ServerConfig(string config_json);
	bool initialize_server_config(std::string json_config);
	
	const string get_ip() const { return m_ip; }
	const string get_port() const { return m_port; }

   private:
	string m_ip;
	string m_port;
};
