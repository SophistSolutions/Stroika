OpenSSL is an open source cryptography and SSL implementation.
If built with Stroika, it can take advantage of these features.

The version of openssl used is defined at the top of the buildall.pl file.

It will first check in the ..\Origs-cache folder for that version and
if not already present, it will automatically download it (to there).

Generally, we do builds of just static versions of the library, because this
is my preference. If you wanted to use a DLL version, you probably would be
just using the OS-installed version?

[[TODO - 64 bit windows builds]]
