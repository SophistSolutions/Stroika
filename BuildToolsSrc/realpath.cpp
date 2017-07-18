// HACK - lifted from https://github.com/harto/realpath-osx
#include <climits>
#include <cstring>
#include <cstdio>
#include <cstdlib>

int main (int argc, char** argv)
{
    size_t pathIdx = 1;
    if (argc == 3 && strcmp (argv[1], "--canonicalize-missing") == 0) {
        pathIdx += 1;
    }
    else if (argc != 2) {
        fprintf (stderr, "Usage: realpath <path>\n");
        return 1;
    }

    char* path = argv[pathIdx];
    char  result[PATH_MAX];

    if (realpath (path, result)) {
        printf ("%s\n", result);
        return 0;
    }
    else {
        fprintf (stderr, "Bad path: %s\n", result);
        return 2;
    }
}
