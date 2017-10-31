
#include<string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

using namespace std;

class MachineInfo
{
  public:
		MachineInfo(string _client_key);

		void get_machine_info( string& _details );

		void send_data( string server_ip, string port, string url);

		void set_client_key(const string& _key);	

  		string get_client_key() const { return m_client_key; } 

  private:
	
	string m_client_key;
};
