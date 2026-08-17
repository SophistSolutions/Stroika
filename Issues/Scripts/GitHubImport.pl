#!/usr/bin/perl
#
# Import the JIRA archive (Issues/Archive/*.json + *.md) into GitHub Issues.
#
#   perl Issues/Scripts/GitHubImport.pl --labels          # what labels would be needed, and which exist
#   perl Issues/Scripts/GitHubImport.pl --create-labels --go
#   perl Issues/Scripts/GitHubImport.pl                   # DRY RUN - prints what it would do
#   perl Issues/Scripts/GitHubImport.pl --go              # actually import
#   perl Issues/Scripts/GitHubImport.pl --go --limit 5    # a cautious first batch
#   perl Issues/Scripts/GitHubImport.pl --go --only STK-972
#
# DRY RUN IS THE DEFAULT. Creating ~1000 issues is irreversible and notifies every repo watcher, so
# nothing is written without --go.
#
# RESUMABLE: every created issue is appended to Issues/STK-to-GitHub.tsv immediately, and keys already
# listed there are skipped. Safe to interrupt and re-run.
#
# WHAT GOES WHERE. GitHub Issues has no custom fields, so everything JIRA tracked that has no GitHub
# equivalent is preserved in the issue BODY two ways:
#   - a machine-readable <!-- jira-import: {...} --> comment (invisible when rendered, greppable, and
#     re-parseable later if this ever needs to feed a Project field or a re-sync), and
#   - the human-readable table already in Issues/Archive/STK-NNNN.md.
# That matters most for 'updated' - the last-modified date BEFORE import - because GitHub stamps
# created_at/updated_at as the import moment and there is no way to set them.
#
# TOKEN: a FINE-GRAINED PAT, scoped to this repository only, with 'Issues: read and write' and nothing
# else. Not the classic PAT. Put it alone on one line in ~/.stroika-github-token (chmod 600), or point
# GITHUB_TOKEN_FILE at it. It is never echoed and never passed on a command line.
#
use strict;
use warnings;
use JSON::PP;
use HTTP::Tiny;
use Getopt::Long;

binmode STDOUT, ':encoding(UTF-8)';    # summaries contain non-ASCII; keep the dry-run listing readable
binmode STDERR, ':encoding(UTF-8)';

my $REPO      = $ENV{GITHUB_REPO} // 'SophistSolutions/Stroika';
my $ARCHIVE   = 'Issues/Archive';
my $MAPFILE   = 'Issues/STK-to-GitHub.tsv';
my $TOKENFILE = $ENV{GITHUB_TOKEN_FILE} // "$ENV{HOME}/.stroika-github-token";
my $MAXBODY   = 64000;    # GitHub's limit is 65536; leave room for the metadata header
# Pacing, set by MEASUREMENT not guesswork: at 1.2s this hit GitHub's SECONDARY rate limit after ~510
# issues, with escalating 403s. That limit is content-creation specific, caps around 500/hour, and does
# NOT show up in /rate_limit - `core` was only 720/5000 when it triggered, so the primary quota tells you
# nothing about it. 7.5s keeps ~480/hour, just under. Slower than it looks like it should be, on purpose.
my $SLEEP = $ENV{IMPORT_SLEEP} // 7.5;

# The .md renderings link attachments RELATIVELY ('](attachments/STK-0647/foo.txt)'), which resolves
# inside the repo but NOT in a GitHub issue body - there is no base to resolve against there, so the link
# silently goes nowhere. Rewrite to absolute blob URLs.
#
# Branch, not the default branch, on purpose: the repo's default is v3-Release, but Issues/ currently
# exists only on v3-Dev, so a default-branch link would 404 until the next promotion. NB these links also
# require the attachments to be PUSHED - committing locally is not enough.
my $LINK_BRANCH = $ENV{GITHUB_LINK_BRANCH} // 'v3-Dev';
sub absolutize_links {
    my ($md) = @_;
    my $base = "https://github.com/$REPO/blob/$LINK_BRANCH/Issues/Archive/attachments/";
    $md =~ s{\]\(attachments/}{]($base}g;
    return $md;
}

my ($go, $only, $limit, $labels_only, $create_labels, $patch_bodies, $clobber_ok)
    = (0, undef, undef, 0, 0, 0, 0);
