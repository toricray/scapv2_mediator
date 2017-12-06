#ifndef __DIAMETER_DECODER_H__
#define __DIAMETER_DECODER_H__

#include <memory.h>

#include "../RawMessage/RawDiameterMessage.h"

#include "../RawMessage/DiameterHeader.h"
#include "../RawMessage/DiameterAVP.h"
#include "../RawMessage/DiameterAVPGroup.h"


namespace CoreDiam {

class CDiameterDecoder
{
public:
	CDiameterDecoder();
	virtual ~CDiameterDecoder();
public:
	int Decode(const unsigned char* _Data, unsigned int _Size, IDiameterMessage* _pMessage);
	int DecodeHeader(const unsigned char *_Data, unsigned int _Size, IDiameterHeader *_pHeader, unsigned int *headerSize);
	int DecodeAVPS(const unsigned char *_Data, unsigned int _Size, IAVP *_pAVP);
protected:
	inline IAVP::AVPType GetAVPType(int _AVPCode);
};

}

#endif //__DIAMETER_DECODER_H__
