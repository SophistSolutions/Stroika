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

In-source references still look like `@todo http://stroika-bugs.sophists.com/browse/STK-972`. That host
is a Sophist Solutions redirect, which is why it - rather than any tracker's own hostname - is what
appears in ~180 code comments: the tracker can move without editing 122 source files. It should now
resolve `STK-NNN` to the corresponding GitHub issue, per `STK-to-GitHub.tsv`.

> If the tracker moves again, **this file and the redirect are the two things to update** - nothing else
> in the tree names the tracker directly.

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
> - `GitHubProjectSync.pl` re-sets project fields from the JIRA metadata, so a Priority you changed by
>   hand reverts. It skips items whose fields are already set unless given `--overwrite`.
>
> Plain `GitHubImport.pl --go` is always safe: it skips any key already in `STK-to-GitHub.tsv`.

`Scripts/GitHubImport.pl` imports the archive into GitHub Issues: title `[STK-972] <summary>`, the
`.md` rendering as the body, a machine-readable `<!-- jira-import: {...} -->` block for everything
GitHub has no field for (notably `updated` - the last-modified date before import, since GitHub stamps
its own), JIRA components/type/priority mapped to labels, and the 427 resolved issues created then
closed. It is DRY RUN unless given `--go`, and resumable - `STK-to-GitHub.tsv` records every mapping as
it goes and already-mapped keys are skipped.

`STK-to-GitHub.tsv` is the `STK-NNN` -> GitHub issue number map. It is what lets the
`stroika-bugs.sophists.com` redirect keep resolving the ~180 in-source `STK-NNN` links after JIRA is
retired, WITHOUT rewriting those links in 122 source files.

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
