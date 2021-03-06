#include "persistence.h"
#include "context.h"
#include "logger.h"

#include "machine.pb.h"
#include "machine_info.hxx"
#include "machine_info-odb.hxx"

#include <google/protobuf/util/time_util.h>

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>

#include <iostream>
#include <memory>

using namespace odb::core;

MachineInfoPtr Persistence::get_orm_object( const string& client_key, const std::string& client_data )
{
    machine::MachineInfo* info = new machine::MachineInfo();
    if ( info->ParseFromString( client_data ) )
    {
	CPU cpu( info->machine().cpu().idle_time() , info->machine().cpu().use_time());
        Memory mem( info->machine().memory().mem_total(),info->machine().memory().mem_available(), info->machine().memory().mem_free() );
        Process process ( info->machine().process().total(),  info->machine().process().running(),  info->machine().process().blocked() );
	SystemInfo sys( info->machine().systeminfo().system_ip(), info->machine().systeminfo().system_name() );

	using namespace google::protobuf::util;
	unsigned long int timestamp = TimeUtil::TimestampToSeconds( info->machine().last_updated() );
	return MachineInfoPtr ( new machineinfo( client_key, mem , cpu , process , sys , timestamp) ) ;
	
    }
    else
    {
	    Logger::log()->critical("Failed to deerialize and parse string from client : {}", client_key);
    }
    return MachineInfoPtr(0); 
}

void Persistence::write_client_info_db( const MachineInfoPtr& client_info )
{
	try 
	{
		Config& config = Context::get_config();
    		std::unique_ptr<database> db (new odb::pgsql::database ( config["db_username"],config["db_passwd"],config["db_name"]));
   		{
			transaction t (db->begin ());
			//t.tracer (stderr_tracer);
			unsigned long machine_id  = db->persist ( *(client_info.get()) );
		 	t.commit();
   		}
	}
	catch(const odb::exception& e)
	{
	    Logger::log()->critical("Failed to persist data tp database : {}", e.what());
	}	
}
