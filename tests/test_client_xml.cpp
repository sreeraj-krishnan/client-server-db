#include "client.h"
#include <iostream>

int main(int args, char** argv)
{
	alert::Client::add_clients(argv[1]);
	for( auto& it : alert::Client::client_map )
	{
		it.second->notify();
	}
	return 0;
}
