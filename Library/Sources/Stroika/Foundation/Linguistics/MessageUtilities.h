/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Linguistics_MessageUtilities_h_
#define _Stroika_Foundation_Linguistics_MessageUtilities_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Common/KeyValuePair.h"
#include "../Configuration/Common.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Very VERY primitive linguistic support, but this could easily evolve over time. Led has a bunch of
 *              lingusitic code I could move here, and I'm sure I could dig up more...
 */

namespace Stroika::Foundation::Linguistics {

    using Characters::String;

    struct MessageUtilities {
        virtual ~MessageUtilities ()                                                                                             = default;
        virtual bool                           AppliesToThisLocale (const String& localeName) const                              = 0;
        virtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const                       = 0;
        virtual String                         PluralizeNoun (const String& s, const optional<String>& sPlural, int count) const = 0;
        virtual String                         MakeNounSingular (const String& s) const                                          = 0;
    };

    /**
     */
    struct MessageUtilities_en : MessageUtilities {
        virtual bool                           AppliesToThisLocale (const String& localeName) const override;
        virtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const override;
        virtual String                         PluralizeNoun (const String& s, const optional<String>& sPlural, int count) const override;
        virtual String                         MakeNounSingular (const String& s) const override;
    };

    /*
     *  Maps locales to MessageUtilities instances, that can be used to lingustically specifically update a message
     */
    class MessageUtiltiesManager {
    public:
        /**
         * Get the currently installed MessageUtiltiesManager
         */
        static shared_ptr<const MessageUtiltiesManager> Get ();

    public:
        /**
         */
        static void Set (const shared_ptr<const MessageUtiltiesManager>& newMsgMgr);

    public:
        MessageUtiltiesManager (const Containers::Sequence<shared_ptr<const MessageUtilities>>& utilObjs = {});

    public:
        /**
         *  Return a legal handler to use - even if before or after main. But prefer picking one based on the current
         *  locale (or argument locale) and what is installed with Configuration if its available.
         *
         *  This is generally unneeded, as you can call MakeNounSingular, RemoveTrailingSentencePunctuation directly (below).
         *  But if you have to make many calls in a row, caching the 'MessageUtilties' - locale specific object - can improve
         *  performance.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
         */
        nonvirtual shared_ptr<const MessageUtilities> LookupHandler (const locale& l = locale{}) const;

    public:
        /**
         *  Uses a MessageUtilities based on the current thread's locale, if called between the start and end of main, and
         *  otherwise uses an arbitrary one.
         */
        nonvirtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const String& msg) const;
        nonvirtual pair<String, optional<String>> RemoveTrailingSentencePunctuation (const locale& l, const String& msg) const;

    public:
        /**
         *  Implement current-ui-language-specific noun-pluralization logic for the given noun string (assuming the count of that noun
         *  is given (english rules - if count != 1 - append s, but we don't wnat that logic to proliferate through the app, so
         *  its easier to localize.
         *
         *  The variation with two strings - the second one is the explicit plural - just plugged in if the count is non-zero
         */
        nonvirtual String PluralizeNoun (const String& s, int count = 1000) const;
        nonvirtual String PluralizeNoun (const String& s, const String& sPlural, int count) const;
        nonvirtual String PluralizeNoun (const locale& l, const String& s, int count = 1000) const;
        nonvirtual String PluralizeNoun (const locale& l, const String& s, const String& sPlural, int count) const;

    public:
        /**
         * Take argument string (assumed noun) munge it so its singular (if it happened to have been plural).
         */
        nonvirtual String MakeNounSingular (const locale& l, const String& s) const;
        nonvirtual String MakeNounSingular (const String& s) const;

    private:
        Containers::Sequence<shared_ptr<const MessageUtilities>>                                                    fMessageHandlers_;
        static inline Execution::Synchronized<shared_ptr<const MessageUtiltiesManager>>                             sTheMessageUtiltiesManager_;
        mutable Execution::Synchronized<optional<Common::KeyValuePair<locale, shared_ptr<const MessageUtilities>>>> fCached_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MessageUtilities.inl"

#endif /*_Stroika_Foundation_Linguistics_MessageUtilities_h_*/
