#include "client.h"
#include "smtpclient.h"
#include "machine_info.hxx"
#include "context.h"

#include <istream>
#include <iostream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace alert;

std::list< Client* > Client::clients;
std::map < const string, Client * const > Client::client_map;

Client::Client(string _key, string _email) : m_key( _key ), m_email( _email ) 
{
}

void Client::alert_for_memory( const TriggerEvent& _event, const MachineInfoPtr& machine_info )
{
	const EventType& eve = _event.first;
	const string trigger_value = _event.second;

	const int trigger = stoi( trigger_value );

	string subject( string("Memory for client ") + m_key );
	string body;
	
	Memory* m = machine_info->get_memory().get();

	float mem_percentage = (100.0 * ( m->mem_total - m->mem_free ) / m->mem_total );
	
	if( eve.second == Condition::equal && (int)mem_percentage == trigger )
	{
		body = string( "Memory consumption reached the trigger value of ") + trigger_value;				
	}
	else if( eve.second == Condition::under && mem_percentage < trigger )
	{
		body = string( "Memory consumption went under the trigger value of ") + trigger_value;				
	}
	else if( eve.second == Condition::under && mem_percentage > trigger )
	{
		body = string( "Memory consumption went over the trigger value of ") + trigger_value;
	}
	
	if( ! body.empty() )	
	{
		Config& config = Context::get_config();
		SMTPClient mailc( config["mail_server"], stol(config["mail_server_port"]),config["mail_username"],config["mail_passwd"]);
		bool ret = mailc.Send(config["mail_username"],m_email, subject , body );
		if ( ret )
		{
			cout << "failed to memory send notification " << "\n";
		}	
	}
}

void Client::alert_for_cpu( const TriggerEvent& _event, const MachineInfoPtr& machine_info )
{
	const EventType& eve = _event.first;
	const string trigger_value = _event.second;

	const int trigger = stoi( trigger_value );

	string subject(string("CPU usage for client ") + m_key);
	string body;
	
	CPU* c = machine_info->get_cpu().get();

	if( eve.second == Condition::equal && (int)c->use_time == trigger )
	{
		body = string( "CPU usage reached the trigger value of ") + trigger_value;				
	}
	else if( eve.second == Condition::under && c->use_time < trigger )
	{
		body = string( "CPU usage under the trigger value of ") + trigger_value;				
	}
	else if( eve.second == Condition::under && c->use_time > trigger )
	{
		body = string( "CPU usage went over the trigger value of ") + trigger_value;
	}
	
	if( ! body.empty() )	
	{
		Config& config = Context::get_config();
		SMTPClient mailc( config["mail_server"], stol( config["mail_server_port"]),config["mail_username"],config["mail_passwd"]);
		bool ret = mailc.Send(config["mail_username"],m_email, subject , body );
		if ( ret )
		{
			cout << "failed to send cpu notification " << "\n";
		}	
	}
	
}

void Client::alert_for_process( const TriggerEvent& _event, const MachineInfoPtr& machine_info )
{
	const EventType& eve = _event.first;
	const string trigger_value = _event.second;

	const int trigger = stoi( trigger_value );

	string subject( string( "Process running limit triggered for client ")  + m_key );
	string body;
	
	Process* p = machine_info->get_process().get();

	if( eve.second == Condition::equal && (int)p->running == trigger )
	{
		body = string( "Process running limit reached the trigger value of ") + trigger_value;				
	}
	else if( eve.second == Condition::under && p->running < trigger )
	{
		body = string( "Process running limit under the trigger value of ") + trigger_value;				
	}
	else if( eve.second == Condition::under && p->running > trigger )
	{
		body = string( "Process running limit went over the trigger value of ") + trigger_value;
	}
	
	if( ! body.empty() )	
	{
		Config& config = Context::get_config();
		SMTPClient mailc( config["mail_server"], stol(config["mail_server_port"]) ,config["mail_username"],config["mail_passwd"]);
		bool ret = mailc.Send(config["mail_username"],m_email, subject , body );
		if ( ret )
		{
			cout << "failed to send process notification " << "\n";
		}	
	}
	
}


