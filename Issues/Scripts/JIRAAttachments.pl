#!/usr/bin/perl
#
# Download the attachments referenced by the archived issues into Issues/Archive/attachments/.
#
#   perl Issues/Scripts/JIRAAttachments.pl [ARCHIVEDIR] [OUTDIR]
#       defaults: Issues/Archive   Issues/Archive/attachments
#
# WHY THIS IS SEPARATE from JIRAExport.sh: JIRA's search API returns attachment METADATA only - filename,
# size, mime type and a content URL - never the bytes. So an issue export alone silently loses every
# attached file. This walks those content URLs.
#
# NO TOKEN NEEDED: the tracker is world-readable, and so are the attachment content URLs (verified). A
# creds file is used if present, but anonymous works.
#
# Layout: Issues/Archive/attachments/STK-NNNN/<sanitized filename>, zero-padded to match the archive.
# Filenames are sanitized ([^A-Za-z0-9._-] -> _) because the originals contain '#', spaces and parens,
# which are portability hazards in a cross-platform repo. The TRUE filename stays in the issue JSON.
#
use strict;
use warnings;
use JSON::PP;

my $archive = $ARGV[0] // 'Issues/Archive';
my $outdir  = $ARGV[1] // 'Issues/Archive/attachments';
my $creds   = $ENV{JIRA_CREDS_FILE} // "$ENV{HOME}/.stroika-jira-creds";

my $cfg = '';
if (-f $creds) {
    $cfg = "/tmp/jira-att-cfg.$$";
    open my $c, '>', $cfg or die $!;
    chmod 0600, $cfg;
    open my $h, '<', $creds or die $!;
    chomp (my $line = <$h>);
    close $h;
    print {$c} qq{user = "$line"\n};
    close $c;
}
END { unlink $cfg if $cfg }

my ($files, $bytes, $skipped) = (0, 0, 0);
for my $jf (sort glob "$archive/*.json") {
    open my $h, '<:raw', $jf or die "$jf: $!";
    my $j = JSON::PP->new->utf8->decode (do { local $/; <$h> });
    close $h;
    my @att = @{$j->{fields}{attachment} || []};
    next unless @att;
    my ($proj, $num) = $j->{key} =~ /^([A-Z]+)-(\d+)$/;
    my $dir = sprintf '%s/%s-%04d', $outdir, $proj, $num;
    mkdir $outdir unless -d $outdir;
    mkdir $dir    unless -d $dir;
    for my $a (@att) {
        my $name = $a->{filename} // 'unnamed';
        $name =~ s/[^A-Za-z0-9._-]/_/g;
        my $path = "$dir/$name";
        if (-f $path and -s $path == ($a->{size} // -1)) { $skipped++; next }    # already have it, right size
        my @cmd = ('curl', '-s', '-L', '--max-time', '120', '-o', $path);
        push @cmd, '--config', $cfg if $cfg;
        push @cmd, $a->{content};
        system (@cmd) == 0 or warn "  FAILED $j->{key} $name\n" and next;
        my $got = -s $path // 0;
        if ($a->{size} and $got != $a->{size}) {
            warn "  SIZE MISMATCH $j->{key} $name: got $got, expected $a->{size}\n";
        }
        $files++;
        $bytes += $got;
        printf "  %-9s %8d B  %s\n", $j->{key}, $got, $name;
    }
}
printf "\ndownloaded %d file(s), %.1f MB%s\n", $files, $bytes / 1048576,
    ($skipped ? " ($skipped already present, skipped)" : '');
