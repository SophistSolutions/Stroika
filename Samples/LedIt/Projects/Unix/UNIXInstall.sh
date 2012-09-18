#!/bin/sh
#	Copyright(c) Lewis Gordon Pringle, Jr. 1994-2001.  All rights reserved
#
#	$Header: /cygdrive/k/CVSRoot/LedIt/UNIXInstall.sh,v 1.3 2002/05/06 21:30:56 lewis Exp $
#
#	Description:
#		Script to help install of the LedIt! wordprocessor for Linux.
#
#	$Log: UNIXInstall.sh,v $
#	Revision 1.3  2002/05/06 21:30:56  lewis
#	<========================================= Led 3.0.1 Released ========================================>
#	
#	Revision 1.2  2001/11/27 00:28:07  lewis
#	<=============== Led 3.0 Released ===============>
#	
#	Revision 1.1  2001/05/24 21:07:10  lewis
#	SPR#0898/0934- first cut at Linux LedIt! installer/readme files etc
#	
#
#		<<Credits: This installer is based on the one from AbiWord - 0.7.14-2-Linux_i386_static>>
#

INSTALL_DATA_FILE=`pwd`/data.tar
DEFAULT_PREFIX=/usr/local
DEFAULT_HOME=LedIt
DEFAULT_LINK_DIR=/usr/local/bin

# Test for SysV echo; NECHO is defined for "no newline" echos
if [ "`echo 'echo\c'`" = "echo\c" ]
then
    NECHO="echo -n"
    POSTNECHO=""
else
    NECHO="echo"
    POSTNECHO="\c"
fi
ECHO="echo"

# Program execution
cat <<EOF

	LedIt! Program Installer, Copyright (C) 2001, Sophist Solutions, Inc.

	This program comes with ABSOLUTELY NO WARRANTY; this software is
	free software, and you are welcome to redistribute it.
EOF

# Make sure the data file is really here
if [ ! -f $INSTALL_DATA_FILE ]
then
    $ECHO ""
    $ECHO "Fatal error: can't find the file [$INSTALL_DATA_FILE]"
    $ECHO "which contains LedIt! program and data files."
    $ECHO ""
    exit 1
fi

# Read prefix from user
cat <<EOF

  Please specify the directory into which you would like to install LedIt!.
  The default directory is [$DEFAULT_PREFIX/$DEFAULT_HOME], 
  but you may provide an alternate path if you wish.  Hit "Enter" to use
  the default value.
EOF

GO=1
while test $GO -eq 1
do
    $ECHO ""
    $NECHO "Installation path for LedIt! software [$DEFAULT_PREFIX/$DEFAULT_HOME]: $POSTNECHO"
    read INSTALL_BASE

    # did they use tildes for home dirs?
    if [ ! -z "`echo $INSTALL_BASE | grep '~'`" ]
    then
	LINK_DIR=`echo $INSTALL_BASE | sed "s:~:$HOME:"`
    fi

    # did they just hit enter?
    if [ -z "$INSTALL_BASE" ]
    then
	INSTALL_BASE="$DEFAULT_PREFIX/$DEFAULT_HOME"
    fi

    # GO off, passed tests
    GO=0
    if [ ! -d $INSTALL_BASE ]
    then
	$ECHO ""
	$NECHO "The directory [$INSTALL_BASE] does not exist; would you like to create it now? (y/n) [y]: $POSTNECHO"
	read CREATE_DIR
	if [ "$CREATE_DIR" = "n" -o "$CREATE_DIR" = "N" ]
	then
	    GO=1
	else
	    mkdir -p $INSTALL_BASE
	    if [ $? -ne 0 ]
	    then
		$ECHO ""
		$ECHO "I couldn't create [$INSTALL_BASE].  You are probably seeing this error"
 		$ECHO "because you do not have sufficient permission to perform this operation."
		$ECHO "You will most likely have to run this script as superuser to write to"
		$ECHO "system directories."
		# loop around again
		GO=1
	    fi
	fi
    else
	$ECHO ""
	$ECHO "  I found an existing directory called [$INSTALL_BASE].  You can choose"
	$ECHO "  to install into this directory, but existing files will be modified or"
	$ECHO "  replaced.  You can also choose not to install into this directory, and"
	$ECHO "  you will be prompted for another."
	$ECHO ""
	$NECHO "Do you want to install into [$INSTALL_BASE]? (y/n) [y]: $POSTNECHO"
	read INSTALL_OVER
	if [ "$INSTALL_OVER" = "n" -o "$INSTALL_OVER" = "N" ]
	then
	    GO=1
	fi
    fi
