/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cctype>
#include <locale>

#include "../Characters/String.h"
#include "../Debug/Assertions.h"
#include "../Execution/Synchronized.h"

#include "MessageUtilities.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Linguistics;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 *************************** MessageUtilities_en ********************************
 ********************************************************************************
 */
bool MessageUtilities_en::AppliesToThisLocale (const String& localeName) const
{
    return String::FromNarrowSDKString (locale ().name ()).StartsWith (localeName);
    //return l.name ().find ("en") == 0;
}

pair<String, optional<String>> MessageUtilities_en::RemoveTrailingSentencePunctuation (const String& msg) const
{
    // super primitive (may want to trim trailing whitespace if any on msg after remove of punctuation but shouldnt be any)
    if (msg.EndsWith (L"."sv)) {
        return pair<String, optional<String>>{msg.SubString (0, -1), L"."sv};
    }
    if (msg.EndsWith (L"?"sv)) {
        return pair<String, optional<String>>{msg.SubString (0, -1), L"?"sv};
    }
    if (msg.EndsWith (L"!"sv)) {
        return pair<String, optional<String>>{msg.SubString (0, -1), L"!"sv};
    }
    return pair<String, optional<String>>{msg, nullopt};
}

/*
 ********************************************************************************
 ************ CurrentLocaleMessageUtilities::Configure **************************
 ********************************************************************************
 */
CurrentLocaleMessageUtilities::Configuration CurrentLocaleMessageUtilities::Configuration::sThe;

namespace {
    struct Info_ {
        locale                       fLocale;
        shared_ptr<MessageUtilities> fHandler;
    };
    static RWSynchronized<optional<Info_>> sCached_;
    Synchronized<bool>                     sUseFakeHandler_ = true; // for calls before start of or after end of main ()
}

CurrentLocaleMessageUtilities::Configuration::Configuration ()
{
    Require (sUseFakeHandler_.load ());
    sUseFakeHandler_ = false;
}

CurrentLocaleMessageUtilities::Configuration::~Configuration ()
{
    Require (not sUseFakeHandler_.load ());
    sUseFakeHandler_ = true;
}

void CurrentLocaleMessageUtilities::Configuration::AddHandler (const shared_ptr<MessageUtilities>& handler)
{
    // NYI
    // if we add handler, invalidate sCached...
    sCached_.rwget ().store (nullopt);
    AssertNotImplemented ();
}

/*
********************************************************************************
******************* CurrentLocaleMessageUtilities ******************************
********************************************************************************
*/
namespace {
    shared_ptr<MessageUtilities> LookupHandler_ ()
    {
        // If not after start and before end of main () - use fake / MessageUtilities_en handler.
        if (sUseFakeHandler_) {
            return make_shared<MessageUtilities_en> ();
        }
        else {
            // If else use cached per-locale handler.
            optional<Info_> co = sCached_.cget ().load ();
            locale          currentLocale{};
            if (not co.has_value () or co->fLocale != currentLocale) {
                // create the appropriate handler for the current locale
                // @todo - for now we only have one!!! - but can search here user installed ones with AddHandler ()
                co = Info_{currentLocale, make_shared<MessageUtilities_en> ()};
                sCached_.rwget ().store (*co);
            }
            return co->fHandler;
        }
    }

}

pair<String, optional<String>> CurrentLocaleMessageUtilities::RemoveTrailingSentencePunctuation (const String& msg)
{
    return LookupHandler_ ()->RemoveTrailingSentencePunctuation (msg);
}
