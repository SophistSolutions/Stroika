#!/usr/bin/perl
#
# Minimal GitHub GraphQL helper - reads a query (or mutation) on stdin, prints the JSON response.
# Used by the Projects v2 setup, since Projects v2 has no REST API.
#
#   echo 'query { viewer { login } }' | perl Issues/Scripts/gql.pl
#
# Token comes from GITHUB_TOKEN_FILE or ~/.stroika-github-token; never echoed.
#
use strict;
use warnings;
use JSON::PP;
use HTTP::Tiny;

my $tf = $ENV{GITHUB_TOKEN_FILE} // "$ENV{HOME}/.stroika-github-token";
open my $h, '<', $tf or die "no token file at $tf\n";
chomp (my $t = <$h>);
close $h;
$t =~ s/^\s+|\s+$//g;

my $q = do { local $/; <STDIN> };
my $r = HTTP::Tiny->new (agent => 'stroika-projects-setup/1.0')->post (
    'https://api.github.com/graphql',
    {headers => {Authorization => "Bearer $t", 'Content-Type' => 'application/json'},
     content => encode_json ({query => $q})});

my $body = $r->{content} // '';
if (!$r->{success}) { $body =~ s/\s+/ /g; die "HTTP $r->{status}: " . substr ($body, 0, 400) . "\n" }
my $j = eval { decode_json ($body) };
die "unparseable response: " . substr ($body, 0, 300) . "\n" unless $j;
if ($j->{errors}) {
    print STDERR "GraphQL errors:\n";
    print STDERR "  - $_->{message}\n" for @{$j->{errors}};
}
print JSON::PP->new->pretty->canonical->encode ($j);
