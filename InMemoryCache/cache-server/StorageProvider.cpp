#include "StorageProvider.h"
#include <boost\shared_ptr.hpp>
#include <string>

using namespace boost::container;

void StorageProvider::save(std::string* key, boost::shared_ptr<vector<boost::uint8_t>>  data)
{

}

std::string StorageProvider::get(const std::string & key)
{
	return std::string();
}
