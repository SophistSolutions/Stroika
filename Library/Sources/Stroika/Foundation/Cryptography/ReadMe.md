# Stroika::[Foundation](../)::Cryptography

This folder contains all the Stroika Library [Foundation](../)::Cryptography source code.

This module mostly contains functions to convert a stream of data in 'unencoded' format to/from
a stream of data in some 'encoded' format (folder 'Encoding').

And it contains functions that compute a cryptographic digest (or hash), in the folder 'Hash'.

- [Digest/](Digest/) - algorithms to 'hash' content
- [Encoding/](Encoding/) - algorithms to 'encrypt' (or decrypt), or otherwise transform such as base64
- [Format.h](Format.h) - utility to format some binary structures in common ways done for crypto
- [PKI](PKI/) - private keys, certificates, pem files, etc
- [Providers](Providers/) - integration with libraries that provide most of the underlying functionality (such as openssl)
- [SSL](SSL/) - code to manage SSL streams (networking)
