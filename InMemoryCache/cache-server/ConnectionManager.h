#pragma once

#include <set>
#include "Connection.h"

class ConnectionManager : private boost::noncopyable
{
public:
	/// Add the specified connection to the manager and start it.
	void start(ConnectionPtr c);

	/// Stop the specified connection.
	void stop(ConnectionPtr c);

	/// Stop all connections.
	void stop_all();


private:

	std::set<ConnectionPtr> connections_;

};