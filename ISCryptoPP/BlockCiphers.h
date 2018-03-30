class BlockCipher
{
private:

public:
	bool ivused;

	virtual CryptoPP::StreamTransformation& GetStreamTransformation() = 0;
	virtual void SetKey(BYTE* key, size_t keylen) = 0;
	virtual void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen) = 0;
	virtual void Reset(BYTE* iv) = 0;
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_CBC
// Desc: Wrapper for a CBC block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_CBC : public BlockCipher
{
private:
	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_CBC()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_CBC : public BlockCipher
{
private:
	CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_CBC()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_CBC_CTS
// Desc: Wrapper for a CBC CTS block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_CBC_CTS : public BlockCipher
{
private:
	CryptoPP::CBC_CTS_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_CBC_CTS()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_CBC_CTS : public BlockCipher
{
private:
	CryptoPP::CBC_CTS_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_CBC_CTS()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_CFB
// Desc: Wrapper for a CFB block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_CFB : public BlockCipher
{
private:
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_CFB()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_CFB : public BlockCipher
{
private:
	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_CFB()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_CFB_FIPS
// Desc: Wrapper for a CFB FIPS block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_CFB_FIPS : public BlockCipher
{
private:
	CryptoPP::CFB_FIPS_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_CFB_FIPS()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_CFB_FIPS : public BlockCipher
{
private:
	CryptoPP::CFB_FIPS_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_CFB_FIPS()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_OFB
// Desc: Wrapper for an OFB block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_OFB : public BlockCipher
{
private:
	CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_OFB()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_OFB : public BlockCipher
{
private:
	CryptoPP::OFB_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_OFB()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_CTR
// Desc: Wrapper for a CTR block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_CTR : public BlockCipher
{
private:
	CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_CTR()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_CTR : public BlockCipher
{
private:
	CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_CTR()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};

//-----------------------------------------------------------------------------
// Name: Encryption/DecryptionBlockCipher_ECB
// Desc: Wrapper for an ECB block cipher
//-----------------------------------------------------------------------------
class EncryptionBlockCipher_ECB : public BlockCipher
{
private:
	CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption enc;

public:
	EncryptionBlockCipher_ECB()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return enc;};
	void SetKey(BYTE* key, size_t keylen)
		{enc.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{enc.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{enc.Resynchronize(iv);}
};
class DecryptionBlockCipher_ECB : public BlockCipher
{
private:
	CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption dec;

public:
	DecryptionBlockCipher_ECB()
	{
		ivused = true;
	}

	CryptoPP::StreamTransformation& GetStreamTransformation()
		{return dec;};
	void SetKey(BYTE* key, size_t keylen)
		{dec.SetKey(key, keylen);}
	void SetKeyWithIV(BYTE* key, size_t keylen, BYTE* iv, size_t ivlen)
		{dec.SetKeyWithIV(key, keylen, iv, ivlen);}
	void Reset(BYTE* iv)
		{dec.Resynchronize(iv);}
};