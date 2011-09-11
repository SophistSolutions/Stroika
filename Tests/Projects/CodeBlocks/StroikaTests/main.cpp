#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/Configuration/Common.h"

#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Debugger.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;




int main()
{
    #if 1
        extern  int CryptographyTests ();
        cout << "Testing Cryptograpy" << endl;
        CryptographyTests ();
    #endif
   #if 1
        extern  int TestJSON ();
        cout << "Testing JSON" << endl;
        TestJSON ();
    #endif
    #if 0
        extern  int TestThreads ();
        cout << "Testing Threads" << endl;
        TestThreads ();
    #endif
    #if 1
        extern  int TestExceptions ();
        cout << "Testing Exceptions" << endl;
        TestExceptions ();
    #endif
    #if 1
        extern  int TestStrings ();
        cout << "Testing Strings" << endl;
        TestStrings ();
    #endif
    #if 0
        extern  int TextXML_SaxParser ();
        cout << "Testing XML - SAX Parser" << endl;
        TextXML_SaxParser ();
    #endif
    #if 1
        extern  int TestDateAndTime ();
        cout << "Testing Date and Time" << endl;
        TestDateAndTime ();
    #endif
    return 0;
}
