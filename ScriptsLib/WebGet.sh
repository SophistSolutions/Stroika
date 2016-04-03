#!/bin/bash
OUTPUTFILE=$1
SRCURL1=$2
SRCURL2=$3
SRCURL3=$4
SRCURL4=$5
SRCURL5=$6
SRCURL6=$7

if [ "$SRCURL2" == "" ] ; then
	SRCURL2=$SRCURL1
fi
if [ "$SRCURL3" == "" ] ; then
	SRCURL3=$SRCURL2
fi
if [ "$SRCURL4" == "" ] ; then
	SRCURL4=$SRCURL1
fi
if [ "$SRCURL5" == "" ] ; then
	SRCURL5=$SRCURL3
fi
if [ "$SRCURL6" == "" ] ; then
	SRCURL6=$SRCURL4
fi

RETRIES=2

echo -n "wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL1 ... "
wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL1
if [ $? -eq 0 ] ; then
	echo "done"
    	exit 0;
fi

echo failed
echo "WARNING: first mirror failed, so retrying:"
echo -n "wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL2 ... "
wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL2
if [ $? -eq 0 ] ; then
	echo "done"
    	exit 0;
fi

echo failed
echo "WARNING: second mirror failed, so retrying:"
echo -n "wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL3 ... "
wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL3
if [ $? -eq 0 ] ; then
	echo "done"
    	exit 0;
fi

echo failed
echo "WARNING: second mirror failed, so retrying:"
echo -n "wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL4 ... "
wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL4
if [ $? -eq 0 ] ; then
	echo "done"
    	exit 0;
fi

echo failed
echo "WARNING: second mirror failed, so retrying:"
echo -n "wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL5 ... "
wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL5
if [ $? -eq 0 ] ; then
	echo "done"
    	exit 0;
fi

echo failed
echo "WARNING: second mirror failed, so retrying:"
echo -n "wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL6 ... "
wget --quiet --tries=$RETRIES -O $OUTPUTFILE $SRCURL6
if [ $? -eq 0 ] ; then
	echo "done"
    	exit 0;
fi

echo Failed downloading $OUTPUTFILE
rm -f $OUTPUTFILE
exit 1
