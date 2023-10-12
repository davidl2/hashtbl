#!/usr/bin/perl -w

dbmopen %xx,'xx',0666;

while (<STDIN>) {
    if (m!^i ([a-z0-9_.\$]+)/([ -~]*)!) {
	$xx{$1}=$2;
    } elsif (m!^e ([a-z0-9_.\$]+)!) {
	undef ($xx{$1});
    } elsif (m!^q ([a-z0-9_.\$]+)!) {	
	if (defined($xx{$1})) {
	    print $xx{$1}."\n";
	} else {
	    print "Not found.\n";
	}
    }
};
