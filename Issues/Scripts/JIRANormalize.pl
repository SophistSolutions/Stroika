#!/usr/bin/perl
#
# Turn the raw paged JSON from jira-export.sh into a per-issue archive with TWO representations:
#
#   STK-NNNN.json  - the record of fidelity. Canonical (sorted keys, stable indent) so a field change
#                    is a one-line diff, and with empty/null fields dropped - that is lossless, since
#                    an absent-and-null field says nothing beyond "the schema has this column", and it
#                    removes ~2/3 of the bytes (JIRA returns ~33 customfield_* per issue, 92% null).
#
#   STK-NNNN.md    - the readable one. JIRA stores descriptions and comments in Atlassian Document
#                    Format: text fragmented across nested nodes, so grepping the JSON for a sentence
#                    fails as soon as a bold word or line break splits it. This flattens ADF to plain
#                    text, which is what makes the archive greppable and reviewable in a diff.
#
#   perl .claude/jira-normalize.pl <rawdir> <outdir>
#
use strict;
use warnings;
use JSON::PP;

my ($rawdir, $outdir) = @ARGV;
die "usage: $0 <rawdir> <outdir>\n" unless $rawdir && $outdir;
mkdir $outdir unless -d $outdir;

my $json = JSON::PP->new->pretty->canonical->utf8;

# ---- drop empty leaves, recursively. Returns undef if the whole node is empty.
sub prune {
    my ($v) = @_;
    if (ref $v eq 'HASH') {
        my %o;
        for my $k (keys %$v) {
            my $p = prune ($v->{$k});
            $o{$k} = $p if defined $p;
        }
        return %o ? \%o : undef;
    }
    if (ref $v eq 'ARRAY') {
        my @o = grep { defined } map { prune ($_) } @$v;
        return @o ? \@o : undef;
    }
    return undef if !defined $v;
    return undef if !ref $v && $v eq '';
    return $v;
}

# ---- flatten Atlassian Document Format (or a plain string) to text
sub adf_text {
    my ($n, $depth) = @_;
    $depth //= 0;
    return '' unless defined $n;
    return $n unless ref $n;                       # old JIRA returned plain strings
    if (ref $n eq 'ARRAY') { return join '', map { adf_text ($_, $depth) } @$n }
    my $t = $n->{type} // '';
    my $kids = $n->{content};
    return $n->{text} // '' if $t eq 'text';
    return "\n"                                  if $t eq 'hardBreak';
    return "---\n"                               if $t eq 'rule';
    if ($t eq 'codeBlock') {
        my $lang = $n->{attrs} && $n->{attrs}{language} ? $n->{attrs}{language} : '';
        return "\n```$lang\n" . adf_text ($kids, $depth) . "\n```\n";
    }
    if ($t eq 'heading') {
        my $lvl = ($n->{attrs} && $n->{attrs}{level}) ? $n->{attrs}{level} : 1;
        return "\n" . ('#' x ($lvl + 2)) . ' ' . adf_text ($kids, $depth) . "\n";
    }
    if ($t eq 'listItem')  { return ('  ' x $depth) . '- ' . adf_text ($kids, $depth) }
    if ($t eq 'bulletList' or $t eq 'orderedList') { return adf_text ($kids, $depth + 1) }
    if ($t eq 'paragraph') { return adf_text ($kids, $depth) . "\n" }
    if ($t eq 'inlineCard' or $t eq 'blockCard') {
        return ($n->{attrs} && $n->{attrs}{url}) ? $n->{attrs}{url} : '';
    }
    if ($t eq 'mediaSingle' or $t eq 'media') { return "[attachment]" }
    return adf_text ($kids, $depth);
}

sub trim { my $s = shift // ''; $s =~ s/\s+$//; $s =~ s/\n{3,}/\n\n/g; return $s }
sub who  { my $u = shift; return 'unknown' unless ref $u; return $u->{displayName} // $u->{emailAddress} // 'unknown' }

my ($pages, $written) = (0, 0);
my %seen;
for my $f (sort glob "$rawdir/_page_*.json") {
    open my $fh, '<:raw', $f or die "$f: $!";
    my $body = do { local $/; <$fh> };
    close $fh;
    my $page = eval { decode_json ($body) };
    if (!$page) { warn "skipping $f - not parseable JSON\n"; next; }
    $pages++;
    for my $issue (@{$page->{issues} || []}) {
        my $key = $issue->{key} or next;
        my $name = $key;
        $name =~ s/^([A-Z]+)-(\d+)$/sprintf ("%s-%04d", $1, $2)/e;
        next if $seen{$name}++;

        # --- fidelity copy, pruned
        delete $issue->{expand};
        my $pruned = prune ($issue) // {};
        open my $j, '>:raw', "$outdir/$name.json" or die $!;
        print {$j} $json->encode ($pruned);
        close $j;

        # --- readable copy
        my $fl = $issue->{fields} || {};
        my @md = ("# $key - " . ($fl->{summary} // '(no summary)'), '');
        push @md, '| | |', '|---|---|';
        push @md, "| status | " . ($fl->{status} ? $fl->{status}{name} : '?') . ' |';
        push @md, "| type | " . ($fl->{issuetype} ? $fl->{issuetype}{name} : '?') . ' |'
            if $fl->{issuetype};
        push @md, "| priority | " . $fl->{priority}{name} . ' |' if $fl->{priority} && $fl->{priority}{name};
        push @md, "| resolution | " . $fl->{resolution}{name} . ' |' if $fl->{resolution} && $fl->{resolution}{name};
        push @md, "| reporter | " . who ($fl->{reporter}) . ' |' if $fl->{reporter};
        push @md, "| assignee | " . who ($fl->{assignee}) . ' |' if $fl->{assignee};
        push @md, "| created | " . ($fl->{created} // '?') . ' |';
        push @md, "| updated | " . ($fl->{updated} // '?') . ' |';
        push @md, "| resolved | " . $fl->{resolutiondate} . ' |' if $fl->{resolutiondate};
        if ($fl->{labels} && @{$fl->{labels}}) { push @md, '| labels | ' . join (', ', @{$fl->{labels}}) . ' |' }
        if ($fl->{attachment} && @{$fl->{attachment}}) {
            # link into attachments/, which JIRAAttachments.pl populates - the search API returns attachment
            # metadata only, so without that script these are names pointing at nothing. Sanitizing must match
            # what that script does, or the links dangle.
            push @md, '| attachments | ' . join (', ', map {
                my $orig = $_->{filename} // '?';
                (my $safe = $orig) =~ s/[^A-Za-z0-9._-]/_/g;
                sprintf '[%s](attachments/%s/%s)', $orig, $name, $safe;
            } @{$fl->{attachment}}) . ' |';
        }
        push @md, '', "<https://stroika.atlassian.net/browse/$key>", '';

        my $d = trim (adf_text ($fl->{description}));
        if (length $d) { push @md, '## Description', '', $d, '' }

        my $cs = $fl->{comment} && $fl->{comment}{comments} ? $fl->{comment}{comments} : [];
        if (@$cs) {
            push @md, '## Comments', '';
            for my $c (@$cs) {
                push @md, '### ' . who ($c->{author}) . ' - ' . ($c->{created} // '?'), '';
                push @md, trim (adf_text ($c->{body})), '';
            }
        }

        open my $m, '>:encoding(UTF-8)', "$outdir/$name.md" or die $!;
        print {$m} join ("\n", @md), "\n";
        close $m;
        $written++;
    }
}
print "read $pages page(s); wrote $written issue(s) (json+md) to $outdir\n";