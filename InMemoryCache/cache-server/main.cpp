#include <iostream>
#include "server.h"

int main(int argc, char* argv[]) {

	//boost::shared_ptr<boost::container::vector<boost::uint8_t>> v;
	//v.reset(new boost::container::vector<boost::uint8_t>(3));
	//v->push_back(2);
	//v->push_back(1);

	//int c = v->at(3);

	//std::cout<<"V is "<<c<<std::endl;

	//int k(0);
	//std::cin >> k;
	//if (k||1)return 0;
	boost::asio::io_service io_service;
	Server serv(io_service, 8082,1020*1024*8);
	
	io_service.run();
	int k(0);
	std::cin>>k;
	return 0;
}