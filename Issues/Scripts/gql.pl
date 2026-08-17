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
my $http = HTTP::Tiny->new (agent => 'stroika-projects-setup/1.0');
my %req  = (headers => {Authorization => "Bearer $t", 'Content-Type' => 'application/json'},
            content => encode_json ({query => $q}));

# Retry 5xx. GitHub's GraphQL endpoint returns intermittent 503s during incidents (observed flapping at
# roughly 1-in-3 while REST stayed healthy), and a one-shot request makes a transient outage look like a
# hard failure - which is exactly how a permissions error and a bad afternoon get confused.
my ($r, $body);
for my $try (1 .. 6) {
    $r    = $http->post ('https://api.github.com/graphql', \%req);
    $body = $r->{content} // '';
    last if $r->{success};
    last if $r->{status} < 500 and $r->{status} != 429;    # 4xx is our problem, not theirs - do not retry
    my $wait = 3 * $try;
    warn "  HTTP $r->{status} (attempt $try) - retrying in ${wait}s\n";
    sleep $wait;
}
if (!$r->{success}) { $body =~ s/\s+/ /g; die "HTTP $r->{status}: " . substr ($body, 0, 400) . "\n" }
my $j = eval { decode_json ($body) };
die "unparseable response: " . substr ($body, 0, 300) . "\n" unless $j;
if ($j->{errors}) {
    print STDERR "GraphQL errors:\n";
    print STDERR "  - $_->{message}\n" for @{$j->{errors}};
}
print JSON::PP->new->pretty->canonical->encode ($j);
