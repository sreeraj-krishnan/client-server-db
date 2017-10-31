#include "client.h"
#include <iostream>

int main(int args, char** argv)
{
	alert::Client::add_clients(argv[1]);
//	alert::Client * c = alert::Client::client_map[ string("x") ];
	for( auto& it : alert::Client::client_map )
	{
		it.second->notify();
	}
/*
	if ( c )
	{
		c->notify();
	}
	else
	{
		cout << "NO SUCH CLIENT!!";
	}
*/
	return 0;
}
