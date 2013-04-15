OpenSSL is an open source cryptography and SSL implementation.
If built with Stroika, it can take advantage of these features.

Just copy the original zipfile into Origs, and update the build.pl script
to know which version to extract.

Generally, we do builds of just static versions of the library, because this
is my preference. If you wanted to use a DLL version, you probably would be
just using the OS-installed version?

[[TODO - 64 bit windows builds]]
