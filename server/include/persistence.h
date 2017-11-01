
#pragma once

#include<boost/shared_ptr.hpp>
#include "machine_info.hxx"

typedef boost::shared_ptr < machineinfo > MachineInfoPtr;

class Persistence
{
	public:
		Persistence(){}
		~Persistence(){}

		static MachineInfoPtr get_orm_object( const string& key, const std::string& client_data);

		static void write_client_info_db( const MachineInfoPtr& client_info );
};