void Client::notify(const MachineInfoPtr& machine_info )
{
	for ( auto& it : this->notify_events )
	{
		const EventType& eve = it.first;
		if( eve.first == Alert::memory )
		{
			alert_for_memory( it , machine_info );
		}
		else if( eve.first == Alert::cpu )
		{
			alert_for_cpu( it , machine_info );
		}
		else if( eve.first == Alert::process )
		{
			alert_for_process( it , machine_info );
		}
	}
}

Client * const Client::get_client( const string& key )
{
	if( client_map.find(key) != client_map.end() )
	{
		return client_map[key];
	}
	return nullptr;
}
bool Client::authenticate_client( const string& key )
{
	if( client_map.find(key) != client_map.end() )
	{
		return true;
	}
	return false;
}

string Client::get_condition_string( Condition condition )
{
	if( condition == Condition::equal )
	{
		return string( "equal to" );
	}
	else if( condition == Condition::under )
	{
		return string("under");
	}
	else if( condition == Condition::over )
	{
		return string("over");
	}
	return string();

}
string Client::get_alert_string( const Alert& alert )
{
	if( alert == Alert::memory )
	{
		return string("memory");
	}
	else if( alert == Alert::cpu )
	{
		return string("cpu");
	}
	else if( alert == Alert::process )
	{
		return string("process");
	}
	return string();
}

Alert Client::get_alert_type( const string& input )
{
	if( input == string("memory"))
	{
		return Alert::memory;
	}
	else if( input == string("cpu") )
	{
		return  Alert::cpu;
	}
	else if( input == string("process") )
	{
		return  Alert::process;
	}
	else
	{
		return  Alert::none;
	}
}

Condition Client::get_condition( const string& input )
{
	if( input == string("equal"))
	{
		return Condition::equal ;
	}
	else if( input == string("under"))
	{
		return Condition::under ;
	}
	else if( input == string("over") )
	{
		return Condition::over ;
	}
	else
	{
		return Condition::none;
	}
}

void Client::push_events(const TriggerEvent _event )
{
	notify_events.push_back( _event );	
}

std::pair<string, string> Client::get_alert_value( const auto& kv, string& type, bool check_xml_attr)
{
	string key;
	if( check_xml_attr )
	{
		key="<xmlattr>.";
	}
	if( kv.second.get(key+"type","") != string("") )
	{
		type = kv.second.get(key+"type","");
	}

	if( kv.second.get(key+"under","") != string("") )
	{
		return std::make_pair<const string, const string>( "under", kv.second.get(key+"under","") );
	}
	else if( kv.second.get(key+"over","") != string() )
	{
		return std::make_pair<const string, const string>( "over", kv.second.get(key+"over","") );
	}
	else if( kv.second.get(key+"equal","") != string() )
	{
		return std::make_pair<const string, const string>( "equal", kv.second.get(key+"equal","") );
	}
	
	return std::make_pair<const string, const string>( "", "" );
}

void Client::add_clients(const std::string xml_file )
{
	using boost::property_tree::ptree;
	ptree pt;

	ifstream file(xml_file);
	if( file.is_open())
	{
	   try 
           {
    		read_xml(file, pt);
		BOOST_FOREACH( ptree::value_type const& v, pt.get_child("clients") ) 
		{
			const string client_key( v.second.get("<xmlattr>.key","") );
			const string client_email( v.second.get<string>("<xmlattr>.mail","") );

			if( client_key == "" || client_email == "" )
			{
				// throw , log
				continue;
			}
			Client *client = new Client( client_key , client_email );
	
			ptree children = v.second;
			for (const auto& kv : children)
			{
				if( kv.first.data() != string("alert") )
				{
					continue;
				}
				string type;
				std::pair<string, string> alert = get_alert_value(kv, type,true);
				if( type == string() )
				{
					alert = get_alert_value(kv,type,false);
				}
				if( type == string() )
				{
					// log throw
					continue;
				}
				
				{
					const EventType& eve = std::pair< Alert , Condition > ( get_alert_type( type ), get_condition( alert.first ) );
					client->push_events( TriggerEvent( eve , alert.second) ); // condition and value( stored as string )
				}
			}
			clients.push_back( client );
			client_map.insert( std::pair < const string, Client * const > ( client_key, client ) );

		}		
	   }catch(std::exception& e) {
		cout << e.what() << "\n";
	   }
	   file.close();	
	}
	else
	{
		//log error
	}
	
}
