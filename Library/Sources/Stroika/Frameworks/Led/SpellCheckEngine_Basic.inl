/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_SpellCheckEngine_Basic_inl_
#define _Stroika_Framework_Led_SpellCheckEngine_Basic_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    // class SpellCheckEngine_Basic
    inline bool SpellCheckEngine_Basic::OtherStringToIgnore_Sentinals (const Led_tString& checkWord)
    {
        return checkWord.length () == 1 and checkWord[0] == '\0';
    }
    /*
    @METHOD:        SpellCheckEngine_Basic::GetTextBreaker
    @DESCRIPTION:   <p>Returns a @'Led_RefCntPtr<T>' wrapper on the @'TextBreaks' subclass associated
                with this SpellCheckEngine.
                </p>
                    <p>If none is associated with the TextStore right now - and default one is built and returned.</p>
                    <p>See also See @'SpellCheckEngine_Basic::SetTextBreaker'.</p>
    */
    inline shared_ptr<TextBreaks> SpellCheckEngine_Basic::GetTextBreaker () const
    {
        if (fTextBreaker == nullptr) {
            fTextBreaker = make_shared<TextBreaks_SpellChecker> ();
        }
        return fTextBreaker;
    }
    /*
    @METHOD:        SpellCheckEngine_Basic::SetTextBreaker
    @DESCRIPTION:   <p>See @'SpellCheckEngine_Basic::GetTextBreaker'.</p>
    */
    inline void SpellCheckEngine_Basic::SetTextBreaker (const shared_ptr<TextBreaks>& textBreaker)
    {
        fTextBreaker = textBreaker;
    }

    // class SpellCheckEngine_Basic
    inline SpellCheckEngine_Basic::Dictionary::Dictionary ()
    {
    }
    inline SpellCheckEngine_Basic::Dictionary::~Dictionary ()
    {
    }

}

#endif /*_Stroika_Framework_Led_SpellCheckEngine_Basic_inl_*/
