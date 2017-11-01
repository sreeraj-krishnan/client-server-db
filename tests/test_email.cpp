#include "smtpclient.h"
#include <iostream>

int main(int argc, char** argv)
{

	SMTPClient mailc("localhost",25,"m1@example.com","miner1");
	int rc  = mailc.Send("m1@example.com","m1@example.com","subject","Hello from C++ SMTP Client!");
	std::cout << "rc : " << rc << "\n";
	return 0;
}
