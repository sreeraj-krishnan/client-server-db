#include "logger.h"
#include "context.h"

#include <string>

using namespace std;

/*
 trace = 0,
    debug = 1,
    info = 2,
    warn = 3,
    err = 4,
    critical = 5,
    off = 6

*/

std::shared_ptr<spdlog::logger>& Logger::log()
{
	// 5 MB rotating 3 files
	static string filename = Context::get_config()["debug.logfile"];
	
	static bool init(false);
	static auto rotating_logger = spdlog::rotating_logger_mt(Context::get_config()["logger_name"] , filename, 1048576 * 5, 3);
	if ( !init )
	{
		rotating_logger->flush_on( spdlog::level::trace );
		if( Context::get_config()["debug.level"] == "info" )
		{
			spdlog::set_level( spdlog::level::info );
		}
		else if(  Context::get_config()["debug.level"] == "debug" )
		{
			spdlog::set_level( spdlog::level::debug );
		}
		else if( Context::get_config()["debug.level"] == "warn" )
		{
			spdlog::set_level( spdlog::level::warn );
		}		
		else if( Context::get_config()["debug.level"] == "err" )
		{
			spdlog::set_level( spdlog::level::err );
		}		
		else if( Context::get_config()["debug.level"] == "critical" )
		{
			spdlog::set_level( spdlog::level::critical );
		}		
		init = true;
	}
	return rotating_logger;
}
