/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cctype>
#include <locale>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Synchronized.h"

#include "MessageUtilities.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Linguistics;
using namespace Stroika::Foundation::Linguistics::MessageUtilities;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 *********************************** Impl_en ************************************
 ********************************************************************************
 */
bool Impl_en::AppliesToThisLocale (const String& localeName) const
{
    return String::FromNarrowSDKString (locale{}.name ()).StartsWith (localeName);
    //return l.name ().find ("en") == 0;
}

pair<String, optional<String>> Impl_en::RemoveTrailingSentencePunctuation (const String& msg) const
{
    // super primitive (may want to trim trailing whitespace if any on msg after remove of punctuation but shouldnt be any)
    if (msg.EndsWith ("."sv)) {
        return {msg.SubString (0, -1), "."sv};
    }
    if (msg.EndsWith ("?"sv)) {
        return {msg.SubString (0, -1), "?"sv};
    }
    if (msg.EndsWith ("!"sv)) {
        return {msg.SubString (0, -1), "!"sv};
    }
    return {msg, nullopt};
}

String Impl_en::PluralizeNoun (const String& s, const optional<String>& sPlural, int count) const
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

String Impl_en::MakeNounSingular (const String& s) const
{
    String r = s;
    // take an ENGLISH string and munge it so its singular (if it happened to have been plural)

    // handle special case of 'wives' -> 'wife' as in 'midwives'
    // @todo there are more like thief, and thieves
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
        // because of diabetes mellitus - (and others???? - don't map trailing 'us' to 'u'
        else if (s[s.length () - 1] == 's' and s[s.length () - 2].IsASCII () and s[s.length () - 2].IsAlphabetic () and
                 (s[s.length () - 2] != 's' and s[s.length () - 2] != 'u')) {
            r = s.substr (0, s.length () - 1);
        }
    }
    return r;
}

/*
 ********************************************************************************
 ************************************ Manager ***********************************
 ********************************************************************************
 */
Manager::Manager (const Containers::Sequence<shared_ptr<const IRep>>& utilObjs)
    : fMessageHandlers_{utilObjs}
{
}

shared_ptr<const IRep> Manager::LookupHandler (const locale& l) const
{
    optional<Common::KeyValuePair<locale, shared_ptr<const IRep>>> cachedVal = fLocaleCache_.load ();
    if (cachedVal && cachedVal->fKey == l) {
        return cachedVal->fValue;
    }
    cachedVal = [&] () -> KeyValuePair<locale, shared_ptr<const IRep>> {
        //  search here user installed ones with AddHandler ()
        for (const shared_ptr<const IRep>& h : fMessageHandlers_.load ()) {
            if (h->AppliesToThisLocale (String::FromNarrowSDKString (l.name ()))) {
                return {l, h};
            }
        }
        // if nothing applies, return this...
        return {l, make_shared<Impl_en> ()};
    }();
    fLocaleCache_.store (cachedVal);
    return cachedVal->fValue;
}
