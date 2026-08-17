#!/bin/sh
#
# Export every JIRA issue in a project to per-issue JSON, for archival in git.
#
#   .claude/jira-export.sh [PROJECTKEY] [OUTDIR]
#       defaults: STK   .claude/jira-archive
#
# CREDENTIALS - never passed on the command line (that would expose them in the process list) and
# never echoed. Create a file readable only by you, OUTSIDE the repo:
#
#   $HOME/.stroika-jira-creds        with one line:   you@example.com:THE_API_TOKEN
#
#   chmod 600 ~/.stroika-jira-creds
#
# Get the token at https://id.atlassian.com/manage-profile/security/api-tokens (self-service, no
# support ticket needed). The token is an ordinary password for HTTP Basic auth alongside your email.
#
SITE="${JIRA_SITE:-https://stroika.atlassian.net}"
PROJECT="${1:-STK}"
OUTDIR="${2:-.claude/jira-archive}"
CREDS="${JIRA_CREDS_FILE:-$HOME/.stroika-jira-creds}"

if [ ! -f "$CREDS" ]; then
    echo "No credentials file at $CREDS" >&2
    echo "Create it with a single line 'email:apitoken' and chmod 600 it. See the header of this script." >&2
    exit 1
fi

# Hand the credential to curl via a config file rather than -u, so it never appears in `ps`.
CURLCFG=$(mktemp)
trap 'rm -f "$CURLCFG"' EXIT INT TERM
chmod 600 "$CURLCFG"
printf 'user = "%s"\n' "$(head -1 "$CREDS")" >"$CURLCFG"

mkdir -p "$OUTDIR"

fetch () { curl -s --max-time 60 --config "$CURLCFG" -H "Accept: application/json" "$@"; }

# Atlassian replaced /rest/api/3/search (startAt paging) with /rest/api/3/search/jql
# (nextPageToken paging) and has been retiring the former. Try the new one, fall back to the old.
probe=$(fetch "$SITE/rest/api/3/search/jql?jql=project%3D$PROJECT&maxResults=1")
case "$probe" in
    *'"issues"'*) API=new ;;
    *)
        probe=$(fetch "$SITE/rest/api/3/search?jql=project%3D$PROJECT&maxResults=1")
        case "$probe" in
            *'"issues"'*) API=old ;;
            *)
                echo "Could not read issues from $SITE. Response (credentials are never printed):" >&2
                echo "$probe" | head -c 400 >&2
                echo >&2
                exit 1
                ;;
        esac
        ;;
esac
echo "using $API search API at $SITE, project $PROJECT"

total=0
if [ "$API" = new ]; then
    token=""
    while : ; do
        url="$SITE/rest/api/3/search/jql?jql=project%3D$PROJECT&maxResults=100&fields=*all"
        [ -n "$token" ] && url="$url&nextPageToken=$token"
        page=$(fetch "$url")
        n=$(echo "$page" | grep -o '"key":"'"$PROJECT"'-[0-9]*"' | wc -l)
        [ "$n" -eq 0 ] && break
        echo "$page" >"$OUTDIR/_page_$(printf %04d $total).json"
        total=$((total + n))
        echo "  fetched $total issues..."
        token=$(echo "$page" | grep -o '"nextPageToken":"[^"]*"' | head -1 | sed 's/.*:"//;s/"$//')
        [ -z "$token" ] && break
    done
else
    startAt=0
    while : ; do
        page=$(fetch "$SITE/rest/api/3/search?jql=project%3D$PROJECT&maxResults=100&startAt=$startAt&fields=*all")
        n=$(echo "$page" | grep -o '"key":"'"$PROJECT"'-[0-9]*"' | wc -l)
        [ "$n" -eq 0 ] && break
        echo "$page" >"$OUTDIR/_page_$(printf %04d $startAt).json"
        total=$((total + n))
        echo "  fetched $total issues..."
        startAt=$((startAt + 100))
    done
fi

echo "done - $total issues into $OUTDIR"
echo
echo "NOTE: this captures issue FIELDS ONLY - not attachments. For a complete archive including"
echo "attachments, also take one Backup Manager zip from the JIRA admin UI (no token required)."
