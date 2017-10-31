#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

#include "machine.pb.h"

using namespace std;



namespace client {
	typedef boost::shared_ptr < machine::MachineInfo > MachineInfoPtr;

	class ClientInfo
	{
		public:
			ClientInfo();
			~ClientInfo();

			machine::MachineInfo* get_machine_info();

		protected:

			long get_cpu_count();
			void get_cpu_idle_values();
			long get_cpu_idle_value( string line );

			void get_mem_info();
			
			void get_process_info();
			void get_system_info();

		private:
			machine::Machine* machine;
			
	};
}
