#include <iostream>
#include "Server.h"

using namespace std;
int main(int argc, char* argv[]) {

	try
	{
		if (argc < 3)
		{
			cerr << "you entered " << argc -1 << " arguments\n";
			cerr << "Usage: SimpleCacheServer <port> <max-storage in bytes>\n";
			cerr << "Press return to exit program " << endl;
			cin.get();
			return 1;
		}
		
		int port = stoi(argv[1]);
		int maxSize = stoi(argv[2]);
		//TODO check that maxsize > size of file
		cout << "starting server listening at port:" << port << " with maximum storage set to " << maxSize << " bytes" << endl;
		boost::asio::io_service io_service;
		Server serv(io_service, port, maxSize);
		io_service.run();
	
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}
	
	return 0;
}