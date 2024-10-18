/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Linguistics::MessageUtilities {

    /*
     ********************************************************************************
     *************** Linguistics::MessageUtilities::Manager *************************
     ********************************************************************************
     */
    inline pair<String, optional<String>> Manager::RemoveTrailingSentencePunctuation (const String& msg) const
    {
        return LookupHandler ()->RemoveTrailingSentencePunctuation (msg);
    }
    inline pair<String, optional<String>> Manager::RemoveTrailingSentencePunctuation (const locale& l, const String& msg) const
    {
        return LookupHandler (l)->RemoveTrailingSentencePunctuation (msg);
    }
    inline String Manager::PluralizeNoun (const String& s, int count) const
    {
        return LookupHandler ()->PluralizeNoun (s, nullopt, count);
    }
    inline String Manager::PluralizeNoun (const String& s, const String& sPlural, int count) const
    {
        return LookupHandler ()->PluralizeNoun (s, sPlural, count);
    }
    inline String Manager::PluralizeNoun (const locale& l, const String& s, int count) const
    {
        return LookupHandler (l)->PluralizeNoun (s, nullopt, count);
    }
    inline String Manager::PluralizeNoun (const locale& l, const String& s, const String& sPlural, int count) const
    {
        return LookupHandler (l)->PluralizeNoun (s, sPlural, count);
    }
    inline String Manager::MakeNounSingular (const locale& l, const String& s) const
    {
        return LookupHandler (l)->MakeNounSingular (s);
    }
    inline String Manager::MakeNounSingular (const String& s) const
    {
        return LookupHandler ()->MakeNounSingular (s);
    }

}
