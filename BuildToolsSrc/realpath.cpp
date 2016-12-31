// HACK - lifted from https://github.com/harto/realpath-osx
#include <cstdlib>
#include <climits>
#include <cstdio>

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: realpath <path>\n");
        return 1;
    }

    char* path = argv[1];
    char result[PATH_MAX];

    if (realpath(path, result)) {
        printf("%s\n", result);
        return 0;
    }
    else {
        fprintf(stderr, "Bad path: %s\n", result);
        return 2;
    }
}
