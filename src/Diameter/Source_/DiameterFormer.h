#ifndef __DIAMETERFORMER_H__
#define __DIAMETERFORMER_H__

#include <memory.h>
#include <netinet/in.h>

namespace CoreDiam {

class CDiameterFormer
{
private:
	enum {
		MIN_BUFFER_SIZE = 64*1024
	};

protected:
	unsigned char *m_Data; //[BUFFER_SIZE];
	int            m_DataSize;

	unsigned char *m_pPointer;
	unsigned char *m_pNewDataBegin;
	unsigned char *m_pNewDataEnd;

	unsigned int   m_MsgLen;

	unsigned char * TakeMsg(unsigned int &_Size);

public:
	CDiameterFormer(int bufferSize = MIN_BUFFER_SIZE);
	virtual ~CDiameterFormer();

	unsigned char * ReceiveData(unsigned char *_Data, unsigned int &_Len);
	int             getCapacity();
	int             getFreeSpace();
	int             getSize();
	bool            isOutOfSpace();
};


class DiamMessageMarker
{
private:
	static const unsigned int INVALID_MSG_LENGTH   = 0xFFFFFFFFU;
	static const unsigned int MSG_LENGTH_MASK      = 0x00FFFFFFU;

	enum {
		LENGTH_CHUNK_SIZE    =  4,
		HEADER_SIZE          = 20
	};

	char   *buffer_;
	size_t  bufferSize_;
	size_t  head_;
	size_t  tail_;

	unsigned int getHeadingMessageLength() {
		if(dataSize() < LENGTH_CHUNK_SIZE)
			return 0;

		unsigned int msgLength = ntohl(*(unsigned int *)head_) & MSG_LENGTH_MASK;
		if(msgLength < dataSize())
			return 0;

		return msgLength;
	}

public:
	DiamMessageMarker(char *buffer, size_t bufferSize)
		: buffer_(buffer)
		, bufferSize_(bufferSize)
		, head_(0)
		, tail_(0)
	{

	}

	size_t dataSize() {
		return tail_ - head_;
	}

	size_t freeSpace() {
		return bufferSize_ - tail_;
	}

	char * tail() {
		return buffer_ + tail_;
	}

	void   incSize(size_t increment) {
		if(tail_ + increment <= bufferSize_)
			tail_ += increment;
		else
			tail_ = bufferSize_;
	}

	bool   hasMessage() {
		return getHeadingMessageLength()? true: false;
	}

	char * dequeueMessage(unsigned int *msgLength) {
		*msgLength = getHeadingMessageLength();
		if(!*msgLength)
			return NULL;

		

		return NULL;
	}
};

}

#endif // __DIAMETERFORMER_H__
