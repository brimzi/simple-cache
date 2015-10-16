#include "Connection.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio/buffer.hpp>
#include "ConnectionManager.h"
#include "StorageProvider.h"

using namespace std;

Connection::Connection(boost::asio::io_service & io_service, ConnectionManager& manager, StorageProvider& storage, int maxData, int maxKey) :socket_(io_service),
connectionManager_(manager), storageProvider_(storage), maxDataSize_(maxData), maxKeySize_(maxKey), opcode_(0)
{

}

tcp::socket & Connection::socket()
{
	return socket_;
}

void Connection::start()
{
	data_.reset(new std::vector<boost::uint8_t>(3));
	auto self(shared_from_this());
	boost::asio::async_read(socket_, boost::asio::buffer(*data_.get()), boost::asio::transfer_at_least(3),
		[this, self](const boost::system::error_code& error, uint32_t bytes_transferred)
	{
		if (!error)
		{
			opcode_ = data_->at(0);
			boost::uint16_t keySize = toInt16(*data_.get(), 1);
			if (keySize <= maxKeySize_)
			{
				startReadKey(keySize);
			}
			else
			{
				sendStatusAndRestart(KeyTooBig, "supplied key is too big.Maximum allowed key size is: " + maxKeySize_);
			}
		}
		else
		{
			connectionManager_.stop(self);
		}
	}

	);

	/*boost::asio::async_read(socket_, boost::asio::buffer(*data_.get()), boost::asio::transfer_at_least(3),
		boost::bind(&Connection::handleReadOpcode, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));*/

}

void Connection::startReadKey(boost::uint16_t& keySize)
{
	key_.resize(keySize);// .clear();//start fresh.TODO consider cost of this
	auto self = shared_from_this();
	boost::asio::async_read(socket_, boost::asio::buffer(key_), boost::asio::transfer_at_least(keySize),
		[this, self, keySize](const boost::system::error_code& error, boost::uint16_t byteTransferred)
	{
		if (!error)
		{
			if (byteTransferred != keySize)
			{
				sendStatusAndRestart(OtherErrors, "Data sent is not equal to the expected size");
			}
			else
			{
				startClientRequestedOp();
			}
		}
		else
		{
			connectionManager_.stop(self);
		}
	}
	
	);

	/*boost::asio::async_read(socket_, boost::asio::buffer(key_), boost::asio::transfer_at_least(keySize),
		boost::bind(&Connection::handleReadKey, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, keySize));*/
}

void Connection::startSetDataOperation()
{
	data_.reset(new std::vector<boost::uint8_t>(4));
	auto self = shared_from_this();
	boost::asio::async_read(socket_, boost::asio::buffer(*data_.get()), boost::asio::transfer_at_least(4),
		[this, self](const boost::system::error_code & error, uint32_t bytes_transferred)
	{
		if (!error)
		{
			uint32_t size = toInt32(*data_.get(), 0);
			if (size <= maxDataSize_)
			{
				data_.reset(new std::vector<boost::uint8_t>(size));
				boost::asio::async_read(socket_, boost::asio::buffer(*data_.get()), boost::asio::transfer_at_least(size),
					boost::bind(&Connection::handleReadRawData, self,
						boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, size));
			}
			else
			{
				sendStatusAndRestart(DataTooBig, "The data sent is too big.Maximum data allowed is: " + maxDataSize_);
			}

		}
		else
		{
			connectionManager_.stop(self);
		}
	});

	/*boost::asio::async_read(socket_, boost::asio::buffer(*data_.get()), boost::asio::transfer_at_least(4),
		boost::bind(&Connection::handleReadRawDataHeader, shared_from_this(),
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));*/

}

void Connection::startGetOperation()
{
	data_ = storageProvider_.get(key_);
	if (data_)
	{

		uint8_t header[] = { Data, data_->size() ,data_->size() >> 8,data_->size() >> 16,data_->size() >> 24 };//TODO verify this

		std::vector<boost::asio::mutable_buffer> bufs = { boost::asio::buffer(header),boost::asio::buffer(*data_.get()) };

		sendResponseAndStart(bufs, data_->size() + 4);
	}
	else
	{
		boost::shared_ptr<std::vector<uint8_t>> r;
		data_ = r;
		sendStatusAndRestart(NoSuchKey, "Requested data not in cache");
	}
}

