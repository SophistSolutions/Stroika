#!/usr/bin/perl
#
# Add imported issues to the GitHub Project and set the fields GitHub Issues itself cannot hold.
#
#   perl Issues/Scripts/GitHubProjectSync.pl --project <number> [--owner <user>] [--limit N] [--go]
#
# DRY RUN unless --go.
#
# WHY THIS EXISTS. GitHub Issues has no custom fields and cannot sort by label, so JIRA's priority and
# 'last modified' have nowhere to live on the issue itself. A Project can hold them as real fields that
# sort and group - which is the only way to answer "show me the worst open bugs" or "show me what has sat
# untouched longest". This copies from the <!-- jira-import: {...} --> block in each issue body, so the
# issue remains the source of truth and this can be re-run at any time.
#
# TOKEN: Projects v2 is GraphQL-only, and the API does NOT accept fine-grained PATs - it wants a CLASSIC
# PAT with the 'project' scope (verify with: curl -D- -H "Authorization: Bearer $T" .../user | grep -i
# x-oauth-scopes). Keep it in ~/.stroika-github-project-token, separate from the fine-grained token used
# for the REST issue import, and revoke it when done.
#
# SCOPES, and the trap in them: 'project' alone is NOT enough. Creating or reading an ORG-owned project
# also needs 'read:org' - without it every org-scoped call fails with "does not have the correct
# permissions", which reads like an org policy problem and is not one. (User-owned projects work with
# 'project' alone, which makes the misdiagnosis easy.) Linking a project to a repository additionally
# needs 'public_repo', because that mutation writes to the REPOSITORY, not to the project.
#
use strict;
use warnings;
use JSON::PP;
use HTTP::Tiny;
use Getopt::Long;

binmode STDOUT, ':encoding(UTF-8)';

my $REPO    = $ENV{GITHUB_REPO} // 'SophistSolutions/Stroika';
my $MAPFILE = 'Issues/STK-to-GitHub.tsv';
my $TF      = $ENV{GITHUB_TOKEN_FILE} // "$ENV{HOME}/.stroika-github-project-token";

my ($projnum, $owner, $limit, $go, $is_org, $overwrite) = (undef, 'SophistSolutions', undef, 0, 1, 0);
GetOptions ('project=i' => \$projnum, 'owner=s' => \$owner, 'limit=i' => \$limit, 'go' => \$go,
            'org!' => \$is_org, 'overwrite' => \$overwrite)
    or die "bad options\n";

# Keys already synced. GitHub is the live tracker now, so re-setting fields from the frozen JIRA metadata
# would revert any Priority changed by hand in the project. Recording what has been synced means a re-run
# only touches NEW issues - which is what you want after each batch of the import.
my $SYNCED = 'Issues/project-synced.tsv';
my %synced;
if (-f $SYNCED) {
    open my $s, '<', $SYNCED or die $!;
    while (<$s>) { chomp; $synced{$_} = 1 if length }
    close $s;
}
die "--project <number> required\n" unless $projnum;
# A project is owned by an ORG or a USER, and they are different GraphQL roots - there is no lookup that
# covers both. Default to org (--noorg for a user-owned one).
my $OWNER_ROOT = $is_org ? 'organization' : 'user';

open my $th, '<', $TF or die "no token file at $TF\n";
chomp (my $token = <$th>);
close $th;
$token =~ s/^\s+|\s+$//g;

my $http = HTTP::Tiny->new (agent => 'stroika-project-sync/1.0');
sub gql {
    my ($q) = @_;
    for my $try (1 .. 6) {
        my $r = $http->post ('https://api.github.com/graphql',
            {headers => {Authorization => "Bearer $token", 'Content-Type' => 'application/json'},
             content => encode_json ({query => $q})});
        if ($r->{success}) {
            my $j = decode_json ($r->{content});
            if ($j->{errors}) { die "GraphQL: " . join ('; ', map { $_->{message} } @{$j->{errors}}) . "\n" }
            return $j->{data};
        }
        # 5xx only - a 4xx is a real error and retrying just hides it
        die "HTTP $r->{status}\n" if $r->{status} < 500 && $r->{status} != 429;
        my $w = 3 * $try;
        warn "  HTTP $r->{status} (try $try) - retry in ${w}s\n";
        sleep $w;
    }
    die "gave up after retries\n";
}

