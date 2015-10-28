#pragma once
#include <boost/asio.hpp>
#include "Connection.h"
#include "ConnectionManager.h"
//TODO Remove namespace in header
using namespace boost::asio::ip;
class Server {

public:
	Server(boost::asio::io_service& io_service, uint32_t port, int maxCacheSize);
	

private:
	void startAccept();
	void handleAccept(const boost::system::error_code& error);
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	ConnectionPtr newConnection_;
	ConnectionManager connectionManager_;
	StorageProvider storageProvider_;
	int maxDataSize_;
	int maxKeySize_;

};
