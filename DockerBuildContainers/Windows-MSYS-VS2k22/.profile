# Because otherwise some MSYS environment variables not yet, and that causes some tools to not work right (e.g. Xerces/cmake/openssl)
if [ "$MSYSTEM" == "" ] ; then . /etc/profile ; fi
