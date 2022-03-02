/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Linguistics_MessageUtilities_inl_
#define _Stroika_Foundation_Linguistics_MessageUtilities_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Linguistics {

    /*
     ********************************************************************************
     ****************** Linguistics::MessageUtiltiesManager *************************
     ********************************************************************************
     */
    inline shared_ptr<const MessageUtiltiesManager> MessageUtiltiesManager::Get ()
    {
        auto l = sTheMessageUtiltiesManager_.load ();
        if (l == nullptr) {
            l = make_shared<const MessageUtiltiesManager> ();
            sTheMessageUtiltiesManager_.store (l);
        }
        return l;
    }
    inline void MessageUtiltiesManager::Set (const shared_ptr<const MessageUtiltiesManager>& newMsgMgr)
    {
        sTheMessageUtiltiesManager_.store (newMsgMgr == nullptr ? make_shared<const MessageUtiltiesManager> () : newMsgMgr);
    }
    inline MessageUtiltiesManager::MessageUtiltiesManager (const Containers::Sequence<shared_ptr<const MessageUtilities>>& utilObjs)
        : fMessageHandlers_{utilObjs}
    {
    }
    inline pair<String, optional<String>> MessageUtiltiesManager::RemoveTrailingSentencePunctuation (const String& msg) const
    {
        return LookupHandler ()->RemoveTrailingSentencePunctuation (msg);
    }
    inline pair<String, optional<String>> MessageUtiltiesManager::RemoveTrailingSentencePunctuation (const locale& l, const String& msg) const
    {
        return LookupHandler (l)->RemoveTrailingSentencePunctuation (msg);
    }
    inline String MessageUtiltiesManager::PluralizeNoun (const String& s, int count) const
    {
        return LookupHandler ()->PluralizeNoun (s, nullopt, count);
    }
    inline String MessageUtiltiesManager::PluralizeNoun (const String& s, const String& sPlural, int count) const
    {
        return LookupHandler ()->PluralizeNoun (s, sPlural, count);
    }
    inline String MessageUtiltiesManager::PluralizeNoun (const locale& l, const String& s, int count) const
    {
        return LookupHandler (l)->PluralizeNoun (s, nullopt, count);
    }
    inline String MessageUtiltiesManager::PluralizeNoun (const locale& l, const String& s, const String& sPlural, int count) const
    {
        return LookupHandler (l)->PluralizeNoun (s, sPlural, count);
    }
    inline String MessageUtiltiesManager::MakeNounSingular (const locale& l, const String& s) const
    {
        return LookupHandler (l)->MakeNounSingular (s);
    }
    inline String MessageUtiltiesManager::MakeNounSingular (const String& s) const
    {
        return LookupHandler ()->MakeNounSingular (s);
    }

}

#endif /*_Stroika_Foundation_Linguistics_MessageUtilities_inl_*/
