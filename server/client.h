#pragma once

#include<list>
#include<map>
#include<string>
#include <boost/shared_ptr.hpp>

using namespace std;

class machineinfo;

typedef boost::shared_ptr < machineinfo > MachineInfoPtr;

namespace alert {
	enum class Alert {
		none=0,
		memory = 1,
		cpu = 2,
		process=3	
	};
	enum class Condition {
		none=0,
		equal = 1,
		under = 2,
		over = 3
	};

typedef std::pair< Alert, Condition>  EventType;

typedef std::pair < EventType , string > TriggerEvent; // condition and value


class Client
{
	public:
		Client(string _key, string _email);
	
		static void add_clients( std::string xml_file);
		static bool authenticate_client( const string& key );
		static Client * const get_client( const string& key );
		void notify( const MachineInfoPtr& machine_info );		
		
	private:

		string m_key;
		string m_email;

		std::list< TriggerEvent > notify_events; // per client

	public:
		void push_events(const TriggerEvent _event );
		
		static std::list< Client* > clients; // full list of clients
		static std::map < const string, Client * const > client_map;

	protected:
		static std::pair<string, string> get_alert_value( const auto& kv, string& type, bool check_xml_attr);
		static Alert get_alert_type( const string& input );
		static Condition get_condition( const string& input );
		static string get_alert_string( const Alert& alert );
		static string get_condition_string( Condition condition );

		void alert_for_memory( const TriggerEvent& event, const MachineInfoPtr& machine_info );
		void alert_for_cpu( const TriggerEvent& event, const MachineInfoPtr& machine_info );
		void alert_for_process( const TriggerEvent& event, const MachineInfoPtr& machine_info );

};

}
