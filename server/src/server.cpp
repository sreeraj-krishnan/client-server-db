#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include "machine.pb.h"

#include "persistence.h"
#include "client.h"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <fstream>
#include <vector>

using namespace std;
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;


int main(int argc, char** argv) {
  
  if( argc < 2 )
  {
	cout << "Usage : ./server <path to xml file>\n";
	exit(1);
  }

  HttpServer server;
  server.config.port = 9090;
  server.config.address = "192.168.1.3";

  alert::Client::add_clients(argv[1]); // path to xml file

  server.resource["^/machine/[a-zA-Z0-9]*$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    auto content = request->content.string();
	string path = request->path;
	cout << "client id : " << path.substr( path.rfind("/")+1, path.length() - path.rfind("/")) << "\n";
	string client_key ( path.substr( path.rfind("/")+1, path.length() - path.rfind("/")) );

      {
	if( ! alert::Client::authenticate_client( client_key ) )
	{
		response->write( SimpleWeb::StatusCode::client_error_unauthorized );
		return;
	}		
      }
{
    MachineInfoPtr machineinfo_serialized = Persistence::get_orm_object( client_key , content );
    Persistence::write_client_info_db( machineinfo_serialized );    
    alert::Client::get_client( client_key )->notify( machineinfo_serialized );
}   
	response->write(SimpleWeb::StatusCode::success_created);

  };

  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
  };

  thread server_thread([&server]() {
    // Start server
    server.start();
  });

  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));
  server_thread.join();
}
