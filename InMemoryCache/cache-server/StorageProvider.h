#pragma once
#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>

using namespace boost::container;
class StorageProvider
{
public:
	StorageProvider(boost::uint32_t maxSize);

	void save(const boost::container::vector<boost::uint8_t>& key
		, boost::shared_ptr<boost::container::vector<boost::uint8_t>>  data);

	boost::shared_ptr<boost::container::vector<boost::uint8_t>> get(const boost::container::vector<boost::uint8_t>& key);

	void remove(const boost::container::vector<boost::uint8_t>& key);

private:
	bool enoughSpace(boost::uint32_t size);

	void createSpace(boost::uint32_t size);
	
	void removeData(std::string& key);
	boost::unordered_map<std::string, boost::shared_ptr<boost::container::vector<boost::uint8_t>>> cache_;
	boost::uint32_t currentSize_;
	boost::uint32_t maxSize_;
};

