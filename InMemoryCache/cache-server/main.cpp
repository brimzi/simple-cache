#include <iostream>
#include "server.h"
int main(int argc, char* argv[]) {

	boost::asio::io_service io_service;
	Server serv(io_service, 8082);
	
	io_service.run();
	int k(0);
	std::cin>>k;
	return 0;
}