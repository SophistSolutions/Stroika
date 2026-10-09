# Stroika::[Foundation](../)::Cryptography

This folder contains all the Stroika Library [Foundation](../)::Cryptography source code.

Cryptography keeps data secret and intact - 'secret' in a broad sense:

- **Encryption**, from strong (AES) to VERY WEAK (Base64, Hex - like rot13: opaque to a parser, and to most readers, but
  secret from no one who recognizes them), all with one shape of API: bytes transformed, and back (folder 'Encoding').
- **Digests** (hashes), from cryptographic ones (MD5) to checksums (CRC32), which show data intact (folder 'Digest').

So a transform that makes data opaque goes here, however weak. Escaping that leaves text readable - XML's QuoteForXML,
JSON's string escapes, URL percent-encoding - belongs to its format, in DataExchange (or IO::Network).

- [Digest/](Digest/ReadMe.md) - algorithms to 'hash' content
- [Encoding/](Encoding/ReadMe.md) - algorithms to 'encrypt' (or decrypt): strong (AES, RC4), to very weak (Base64, Hex)
- [Format.h](Format.h) - utility to format some binary structures in common ways done for crypto
- [PKI](PKI/ReadMe.md) - private keys, certificates, pem files, etc
- [Providers](Providers/ReadMe.md) - integration with libraries that provide most of the underlying functionality (such as openssl)
- [SSL](SSL/ReadMe.md) - code to manage SSL streams (networking)
