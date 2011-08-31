#TRY THESE AGAIN, but for now generate too many problems...
#use strict;
#use warnings;

### THis file is minimally acceptable but mostly wrong.
### SEE http://bugzilla/show_bug.cgi?id=736
###


#print ("BEFORE SETTING PATH ENV=$ENV{'PATH'}\n");

#Only needed because I cannot find a way to run vcvarsall.bat from the perl script (and capture its environment vars)
$ENV{'PATH'} = "/cygdrive/c/Windows/Microsoft.NET/Framework/v4.0.30319:" . $ENV{'PATH'};


#foreach $key (sort(keys %ENV)) {
#    print "$key = $ENV{$key}<br>\n";
#}

local $PROGRAMFILESDIR= $ENV{'PROGRAMFILES'};
local $PROGRAMFILESDIR= `cygpath \"$PROGRAMFILESDIR\"`;
chomp($PROGRAMFILESDIR);
$ENV{'PATH'} = $ENV{'PATH'} . ":$PROGRAMFILESDIR/Microsoft Visual Studio 10.0/VC/";
$ENV{'PATH'} = $ENV{'PATH'} . ":/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 10.0/VC/";
$ENV{'PATH'} = $ENV{'PATH'} . ":$PROGRAMFILESDIR/Microsoft Visual Studio 10.0/VC/vcpackages/";
$ENV{'PATH'} = $ENV{'PATH'} . ":/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 10.0/VC/vcpackages/";



#maybe elsewhere add 'SDKDIR' environment var?
#$ENV{'PATH'} = $ENV{'PATH'} . ":/cygdrive/c/Program Files/Microsoft SDKs/Windows/v6.0A/bin/";
#$ENV{'PATH'} = $ENV{'PATH'} . ":/cygdrive/c/Program Files (x86)/Microsoft SDKs/Windows/v6.0A/bin/";
#$ENV{'PATH'} = $ENV{'PATH'} . ":/cygdrive/c/Program Files/Microsoft SDKs/Windows/v6.1/bin/";
#$ENV{'PATH'} = $ENV{'PATH'} . ":/cygdrive/c/Program Files (x86)/Microsoft SDKs/Windows/v6.1/bin/";

# WE SHOULD call
# 		call vcvarsall.bat x86
# and capture its results, but I'm not quite sore how yet....
### SEE http://bugzilla/show_bug.cgi?id=736


#print ("AFTER SETTING PATH ENV=$ENV{'PATH'}\n");

1
