#ifndef __ISCRYPTOPP_H
#define __ISCRYPTOPP_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_CRYPTOPP
	#define CRYPTOPP_EXTERN_FUNC		__declspec(dllexport)
#else
	#define CRYPTOPP_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum CipherMode
{
	CM_UNKNOWN,
	CM_CBC,		 // CBC mode
	CM_CBC_CTS,	 // CBC mode with ciphertext stealing
	CM_CFB,		 // CFB mode
	CM_CFB_FIPS, // CFB mode FIPS variant
	CM_OFB,		 // OFB mode
	CM_CTR,		 // CTR mode
	CM_ECB		 // ECB mode
};
enum EncryptionMode
{
	EM_ENCRYPT,
	EM_DECRYPT
};

//-----------------------------------------------------------------------------
// Name: IEncryption
// Desc: Interface to the Encryption class
//-----------------------------------------------------------------------------
typedef class IEncryption
{
public:

	virtual void SetKey(BYTE* key, size_t keylen) = 0;
	virtual void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen) = 0;
	virtual void Put(BYTE* data, size_t datalen) = 0;
	virtual void Finalize() = 0;
	virtual std::string& GetResult() = 0;
	virtual void Reset(BYTE* iv) = 0;
	virtual void Release() = 0;
}* LPENCRYPTION;

//-----------------------------------------------------------------------------
// Name: ICryptoPP
// Desc: Interface to the CryptoPP class
//-----------------------------------------------------------------------------
typedef class ICryptoPP
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;
	virtual Result CreateEncryption(CipherMode mode, EncryptionMode encmode, IEncryption** enc) = 0;
	virtual Result ToZerolessKey(byte* key, int keylen, byte** nnkey, int* nnkeylen) const = 0;
	virtual Result FromZerolessKey(byte* nnkey, int nnkeylen, byte** key, int* keylen) const = 0;
	virtual void PasswordToKey16(const String& pwdstr, BYTE* key) const = 0;
	virtual void Release() = 0;
}* LPCRYPTOPP;

extern "C" CRYPTOPP_EXTERN_FUNC LPCRYPTOPP __cdecl CreateCryptoPP();
extern "C" CRYPTOPP_EXTERN_FUNC bool __cdecl CheckCryptoPPSupport(LPTSTR* missingdllname);

#endif