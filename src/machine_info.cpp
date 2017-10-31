#include "machine_info.h"

MachineInfo::MachineInfo(string _client_key ) : m_client_key( _client_key ) 
{
	
}

void MachineInfo::get_machine_info( string& _details )
{	
	
}

void MachineInfo::send_data( string server_ip, string port, string url)
{
	
}

void MachineInfo::set_client_key(const string& _key)
{
	m_client_key = _key;	
}

string MachineInfo::get_client_key() const 
{ 
	return m_client_key; 
}


