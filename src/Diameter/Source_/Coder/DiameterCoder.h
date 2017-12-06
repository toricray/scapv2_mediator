#ifndef __DIAMETER_CODER_H__
#define __DIAMETER_CODER_H__

#include "../RawMessage/RawDiameterMessage.h"

namespace CoreDiam
{
	
class CDiameterCoder
{
public:
	CDiameterCoder();
	virtual ~CDiameterCoder();
public:
	int Encode(unsigned char* _Buffer, unsigned int _BufferSize, unsigned int& _Size, IDiameterMessage* _pMessage);
protected:
	inline unsigned char* EncodeAVPs(unsigned char* _Buffer, unsigned int _BufferSize, unsigned int& _Size, IAVP* _pAVP);
};
	
}

#endif //__DIAMETER_CODER_H__
