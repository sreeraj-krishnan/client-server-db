#include "server.h" 
#include <iostream>
#include <string>
#include "client_info.h"
#include "context.h"

using namespace std;

int main ( int argc , char** argv )
{
	Context context("config/context.json");
	Server server;
		
	while( 1 )
	{
		client::ClientInfo c;
        	client::MachineInfoPtr info = c.get_machine_info();;
		string binary_data;
	
		if(  info->SerializeToString( &binary_data ) )
		{
			if ( !server.send_data( binary_data ) )
			{
				//cout << "successfully send data\n";
				cerr << "failed to send details to server\n";
			}
			else
			{
				cout << "successfully send data\n";
			}
		}
		else
		{	
			cerr << "failed to serialize data\n";
			//log throw 
		}
		sleep( stoi(Context::get_config()["send_stat_frequency"]));
	}

	return 0;
}
