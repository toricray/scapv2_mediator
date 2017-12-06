#ifndef __DIAMETERCONNECTION_H__
#define __DIAMETERCONNECTION_H__

#include "DiameterSocket.h"

namespace CoreDiam {

class DiamConnection
{
private:
	class DiamConnectionImpl;

	DiamConnectionImpl *impl_;
public:
	DiamConnection(DiamSocket::SocketHandle handle);
	~DiamConnection();

	DiamSocket::SocketHandle getSocketHandle();

	int receive() {
		return 0;
	}

	int send() {
		return 0;
	}
};


}  // namespace CoreDiam

#endif  // __DIAMETERCONNECTION_H__
