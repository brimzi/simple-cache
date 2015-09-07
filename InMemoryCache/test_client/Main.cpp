#include <iostream>
#include <boost/asio.hpp>
#include <vector>
using boost::asio::ip::tcp;


void sendData(tcp::socket& socket,std::string& key,std::string& data)
{
	uint8_t ksize1 = key.size();
	uint8_t ksize2= key.size() >> 8;
	uint8_t header[] = { 0,ksize1,ksize2 };
	socket.send(boost::asio::buffer(header));

	//uint8_t key[] = { 0,1,2,3,4,5,6,7,8,9 };

	socket.send(boost::asio::buffer(key));

	uint8_t dsize4 = data.size()>>24;
	uint8_t dsize3 = data.size() >> 16;
	uint8_t dsize2 = data.size() >> 8;
	uint8_t dsize1 = data.size();

	uint8_t data_header[] = { dsize1,dsize2,dsize3,dsize4 };
	socket.send(boost::asio::buffer(data_header));

	//uint8_t data[] = { 1,2,20,1,2,4,5,60,56,12 };
	socket.send(boost::asio::buffer(data));

	uint8_t result[128];
	int read = socket.read_some(boost::asio::buffer(result));
}

void deleteData(tcp::socket& socket, std::string& key) 
{
	uint8_t ksize1 = key.size() ;
	uint8_t ksize2 = key.size() >> 8;
	uint8_t header[] = { 2,ksize1,ksize2 };
	socket.send(boost::asio::buffer(header));

	socket.send(boost::asio::buffer(key));

	uint8_t dataheader[5];
	int read = boost::asio::read(socket, boost::asio::buffer(dataheader, 5), boost::asio::transfer_at_least(5));

	const int size = dataheader[4] << 24 | dataheader[3] << 16 | dataheader[2] << 8 | dataheader[1];

	std::vector<uint8_t> data(size);
	int read2 = boost::asio::read(socket, boost::asio::buffer(data), boost::asio::transfer_at_least(size));
}

std::string getData(tcp::socket& socket, std::string& key)
{
	uint8_t ksize1 = key.size() ;
	uint8_t ksize2 = key.size() >> 8;
	uint8_t header[] = { 1,ksize1,ksize2 };
	socket.send(boost::asio::buffer(header));

	socket.send(boost::asio::buffer(key));
	uint8_t dataheader[5];
	int read=boost::asio::read(socket, boost::asio::buffer(dataheader,5), boost::asio::transfer_at_least(5));

	const int size = dataheader[4] << 24 | dataheader[3] << 16 | dataheader[2] << 8 | dataheader[1];;

	std::vector<uint8_t> data(size);
	int read2 = boost::asio::read(socket, boost::asio::buffer(data), boost::asio::transfer_at_least(size));

	return{ data.begin(),data.end() };
}

int main(int argc, char* argv[]) {

	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(tcp::v4(), "localhost", "8082");
	tcp::resolver::iterator endpoint = resolver.resolve(query);
	tcp::socket socket(io_service);

	boost::asio::connect(socket, endpoint);
	std::string key = "testkey1";
	std::string data = "testdata1testdata1testdata1";
	sendData(socket, key, data);

	std::string resp=getData(socket, key);

	resp.push_back(2);
	resp.push_back('!');
	sendData(socket, key, resp);


	deleteData(socket,key);

	std::string resp2 = getData(socket, key);

	std::cout << "Enter any key and press enter to exit" << std::endl;
	int k;
	std::cin >> k;
	socket.close();
	return 0;
}