done

########################################################################
# Blow up our data file
########################################################################

$ECHO ""
$ECHO "Installing LedIt! software in [$INSTALL_BASE]..."
cd $INSTALL_BASE
tar xf $INSTALL_DATA_FILE

if [ $? -ne 0 ] 
then 
    $ECHO "" 
    $ECHO "  Oops, tar seems to be having some trouble."
	$ECHO "  Refer to the errors above for more details." 
    $ECHO "  Installation aborted." 
    $ECHO "" 
    exit 1 
fi


cd $INSTALL_BASE



########################################################################
# Ask the user if he would like to set up symlinks to the script
# we just created.
########################################################################

cat <<EOF

  LedIt! program is now installed:
      $INSTALL_BASE/bin/LedIt

  As a convenience, I can install symbolic links to the installed
  executables so you and other users do not have to modify
  your paths to execute the LedIt! program.  For example, if you
  proceed and specify "$DEFAULT_LINK_DIR", I will create a symbolic links like
  "$DEFAULT_LINK_DIR/LedIt" and point it to the executable just
  previously installed.
EOF

# go for symlinks
GO=1
ASKED_LINKS=0
while test $GO -eq 1
do
    if [ $ASKED_LINKS -eq 0 ]
    then
	ASKED_LINKS=1
	$ECHO ""
	$NECHO "Do you want to provide a directory for these symbolic links? (y/n) [y]: $POSTNECHO"
	read MAKE_LINKS
	if [ "$MAKE_LINKS" = "n" -o "$MAKE_LINKS" = "N" ]
	then
	    $ECHO ""
	    $ECHO "Installation complete."
	    $ECHO ""
	    exit
	fi
    fi

    $ECHO ""
    $ECHO "In which directory shall I install the symbolic links?"
    $NECHO "[$DEFAULT_LINK_DIR]: $POSTNECHO"
    read LINK_DIR

    # did they use tildes for home dirs?
    if [ ! -z "`echo $LINK_DIR | grep '~'`" ]
    then
	LINK_DIR=`echo $LINK_DIR | sed "s:~:$HOME:"`
    fi

    # did they just hit enter?
    if [ -z "$LINK_DIR" ]
    then
	LINK_DIR="$DEFAULT_LINK_DIR"
    fi

    # GO off, passed tests
    GO=0
    if [ ! -d $LINK_DIR ]
    then
	$ECHO ""
	$NECHO "The directory [$LINK_DIR] does not exist; would you like to create it now? (y/n) [y]: $POSTNECHO"
	read CREATE_DIR
	if [ "$CREATE_DIR" = "n" -o "$CREATE_DIR" = "N" ]
	then
	    GO=1
	else
	    mkdir -p $LINK_DIR
	    if [ $? -ne 0 ]
	    then
		$ECHO ""
		$ECHO "  I couldn't create [$LINK_DIR].  You are probably seeing this error"
 		$ECHO "  because you do not have sufficient permission to perform this operation."
		$ECHO "  You will most likely have to run this script as superuser to write to"
		$ECHO "  system directories."
		$ECHO ""
		$ECHO "  If you wish, you can manually create these links at a later time."
		$ECHO "  You may cancel the installation of these links by issuing a"
		$ECHO "  terminal interrupt (usually Control-C), or you can provide another"
		$ECHO "  directory now."
		# loop again
		GO=1
	    fi
	fi
    fi
done

# Do the linkage to the installed, dynamically-generated SCRIPTS,
# not the actual executable binaries.  We install two links, one with 
# proper caps and one for the strong proponents of lowercase naming.
#
# Add more pairs here for any more binaries we install.

# NOTE : Solaris ln doesn't seem to honor the -f (force flag), so
# NOTE : we have to remove them first.
rm -f $LINK_DIR/LedIt; ln -s $INSTALL_BASE/bin/LedIt $LINK_DIR/LedIt
rm -f $LINK_DIR/ledit; ln -s $INSTALL_BASE/bin/LedIt $LINK_DIR/ledit

########################################################################
# Bye!
########################################################################
$ECHO ""
$ECHO "Installation complete and symbolic links installed."
$ECHO ""
