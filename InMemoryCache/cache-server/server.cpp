#include "server.h"
#include <boost/bind.hpp>
#include <iostream>
#include "ConnectionManager.h"
#include "connection.h"



Server::Server(boost::asio::io_service& io_service, uint8_t port, int maxCacheSize)
	: io_service_(io_service),	acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), storageProvider_(maxCacheSize)
	,maxDataSize_(1048576),maxKeySize_(256)
{
	startAccept();
}
void Server::startAccept()
{
	newConnection_.reset(new Connection(io_service_, connectionManager_, storageProvider_,maxDataSize_,maxKeySize_));
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
