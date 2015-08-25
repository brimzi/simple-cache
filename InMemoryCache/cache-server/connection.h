#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/container/vector.hpp>
#include <boost/enable_shared_from_this.hpp>


using namespace boost::asio::ip;
class ConnectionManager;

class connection:public boost::enable_shared_from_this<connection> {
public:
	explicit connection(boost::asio::io_service& io_service, ConnectionManager& manager );

	tcp::socket& socket();

	void start();
	
	void stop();


private:
	void handle_readOpcode(const boost::system::error_code& error,unsigned int bytes_transferred);

	void handle_readKey(const boost::system::error_code& error, boost::uint16_t byteTransferred, boost::uint16_t expectedkeySize);

	void handle_ReadRawDataHeader(const boost::system::error_code& error, unsigned int bytes_transferred);

	void handleReadRawData(const boost::system::error_code& error, boost::uint32_t bytes_transferred, boost::uint32_t expected);

	void startClientRequestedOp();

	void startReadKey(boost::uint16_t keySize);

	void startSetDataOperation();

	void retriveAndSendData(boost::uint16_t key);

	void deleteData(boost::uint16_t key);
	tcp::socket socket_;
	//boost::array<boost::uint8_t,512>  data_;//TODO turn it into a dynamic structure
	//boost::uint16_t key_;

	boost::uint8_t opcode_;
	boost::container::vector<boost::uint8_t> key_;
	boost::container::vector<boost::uint8_t> data_;
	ConnectionManager& connectionManager_;
	
	boost::shared_ptr<boost::container::vector<boost::uint8_t>> data_;

	
	
	
};

typedef boost::shared_ptr<connection> ConnectionPtr;