#pragma once
#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "EvictionStrategy.h"

//using namespace boost::container;
class StorageProvider
{
public:
	StorageProvider(boost::uint32_t maxSize);

	void save(const std::vector<boost::uint8_t>& key
		, boost::shared_ptr<std::vector<boost::uint8_t>>  data);

	boost::shared_ptr<std::vector<boost::uint8_t>> get(const std::vector<boost::uint8_t>& key);

	

	int remove(const std::vector<boost::uint8_t>& key);

private:
	bool enoughSpace(boost::uint32_t size);

	void createSpace(boost::uint32_t size);
	
	int removeData(std::string& key);

	boost::unordered_map<std::string, boost::shared_ptr<std::vector<uint8_t>>> cache_;
	uint32_t currentSize_;
	uint32_t maxSize_;
	boost::shared_ptr<EvictionStrategy> evictor_;
};

