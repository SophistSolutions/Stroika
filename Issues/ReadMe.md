# Issues

## Reporting a bug, or asking a question

Stroika's live issue tracker is **JIRA**, at <https://stroika.atlassian.net/browse/STK>. It is
world-readable without an account, so you can search existing issues before filing.

In-source references to issues look like `@todo http://stroika-bugs.sophists.com/browse/STK-972`.
That host is a Sophist Solutions redirect to the tracker, which is why it - rather than the
`atlassian.net` hostname - is what appears in code comments. The indirection means the tracker can
move without touching the ~180 links in the source tree.

GitHub Issues on this repository is also enabled, and is the lower-friction path if you do not have
(and do not want) an Atlassian account.

> If the tracker ever moves, **this file and the redirect are the two things to update** - nothing
> else in the tree names the tracker directly.

## Archive/

A point-in-time export of the issue database, **two files per issue**, because no single format does
both jobs:

| | purpose | lossy? |
|---|---|---|
| `STK-NNNN.json` | restore / migrate. Structure: ids, links, status, resolution, timestamps, custom fields, comment identity. | no, for practical purposes - see below |
| `STK-NNNN.md`   | read / grep / review a diff. Rendered plain text. | yes, deliberately |

Zero-padded so the directory sorts numerically.

Why both: this archive exists because the tracker is hosted, and hosted things lose data - some of
this database went missing once and came back only after complaint, with no local copy at the time.
Meeting that requirement needs the JSON, since markdown would let you *read* the history but not
*rebuild* it. But the reason it lives in this repo rather than a repo of its own is co-location - so
`grep STK-972` finds the `@todo` and the ticket together, offline - and that needs the markdown,
because JIRA stores text in Atlassian Document Format, fragmented across nested nodes: grepping the
JSON for a sentence fails as soon as a bold word or line break splits it.

It is cheap enough that having both is not worth arguing about: 14.6MB + 1.4MB on disk compresses to
**0.84MB + 0.34MB** of git objects. And since it is one file per issue rather than one per API page,
a refresh only rewrites issues that actually changed - at this project's rate (~20 issue updates
across 2025-2026) that is kilobytes a year.

### Rules

- **Never hand-edit anything in `Archive/`.** Both forms are generated. Fix the tracker, or fix the
  scripts and regenerate.
- The `.md` files are derived from the `.json` data, so improving the renderer churns all of them in
  one diff. That is expected; keep such a commit separate from content refreshes.

### Known gaps

- **`STK-1017` through `STK-1020` are absent.** Everything else in `STK-1`..`STK-1029` is present.
  Four consecutive numbers is a bulk delete or the tail of the data-loss incident, not attrition.
- **Attachments are not included** - the search API returns fields only; the `.md` notes an
  attachment's filename where one exists, but not its content. For a complete archive, also take a
  zip from the JIRA admin UI (Site settings -> System -> Backup manager), which needs no API token.

## Scripts/

```bash
Issues/Scripts/JIRAExport.sh    [PROJECT] [RAWDIR]   # fetch -> raw paged JSON
perl Issues/Scripts/JIRANormalize.pl  <RAWDIR> Issues/Archive
```

`JIRAExport.sh` needs an Atlassian API token - self-service at
<https://id.atlassian.com/manage-profile/security/api-tokens>, no support request involved. It reads
`email:token` from `~/.stroika-jira-creds` (override with `JIRA_CREDS_FILE`), hands it to curl via a
`--config` file so it never appears in the process list, and never echoes it. Keep that file outside
the repo, mode 600.

The raw paged JSON is scratch - it is not checked in. Only the normalized per-issue pair is.

`JIRAExport.sh` auto-detects whether the site serves the newer `/rest/api/3/search/jql`
(nextPageToken paging) or the older `/rest/api/3/search` (startAt paging); Atlassian has been
retiring the latter.
