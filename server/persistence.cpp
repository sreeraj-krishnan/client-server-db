#include "persistence.h"
#include "machine.pb.h"

#include <memory>
#include "machine_info.hxx"
#include "machine_info-odb.hxx"

#include <iostream>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include <odb/pgsql/database.hxx>

using namespace odb::core;

MachineInfoPtr Persistence::get_orm_object( const std::string& client_data )
{
    machine::MachineInfo* info = new machine::MachineInfo();
    if ( info->ParseFromString( client_data ) )
    {
	CPU cpu( info->machine().cpu().idle_time() , info->machine().cpu().use_time());
        Memory mem( info->machine().memory().mem_total(),info->machine().memory().mem_available(), info->machine().memory().mem_free() );
        Process process ( info->machine().process().total(),  info->machine().process().running(),  info->machine().process().blocked() );
	SystemInfo sys( info->machine().systeminfo().system_ip(), info->machine().systeminfo().system_name() );

	return MachineInfoPtr ( new machineinfo( mem , cpu , process , sys ) ) ;
        	
	
    }
    else
    {
	/// log error and throw
    }
    return MachineInfoPtr(0); 
}

void Persistence::write_client_info_db( const MachineInfoPtr& client_info )
{
	try 
	{
		// TODO read user name / passwd / db from configuration
    		std::unique_ptr<database> db (new odb::pgsql::database ("postgres","postgres","mydb"));
   		{
			transaction t (db->begin ());
			//t.tracer (stderr_tracer);
			unsigned long machine_id  = db->persist ( *(client_info.get()) );
		 	t.commit();
   		}
	}
	catch(const odb::exception& e)
	{
		std::cerr<< e.what();
		/// throw 
	}	
}
