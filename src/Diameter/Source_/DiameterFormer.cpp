#include "DiameterFormer.h"


namespace CoreDiam {

const unsigned int INVALID_MSG_LENGTH   = 0xFFFFFFFFU;
const unsigned int MSG_LENGTH_MASK      = 0x00FFFFFFU;
const unsigned int LENGTH_CHUNK_SIZE    = 4;

CDiameterFormer::CDiameterFormer(int bufferSize)
	: m_DataSize(bufferSize)
{

	if(m_DataSize < MIN_BUFFER_SIZE)
		m_DataSize = MIN_BUFFER_SIZE;
	m_Data          = new unsigned char[m_DataSize];
	m_pPointer      = m_Data;
	m_pNewDataBegin = NULL;
	m_pNewDataEnd   = NULL;
	m_MsgLen        = INVALID_MSG_LENGTH;
}
//----------------------------------------------------------------------------
CDiameterFormer::~CDiameterFormer()
{

	delete[] m_Data;
}
//----------------------------------------------------------------------------
inline unsigned char * CDiameterFormer::TakeMsg(unsigned int &_Size)
{

	unsigned int dataSize = m_pPointer - m_Data;

	if(m_MsgLen == INVALID_MSG_LENGTH)
	{
		if(dataSize < LENGTH_CHUNK_SIZE)
		{
			unsigned int missingSize = LENGTH_CHUNK_SIZE - dataSize;

			if((unsigned int)(m_pNewDataEnd - m_pNewDataBegin) < missingSize)
			{
				_Size = INVALID_MSG_LENGTH;
				return NULL;
			}

			if(!dataSize)
			{
				//memcpy(&m_MsgLen, m_pNewDataBegin, sizeof(unsigned int));
				//m_MsgLen = ntohl(m_MsgLen) & MSG_LENGTH_MASK;
				m_MsgLen = ntohl(*reinterpret_cast<unsigned int *>(m_pNewDataBegin)) & MSG_LENGTH_MASK;
			}
			else
			{
				if(missingSize > (unsigned int)getFreeSpace())
				{
					_Size = INVALID_MSG_LENGTH;
					return NULL;
				}

				memcpy(m_pPointer, m_pNewDataBegin, missingSize);

				m_pNewDataBegin += missingSize;
				m_pPointer      += missingSize;
				dataSize        += missingSize;

				//m_pNewDataEnd   += (sizeof(unsigned int) - dataSize);

				//memcpy(&m_MsgLen, m_Data, sizeof(unsigned int));
				//m_MsgLen = ntohl(m_MsgLen) & MSG_LENGTH_MASK;
				m_MsgLen = ntohl(*reinterpret_cast<unsigned int *>(m_Data)) & MSG_LENGTH_MASK;
			}
		}
		else
		{
			//memcpy(&m_MsgLen, m_Data, sizeof(unsigned int));
			//m_MsgLen = ntohl(m_MsgLen) & MSG_LENGTH_MASK;
			m_MsgLen = ntohl(*reinterpret_cast<unsigned int *>(m_Data)) & MSG_LENGTH_MASK;
		}
	}

	//unsigned int LenData = m_pPointer - m_Data;

	_Size = m_MsgLen;

	if(dataSize + (unsigned int)(m_pNewDataEnd - m_pNewDataBegin) < m_MsgLen)
		return NULL;

	unsigned char *msg; // = dataSize? m_Data: m_pNewDataBegin;

	if(!dataSize)
	{
		msg = m_pNewDataBegin;
	}
	else
	{
		msg = m_Data;
		if(m_MsgLen - dataSize > (unsigned int)getFreeSpace())
			return NULL;

		memcpy(m_pPointer, m_pNewDataBegin, m_MsgLen - dataSize);
		m_pPointer = m_Data;
	}

	m_pNewDataBegin += m_MsgLen - dataSize;

	m_MsgLen = INVALID_MSG_LENGTH;

	return msg;
}
//----------------------------------------------------------------------------
unsigned char * CDiameterFormer::ReceiveData(unsigned char *_Data, unsigned int &_Len)
{

	if(_Data)
	{
		// it must be 1-st call - initializing
		m_pNewDataBegin = _Data;
		m_pNewDataEnd   = _Data + _Len;
	}
	else
	{
		// sequential calls must be with _Data = NULL
	}

	_Data = TakeMsg(_Len);

	if(!_Data)
	{
		if((unsigned int)(m_pNewDataEnd - m_pNewDataBegin) <= (unsigned int)getFreeSpace())
		{
			memcpy(m_pPointer, m_pNewDataBegin, m_pNewDataEnd - m_pNewDataBegin);
			m_pPointer += (unsigned int)(m_pNewDataEnd - m_pNewDataBegin);
			m_pNewDataBegin = m_pNewDataEnd;
		}
	}

	return _Data;
}
//----------------------------------------------------------------------------
inline int CDiameterFormer::getCapacity()
{
	return m_DataSize;
}
//----------------------------------------------------------------------------
inline int CDiameterFormer::getFreeSpace()
{
	return m_DataSize - (m_pPointer - m_Data);
}
//----------------------------------------------------------------------------
int CDiameterFormer::getSize()
{
	return (m_pPointer - m_Data);
}
//----------------------------------------------------------------------------
bool CDiameterFormer::isOutOfSpace()
{
	return (unsigned int)(m_pNewDataEnd - m_pNewDataBegin) > (unsigned int)getFreeSpace();
}


} // namespace CoreDiam
