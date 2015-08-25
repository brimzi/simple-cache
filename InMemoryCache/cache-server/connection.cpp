#include "connection.h"
#include <iostream>
#include <boost/bind.hpp>
#include "ConnectionManager.h"
#include "StorageProvider.h"

using namespace std;

connection::connection(boost::asio::io_service & io_service, ConnectionManager& manager):socket_(io_service),
										connectionManager_(manager)
{
}

tcp::socket & connection::socket()
{
	return socket_;
}

void connection::start()
{
	data_.reset(new boost::container::vector<boost::uint16_t>);
	boost::asio::async_read(socket_, data_, boost::asio::transfer_at_least(3),
		 boost::bind(&connection::handle_readOpcode, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));

}

void connection::startReadKey(boost::uint16_t keySize)
{
	cout << "startReadKey " << endl;
	key_.clear();//start fresh.TODO consider cost of this
	boost::asio::async_read(socket_, key_, boost::asio::transfer_at_least(keySize),
		boost::bind(&connection::handle_readKey, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, keySize));
}

void connection::startSetDataOperation()
{
	cout << "startSet " << endl;
	(*data_.get()).clear();
	boost::asio::async_read(socket_, data_,
		boost::bind(&connection::handle_ReadRawDataHeader, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

}

void connection::stop()
{
	socket_.close();
	std::cout << "Connection Closed: " << socket_.remote_endpoint().address()<<" at "<< socket_.remote_endpoint().port() <<std::endl;
}

void connection::handle_readOpcode(const boost::system::error_code& error, unsigned int bytes_transferred)
{
	if (!error) 
	{
		//std::string keyString(header_.begin()+1, header_.end()+3);
		//boost::uint8_t opcode = std::stoi(opcodeString);
		opcode_ = (*data_.get())[0];
		boost::uint16_t keySize= ((*data_.get())[1] << 8)| (*data_.get())[2];
		std::cout << "Opcode Received: " << opcode_ << std::endl;
		startReadKey(keySize);
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void connection::handle_readKey(const boost::system::error_code& error, boost::uint16_t byteTransferred, boost::uint16_t expectedkeySize)
{
	if (!error)
	{
		(*data_.get()).clear();//we will reuse the same object
		boost::asio::async_read(socket_, data_, boost::asio::transfer_at_least(4),
			boost::bind(&connection::handle_ReadRawDataHeader, shared_from_this(),
				boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void connection::handle_ReadRawDataHeader(const boost::system::error_code & error, unsigned int bytes_transferred)
{
	if (!error)
	{
		//TODO handle big endian machine
		boost::uint32_t size = ((*data_.get())[0] << 24) | ((*data_.get())[1] << 16) | ((*data_.get())[2] << 8)
			| ((*data_.get())[3]);
		(*data_.get()).clear();
		boost::asio::async_read(socket_, data_, boost::asio::transfer_at_least(size),
			boost::bind(&connection::handleReadRawData, shared_from_this(),
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,size));
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void connection::handleReadRawData(const boost::system::error_code& error, boost::uint32_t bytes_transferred, boost::uint32_t expected)
{
	if (!error)
	{
		if (bytes_transferred != expected)
		{
			//Send Error
		}
		else
		{
			storageProvider_.save(key_,data_);
		}
		//We assume data will be read in one go so we start again
		start();
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void connection::startClientRequestedOp() 
{
	switch (opcode_) 
	{
	case 0:
		startSetDataOperation();
		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	default: 
	{

	}
	}
}

void connection::retriveAndSendData(boost::uint16_t key)
{

}

void connection::deleteData(boost::uint16_t key)
{
}


