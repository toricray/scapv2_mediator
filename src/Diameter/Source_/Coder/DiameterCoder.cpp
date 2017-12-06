#include <netinet/in.h>
#include <cstring>
#include "DiameterCoder.h"


namespace CoreDiam {

CDiameterCoder::CDiameterCoder()
{
}

CDiameterCoder::~CDiameterCoder()
{
}

int CDiameterCoder::Encode(unsigned char* _Buffer, unsigned int _BufferSize, unsigned int& _Size, IDiameterMessage* _pMessage)
{
	if (_BufferSize < 20)
	{
		return -1;
	}

	_Size = 0;

	//unsigned char* BufferEnd = _Buffer + _Size;
	unsigned char* Pointer = _Buffer;

	IDiameterHeader* pHeader = _pMessage->GetHeader();
	if (!pHeader)
	{
		return -1;
	}

	Pointer += 4;//skip version and length

	int Block4 = htonl(pHeader->GetCommandCode());
	memcpy(Pointer, &Block4, 4);
	*Pointer = pHeader->GetCommandFlags();
	Pointer += 4;

	Block4 = htonl(pHeader->GetApplicationID());
	memcpy(Pointer, &Block4, 4);
	Pointer += 4;

	Block4 = htonl(pHeader->GetHopByHopIdentifier());
	memcpy(Pointer, &Block4, 4);
	Pointer += 4;

	Block4 = htonl(pHeader->GetEndToEndIdentifier());
	memcpy(Pointer, &Block4, 4);
	Pointer += 4;

	_Size = 20;

	IAVP* pAVP = _pMessage->GetAVPs();
	if (!pAVP)
	{
		return -1;
	}

	if (!EncodeAVPs(Pointer, _BufferSize - 20, _Size, pAVP))
	{
		_Size = 0;
		return -1;
	}

	Block4 = htonl(_Size);
	memcpy(_Buffer, &Block4, 4);
	*_Buffer = 0x01;
	return 0;
}

unsigned char* CDiameterCoder::EncodeAVPs(unsigned char* _Buffer, unsigned int _BufferSize, unsigned int& _Size, IAVP* _pAVP)
{
	unsigned char* Pointer = _Buffer;
	_pAVP->ResetList();
	IAVP* pAVP;
	while ((pAVP = _pAVP->GetNextAVP()) != NULL)
	{
		//int Code = pAVP->GetCode();//test
		//bool EncodeData = false;
		switch (pAVP->GetType())
		{
			case IAVP::BASE:
			case IAVP::BASICSET:
				return NULL;

			case IAVP::BASIC:
				{
					int VendorID = pAVP->GetVendorID();
					bool IsVendorSpecific = ((pAVP->GetFlags()) & 0x80);

					unsigned int DataSize;
					const unsigned char* AVPData = pAVP->GetData(DataSize);

					unsigned int FillerSize = ((DataSize % 4) == 0) ? 0 : (4 - (DataSize % 4));

					unsigned int AVPSize = 8 + ((IsVendorSpecific) ? 4 : 0) + DataSize;

					if (Pointer + AVPSize + FillerSize > _Buffer + _BufferSize)
						return NULL;

					int Block4 = htonl(pAVP->GetCode());
					memcpy(Pointer, &Block4, 4);
					Pointer += 4;

					Block4 = htonl(AVPSize);
					memcpy(Pointer, &Block4, 4);
					*Pointer = pAVP->GetFlags();
					Pointer += 4;

					if (IsVendorSpecific)
					{
						Block4 = htonl(VendorID);
						memcpy(Pointer, &Block4, 4);
						Pointer += 4;
					}

					memcpy(Pointer, AVPData, DataSize);
					Pointer += DataSize;

					memset(Pointer, 0, FillerSize);
					Pointer += FillerSize;

					_Size += AVPSize;
					_Size += FillerSize;

					break;
				}
			case IAVP::GROUPED:
				{
					int VendorID = pAVP->GetVendorID();
					bool IsVendorSpecific = ((pAVP->GetFlags()) & 0x80);

					unsigned char* SavePointer = Pointer;

					if (Pointer + 8 > _Buffer + _BufferSize)
						return NULL;

					int GroupedHeaderLen = 8;

					int Block4 = htonl(pAVP->GetCode());
					memcpy(Pointer, &Block4, 4);
					Pointer += 4;

					Pointer += 4; //skip length

					if (IsVendorSpecific)
					{
						if (Pointer + 4 > _Buffer + _BufferSize)
							return NULL;
						Block4 = htonl(VendorID);
						memcpy(Pointer, &Block4, 4);
						Pointer += 4;
						GroupedHeaderLen += 4;
					}

					_Size += GroupedHeaderLen;

					Pointer = EncodeAVPs(Pointer, _BufferSize - static_cast<unsigned int>(Pointer - _Buffer), _Size, pAVP);

					unsigned int GroupedSize = static_cast<unsigned int>(Pointer - SavePointer);

					Block4 = htonl(GroupedSize);
					memcpy(SavePointer + 4, &Block4, 4);//fill length
					*(SavePointer + 4) = pAVP->GetFlags();

					break;
				}
			case IAVP::GROUPEDSET:
				{
/*					pAVP->ResetList();
					IAVP* pSubAVP;
					while (pSubAVP = pAVP->GetNextAVP())
					{*/
						Pointer = EncodeAVPs(Pointer, _BufferSize - static_cast<unsigned int>(Pointer - _Buffer), _Size, pAVP);//pSubAVP);
						if (!Pointer)
							return NULL;
					//}

					break;
				}
		}
	}
	return Pointer;
}

}
