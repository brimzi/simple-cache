#include "StorageProvider.h"
#include <boost\shared_ptr.hpp>
#include <boost/container/vector.hpp>
#include <string>

using namespace boost::container;

StorageProvider::StorageProvider(boost::uint32_t maxSize):maxSize_(maxSize),currentSize_(0),cache_()
{
}

void StorageProvider::save(const boost::container::vector<boost::uint8_t>& key, boost::shared_ptr<vector<boost::uint8_t>> data)
{
	std::string keyString(key.begin(), key.end());//TODO think about the cost here

	boost::uint32_t size = (*data.get()).size();

	if (!enoughSpace(size)) {
		createSpace(size);
	}

	//we add to cache
	cache_[keyString] = data;
	currentSize_ += size;
	
}

boost::shared_ptr<vector<boost::uint8_t>> StorageProvider::get(const std::string & key)
{
	return boost::shared_ptr<vector<boost::uint8_t>>();
}

void StorageProvider::remove(const boost::container::vector<boost::uint8_t>& key)
{
}

bool StorageProvider::enoughSpace(boost::uint32_t size)
{
	return (currentSize_+size)<=maxSize_;
}

void StorageProvider::createSpace(boost::uint32_t size)
{
	while ((currentSize_ + size) > maxSize_) 
	{
		std::string dataToRemove;//TODO get from heap?


		cache_.erase(dataToRemove);
		currentSize_ -= size;
	}
}


