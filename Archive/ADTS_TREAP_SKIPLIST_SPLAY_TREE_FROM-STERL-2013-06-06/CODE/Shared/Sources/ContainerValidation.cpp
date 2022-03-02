#include "../Headers/ContainerValidation.h"

#if qDebug
TestTitle::TestTitle (const char* title, int tabs, bool verbose) :
    fTabs (tabs),
    fVerbose (verbose)
{
    if (fVerbose) {
        cout << tab (fTabs) << title << endl;
    }
}

TestTitle::~TestTitle ()
{
      if (fVerbose) {
        cout << tab (fTabs) << "passed" << endl;
    }
}
#endif

