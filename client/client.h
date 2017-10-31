#pragma once

#include <string>

using namespace std;

class Client
{
	protected:
			
	public:
		Client( const string _unique_key);
		
		void send_info_to_server();

	private:
		Server mserver;	
		string m_unique_key;		
};
