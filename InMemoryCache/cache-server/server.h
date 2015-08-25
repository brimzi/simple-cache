#pragma once
#include <boost/asio.hpp>
#include "connection.h"
#include "ConnectionManager.h"

using namespace boost::asio::ip;
class Server {

public:
	Server(boost::asio::io_service& io_service, short port, int maxCacheSize);
	


private:
	void startAccept();
	void handleAccept(const boost::system::error_code& error);

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	ConnectionPtr newConnection_;
	ConnectionManager connectionManager_;
	StorageProvider storageProvider_;

};
