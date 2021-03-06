#pragma once

#include <string>
#include <odb/core.hxx>
#include <memory>
#include<iostream>

using namespace std;

#pragma db value
struct Memory 
{
    int mem_total;
    int mem_available;
    int mem_free;
    Memory(){}
    ~Memory(){}
    Memory(int total, int available, int free) : mem_total( total ), mem_available( available ), mem_free( free ) {}	
};

#pragma db value
struct CPU {
    float idle_time;
    float use_time;
    CPU(){}
    ~CPU(){}
    CPU( float idle, float used ) : idle_time( idle ), use_time( used ) {}
};

#pragma db value
struct Process {
        int total;
        int running;
        int blocked;
	Process(){}
	~Process(){}
	Process( int tot, int run, int block): total( tot ), running( run ), blocked ( block ) {}
};

#pragma db value
struct SystemInfo {
	string system_ip;
	string system_name;
	SystemInfo(){}
	~SystemInfo(){}
	SystemInfo( string ip, string name ) : system_ip( ip ), system_name( name ) {}
};

#pragma db object  
class machineinfo
{
public:
  machineinfo(){}
 
  machineinfo( const string& key, const Memory& mem, const CPU& _cpu ,const Process& proc , const SystemInfo& sys, const unsigned long& stamp) : client_key(key)  ,memory( new Memory( mem )) , cpu( new CPU( _cpu )) , process( new Process( proc )) , system_info( new SystemInfo(sys)), time_stamp( stamp ) {}

  virtual ~machineinfo(){}

  friend class odb::access;

  shared_ptr < Memory >& get_memory()  { return memory; } 
  shared_ptr < CPU >& get_cpu() { return cpu; } 
  shared_ptr < Process >& get_process() { return process; } 
  shared_ptr < SystemInfo >& get_system_info() { return system_info; } 

private:
  #pragma db id auto  
  unsigned long int id_; 
	
  string client_key;

  #pragma db value_not_null unordered  
  shared_ptr < Memory > memory;
   
  #pragma db value_not_null unordered  
  shared_ptr < CPU >  cpu;
  
  #pragma db value_not_null unordered  
  shared_ptr < Process > process ;
   
  #pragma db value_not_null unordered  
  shared_ptr < SystemInfo > system_info ;

  unsigned long int time_stamp;   

};
