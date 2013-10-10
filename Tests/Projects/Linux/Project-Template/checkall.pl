#!/usr/bin/perl 

###require "../../../ScriptsLib/test-checkall-template.pl";
###
###DoCheckBuilds (NNN);


CheckFileExists ("../../../../Builds/DefaultConfiguration/TestNNN");

print "[SUCCEEDED]\r\n";


sub     CheckFileExists {
        local $file = $_[0];
        if (not (-e $file)) {
                Failed ("$file does not exist");
        }
}


sub Failed {
        local $reason = $_[0];
        print "[FAILED]: $reason\r\n";
        exit(1);
}
