# Stroika::[Foundation](../../)::[Cryptography](../)::Encoding

This folder contains all the Stroika Library [Foundation](../../)::[Cryptography](../)::Encoding source code.

## Overview

Encryption - bytes transformed, and back - from strong to VERY WEAK ([Cryptography](../ReadMe.md) says why Base64 and Hex are
here).

### Algorithms

- [Algorithm/AES.h](Algorithm/AES.h)
- [Algorithm/Base64.h](Algorithm/Base64.h) - very weak: opaque to a parser, and to most readers
- [Algorithm/Hex.h](Algorithm/Hex.h) - very weak: opaque to a parser, and to most readers
- [Algorithm/RC4.h](Algorithm/RC4.h)

### Other

- [OpenSSLCryptoStream.h](OpenSSLCryptoStream.h)
