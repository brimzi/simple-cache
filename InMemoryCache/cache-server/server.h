#pragma once
#include <boost/asio.hpp>
#include "connection.h"
#include "ConnectionManager.h"

using namespace boost::asio::ip;
class Server {

public:
	Server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		startAccept();
	}


private:
	void startAccept();
	void handleAccept(const boost::system::error_code& error);

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
	ConnectionPtr newConnection_;
	ConnectionManager connectionManager_;

};
