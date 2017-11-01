#pragma once

#include <map>
#include <string>

using namespace std;

typedef map<const string, const string > Config;

class Context
{
	public:
	Context( string config_file );

	static Config& get_config();

	private:
	static Config m_config;
};
