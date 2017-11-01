#include "server.h" 
#include <iostream>
#include <string>
#include "client_info.h"

using namespace std;

int main ( int argc , char** argv )
{
	if ( argc < 2 )
	{
		cout << "usage : ./client <server-config-file> \n";
		exit(1);
	}
	Server server( argv[1] );
	
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
		}
		else
		{	
			cerr << "failed to serialize data\n";
			//log throw 
		}
		sleep(5);
	}

	return 0;
}
