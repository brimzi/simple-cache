#include "server.h"
#include <boost/bind.hpp>
#include <iostream>
#include "ConnectionManager.h"
#include "connection.h"

void Server::startAccept()
{
	newConnection_.reset(new connection(io_service_, connectionManager_));
	acceptor_.async_accept(newConnection_->socket(),boost::bind(&Server::handleAccept, this,
			boost::asio::placeholders::error));
}

void Server::handleAccept(const boost::system::error_code & error)
{
	if (!error)
	{
		auto endp=newConnection_.get()->socket().remote_endpoint();
		std::cout << "new connection from " <<endp.address() 
			      <<" at port " <<endp.port()<<std::endl;
		connectionManager_.start(newConnection_);
	}

	startAccept();
}