GetOptions ('go' => \$go, 'only=s' => \$only, 'limit=i' => \$limit,
            'labels' => \$labels_only, 'create-labels' => \$create_labels,
            'patch-bodies' => \$patch_bodies, 'i-know-this-overwrites' => \$clobber_ok)
    or die "bad options\n";

# ---------------------------------------------------------------- label mapping (REVIEW THIS)
# JIRA component -> GitHub label. Follows the Foundation::X convention already on the repo. Components
# with no obvious home are mapped to a new label of the same shape; --labels tells you which of these
# do not exist yet, and --create-labels makes them.
my %COMPONENT_LABEL = (
    'Build-System'                 => 'Build System',
    'Foundation'                   => 'Foundation::*',
    'Foundation-Cache'             => 'Foundation::Cache',
    'Foundation-Characters'        => 'Foundation::Characters',
    'Foundation-Common'            => 'Foundation::Common',
    'Foundation-Configuration'     => 'Foundation::Configuration',
    'Foundation-Containers'        => 'Foundation::Containers',
    'Foundation-Cryptography'      => 'Foundation::Cryptography',
    'Foundation-Database'          => 'Foundation::Database',
    'Foundation-DataExchange'      => 'Foundation::DataExchange',
    'Foundation-Debug'             => 'Foundation::Debug',
    'Foundation-Execution'         => 'Foundation::Execution',
    'Foundation-IO'                => 'Foundation::IO',
    'Foundation-IO-Filesystem'     => 'Foundation::IO',
    'Foundation-IO-Network'        => 'Foundation::IO',
    'Foundation-Math'              => 'Foundation::Math',
    'Foundation-Memory'            => 'Foundation::Memory',
    'Foundation-Streams'           => 'Foundation::Streams',
    'Foundation-Time'              => 'Foundation::Time',
    'Foundation-Traversal'         => 'Foundation::Traversal',
    'Framework'                    => 'Frameworks::*',
    'Frameworks-Led'               => 'Frameworks::*',
    'Frameworks-Service'           => 'Frameworks::*',
    'Frameworks-SystemPerformance' => 'Frameworks::SystemPerformance',
    'Frameworks-UPnP'              => 'Frameworks::*',
    'Frameworks-WebServer'         => 'Frameworks::*',
    'Frameworks-WebService'        => 'Frameworks::*',
    'Documentation'                => 'Documentation',
    'Overall-System'               => 'Overall-System',
    'Samples'                      => 'Samples',
    'Testing'                      => 'Testing',
    'ThirdPartyProducts'           => 'ThirdPartyProducts',
);
my $MARKER_LABEL = 'jira-import';    # so the whole batch stays identifiable and filterable
sub type_label     { 'type::'     . $_[0] }
sub priority_label { 'priority::' . $_[0] }

# Label EVERY priority, including Medium. Medium happens to be JIRA's default value, and it is tempting
# to treat it as "never set" and drop it as noise - that was wrong. LGP assigns it deliberately, so the
# label carries real signal: this issue HAS been evaluated and judged middling. An unlabelled issue means
# nobody looked, which is a different and useful state to be able to see. Do not re-add the skip.
my $SKIP_DEFAULT_PRIORITY = 0;
my $DEFAULT_PRIORITY      = 'Medium';

