
#include "client_http.hpp"
#include "server_http.hpp"
#include "utility.hpp"

#include<iostream>
#include "client_info.h"
#include <string>

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;

/*
		info->Clear();
		cout << " idle time : " << c->machine().cpu().idle_time() << "\n";
		c->ParseFromString( binary_data);
		cout << " idle time : " << c->machine().cpu().idle_time() << "\n";
*/

int main( int argc, char** argv)
{
	client::ClientInfo c;
	machine::MachineInfo* info = c.get_machine_info();
      
	string binary_data;

	if (  info->SerializeToString( &binary_data ) )
	{
  		HttpClient client("192.168.1.3:9090");


		  // Synchronous request examples
		  try {
			    SimpleWeb::CaseInsensitiveMultimap headers;
			    headers.insert( std::pair<string,string>("Content-type","binary")) ;
			    headers.insert( std::pair<string,string>("Accept","application/json")) ;
			    auto r2 = client.request("POST", "/machine?id=123", binary_data, headers);
			    cout << r2->status_code << endl;
			    cout << r2->content.rdbuf() << endl;

  			}
		  catch(const SimpleWeb::system_error &e) {
		    cerr << "Client request error: " << e.what() << endl;
  		  }
				
	}

	return 0;
}
