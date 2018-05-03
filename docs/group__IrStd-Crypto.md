# IrStd-Crypto
Helper function to deal with crypto algorithms. 
# Member(s)
- [std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::SHA256() const](#group__IrStd-Crypto_1gae7b0f5d68b9f5f74e2c82dc7c9f87dba)
- [std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::HMACSHA512(const IrStd::Type::Buffer &key) const](#group__IrStd-Crypto_1ga3a4af1afa14a74e8109a995198a97ab6)
- [std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::HMACSHA1(const IrStd::Type::Buffer &key) const](#group__IrStd-Crypto_1ga95634a14a61c926ac04707d0f5198e32)
- [IrStd::Crypto::Crypto(Args &&... args)](#group__IrStd-Crypto_1ga2de1f4dac4bc431d4256f22fb760ddd6)
## Function(s)


<a name='group__IrStd-Crypto_1ga2de1f4dac4bc431d4256f22fb760ddd6'></a> `public` `template <class ... Args>` `inline` **IrStd::Crypto::Crypto(Args &&... args)**

Initialize the crypto object with a buffer to process. 




<a name='group__IrStd-Crypto_1gae7b0f5d68b9f5f74e2c82dc7c9f87dba'></a> `public` **std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::SHA256() const**

Generates a SHA-256 hash. 
<br/>**Return**
<br/>The buffer containing the hash 




<a name='group__IrStd-Crypto_1ga95634a14a61c926ac04707d0f5198e32'></a> `public` **std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::HMACSHA1(const IrStd::Type::Buffer &key) const**

Cypher the internal buffer with the HMAC-SHA-1 algorithm. 
<br/>**Parameters**
<br/>`key` The key to be used 
<br/>
<br/>**Return**
<br/>The buffer containing the cyphered message 




<a name='group__IrStd-Crypto_1ga3a4af1afa14a74e8109a995198a97ab6'></a> `public` **std::unique_ptr<IrStd::Type::Buffer> IrStd::Crypto::HMACSHA512(const IrStd::Type::Buffer &key) const**

Cypher the internal buffer with the HMAC-SHA-512 algorithm. 
<br/>**Parameters**
<br/>`key` The key to be used 
<br/>
<br/>**Return**
<br/>The buffer containing the cyphered message 


