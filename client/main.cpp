
#include "client_http.hpp"
#include "server_http.hpp"
#include "utility.hpp"

#include<iostream>
#include "client_info.h"
#include <string>
#include <unistd.h>

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;

int main( int argc, char** argv)
{
    int count(100);
    while( count-- > 0 ) 
    {
	client::ClientInfo c;
	//machine::MachineInfo* info = c.get_machine_info();
        client::MachineInfoPtr info = c.get_machine_info();;

	string binary_data;

	if (  info->SerializeToString( &binary_data ) )
	{
  		HttpClient client("192.168.1.3:9090");
		char let[] = "xyzw";
		  // Synchronous request examples
		  try {
			    SimpleWeb::CaseInsensitiveMultimap headers;
			    headers.insert( std::pair<string,string>("Content-type","binary")) ;
			    headers.insert( std::pair<string,string>("Accept","application/json")) ;
			    auto r2 = client.request("POST", string("/machine/")+let[count%4], binary_data, headers);
			    cout << r2->status_code << endl;

  			}
		  catch(const SimpleWeb::system_error &e) {
		    cerr << "Client request error: " << e.what() << endl;
  		  }
	}
        sleep(1);
   }

	return 0;
}