# ---------------------------------------------------------------- http
my $token;
sub load_token {
    open my $h, '<', $TOKENFILE
        or die "No token file at $TOKENFILE.\nCreate a FINE-GRAINED PAT (this repo only, Issues: read+write),\n"
             . "put it alone on one line, chmod 600. See the header of this script.\n";
    chomp ($token = <$h>);
    close $h;
    $token =~ s/^\s+|\s+$//g;
    die "token file $TOKENFILE looks empty\n" unless length $token;
}
my $http = HTTP::Tiny->new (agent => 'stroika-jira-import/1.0');
sub api {
    my ($method, $path, $payload) = @_;
    my %opt = (headers => {'Authorization' => "Bearer $token", 'Accept' => 'application/vnd.github+json',
                           'X-GitHub-Api-Version' => '2022-11-28'});
    if ($payload) { $opt{content} = encode_json ($payload); $opt{headers}{'Content-Type'} = 'application/json' }
    for my $try (1 .. 8) {
        my $r = $http->request ($method, "https://api.github.com$path", \%opt);
        return decode_json ($r->{content} || '{}') if $r->{success};
        # 403/429: secondary rate limit. Honour Retry-After / x-ratelimit-reset when GitHub tells us how
        # long to wait - blind exponential backoff just guesses, and guessed too short here (a 30..210s
        # ladder never reached the ~1h the limit actually needed, so it burned 8 tries and died).
        if (($r->{status} == 403 || $r->{status} == 429) && $try < 8) {
            my $h    = $r->{headers} || {};
            my $ra   = $h->{'retry-after'};
            my $rst  = $h->{'x-ratelimit-reset'};
            my $wait = $ra                       ? $ra + 2
                     : ($rst && $rst > time ())  ? ($rst - time ()) + 5
                     :                             60 * $try;
            $wait = 3600 if $wait > 3600;    # a sane ceiling
            warn "  rate limited (HTTP $r->{status}); waiting ${wait}s"
               . ($ra ? " (Retry-After)" : $rst ? " (until reset)" : " (no hint given)") . "\n";
            sleep $wait;
            next;
        }
        # 5xx: GitHub's problem, not ours. A ~1450-call run WILL meet one of these during an incident, and
        # aborting the whole import over a transient 503 is the wrong answer - the map makes us resumable,
        # but only if we get far enough to write it.
        if ($r->{status} >= 500 && $try < 8) {
            my $wait = 5 * $try;
            warn "  HTTP $r->{status} (try $try) - retrying in ${wait}s\n";
            sleep $wait;
            next;
        }
        my $msg = $r->{content} // '';
        $msg =~ s/\s+/ /g;
        die "HTTP $r->{status} on $method $path: " . substr ($msg, 0, 300) . "\n";
    }
}

# ---------------------------------------------------------------- read the archive
my @issues;
for my $jf (sort glob "$ARCHIVE/*.json") {
    open my $h, '<:raw', $jf or die "$jf: $!";
    my $j = JSON::PP->new->utf8->decode (do { local $/; <$h> });
    close $h;
    my $fl = $j->{fields} || {};
    (my $mdf = $jf) =~ s/\.json$/.md/;
    my $md = '';
    if (-f $mdf) { open my $m, '<:raw', $mdf or die $!; $md = do { local $/; <$m> }; close $m }
    $md =~ s/^\#[^\n]*\n//;          # the H1 becomes the GitHub title
    $md = absolutize_links ($md);    # relative attachment links do not resolve in an issue body
    push @issues, {key => $j->{key}, fields => $fl, md => $md};
}
# numeric order, so GitHub numbers ascend with STK numbers
@issues = sort { ($a->{key} =~ /(\d+)/)[0] <=> ($b->{key} =~ /(\d+)/)[0] } @issues;
@issues = grep { $_->{key} eq $only } @issues if $only;

# ---------------------------------------------------------------- what labels are needed
my (%needed, @unmapped);
for my $i (@issues) {
    $needed{$MARKER_LABEL}++;
    for my $c (@{$i->{fields}{components} || []}) {
        if (my $l = $COMPONENT_LABEL{$c->{name}}) { $needed{$l}++ } else { push @unmapped, $c->{name} }
    }
    $needed{type_label ($i->{fields}{issuetype}{name})}++     if $i->{fields}{issuetype};
    $needed{priority_label ($i->{fields}{priority}{name})}++
        if $i->{fields}{priority} and not ($SKIP_DEFAULT_PRIORITY and $i->{fields}{priority}{name} eq $DEFAULT_PRIORITY);
}

