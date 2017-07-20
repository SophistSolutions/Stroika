// HACK - lifted from https://github.com/harto/realpath-osx
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

int main (int argc, char** argv)
{
    using namespace std;
    size_t pathIdx             = 1;
    bool   canonicalizeMissing = false;
    if (argc == 3 && strcmp (argv[1], "--canonicalize-missing") == 0) {
        pathIdx += 1;
        canonicalizeMissing = true;
    }
    else if (argc != 2) {
        fprintf (stderr, "Usage: realpath <path>\n");
        return 1;
    }

    char* path = argv[pathIdx];
    char  result[PATH_MAX];

    if (canonicalizeMissing) {
        // super-primitive impl - wont work with funny (quote) chars in pathnames
        string cmd;
        cmd += "echo `dirname " + string (path) + "`'/'`basename " + string (path) + "`";
        FILE* in = nullptr;
        if ((in = popen (cmd.c_str (), "r")) != NULL) {
            fgets (result, sizeof (result) / sizeof (result[0]), in);
            printf ("%s\n", result);
            pclose (in);
            return 0;
        }
    }
    else if (realpath (path, result)) {
        printf ("%s\n", result);
        return 0;
    }
    else {
        fprintf (stderr, "Bad path: %s\n", result);
        return 2;
    }
}
