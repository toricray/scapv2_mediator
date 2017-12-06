#include <netinet/in.h>
#include "DiameterDecoder.h"

namespace CoreDiam {

CDiameterDecoder::CDiameterDecoder()
{

}

CDiameterDecoder::~CDiameterDecoder()
{

}

int CDiameterDecoder::Decode(const unsigned char* _Data, unsigned int _Size, IDiameterMessage* _pMessage)
{

	IDiameterHeader *pHeader = _pMessage->GetHeader();
	if(!pHeader)
		return -1;

	unsigned int headerSize = 0;
	if(-1 == DecodeHeader(_Data, _Size, pHeader, &headerSize))
		return -1;

	IAVP *pAVP = _pMessage->GetAVPs();
	if(!pAVP)
		return -1;

	//VTUNE_TASK_BEGIN(vtuneAmpHolder.hDecodeTask);

	int result = DecodeAVPS(_Data + headerSize, _Size - headerSize, pAVP);

	//VTUNE_TASK_END();

	return result;
}

int CDiameterDecoder::DecodeHeader(const unsigned char *_Data, unsigned int _Size, IDiameterHeader *_pHeader, unsigned int *headerSize)
{

	const unsigned char* Pointer = _Data;
	if (_Size < 20)
		return -1;

	unsigned int Length;
	memcpy(&Length, Pointer, sizeof(unsigned int));
	Length = ntohl(Length) & 0x00ffffff;

	if (Length != _Size)
		return -1;

	Pointer += 4;

	unsigned char Flags = *Pointer;

	unsigned int CommandCode;
	memcpy(&CommandCode, Pointer, sizeof(unsigned int));
	CommandCode = ntohl(CommandCode) & 0x00ffffff;

	Pointer += 4;

	unsigned int ApplicationID, HbHID, EtEID;

	memcpy(&ApplicationID, Pointer, sizeof(unsigned int));
	Pointer += 4;
	ApplicationID = ntohl(ApplicationID);

	memcpy(&HbHID, Pointer, sizeof(unsigned int));
	Pointer += 4;
	HbHID = ntohl(HbHID);

	memcpy(&EtEID, Pointer, sizeof(unsigned int));
	Pointer += 4;
	EtEID = ntohl(EtEID);

	_pHeader->SetCommandFlags(Flags);
	_pHeader->SetCommandCode(CommandCode);
	_pHeader->SetApplicationID(ApplicationID);
	_pHeader->SetHopByHopIdentifier(HbHID);
	_pHeader->SetEndToEndIdentifier(EtEID);

	*headerSize = static_cast<unsigned int>(Pointer - _Data);

	return 0;
}

int CDiameterDecoder::DecodeAVPS(const unsigned char* _Data, unsigned int _Size, IAVP* _pAVP)
{

	const unsigned char* Pointer = _Data;

	while(Pointer != _Data + _Size)
	{
		if (Pointer + 8 > _Data + _Size)
			return -1;

		int AVPCode, Length;
		
		memcpy(&AVPCode, Pointer, sizeof(unsigned int));
		AVPCode = ntohl(AVPCode);

		unsigned char Flags = *(Pointer + 4);
		memcpy(&Length, Pointer + 4, sizeof(unsigned int));
		Length = ntohl(Length) & 0x00ffffff;

		const unsigned char* NextPointer = Pointer + Length + ((Length % 4 == 0) ? 0 : (4 - Length % 4));
		if(NextPointer > _Data + _Size)
			return -1;

		int VendorID = 10415;
		if(Flags & 0x80)
		{
			memcpy(&VendorID, Pointer + 8, sizeof(unsigned int));
			VendorID = ntohl(VendorID);
			Pointer += 4;
			Length  -= 4;
		}

		Pointer += 8;
		Length  -= 8;

		switch(GetAVPType(AVPCode))
		{
			case IAVP::BASE:
			case IAVP::BASICSET:
				return -1;

			case IAVP::BASIC:
				{
					CDiameterAVP* pAVP = new CDiameterAVP(IAVP::BASIC, Pointer, Length, AVPCode, Flags, VendorID);
					if (!_pAVP->SetAVP(pAVP))
					{
						delete pAVP;
						return -1;
					}
					break;
				}

			case IAVP::GROUPEDSET:
				{
					IAVP* AVPs = _pAVP->GetAVPByCode(AVPCode);
					if (!AVPs)
					{
						AVPs = new CDiameterAVPGroup(IAVP::GROUPEDSET, AVPCode);
						if (!_pAVP->SetAVP(AVPs))
						{
							delete AVPs;
							return -1;
						}
					}

					IAVP* GroupedAVP = new CDiameterAVP(IAVP::GROUPED, Pointer, Length, AVPCode, Flags, VendorID);
					if(DecodeAVPS(Pointer, Length, GroupedAVP) != -1)
					{
						if (!AVPs->SetAVP(GroupedAVP, 0xffffffff))
						{
							delete GroupedAVP;
							return -1;
						}
					}
					else
					{
						delete GroupedAVP;
						return -1;
					}
					break;
				}

			case IAVP::GROUPED:
				{
					IAVP* GroupedAVP = new CDiameterAVP(IAVP::GROUPED, Pointer, Length, AVPCode, Flags, VendorID);
					if (DecodeAVPS(Pointer, Length, GroupedAVP) != -1)
					{
						if (!_pAVP->SetAVP(GroupedAVP))
						{
							delete GroupedAVP;
							return -1;
						}
					}
					else
					{
						delete GroupedAVP;
						return -1;
					}
					break;
					
				}
		}

		Pointer = NextPointer;
	}

	return 0;
}

IAVP::AVPType CDiameterDecoder::GetAVPType(int _AVPCode)
{
	if (_AVPCode == 440)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 431)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 437)
		return IAVP::GROUPED;
	if (_AVPCode == 446)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 443)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 873)
		return IAVP::GROUPED;
	if (_AVPCode == 876)
		return IAVP::GROUPED;
	if (_AVPCode == 823)
		return IAVP::GROUPED;
	if (_AVPCode == 833)
		return IAVP::GROUPED;
	if (_AVPCode == 456)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 413)
		return IAVP::GROUPED;
	if (_AVPCode == 458)
		return IAVP::GROUPED;
	if (_AVPCode == 430)
		return IAVP::GROUPED;
	if (_AVPCode == 874)
		return IAVP::GROUPED;
	if (_AVPCode == 865)
		return IAVP::GROUPED;
	if (_AVPCode == 2000)
		return IAVP::GROUPED;
	if (_AVPCode == 2026)
		return IAVP::GROUPED;
	if (_AVPCode == 1249)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 877)
		return IAVP::GROUPED;
	if (_AVPCode == 886)
		return IAVP::GROUPED;
	if (_AVPCode == 1201)
		return IAVP::GROUPED;
	if (_AVPCode == 1213)
		return IAVP::GROUPED;
	if (_AVPCode == 2026)
		return IAVP::GROUPEDSET;
	if (_AVPCode == 2027)
		return IAVP::GROUPED;
	return IAVP::BASIC;
}

}
