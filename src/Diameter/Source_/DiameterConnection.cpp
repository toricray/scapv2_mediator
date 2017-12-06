#include "DiameterConnection.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>

namespace CoreDiam {

//----------------------------------------------------------------------------
// BufferedReceiver
//----------------------------------------------------------------------------
class DiamSocketReceiver
{
private:
	enum {
		BUFFER_SIZE = 8*1024*1024
	};

	//enum ReceiveResult {
	//	RECV_OK,
	//	RECV_FA
	//};

	DiamSocket::SocketHandle  hSocket_;
	char                      buffer_[BUFFER_SIZE];

	char                     *data_;         // pointer to unread received data in buffer_
	uint32_t                  dataSize_;     // size of unread received data
	uint32_t                  size_;         // size of used space in buffer_

protected:
	uint32_t getPacketSize()
	{
		if(dataSize_ < 4 || data_ == NULL)
			return 0;

		uint32_t packetSize = ntohl(*(uint32_t *)data_) & 0x00FFFFFFFFUL;
		if(dataSize_ >= packetSize)
			return packetSize;

		return 0;
	}

	//char *   getPacket()
	//{
	//	return data_;
	//}

	//uint32_t getPacketSize()
	//{
	//	if(dataSize_ < 4 || data_ == NULL)
	//		return 0;
	//	return (ntohl(*(uint32_t *)data_) & 0x00FFFFFFFFUL);
	//}

	void     eatData(uint32_t size)
	{
		if(size < dataSize_)
		{
			data_     += size;
			dataSize_ -= size;
		}
		else
		{
			data_     = NULL;
			dataSize_ = 0;
			size_     = 0;
		}
	}

	virtual void handlePacket(const char *data, uint32_t size)
	{
		//
	}

public:
	DiamSocketReceiver(DiamSocket::SocketHandle handle)
		: hSocket_(handle)
		, data_(NULL)
		, dataSize_(0)
		, size_(0)
	{
	}

	virtual ~DiamSocketReceiver() {}

	int receive() 
	{

		int result = DiamSocket::recv(hSocket_, buffer_ + size_, BUFFER_SIZE - size_);
		if(result == DiamSocket::DS_SOCKET_ERROR || !result)
			return result;

		if(!data_)
			data_ = buffer_;

		dataSize_ += result;
		size_     += result;

		for(uint32_t packetSize = 0; 0 < (packetSize = getPacketSize()); eatData(packetSize))
		{
			handlePacket(data_, packetSize);
		}

		// align remainder towards buffer start
		if(dataSize_)
		{
			if(data_ == NULL)
			{
				dataSize_ = 0;
				size_     = 0;
			}
			else
			{
				memcpy(buffer_, data_, dataSize_);
				data_ = buffer_;
				size_ = dataSize_;
			}
		}
		else
		{
			data_     = NULL;
			size_     = 0;
		}

		return result;
	}
};

//----------------------------------------------------------------------------
// DiameterConnection::DiameterConnectionImpl
//----------------------------------------------------------------------------
class DiamConnection::DiamConnectionImpl
{
public:
	DiamSocket::SocketHandle hSocket_;
	DiamSocketReceiver       receiver_;

public:
	DiamConnectionImpl(DiamSocket::SocketHandle handle)
		: hSocket_(handle)
		, receiver_(handle)
	{
	}
};

//----------------------------------------------------------------------------
// DiameterConnection
//----------------------------------------------------------------------------
DiamConnection::DiamConnection(DiamSocket::SocketHandle handle)
{

	impl_ = new DiamConnectionImpl(handle);
}
//----------------------------------------------------------------------------
DiamConnection::~DiamConnection()
{

	delete impl_;
}
//----------------------------------------------------------------------------
DiamSocket::SocketHandle DiamConnection::getSocketHandle()
{
	return impl_->hSocket_;
}

}  // namespace DiamCore