if ($labels_only or $create_labels) {
    load_token ();
    my %have;
    for my $page (1 .. 5) {
        my $r = api ('GET', "/repos/$REPO/labels?per_page=100&page=$page");
        last unless @$r;
        $have{$_->{name}} = 1 for @$r;
    }
    my @missing = sort grep { !$have{$_} } keys %needed;
    printf "labels needed: %d   already on repo: %d   MISSING: %d\n",
        scalar keys %needed, (scalar keys %needed) - @missing, scalar @missing;
    printf "  %-34s %5d issues  %s\n", $_, $needed{$_}, ($have{$_} ? 'exists' : 'MISSING')
        for sort { $needed{$b} <=> $needed{$a} } keys %needed;
    if (@unmapped) {
        my %u; $u{$_}++ for @unmapped;
        print "\nJIRA components with NO mapping (edit %COMPONENT_LABEL): ", join (', ', sort keys %u), "\n";
    }
    if ($create_labels and @missing) {
        if (!$go) { print "\n(dry run - re-run with --go to create the ", scalar @missing, " missing labels)\n"; exit 0 }
        for my $l (@missing) {
            api ('POST', "/repos/$REPO/labels", {name => $l, color => 'ededed',
                 description => 'imported from JIRA component/type/priority'});
            print "  created label: $l\n";
            select undef, undef, undef, 0.3;
        }
    }
    exit 0;
}

