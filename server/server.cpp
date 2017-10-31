#include "client_http.hpp"
#include "server_http.hpp"
#include "machine.pb.h"
#include "status_code.hpp"
#include "persistence.h"
#include "client.h"

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <fstream>
#include <vector>

using namespace std;
// Added for the json-example:
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

  // Add resources using path-regex and method-string, and an anonymous function
  // POST-example for the path /string, responds the posted string
  server.resource["^/machine/[a-zA-Z0-9]*$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    auto content = request->content.string();
	cout << "path :" <<  request->path << "\n";
	string path = request->path;
	cout << "client id : " << path.substr( path.rfind("/")+1, path.length() - path.rfind("/")) << "\n";
	string client_key ( path.substr( path.rfind("/")+1, path.length() - path.rfind("/")) );

      {
	if( ! alert::Client::authenticate_client( client_key ) )
	{
    		SimpleWeb::CaseInsensitiveMultimap headers;
		headers.insert ( std::pair<string, string>("content-type", "application/json") );
		response->write(SimpleWeb::StatusCode::client_error_unauthorized, SimpleWeb::status_code( SimpleWeb::StatusCode::client_error_unauthorized), headers);
		return;
	}		
      }
{
    MachineInfoPtr machineinfo_serialized = Persistence::get_orm_object( content );
    Persistence::write_client_info_db( machineinfo_serialized );    
    alert::Client::get_client( client_key )->notify( machineinfo_serialized );
}   
    SimpleWeb::CaseInsensitiveMultimap headers;
    headers.insert ( std::pair<string, string>("content-type", "application/json") );
    response->write(SimpleWeb::StatusCode::success_created, SimpleWeb::status_code( SimpleWeb::StatusCode::success_created ), headers);


  };

  // POST-example for the path /json, responds firstName+" "+lastName from the posted json
  // Responds with an appropriate error message if the posted json is not valid, or if firstName or lastName is missing
  // Example posted json:
  // {
  //   "firstName": "John",
  //   "lastName": "Smith",
  //   "age": 25
  // }
  server.resource["^/authenticate$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      ptree pt;
      read_json(request->content, pt);

      auto name = pt.get<string>("firstName") + " " + pt.get<string>("lastName");

      *response << "HTTP/1.1 200 OK\r\n"
                << "Content-Length: " << name.length() << "\r\n\r\n"
                << name;
    }
    catch(const exception &e) {
      *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
                << e.what();
    }


    // Alternatively, using a convenience function:
    // try {
    //     ptree pt;
    //     read_json(request->content, pt);

    //     auto name=pt.get<string>("firstName")+" "+pt.get<string>("lastName");
    //     response->write(name);
    // }
    // catch(const exception &e) {
    //     response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
    // }
  };

