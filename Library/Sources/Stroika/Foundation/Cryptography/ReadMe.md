# Stroika::[Foundation](../)::Cryptography

This folder contains all the Stroika Library [Foundation](../)::Cryptography source code.

This module mostly contains functions to convert a stream of data in 'unencoded' format to/from
a stream of data in some 'encoded' format (folder 'Encoding').

And it contains functions that compute a cryptographic digest (or hash), in the folder 'Hash'.

NB: it is because of this organizational principle, that Base64 is grouped here
(not in data format exchange).

- [Digest/](Digest/)
- [Encoding/](Encoding/)
- [Format.h](Format.h)
- [OpenSSL](OpenSSL/)
- [SSL](SSL/)
