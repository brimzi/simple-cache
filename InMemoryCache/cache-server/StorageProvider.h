#pragma once
#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>
class StorageProvider
{
public:
	void save(std::string* key, boost::shared_ptr<vector<boost::uint8_t>>  data);

	std::string get(const std::string& key);

private:
	bool canAccomodate(boost::uint32_t size);

	void evictOldest();

	boost::unordered_map<boost::uint16_t,boost::container::vector<boost::uint8_t>*> data_;
	boost::uint32_t currentSize_;
};

