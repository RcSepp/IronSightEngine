#ifndef __CRYPTOPP_H
#define __CRYPTOPP_H

#include "ISCryptoPP.h"

// Crypto++ headers
#include "osrng.h"
#include "modes.h"
#include "base64.h"
#include "hex.h"

// Crypto++ Library
#ifdef _X64
	#ifdef _DEBUG
		#pragma comment(lib, "cryptlibx64d")
	#else
		#pragma comment(lib, "cryptlibx64")
	#endif
#else // x86
	#ifdef _DEBUG
		#pragma comment(lib, "cryptlibd")
	#else
		#pragma comment(lib, "cryptlib")
	#endif
#endif

#include "BlockCiphers.h"


//-----------------------------------------------------------------------------
// Name: Encryption
// Desc: Wrapper for a CryptoPP encryption or decryption (depending on encmode)
//-----------------------------------------------------------------------------
class Encryption : public IEncryption
{
private:
	CipherMode mode;
	EncryptionMode encmode;
	BlockCipher* cipher;
	CryptoPP::StreamTransformationFilter* pipeline;
	std::string ciphertext;
	bool ivused;

public:

	Encryption(CipherMode mode, EncryptionMode encmode)
	{
		this->mode = mode;
		this->encmode = encmode;

		if(encmode == EM_ENCRYPT)
			switch(mode)
			{
			case CM_CBC:
				cipher = new EncryptionBlockCipher_CBC();
				ivused = true;
				break;
			case CM_CBC_CTS:
				cipher = new EncryptionBlockCipher_CBC_CTS();
				ivused = true;
				break;
			case CM_CFB:
				cipher = new EncryptionBlockCipher_CFB();
				ivused = true;
				break;
			case CM_CFB_FIPS:
				cipher = new EncryptionBlockCipher_CFB_FIPS();
				ivused = true;
				break;
			case CM_OFB:
				cipher = new EncryptionBlockCipher_OFB();
				ivused = true;
				break;
			case CM_CTR:
				cipher = new EncryptionBlockCipher_CTR();
				ivused = true;
				break;
			case CM_ECB:
				cipher = new EncryptionBlockCipher_ECB();
				ivused = false;
			}
		else // encmode == EM_DECRYPT
			switch(mode)
			{
			case CM_CBC:
				cipher = new DecryptionBlockCipher_CBC();
				ivused = true;
				break;
			case CM_CBC_CTS:
				cipher = new DecryptionBlockCipher_CBC_CTS();
				ivused = true;
				break;
			case CM_CFB:
				cipher = new DecryptionBlockCipher_CFB();
				ivused = true;
				break;
			case CM_CFB_FIPS:
				cipher = new DecryptionBlockCipher_CFB_FIPS();
				ivused = true;
				break;
			case CM_OFB:
				cipher = new DecryptionBlockCipher_OFB();
				ivused = true;
				break;
			case CM_CTR:
				cipher = new DecryptionBlockCipher_CTR();
				ivused = true;
				break;
			case CM_ECB:
				cipher = new DecryptionBlockCipher_ECB();
				ivused = false;
			}
		pipeline = new CryptoPP::StreamTransformationFilter(cipher->GetStreamTransformation(), new CryptoPP::StringSink(ciphertext));
	};

	void SetKey(BYTE* key, size_t keylen)
		{cipher->SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
	{
		if(cipher->ivused)
			cipher->SetKeyWithIV(key, keylen, iv, ivlen);
		else
			cipher->SetKey(key, keylen);
	}
	void Put(BYTE* data, size_t datalen)
		{pipeline->Put(data, datalen);}
	void Finalize()
		{pipeline->MessageEnd();}
	std::string& GetResult()
		{return ciphertext;}
	void Reset(BYTE* iv)
		{cipher->Reset(iv);}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: CryptPP
// Desc: API to the CryptoPP library
//-----------------------------------------------------------------------------
class CryptPP : public ICryptoPP
{
private:
	std::list<Encryption*> ciphers;

public:

	CryptPP()
	{
	};

	void Sync(GLOBALVARDEF_LIST);
	Result Init();
	Result CreateEncryption(CipherMode mode, EncryptionMode encmode, IEncryption** enc);
	Result ToZerolessKey(byte* key, int keylen, byte** nnkey, int* nnkeylen) const;
	Result FromZerolessKey(byte* nnkey, int nnkeylen, byte** key, int* keylen) const;
	void PasswordToKey16(const String& pwdstr, BYTE* key) const;
	void Release();
};

#endif