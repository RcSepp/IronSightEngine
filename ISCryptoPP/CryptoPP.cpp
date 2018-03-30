#include "CryptoPP.h"

//-----------------------------------------------------------------------------
// Name: CreateCryptPP()
// Desc: DLL API for creating an instance of CryptPP
//-----------------------------------------------------------------------------
__declspec(dllexport) LPCRYPTOPP __cdecl CreateCryptoPP()
{
	return CheckCryptoPPSupport(NULL) ? new CryptPP() : NULL;
}

//-----------------------------------------------------------------------------
// Name: CheckCryptPPSupport()
// Desc: DLL API for checking support for CryptPP on the target system
//-----------------------------------------------------------------------------
#ifdef EXPORT_CRYPTOPP
	__declspec(dllexport) bool __cdecl CheckCryptoPPSupport(LPTSTR* missingdllname)
#else
	__declspec(dllimport) bool __cdecl CheckCryptoPPSupport(LPTSTR* missingdllname)
#endif
{
	if(!Engine::FindDLL("cryptopp.dll", missingdllname))
		return false;

   return true;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void CryptPP::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize CryptoPP
//-----------------------------------------------------------------------------
Result CryptPP::Init()
{
	//EDIT

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateEncryption()
// Desc: Create a new instance of Encryption
//-----------------------------------------------------------------------------
Result CryptPP::CreateEncryption(CipherMode mode, EncryptionMode encmode, IEncryption** enc)
{
	if(mode == CM_UNKNOWN)
		return ERR("Invalid parameter: mode CM_UNKNOWN");

	Encryption* newenc;
	CHKALLOC(newenc = new Encryption(mode, encmode));

	ciphers.push_back(newenc);
	*enc = newenc;

	return R_OK;
}

Result CryptPP::ToZerolessKey(byte* key, int keylen, byte** zlkey, int* zlkeylen) const
{
	*zlkeylen = (int)ceil(keylen * 8.0f / 7.0f);
	CHKALLOC(*zlkey = new byte[*zlkeylen]);

	for(int i = 0, j = 0; j < *zlkeylen; i += 7, j++)
	{
		int i1 = i / 8;
		int i2 = (i + 7) / 8;
		int o1 = 7 - (8 - i % 8);
		int o2 = 8 - (i + 7) % 8;

		(*zlkey)[j] = 0x80; // Set first bit (effectively prohibiting key sequence from having zeros)
		if(o1 >= 0)
			(*zlkey)[j] |= (key[i1] << o1) & 0x7F; // Add bits from byte key[i1]
		if(i2 < keylen)
			(*zlkey)[j] |= (key[i2] >> o2) & 0x7F; // Add bits from byte key[i2]
	}


	/*for(int y = 0; y < *zlkeylen; y++)
	{
		for(int x = 7; x >= 0; x--)
			cle->Print(((*zlkey)[y] >> x) & 0x1);
		cle->Print(", ");
	}
	cle->Print("\n");*/

	return R_OK;
}
Result CryptPP::FromZerolessKey(byte* zlkey, int zlkeylen, byte** key, int* keylen) const
{
	*keylen = zlkeylen * 7 / 8;
	CHKALLOC(*key = new byte[*keylen]);
	ZeroMemory(*key, *keylen * sizeof(byte));


	for(int i = 0, j = 0; j < zlkeylen; i += 7, j++)
	{
		int i1 = i / 8;
		int i2 = (i + 7) / 8;
		int o1 = (i + 7) % 8;
		int o2 = 8 - (i + 7) % 8;

		(*key)[i1] |= (zlkey[j] & 0x7F) >> o1;
		(*key)[i2] |= (zlkey[j] & 0x7F) << o2;
	}

	/*for(int y = 0; y < *keylen; y++)
	{
		for(int x = 7; x >= 0; x--)
			cle->Print(((*key)[y] >> x) & 0x1);
		cle->Print(", ");
	}*/

	return R_OK;
}

void CryptPP::PasswordToKey16(const String& pwdstr, BYTE* key) const
{
	static const DWORD CRC_POLYNOMINALS[] = {0xEDB88320, 0x82F63B78, 0xEB31D82E, 0xD5828281};

	for(byte i = 0; i < 4; i++)
	{
		char* buffer = pwdstr.ToCharString();
		size_t bufferlen = pwdstr.length();

		// Compute CRC-32
		DWORD crc = 0xFFFFFFFF;
		while(bufferlen--)
		{
			DWORD temp = (DWORD)((crc & 0xFF) ^ *buffer++);
			for(byte j = 0; j < 8; j++)
			{
				if(temp & 0x1)
					temp = (temp >> 1) ^ CRC_POLYNOMINALS[i];
				else
					temp >>= 1;
			}
			crc = (crc >> 8) ^ temp;
		}

		// Store CRC as part of the key
		((DWORD*)key)[i] = crc ^ 0xFFFFFFFF;
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void CryptPP::Release()
{
	for(std::list<Encryption*>::iterator iter = ciphers.begin(); iter != ciphers.end(); iter++)
		(*iter)->Release();
	ciphers.clear();

	delete this;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Encryption::Release()
{
	//EDIT

	delete this;
}