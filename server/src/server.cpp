#include "client_http.hpp"
#include "server_http.hpp"
#include "status_code.hpp"
#include "machine.pb.h"
#include "context.h"
#include "logger.h"

#include "persistence.h"
#include "client.h"
#include "server_config.h"

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
  
  Context context("config/context.json"); // initialize context
  alert::Client::add_clients("config/client-list.xml"); // path to xml file


  HttpServer server;
  server.config.port =  stoi (Context::get_config()["server_port"] ); 
  server.config.address = Context::get_config()["server_ip"]; 

  Logger::log()->info("Server initialized" );

  server.resource["^/machine/[a-zA-Z0-9]*$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    auto content = request->content.string();
	string path = request->path;
	string client_key ( path.substr( path.rfind("/")+1, path.length() - path.rfind("/")) );

      {
	if( ! alert::Client::authenticate_client( client_key ) )
	{
		response->write( SimpleWeb::StatusCode::client_error_unauthorized );
		Logger::log()->critical("Unknown client request recieved from client {0}", client_key );
		return;
	}		
      }
{
    MachineInfoPtr machineinfo_serialized = Persistence::get_orm_object( client_key , content );
    Persistence::write_client_info_db( machineinfo_serialized );    
    alert::Client::get_client( client_key )->notify( machineinfo_serialized );
}   
	Logger::log()->info("{0} client info persisted", client_key);
	response->write(SimpleWeb::StatusCode::success_created);

  };

  server.on_error = [](shared_ptr<HttpServer::Request> request, const SimpleWeb::error_code & ec) {
	Logger::log()->critical("Error request : {0}", request->content.string());
  };

  thread server_thread([&server]() {
    server.start();
  });

  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));
  server_thread.join();
}
