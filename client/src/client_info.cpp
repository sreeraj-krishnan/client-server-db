#include "client_info.h"
#include "stdlib.h"
#include "stdio.h"
//#include "string.h"
#include <unistd.h>
#include <chrono>

#include <iostream>
#include <fstream>
#include <map>
#include <boost/asio/ip/host_name.hpp>
#include <google/protobuf/util/time_util.h>

#include "machine.pb.h"

using namespace client;
using namespace std;


ClientInfo::ClientInfo(): machine(0) 
{
	machine = new machine::Machine();
	//assert( machine == 0 );
}

ClientInfo::~ClientInfo()
{
}

void ClientInfo::get_mem_info()
{
	long mem_total(0), mem_available(0), mem_free(0);

	std::string delimiter(":");
	std::string token;
	std::string line;
	
	size_t pos = 0;
	
	ifstream file("/proc/meminfo");

	mem_total = mem_available = mem_free = 0;

	if ( file.is_open() )
	{
		while( getline( file, line ) || (  ! ( mem_total && mem_available && mem_free ) ) )
		{
	  		while ((pos = line.find(delimiter)) != std::string::npos)
	  		{
				// MemTotal ,MemFree , MemAvailable
				token = line.substr(0, pos);
				line.erase(0,pos+delimiter.length());
				if ( token.length() == 0 )
				{
					continue;
				}
				if( token == "MemTotal")
				{
					mem_total = stol(line);
				}
				else if( token == "MemFree" )
				{
					mem_free = stol(line);
				}
				else if ( token == "MemAvailable" )
				{
					mem_available = stol(line);
				}
			}
		}	
		file.close();
	}

	machine::Machine::Memory* mem =  new  machine::Machine::Memory();
	mem->set_mem_total( mem_total );
	mem->set_mem_free( mem_free );
	mem->set_mem_available( mem_available );
	machine->set_allocated_memory ( mem );

}

long ClientInfo::get_cpu_count( )
{
	std::string delimiter(" ");
	std::string token;
	std::string line;

	size_t pos = 0;

	int cpu_count(0);
    	int token_count(0);
	
	ifstream file("/proc/stat");
    	if ( !file.is_open() )
	{	
		//log error
		return 0;
	}
	
	while( getline( file, line ) )
	{
	  while ((pos = line.find(delimiter)) != std::string::npos)
	  {
		token = line.substr(0, pos);
		line.erase(0,pos+delimiter.length());
		if ( token.length() == 0 )
		{
			continue;
		}
		if ( token_count == 0 )
		{
			if ( token.length() >= 3 && token.substr(0,3) == string("cpu") )
			{
				cpu_count++;
			}
			break;
		}
	  }
	  token_count = 0;
	}
	file.close();
	return cpu_count - 1; // cpu , cpu0 , cpu1 => 2 cpu
}

void ClientInfo::get_cpu_idle_values()
{
    long idle_value1(0), idle_value2(0);
    string line;

    ifstream file("/proc/stat");
    if ( file.is_open() )
    {
	while( getline(file, line) || idle_value2  )
	{
		if ( !idle_value1 )
		{
			idle_value1 = get_cpu_idle_value(line);
		}
		if ( idle_value1 )
		{
			sleep(1);
			file.seekg(0);
			getline(file, line);
			idle_value2 = get_cpu_idle_value(line);
			if ( idle_value2 )
			{
				break;
			}
		}
	}
	file.close();
   }
   else
   {	
	//log error
   }
   long cpu_count = get_cpu_count();
   float cpu_idle =  100.0 * ( (float) ( idle_value2 - idle_value1 ) / ((float)cpu_count * 100) );
   float cpu_use = 100 - cpu_idle;        
   machine::Machine::CPU*  cpu  =  new machine::Machine::CPU() ;
   cpu->set_idle_time( cpu_idle );
   cpu->set_use_time( cpu_use );
   machine->set_allocated_cpu( cpu );
   
}

long ClientInfo::get_cpu_idle_value( string line )
{
	std::string delimiter(" ");
	std::string token0;
	std::string token;

	size_t pos = 0;

	int token_count(0);
	int cpu_count(0);
	long idle_value(0);

	while ((pos = line.find(delimiter)) != std::string::npos)
	{
		token = line.substr(0, pos);
		line.erase(0,pos+delimiter.length());
		if ( token.length() == 0 )
		{
			continue;
		}
		if ( token_count == 0 )
		{
			token0 = token;			
			
			if ( token != string("cpu") )
			{
				return 0;
			}
		}
		
		if ( token_count == 4 && token0 == string("cpu") )
		{
			idle_value = stol( token );
			break;
		}
		token_count++;
	}
	return idle_value;
}


void ClientInfo::get_process_info()
{
    int processes(0);
    int blocked(0);
    int running(0);
    
    string delimiter(" ");
    string token;
    string line;
	
    size_t pos = 0;

    ifstream file("/proc/stat");
    if ( file.is_open() )
    {
	while( getline(file, line) )
	{
	  	while ((pos = line.find(delimiter)) != std::string::npos)
	  	{
			token = line.substr(0, pos);
			line.erase(0,pos+delimiter.length());
			if ( token.length() == 0 )
			{
				continue;
			}
			if( token == "processes")
			{
				processes = stoi(line);
			}
			else if( token == "procs_running" )
			{
				running = stoi(line);
			}
			else if ( token == "procs_blocked" )
			{
				blocked = stoi(line);
			}
			else 
			{
				break;
			}
		}
	}
	file.close();
   }
   else
   {	
	//log error
   }
   
   machine::Machine::Process* process =  new  machine::Machine::Process() ;
   process->set_total( processes );
   process->set_running( running );   
   process->set_blocked( blocked );

   machine->set_allocated_process ( process );

}

void ClientInfo::get_system_info()
{
	machine::Machine::SystemInfo* si = new machine::Machine::SystemInfo();
	si->set_system_ip( "ip" );
	si->set_system_name( boost::asio::ip::host_name() );
	machine->set_allocated_systeminfo ( si );
}

MachineInfoPtr ClientInfo::get_machine_info()
{
   get_cpu_idle_values();
   get_mem_info();
   get_process_info();
   get_system_info();
	
   using namespace google::protobuf::util;
   machine->set_allocated_last_updated( new google::protobuf::Timestamp ( TimeUtil::GetCurrentTime() )  );

   MachineInfoPtr machine_info( new machine::MachineInfo() );
   machine_info->set_allocated_machine( machine );
   return machine_info;

}


