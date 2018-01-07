#!/bin/bash
#
#	Put in one place messages for installing missing tools/components.
#	The packages / names often differ from system to system
#

TOOL=$1

OS=$OSTYPE
DistroBasedOn="Unknown"

if [ -f /etc/redhat-release ] ; then
    DistroBasedOn='RedHat'
    DIST=`cat /etc/redhat-release |sed s/\ release.*//`
    PSUEDONAME=`cat /etc/redhat-release | sed s/.*\(// | sed s/\)//`
    REV=`cat /etc/redhat-release | sed s/.*release\ // | sed s/\ .*//`
elif [ -f /etc/SuSE-release ] ; then
    DistroBasedOn='SuSe'
    PSUEDONAME=`cat /etc/SuSE-release | tr "\n" ' '| sed s/VERSION.*//`
    REV=`cat /etc/SuSE-release | tr "\n" ' ' | sed s/.*=\ //`
elif [ -f /etc/mandrake-release ] ; then
    DistroBasedOn='Mandrake'
    PSUEDONAME=`cat /etc/mandrake-release | sed s/.*\(// | sed s/\)//`
    REV=`cat /etc/mandrake-release | sed s/.*release\ // | sed s/\ .*//`
elif [ -f /etc/debian_version ] ; then
    DistroBasedOn='Debian'
    DIST=`cat /etc/lsb-release | grep '^DISTRIB_ID' | awk -F=  '{ print $2 }'`
    PSUEDONAME=`cat /etc/lsb-release | grep '^DISTRIB_CODENAME' | awk -F=  '{ print $2 }'`
    REV=`cat /etc/lsb-release | grep '^DISTRIB_RELEASE' | awk -F=  '{ print $2 }'`
fi
case `uname` in
   CYGWIN*)  DistroBasedOn='cygwin' ;;
esac

echo -n "Missing component $TOOL"
if [ $TOOL == "libtool" ] ; then
	if [ $DistroBasedOn == "Debian" ] ; then
		echo -n ": try apt-get install libtool-bin"
	fi
elif [ $TOOL == "7za" ] ; then
	if [ $DistroBasedOn == "Debian" ] ; then
		echo -n ": try apt-get install p7zip-full"
	elif [ $DistroBasedOn == "cygwin" ] ; then
		echo -n ": try cygwin setup, package name p7zip"
	fi
elif [ $TOOL == "tr" ] ; then
	if [ $DistroBasedOn == "Debian" ] ; then
		echo -n ": try apt-get install coreutils"
	fi
elif [ $TOOL == "unix2dos" ] ; then
	if [ $DistroBasedOn == "Debian" ] ; then
		echo -n ": try apt-get install dos2unix"
	elif [ $DistroBasedOn == "cygwin" ] ; then
		echo -n ": try cygwin setup, package name dos2unix"
	fi
elif [ $TOOL == "realpath" ] ; then
	if [ `uname -s` == "Darwin" ] ; then
		echo -n ": try make install-realpath"
	elif [ $DistroBasedOn == "Debian" ] ; then
		echo -n ": try apt-get install coreutils"
	fi
elif [ $TOOL == "node" ] ; then
	if [ $DistroBasedOn == "cygwin" ] ; then
		echo -n ": try installing https://nodejs.org/en/ msi installer"
	fi
	else 
		echo -n ": try apt-get install nodejs-legacy"
	fi
else
	if [ `uname -s` == "Darwin" ] ; then
		echo -n ": try brew install $TOOL"
	elif [ $DistroBasedOn == "Debian" ] ; then
		echo -n ": try apt-get install $TOOL"
	elif [ $DistroBasedOn == "cygwin" ] ; then
		echo -n ": try cygwin setup, package name $TOOL"
	fi
fi
echo

