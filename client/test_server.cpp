#include "server.h" 
#include <iostream>
#include <string>
#include "client_info.h"

using namespace std;

int main ( int argc , char** argv )
{
	Server server( argv[1] );
	client::ClientInfo c;
        client::MachineInfoPtr info = c.get_machine_info();;
	string binary_data;
	
	if(  info->SerializeToString( &binary_data ) )
	{
		if ( server.send_data( binary_data ) )
		{
			cout << "successfully send data\n";
		}
	}
	else
	{	
		//log throw 
	}

	return 0;
}
