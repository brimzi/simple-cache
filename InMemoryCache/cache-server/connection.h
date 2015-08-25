#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/container/vector.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "StorageProvider.h"

using namespace boost::asio::ip;
class ConnectionManager;

enum OpCodes
{
	Set,Get,Delete,Status,Data
};

enum ErrorCodes 
{
	Ok,NoSuchKey,KeyTooBig,DataTooBig,OtherErrors
};


class Connection:public boost::enable_shared_from_this<Connection> {
public:
	explicit Connection(boost::asio::io_service& io_service, ConnectionManager& manager,  StorageProvider& storage);

	tcp::socket& socket();

	void start();
	
	void stop();


private:
	void handle_readOpcode(const boost::system::error_code& error,unsigned int bytes_transferred);

	void handle_readKey(const boost::system::error_code& error, boost::uint16_t byteTransferred, boost::uint16_t expectedkeySize);

	void handle_ReadRawDataHeader(const boost::system::error_code& error, unsigned int bytes_transferred);

	void handleReadRawData(const boost::system::error_code& error, boost::uint32_t bytes_transferred, boost::uint32_t expected);

	void handleWriteReqResponse(const boost::system::error_code& error);

	void startClientRequestedOp();

	void startReadKey(boost::uint16_t keySize);

	void startSetDataOperation();

	void startGetOperation();

	void startDeleteOperation();

	void sendResponseAndStart(boost::shared_ptr<boost::container::vector<boost::uint8_t>> resp);



	

	void sendStatusAndRestart(ErrorCodes code, std::string message);

	void retriveAndSendData(boost::uint16_t key);

	void deleteData(boost::uint16_t key);
	tcp::socket socket_;

	boost::uint8_t opcode_;
	boost::container::vector<boost::uint8_t> key_;

	ConnectionManager& connectionManager_;
	StorageProvider& storageProvider_;
	boost::shared_ptr<boost::container::vector<boost::uint8_t>> data_;

	
	
	
};

typedef boost::shared_ptr<Connection> ConnectionPtr;