# ---- discover the project and its fields by NAME, so renaming a field does not silently no-op
my $d = gql (qq{query { $OWNER_ROOT(login:"$owner") { projectV2(number:$projnum) { id title url
      fields(first:30) { nodes {
        ... on ProjectV2Field { id name dataType }
        ... on ProjectV2SingleSelectField { id name dataType options { id name } } } } } } }});
my $proj = $d->{$OWNER_ROOT}{projectV2} or die "project $projnum not found for $OWNER_ROOT $owner\n";
my %F;
for my $f (@{$proj->{fields}{nodes}}) {
    next unless $f->{name};
    $F{$f->{name}} = {id => $f->{id}, type => $f->{dataType},
                      opt => {map { $_->{name} => $_->{id} } @{$f->{options} || []}}};
}
for my $need ('Priority', 'JIRA key', 'JIRA created', 'JIRA updated', 'JIRA type') {
    die "project is missing field '$need' - create it first\n" unless $F{$need};
}
print "project: $proj->{title}  $proj->{url}\n";

# ---- the map tells us which issues to touch
open my $mh, '<', $MAPFILE or die "no $MAPFILE - run the import first\n";
my @rows;
while (<$mh>) { chomp; my ($k, $n) = split /\t/; push @rows, [$k, $n] if $k and $n }
close $mh;
print scalar (@rows), " issue(s) in the map\n\n";

my ($owner_login, $repo_name) = split m{/}, $REPO;
my $n = 0;
for my $r (@rows) {
    my ($key, $num) = @$r;
    last if defined $limit and $n >= $limit;
    if ($synced{$key} and not $overwrite) { next }    # already done - do not revert hand edits
    $n++;

    # read the metadata back out of the issue body - the issue stays the source of truth
    my $q = qq{query { repository(owner:"$owner_login", name:"$repo_name") { issue(number:$num) { id body } } }};
    my $iss = gql ($q)->{repository}{issue} or do { warn "  $key: issue #$num not found\n"; next };
    my ($json) = $iss->{body} =~ /<!-- jira-import:\s*(\{.*?\})\s*-->/s;
    if (!$json) { warn "  $key: no jira-import metadata in #$num - skipping\n"; next }
    my $m = eval { decode_json ($json) } or do { warn "  $key: unparseable metadata\n"; next };

    my @sets;
    push @sets, ['JIRA key', qq{text:"$m->{key}"}] if $m->{key};
    for my $pair (['JIRA created', 'created'], ['JIRA updated', 'updated']) {
        next unless $m->{$pair->[1]};
        my ($date) = $m->{$pair->[1]} =~ /^(\d{4}-\d{2}-\d{2})/ or next;
        push @sets, [$pair->[0], qq{date:"$date"}];
    }
    for my $pair (['Priority', 'priority'], ['JIRA type', 'type']) {
        my $v = $m->{$pair->[1]} or next;
        my $oid = $F{$pair->[0]}{opt}{$v} or do { warn "  $key: no option '$v' on $pair->[0]\n"; next };
        push @sets, [$pair->[0], qq{singleSelectOptionId:"$oid"}];
    }

    if (!$go) {
        printf "[dry] %-9s #%-4d %s\n", $key, $num,
            join (', ', map { "$_->[0]=" . ($_->[1] =~ /"([^"]*)"/ ? $1 : '?') } @sets);
        next;
    }

    my $item = gql (qq{mutation { addProjectV2ItemById(input:{projectId:"$proj->{id}", contentId:"$iss->{id}"})
                       { item { id } } }})->{addProjectV2ItemById}{item}{id};
    for my $s (@sets) {
        gql (qq{mutation { updateProjectV2ItemFieldValue(input:{projectId:"$proj->{id}", itemId:"$item",
                fieldId:"$F{$s->[0]}{id}", value:{$s->[1]}}) { projectV2Item { id } } }});
        select undef, undef, undef, 0.25;
    }
    open my $sf, '>>', $SYNCED or die $!;
    print {$sf} "$key\n";
    close $sf;
    printf "  %-9s #%-4d -> item set (%d field(s))\n", $key, $num, scalar @sets;
}
print $go ? "\nsynced $n issue(s)\n" : "\nDRY RUN - $n issue(s). Re-run with --go.\n";