#if 0
  // GET-example for the path /info
  // Responds with request-information
  server.resource["^/info$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    stringstream stream;
    stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
    stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";
    for(auto &header : request->header)
      stream << header.first << ": " << header.second << "<br>";

    // Find length of content_stream (length received using content_stream.tellp())
    stream.seekp(0, ios::end);

    *response << "HTTP/1.1 200 OK\r\nContent-Length: " << stream.tellp() << "\r\n\r\n"
              << stream.rdbuf();


    // Alternatively, using a convenience function:
    // stringstream stream;
    // stream << "<h1>Request from " << request->remote_endpoint_address << " (" << request->remote_endpoint_port << ")</h1>";
    // stream << request->method << " " << request->path << " HTTP/" << request->http_version << "<br>";
    // for(auto &header: request->header)
    //     stream << header.first << ": " << header.second << "<br>";
    // response->write(stream);
  };

  // GET-example for the path /match/[number], responds with the matched string in path (number)
  // For instance a request GET /match/123 will receive: 123
  server.resource["^/match/([0-9]+)$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    string number = request->path_match[1];
    //*response << "HTTP/1.1 200 OK\r\nContent-Length: " << number.length() << "\r\n\r\n"
     //         << number;
    SimpleWeb::CaseInsensitiveMultimap headers;
    headers.insert ( std::pair<string, string>("content-type", "application/json") );
    response->write(SimpleWeb::StatusCode::success_ok, SimpleWeb::status_code( SimpleWeb::StatusCode::success_ok ), headers);


    // Alternatively, using a convenience function:
    // response->write(request->path_match[1]);
  };

  // Get example simulating heavy work in a separate thread
  server.resource["^/work$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> /*request*/) {
    thread work_thread([response] {
      this_thread::sleep_for(chrono::seconds(5));
      response->write("Work done");
    });
    work_thread.detach();
  };

  // Default GET-example. If no other matches, this anonymous function will be called.
  // Will respond with content in the web/-directory, and its subdirectories.
  // Default file: index.html
  // Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
  server.default_resource["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      auto web_root_path = boost::filesystem::canonical("web");
      auto path = boost::filesystem::canonical(web_root_path / request->path);
      // Check if path is within web_root_path
      if(distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
         !equal(web_root_path.begin(), web_root_path.end(), path.begin()))
        throw invalid_argument("path must be within root path");
      if(boost::filesystem::is_directory(path))
        path /= "index.html";

      SimpleWeb::CaseInsensitiveMultimap header;

//    Uncomment the following line to enable Cache-Control
//    header.emplace("Cache-Control", "max-age=86400");

#ifdef HAVE_OPENSSL
//    Uncomment the following lines to enable ETag
//    {
//      ifstream ifs(path.string(), ifstream::in | ios::binary);
//      if(ifs) {
//        auto hash = SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
//        header.emplace("ETag", "\"" + hash + "\"");
//        auto it = request->header.find("If-None-Match");
//        if(it != request->header.end()) {
//          if(!it->second.empty() && it->second.compare(1, hash.size(), hash) == 0) {
//            response->write(SimpleWeb::StatusCode::redirection_not_modified, header);
//            return;
//          }
//        }
//      }
//      else
//        throw invalid_argument("could not read file");
//    }
#endif

      auto ifs = make_shared<ifstream>();
      ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

      if(*ifs) {
        auto length = ifs->tellg();
        ifs->seekg(0, ios::beg);

        header.emplace("Content-Length", to_string(length));
        response->write(header);

        // Trick to define a recursive function within this scope (for example purposes)
        class FileServer {
        public:
          static void read_and_send(const shared_ptr<HttpServer::Response> &response, const shared_ptr<ifstream> &ifs) {
            // Read and send 128 KB at a time
            static vector<char> buffer(131072); // Safe when server is running on one thread
            streamsize read_length;
            if((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
              response->write(&buffer[0], read_length);
              if(read_length == static_cast<streamsize>(buffer.size())) {
                response->send([response, ifs](const SimpleWeb::error_code &ec) {
                  if(!ec)
                    read_and_send(response, ifs);
                  else
                    cerr << "Connection interrupted" << endl;
                });
              }
            }
          }
        };
        FileServer::read_and_send(response, ifs);
      }
      else
        throw invalid_argument("could not read file");
    }
    catch(const exception &e) {
      response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
    }
  };
#endif

  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
  };

  thread server_thread([&server]() {
    // Start server
    server.start();
  });

  // Wait for server to start so that the client can connect
  this_thread::sleep_for(chrono::seconds(1));
#if 0
  // Client examples
  HttpClient client("192.168.1.3:9090");

  string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

  // Synchronous request examples
  try {
    auto r1 = client.request("GET", "/match/123");
    cout << r1->content.rdbuf() << endl; // Alternatively, use the convenience function r1->content.string()

    //auto r2 = client.request("POST", "/string", json_string);
    //cout << r2->content.rdbuf() << endl;
  }
  catch(const SimpleWeb::system_error &e) {
    cerr << "Client request error: " << e.what() << endl;
  }

  // Asynchronous request example
  client.request("POST", "/json", json_string, [](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
    if(!ec)
      cout << response->content.rdbuf() << endl;
  });
  client.io_service->run();
#endif
  server_thread.join();
}
