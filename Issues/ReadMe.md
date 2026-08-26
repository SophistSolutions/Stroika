# Issues

## Reporting a bug, or asking a question

**Use [GitHub Issues](https://github.com/SophistSolutions/Stroika/issues) on this repository.** That is
where Stroika's issues live and where all new work is tracked.

The [Stroika Issues project](https://github.com/SophistSolutions/Stroika/projects) is a view over those
same issues carrying fields GitHub Issues has no room for - notably a sortable Priority. Issues are
filed and discussed on the Issues tab; the project is for ordering them.

### JIRA is retired

Stroika used JIRA (`stroika.atlassian.net`, project `STK`) until 2026-08. It is **no longer used** - do
not file there, and treat anything it still shows as frozen as of 2026-08-16. Its 1025 issues were
exported into `Archive/` here and imported into GitHub Issues; `STK-to-GitHub.tsv` maps the old keys to
the new issue numbers.

In-source references point straight at GitHub - `@todo https://github.com/SophistSolutions/Stroika/issues/1128`.
There is no indirection: 339 such links across 140 files name the tracker directly, and no `STK-NNN`
reference in the tree lacks a resolvable URL.

They used to go through a Sophist Solutions redirect (`stroika-bugs.sophists.com/browse/STK-NNN`), on the
theory that the tracker could then move without editing source. In practice that never held - 196 links in
120 files had already been rewritten to GitHub URLs by hand - so on 2026-08-26 the remaining 144 stragglers
(34 still naming `stroika.atlassian.net`, 110 naming the redirect) were mapped through `STK-to-GitHub.tsv`
and rewritten too. The redirect is no longer load-bearing for anything in this tree.

> If the tracker moves again, `git grep github.com/SophistSolutions/Stroika/issues` finds every site.
> That is the price of links that resolve without a redirect someone has to keep alive - paid deliberately,
> since the redirect had already been bypassed in 120 files anyway.

## Archive/

A point-in-time export of the issue database: `STK-NNNN.json`, one file per issue, zero-padded so the
directory sorts numerically. Plus `attachments/`.

This exists because the tracker is hosted, and hosted things lose data - some of this database went
missing once and came back only after complaint, with no local copy at the time. That argument did NOT
retire with JIRA: GitHub can equally lose data, suspend an account, or change terms, so a copy that is
not on someone else's servers is still the point. ~14.6MB on disk, 0.84MB of git objects.

There used to be a rendered `STK-NNNN.md` beside each `.json`. They were dropped once the migration
finished: they existed to be readable/greppable history AND to be the GitHub issue bodies, and the second
purpose is spent now that the issues are in GitHub. `Scripts/JIRANormalize.pl --md` still generates them
if you want them - see the note under Scripts/ about needing them for `GitHubImport.pl`.

### Rules

- **Never hand-edit anything in `Archive/`.** It is generated. Fix the source, or fix the scripts and
  regenerate.
- The `.json` is canonical (sorted keys, stable indent) with empty fields dropped, so a refresh produces a
  readable one-line diff per changed field rather than a reshuffle.

### Known gaps

- **`STK-1017` through `STK-1020` are absent.** Everything else in `STK-1`..`STK-1029` is present.
  Four consecutive numbers is a bulk delete or the tail of the data-loss incident, not attrition.
- Attachments ARE included, in `Archive/attachments/STK-NNNN/`, and the `.md` links to them - but they
  come from a SEPARATE pass (`Scripts/JIRAAttachments.pl`), because the search API returns attachment
  metadata only. An export without that pass silently keeps the filenames and loses every byte. 16
  files, 7.3MB raw / 0.55MB compressed. Filenames on disk are sanitized
  (`[^A-Za-z0-9._-]` -> `_`) since the originals contain `#`, spaces and parens; the true name is in
  the JSON and is used as the link text.
- A JIRA admin Backup Manager zip (Site settings -> System -> Backup manager, no API token needed) is
  still the only fully self-contained archive, and worth taking once.

## Migration to GitHub Issues

The archive above is history; GitHub Issues is now the live tracker. JIRA had become unreachable in
practice - four issue updates across 2025-2026, and no outside reporter was going to create an Atlassian
account to file a bug.

> **THE IMPORT SCRIPTS ARE ONE-WAY, AND RE-RUNNING TWO OF THEM DESTROYS WORK.** They were written to load
> an empty tracker. Now that GitHub is where work actually happens:
> - `GitHubImport.pl --patch-bodies` overwrites an issue's body from the `.md`. Any edit made in GitHub is
>   lost. It requires `--i-know-this-overwrites` for that reason.
>
> Plain `GitHubImport.pl --go` is always safe: it skips any key already in `STK-to-GitHub.tsv`.

`Scripts/GitHubImport.pl` imports the archive into GitHub Issues: title `[STK-972] <summary>`, the
`.md` rendering as the body, a machine-readable `<!-- jira-import: {...} -->` block for everything
GitHub has no field for (notably `updated` - the last-modified date before import, since GitHub stamps
its own), JIRA components/type/priority mapped to labels, and the 427 resolved issues created then
closed. It is DRY RUN unless given `--go`, and resumable - `STK-to-GitHub.tsv` records every mapping as
it goes and already-mapped keys are skipped.

`STK-to-GitHub.tsv` is the `STK-NNN` -> GitHub issue number map. In-source links no longer need it - they
were rewritten to point at GitHub directly on 2026-08-26 - but it remains how you resolve an `STK-NNNN`
name found in `Archive/`, in `Release-Notes.md`, or in the captured output under
`Tests/HistoricalRegressionTestResults/`, none of which were rewritten.

## The GitHub Project, and what it can and cannot do for you

Issues live in project #1 "Stroika Issues". It exists because GitHub Issues has no custom fields and
**cannot sort by label** - so priority, and "how long has this sat untouched", have nowhere to live on the
issue itself. A Project holds them as real fields that sort and group, which is the only way to ask "show
me the worst open bugs".

A new issue is added to the project automatically (the project's own auto-add workflow) and gets
`Status = Todo`. **Priority is NOT set automatically** - Projects v2 has no default value for a custom
field, and the built-in workflows can only set Status. So a new issue sits with Priority empty until it is
set by hand, or until someone writes an Action to do it. Verified 2026-08-19 against #1160.

`GitHubProjectSync.pl` used to backfill those fields from the `<!-- jira-import: {...} -->` block during
the migration. It was deleted afterwards: it only ever worked on JIRA-imported issues, so it could never
help a new one, and it needed a credential deliberately revoked once the migration finished. Its
hard-won API notes, which is the part worth keeping:

- **Projects v2 is GraphQL-only** - there is no REST API for it. See `Scripts/gql.pl`.
- **Its mutations need a CLASSIC PAT**, not a fine-grained one. (Reads are fine with a fine-grained token -
  verified 2026-08-19 - so a failure here is about writing, not about reaching the API at all.)
- **The `project` scope alone is not enough for an ORG-owned project**; it also needs `read:org`. Without
  it every org-scoped call fails with "does not have the correct permissions", which reads exactly like an
  org policy problem and is not one. User-owned projects work with `project` alone, which is what makes
  the misdiagnosis easy.
- **Linking a project to a repository additionally needs `public_repo`**, because that mutation writes to
  the REPOSITORY, not to the project.

So if you ever need to bulk-set fields again - backfilling Priority across many issues, say - expect to
mint a fresh classic PAT with all three scopes, and revoke it when done.

## Scripts/

```bash
Issues/Scripts/JIRAExport.sh      [PROJECT] [RAWDIR]   # fetch issues -> raw paged JSON
perl Issues/Scripts/JIRAAttachments.pl [ARCHIVE] [OUTDIR]   # fetch attachment bytes (no token needed)
perl Issues/Scripts/JIRANormalize.pl   <RAWDIR> Issues/Archive
perl Issues/Scripts/GitHubImport.pl    --labels | --create-labels --go | --go [--limit N|--only KEY]
echo '<graphql>' | perl Issues/Scripts/gql.pl                # Projects v2 has no REST API
```

`JIRAExport.sh` needs an Atlassian API token - self-service at
<https://id.atlassian.com/manage-profile/security/api-tokens>, no support request involved. It reads
`email:token` from `~/.stroika-jira-creds` (override with `JIRA_CREDS_FILE`), hands it to curl via a
`--config` file so it never appears in the process list, and never echoes it. Keep that file outside
the repo, mode 600.

The raw paged JSON is scratch - it is not checked in. Only the normalized per-issue `.json` is.

**`GitHubImport.pl` needs the `.md` renderings, which are no longer checked in.** It reads
`STK-NNNN.md` beside `STK-NNNN.json` to build an issue body, so regenerate them into a scratch
directory first and point it there:

```bash
perl Issues/Scripts/JIRANormalize.pl .claude/jira-archive /tmp/rendered --md
```

Without that, every body would be the metadata comment and nothing else. This only matters if you re-run
the import or `--patch-bodies`; the migration itself is done.

`JIRAExport.sh` auto-detects whether the site serves the newer `/rest/api/3/search/jql`
(nextPageToken paging) or the older `/rest/api/3/search` (startAt paging); Atlassian has been
retiring the latter.
