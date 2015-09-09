#include "StorageProvider.h"
#include <boost\shared_ptr.hpp>
#include <boost/container/vector.hpp>
#include <string>

using namespace boost::container;

StorageProvider::StorageProvider(boost::uint32_t maxSize):maxSize_(maxSize),currentSize_(0),cache_()
{
	evictor_.reset(new OldestInsertionEviction());
}

void StorageProvider::save(const std::vector<boost::uint8_t>& key, boost::shared_ptr<std::vector<boost::uint8_t>> data)
{
	std::string keyString(key.begin(), key.end());//TODO think about the cost here
	if (!enoughSpace(data->size())) {
		createSpace(data->size());
	}
	//cache_[keyString] = data;
	//we check if its already in cache
	if (cache_.find(keyString)==cache_.end())
	{
		evictor_->addKey(keyString);
		currentSize_ += data->size();
	}
	else
	{
		evictor_->refreshKey(keyString);
		currentSize_=(currentSize_- cache_[keyString]->size())+ data->size();
	}
	cache_[keyString] = data;
}

boost::shared_ptr<std::vector<boost::uint8_t>> StorageProvider::get(const std::vector<boost::uint8_t>& key)
{
	std::string keyString(key.begin(), key.end());//TODO think about the cost here

	if (cache_.find(keyString) == cache_.end())
		return 0;

	return cache_[keyString];
}

int StorageProvider::remove(const std::vector<boost::uint8_t>& key)
{
	std::string dataToRemove(key.begin(), key.end());
	return removeData(dataToRemove);
}

bool StorageProvider::enoughSpace(boost::uint32_t size)
{
	return (currentSize_+size)<=maxSize_;
}

void StorageProvider::createSpace(boost::uint32_t size)
{
	while ((currentSize_ + size) > maxSize_) 
	{
		std::string dataToRemove = evictor_->nextEviction();
		removeData(dataToRemove);
	}
}

bool StorageProvider::removeData(std::string& key)
{
	if (cache_.find(key) != cache_.end())
	{
		currentSize_ -= cache_[key]->size();
		cache_.erase(key);
		evictor_->deleteKey(key);
		return 1;
	}
	else
	{
		return 0;
	}
	
}


