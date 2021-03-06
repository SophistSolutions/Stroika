/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cctype>
#include <locale>

#include "../Characters/String.h"
#include "../Containers/Collection.h"
#include "../Debug/Assertions.h"
#include "../Execution/Synchronized.h"

#include "MessageUtilities.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
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

String MessageUtilities_en::PluralizeNoun (const String& s, const optional<String>& sPlural, int count) const
{
    // Implement VERY WEAK ENGLISH rules for now... (ignores y ->ies, and other cases too)
    if (count == 1) {
        return s;
    }
    else if (sPlural) {
        return *sPlural;
    }
    else {
        String tmp = s;
        tmp.push_back ('s');
        return tmp;
    }
}

String MessageUtilities_en::MakeNounSingular (const String& s) const
{
    String r = s;
    // take an ENGLISH string and munge it so its singular (if it happened to have been plural)

    // hande special case of 'wives' -> 'wife' as in 'midwives'
    // @todo there are more like thief, and theives
    if (r.length () >= 5) {
        size_t l = r.length ();
        if (s[l - 5] == 'w' and s[l - 4] == 'i' and s[l - 3] == 'v' and s[l - 2] == 'e' and s[l - 1] == 's') {
            r = r.substr (0, l - 3);
            r.push_back ('f');
            r.push_back ('e');
            return r;
        }
    }

    // trim trailing s from the name (if prev letter is a non-s consonant)
    // or change 'ies' to 'y' at end
    if (s.length () > 3) {
        if (s[s.length () - 3] == 'i' and s[s.length () - 2] == 'e' and s[s.length () - 1] == 's') {
            //r = s.substr (0, s.length () - 3) + "y";
            r = s.substr (0, s.length () - 3);
            r.push_back ('y');
        }
        else if (s.length () > 4 and s[s.length () - 4] == 's' and s[s.length () - 3] == 's' and s[s.length () - 2] == 'e' and s[s.length () - 1] == 's') {
            r = s.substr (0, s.length () - 2);
        }
        else if (s[s.length () - 3] == 's' and s[s.length () - 2] == 'e' and s[s.length () - 1] == 's') {
            r = s.substr (0, s.length () - 1);
        }
        // because of diabets mellitus - (and others???? - don't map trailing 'us' to 'u'
        else if (s[s.length () - 1] == 's' and s[s.length () - 2].IsASCII () and s[s.length () - 2].IsAlphabetic () and (s[s.length () - 2] != 's' and s[s.length () - 2] != 'u')) {
            r = s.substr (0, s.length () - 1);
        }
    }
    return r;
}

/*
 ********************************************************************************
 ************************ MessageUtiltiesManager ********************************
 ********************************************************************************
 */
shared_ptr<const MessageUtilities> MessageUtiltiesManager::LookupHandler (const locale& l) const
{
    optional<Common::KeyValuePair<locale, shared_ptr<const MessageUtilities>>> cachedVal = fCached_.load ();
    if (cachedVal && cachedVal->fKey == l) {
        return cachedVal->fValue;
    }
    cachedVal = [&] () {
        //  search here user installed ones with AddHandler ()
        for (shared_ptr<const MessageUtilities> h : fMessageHandlers_) {
            if (h->AppliesToThisLocale (String::FromNarrowSDKString (l.name ()))) {
                return Common::KeyValuePair<locale, shared_ptr<const MessageUtilities>>{l, h};
            }
        }
        // if nothing applies, return this...
        return Common::KeyValuePair<locale, shared_ptr<const MessageUtilities>>{l, make_shared<MessageUtilities_en> ()};
    }();
    fCached_.store (cachedVal);
    return cachedVal->fValue;
}
#if 0

/*
 ********************************************************************************
 ************ CurrentLocaleMessageUtilities::Configure **************************
 ********************************************************************************
 */
namespace {
    struct Info_ {
        locale                       fLocale;
        shared_ptr<MessageUtilities> fHandler;
    };
    RWSynchronized<optional<Info_>>                        sCached_;
    Synchronized<Collection<shared_ptr<MessageUtilities>>> sHandlers_;
#if qCompilerAndStdLib_atomic_bool_initialize_before_main_Buggy
    // just hope for this best - this isn't super critical here, so maybe can get away with it. could also use explict atomic_loads on bool value if mattered
    bool sUseFakeHandler_{true}; // for calls before start of or after end of main ()
#else
    atomic<bool> sUseFakeHandler_{true}; // for calls before start of or after end of main ()
#endif
}

CurrentLocaleMessageUtilities::Configuration CurrentLocaleMessageUtilities::Configuration::sThe;

CurrentLocaleMessageUtilities::Configuration::Configuration ()
{
    Require (sUseFakeHandler_);
    sUseFakeHandler_ = false;
}

CurrentLocaleMessageUtilities::Configuration::~Configuration ()
{
    Require (not sUseFakeHandler_);
    sUseFakeHandler_ = true;
}

void CurrentLocaleMessageUtilities::Configuration::AddHandler (const shared_ptr<MessageUtilities>& handler)
{
    sHandlers_.rwget ()->Add (handler);
    sCached_.rwget ().store (nullopt);
}
/*
********************************************************************************
******************* CurrentLocaleMessageUtilities ******************************
********************************************************************************
*/
shared_ptr<MessageUtilities> CurrentLocaleMessageUtilities::LookupHandler ()
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
            co = [&] () {
                //  search here user installed ones with AddHandler ()
                for (shared_ptr<MessageUtilities> h : sHandlers_.load ()) {
                    if (h->AppliesToThisLocale (String::FromNarrowSDKString (currentLocale.name ()))) {
                        return Info_{currentLocale, h};
                    }
                }
                return Info_{currentLocale, make_shared<MessageUtilities_en> ()};
            }();
            sCached_.rwget ().store (*co);
        }
        return co->fHandler;
    }
}

pair<String, optional<String>> CurrentLocaleMessageUtilities::RemoveTrailingSentencePunctuation (const String& msg)
{
    return LookupHandler ()->RemoveTrailingSentencePunctuation (msg);
}

String CurrentLocaleMessageUtilities::PluralizeNoun (const String& s, int count)
{
    return LookupHandler ()->PluralizeNoun (s, nullopt, count);
}

String CurrentLocaleMessageUtilities::PluralizeNoun (const String& s, const String& sPlural, int count)
{
    return LookupHandler ()->PluralizeNoun (s, sPlural, count);
}

String CurrentLocaleMessageUtilities::MakeNounSingular (const String& s)
{
    return LookupHandler ()->MakeNounSingular (s);
}
#endif