void Connection::startDeleteOperation()
{
	if (storageProvider_.remove(key_))
	{
		sendStatusAndRestart(Ok, "OK");
	}
	else
	{
		sendStatusAndRestart(OtherErrors, "Supplied key not found in cache");
	}

}



void Connection::stop()
{
	std::string address = socket_.remote_endpoint().address().to_string() + ":" + std::to_string(socket_.remote_endpoint().port());
	socket_.close();
	std::cout << "Connection Closed: " << address << std::endl;
}

void Connection::handleReadOpcode(const boost::system::error_code& error, uint32_t bytes_transferred)
{
	if (!error)
	{
		opcode_ = data_->at(0);
		boost::uint16_t keySize = toInt16(*data_.get(), 1);
		if (keySize <= maxKeySize_)
		{
			startReadKey(keySize);
		}
		else
		{
			sendStatusAndRestart(KeyTooBig, "supplied key is too big.Maximum allowed key size is: " + maxKeySize_);
		}
	}
	else
	{
		connectionManager_.stop(shared_from_this());
	}
}

void Connection::handleReadKey(const boost::system::error_code& error, boost::uint16_t byteTransferred, boost::uint16_t expectedkeySize)
{
	if (!error)
	{
		if (byteTransferred != expectedkeySize)
		{
			sendStatusAndRestart(OtherErrors, "Data sent is not equal to the expected size");
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

void Connection::handleReadRawDataHeader(const boost::system::error_code & error, uint32_t bytes_transferred)
{
	if (!error)
	{
		uint32_t size = toInt32(*data_.get(), 0);
		if (size <= maxDataSize_)
		{
			data_.reset(new std::vector<boost::uint8_t>(size));
			boost::asio::async_read(socket_, boost::asio::buffer(*data_.get()), boost::asio::transfer_at_least(size),
				boost::bind(&Connection::handleReadRawData, shared_from_this(),
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, size));
		}
		else
		{
			sendStatusAndRestart(DataTooBig, "The data sent is too big.Maximum data allowed is: " + maxDataSize_);
		}

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
			storageProvider_.save(key_, data_);
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
	case 4:
		sendStatusAndRestart(OtherErrors, "Opcode you sent is not valid in this case");
		break;
	default:
	{
		sendStatusAndRestart(OtherErrors, "Opcode you sent does not exist");
	}
	}
}

void Connection::sendStatusAndRestart(ErrorCodes code, std::string message) {
	data_.reset(new std::vector<uint8_t>);
	data_.get()->push_back(Status);
	data_.get()->push_back(code);
	data_.get()->push_back(0);
	data_.get()->push_back(0);
	data_.get()->push_back(0);
	for (auto letter : message)
	{
		data_.get()->push_back(letter);
	}
	std::vector<boost::asio::mutable_buffer> bufs = { boost::asio::buffer(*data_.get()) };
	sendResponseAndStart(bufs, data_->size());
}

void Connection::sendResponseAndStart(std::vector<boost::asio::mutable_buffer>& resp, uint32_t size)
{
	boost::asio::async_write(socket_, resp, boost::asio::transfer_at_least(size), boost::bind(&Connection::handleWriteReqResponse,
		shared_from_this(), boost::asio::placeholders::error));
}


uint32_t Connection::toInt32(const std::vector<uint8_t>& intBytes, uint32_t start)
{
	return (intBytes[start + 3] << 24) | (intBytes[start + 2] << 16) | (intBytes[start + 1] << 8) | intBytes[start];
}

uint16_t Connection::toInt16(const std::vector<uint8_t>& intBytes, uint32_t start)
{
	return  (intBytes[start + 1] << 8) | intBytes[start];
}


