#include "connection.h"
#include <iostream>
#include <boost/bind.hpp>
#include "ConnectionManager.h"
#include "StorageProvider.h"

using namespace std;

Connection::Connection(boost::asio::io_service & io_service, ConnectionManager& manager,StorageProvider& storage):socket_(io_service),
										connectionManager_(manager),storageProvider_(storage)
{
}

tcp::socket & Connection::socket()
{
	return socket_;
}

void Connection::start()
{
	data_.reset(new boost::container::vector<boost::uint8_t>);
	boost::asio::async_read(socket_, (*data_.get()), boost::asio::transfer_at_least(3),
		 boost::bind(&Connection::handle_readOpcode, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

}

void Connection::startReadKey(boost::uint16_t keySize)
{
	cout << "startReadKey " << endl;
	key_.clear();//start fresh.TODO consider cost of this
	boost::asio::async_read(socket_, key_, boost::asio::transfer_at_least(keySize),
		boost::bind(&Connection::handle_readKey, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, keySize));
}

void Connection::startSetDataOperation()
{
	cout << "startSet " << endl;
	(*data_.get()).clear();
	boost::asio::async_read(socket_, (*data_.get()), boost::asio::transfer_at_least(4),
		boost::bind(&Connection::handle_ReadRawDataHeader, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

}

void Connection::startGetOperation()
{
	boost::shared_ptr<boost::container::vector<boost::uint8_t>> res = storageProvider_.get(key_);
	(*res.get()).insert((*res.get()).begin(),Data);//TODO consider the performance hit here,maybe I use a deque??
	
	sendResponseAndStart(res);

}

void Connection::startDeleteOperation()
{
	storageProvider_.remove(key_);//TODO may wanna consider having a status,incase of error
	sendStatusAndRestart(Ok, "OK");
}



void Connection::stop()
{
	socket_.close();
	std::cout << "Connection Closed: " << socket_.remote_endpoint().address()<<" at "<< socket_.remote_endpoint().port() <<std::endl;
}

void Connection::handle_readOpcode(const boost::system::error_code& error, unsigned int bytes_transferred)
{
	if (!error) 
	{
		opcode_ = (*data_.get())[0];
		boost::uint16_t keySize= ((*data_.get())[1] << 8)| (*data_.get())[2];//TODO handle endianess
		std::cout << "Opcode Received: " << opcode_ << std::endl;
		startReadKey(keySize);
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void Connection::handle_readKey(const boost::system::error_code& error, boost::uint16_t byteTransferred, boost::uint16_t expectedkeySize)
{
	if (!error)
	{
		if (byteTransferred != expectedkeySize)
		{
			sendStatusAndRestart(OtherErrors, "Data sent is not equal to data expected");
		}
		else 
		{
			startClientRequestedOp();
		}
		
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void Connection::handle_ReadRawDataHeader(const boost::system::error_code & error, unsigned int bytes_transferred)
{
	if (!error)
	{
		//TODO handle big endian machine
		boost::uint32_t size = ((*data_.get())[0] << 24) | ((*data_.get())[1] << 16) | ((*data_.get())[2] << 8)
			| ((*data_.get())[3]);
		(*data_.get()).clear();
		boost::asio::async_read(socket_, (*data_.get()), boost::asio::transfer_at_least(size),
			boost::bind(&Connection::handleReadRawData, shared_from_this(),
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,size));
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void Connection::handleReadRawData(const boost::system::error_code& error, boost::uint32_t bytes_transferred, boost::uint32_t expected)
{
	if (!error)
	{
		if (bytes_transferred != expected)
		{
			sendStatusAndRestart(OtherErrors, "Data size sent is not equal to data expected");
		}
		else
		{
			storageProvider_.save(key_,data_);//TODO may wanna consider having a status,incase of error
			sendStatusAndRestart(Ok, "OK");
		}

	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void Connection::handleWriteReqResponse(const boost::system::error_code & error)
{
	if (!error)
	{
		start();
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void Connection::startClientRequestedOp() 
{
	switch (opcode_) 
	{
	case 0:
		startSetDataOperation();
		break;
	case 1:
		startGetOperation();
		break;
	case 2:
		startDeleteOperation();
		break;
	case 3:
		sendStatusAndRestart(OtherErrors, "Opcode you sent is not valid in this case");
		break;
	case 4:
		sendStatusAndRestart(OtherErrors, "Opcode you sent is not valid in this case");
		break;
	default: 
	{
		sendStatusAndRestart(OtherErrors, "Opcode you sent does not exist");
	}
	}
}






void Connection::sendStatusAndRestart(ErrorCodes code,std::string message) {
	boost::shared_ptr<boost::container::vector<boost::uint8_t>> resp(new boost::container::vector<boost::uint8_t>);
	resp.get()->push_back(Status);
	resp.get()->push_back(0);
	resp.get()->push_back(0);
	resp.get()->push_back(0);
	resp.get()->push_back(code);
	for (auto letter : message)
	{
		resp.get()->push_back(letter);
	}
	sendResponseAndStart(resp);
}

void Connection::sendResponseAndStart(boost::shared_ptr<boost::container::vector<boost::uint8_t>> resp)
{
	boost::asio::async_write(socket_,(*resp.get()), boost::bind(&Connection::handleWriteReqResponse,
		shared_from_this(),boost::asio::placeholders::error));
}

void Connection::retriveAndSendData(boost::uint16_t key)
{

}

void Connection::deleteData(boost::uint16_t key)
{
}