# ---------------------------------------------------------------- title / body
# Shared by the create path and by --patch-bodies, so a change to the body format cannot apply to new
# issues but silently not to re-patched ones.
sub build_title {
    my ($i) = @_;
    my $t = sprintf '[%s] %s', $i->{key}, ($i->{fields}{summary} // '(no summary)');
    return length $t > 253 ? substr ($t, 0, 250) . '...' : $t;
}
sub build_body {
    my ($i) = @_;
    my $fl = $i->{fields};
    # everything GitHub has no field for, kept machine-readable so GitHubProjectSync.pl can read it back
    my %meta = (
        key         => $i->{key},
        created     => $fl->{created},
        updated     => $fl->{updated},    # last modified BEFORE import - GitHub stamps its own dates
        status      => $fl->{status} ? $fl->{status}{name} : undef,
        resolution  => $fl->{resolution} ? $fl->{resolution}{name} : undef,
        resolved    => $fl->{resolutiondate},
        type        => $fl->{issuetype} ? $fl->{issuetype}{name} : undef,
        priority    => $fl->{priority} ? $fl->{priority}{name} : undef,
        reporter    => $fl->{reporter} ? ($fl->{reporter}{displayName} // undef) : undef,
        assignee    => $fl->{assignee} ? ($fl->{assignee}{displayName} // undef) : undef,
        components  => [map { $_->{name} } @{$fl->{components} || []}],
        fixVersions => [map { $_->{name} } @{$fl->{fixVersions} || []}],
        jiraLabels  => $fl->{labels} || [],
        links       => [map { {type => $_->{type}{name}, key => ($_->{outwardIssue} || $_->{inwardIssue} || {})->{key}} }
                        @{$fl->{issuelinks} || []}],
        attachments => [map { $_->{filename} } @{$fl->{attachment} || []}],
    );
    my $metaline = '<!-- jira-import: ' . JSON::PP->new->canonical->encode (\%meta) . ' -->';
    my $body     = $metaline . "\n\n" . $i->{md};
    if (length $body > $MAXBODY) {
        my $keep = $MAXBODY - length ($metaline) - 200;
        $body = $metaline . "\n\n" . substr ($i->{md}, 0, $keep)
              . "\n\n---\n*Truncated for GitHub's issue body limit. Full text: `$ARCHIVE/"
              . sprintf ('%s-%04d', ($i->{key} =~ /^([A-Z]+)/)[0], ($i->{key} =~ /(\d+)/)[0]) . ".md` in this repo.*\n";
    }
    return $body;
}

# ---------------------------------------------------------------- resume map
my %done;
if (-f $MAPFILE) {
    open my $h, '<', $MAPFILE or die $!;
    while (<$h>) { chomp; my ($k, $n) = split /\t/; $done{$k} = $n if $k }
    close $h;
}
print "already imported (from $MAPFILE): ", scalar keys %done, "\n" if %done;

load_token () if $go;

# ---------------------------------------------------------------- --patch-bodies
# Rewrite the body of issues ALREADY imported, from the current .md. Needed whenever the renderer
# improves after an import - eg the attachment links, which did not exist when the first batch went in.
# Bodies only: labels and state are left alone, so this is safe to re-run.
if ($patch_bodies) {
    # GitHub is the LIVE tracker now, so this is destructive: it replaces the body wholesale from the .md,
    # discarding anything edited in GitHub since the import. Harmless during the initial load, not after.
    if ($go and not $clobber_ok) {
        die "REFUSING: --patch-bodies overwrites issue bodies from the archive, discarding any edits made\n"
          . "in GitHub since the import. That was safe during the initial load; it is not any more.\n"
          . "Re-run with --i-know-this-overwrites if you are sure.\n";
    }
    my %bykey = map { $_->{key} => $_ } @issues;
    my $p = 0;
    for my $key (sort { ($a =~ /(\d+)/)[0] <=> ($b =~ /(\d+)/)[0] } keys %done) {
        last if defined $limit and $p >= $limit;
        next if $only and $key ne $only;
        my $i = $bykey{$key} or do { warn "  $key: in the map but not in the archive\n"; next };
        $p++;
        my $num  = $done{$key};
        my $body = build_body ($i);
        if (!$go) {
            my $n = () = $body =~ m{blob/$LINK_BRANCH/Issues/Archive/attachments}g;
            printf "[dry] %-9s #%-4d %6d chars, %d attachment link(s)\n", $key, $num, length $body, $n;
            next;
        }
        api ('PATCH', "/repos/$REPO/issues/$num", {body => $body});
        my $n = () = $body =~ m{blob/$LINK_BRANCH/Issues/Archive/attachments}g;
        printf "  %-9s #%-4d patched (%d chars, %d attachment link(s))\n", $key, $num, length $body, $n;
        select undef, undef, undef, $SLEEP;
    }
    print $go ? "\npatched $p body(s)\n" : "\nDRY RUN - $p body(s) would be patched. Re-run with --go.\n";
    exit 0;
}

my %have_label;
if ($go) {
    for my $page (1 .. 5) {
        my $r = api ('GET', "/repos/$REPO/labels?per_page=100&page=$page");
        last unless @$r;
        $have_label{$_->{name}} = 1 for @$r;
    }
}

my $n = 0;
for my $i (@issues) {
    last if defined $limit and $n >= $limit;
    next if $done{$i->{key}};
    my $fl = $i->{fields};
    $n++;

    my $title = build_title ($i);
    my $body  = build_body ($i);

    my @lab = ($MARKER_LABEL);
    push @lab, $COMPONENT_LABEL{$_->{name}} for grep { $COMPONENT_LABEL{$_->{name}} } @{$fl->{components} || []};
    push @lab, type_label ($fl->{issuetype}{name})    if $fl->{issuetype};
    push @lab, priority_label ($fl->{priority}{name})
        if $fl->{priority} and not ($SKIP_DEFAULT_PRIORITY and $fl->{priority}{name} eq $DEFAULT_PRIORITY);
    my %seenl; @lab = grep { !$seenl{$_}++ } @lab;
    @lab = grep { $have_label{$_} } @lab if $go;    # never fail an import over a missing label

    my $closed = $fl->{resolution} ? 1 : 0;
    my $reason = ($fl->{resolution} && $fl->{resolution}{name} =~ /^(Won't Do|Duplicate|Cannot Reproduce)$/)
                 ? 'not_planned' : 'completed';

    if (!$go) {
        printf "[dry] %-9s %-4s labels=%-58s %s\n", $i->{key}, ($closed ? 'CLSD' : 'open'),
            join (',', @lab), substr ($fl->{summary} // '', 0, 60);
        next;
    }

    my $created = api ('POST', "/repos/$REPO/issues", {title => $title, body => $body, labels => \@lab});
    my $num = $created->{number};
    open my $mh, '>>', $MAPFILE or die $!;
    print {$mh} "$i->{key}\t$num\n";
    close $mh;
    if ($closed) {
        select undef, undef, undef, 0.4;
        api ('PATCH', "/repos/$REPO/issues/$num", {state => 'closed', state_reason => $reason});
    }
    printf "  %-9s -> #%-5d %s%s\n", $i->{key}, $num, ($closed ? "closed($reason) " : ''),
        substr ($fl->{summary} // '', 0, 50);
    select undef, undef, undef, $SLEEP;
}

print $go ? "\nimported $n issue(s)\n" : "\nDRY RUN - $n issue(s) would be imported. Re-run with --go.\n";
print "map: $MAPFILE\n" if $go;
