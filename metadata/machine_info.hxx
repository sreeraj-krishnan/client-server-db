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
    int mem_avaialable;
    int mem_free;
    Memory(){}
    ~Memory(){}
    Memory(int total, int available, int free) : mem_total( total ), mem_avaialable( available ), mem_free( free ) {}	
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
 
  machineinfo( const Memory& mem, const CPU& _cpu ,const Process& proc , const SystemInfo& sys) : memory( new Memory( mem )) , cpu( new CPU( _cpu )) , 
				process( new Process( proc )) , system_info( new SystemInfo(sys)) {}

  virtual ~machineinfo(){}

  friend class odb::access;


private:
  #pragma db id auto  
  unsigned long int id_; 

  #pragma db value_not_null unordered  
  shared_ptr < Memory > memory;
   
  #pragma db value_not_null unordered  
  shared_ptr < CPU >  cpu;
  
  #pragma db value_not_null unordered  
  shared_ptr < Process > process ;
   
  #pragma db value_not_null unordered  
  shared_ptr < SystemInfo > system_info ;
  
};
