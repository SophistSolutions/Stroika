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
        auto backTick = [](const string& cmd) -> string {
            char result[PATH_MAX] = "\0";
            // super-primitive impl - wont work with funny (quote) chars in pathnames
            FILE* in = nullptr;
            if ((in = popen (cmd.c_str (), "r")) != NULL) {
                fgets (result, sizeof (result) / sizeof (result[0]), in);
                pclose (in);
                size_t n = strlen (result);
                if (n > 0 and result[n - 1] == '\n') {
                    result[n - 1] = '\0';
                }
                return result;
            }
            fprintf (stderr, "backTick failed\n");
            ::exit (1);
            return {};
        };
        // super-primitive impl - wont work with funny (quote) chars in pathnames
        string dirName  = backTick ("dirname " + string (path));
        string basename = backTick ("basename " + string (path));
        if (dirName.length () != 0 && dirName[0] != '/') {
            dirName = backTick ("pwd") + "/" + dirName;
        }
        dirName += "/";
        string result = (dirName + basename);
        // replace A/B/../ with A
        {
            for (size_t i = 0; (i = result.find ("/../")) != string::npos;) {
                if (i == 0) {
                    fprintf (stderr, ".. in path not found\n");
                    ::exit (1);
                }
                size_t startOfDirToEliminate = result.rfind ('/', i - 1);
                if (startOfDirToEliminate == string::npos) {
                    fprintf (stderr, ".. in path not found\n");
                    ::exit (1);
                }
                result = result.substr (0, startOfDirToEliminate) + result.substr (i + 3);
            }
        }
        printf ("%s\n", result.c_str ());
        return 0;
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
