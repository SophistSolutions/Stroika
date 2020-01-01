/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_SpellCheckEngine_inl_
#define _Stroika_Framework_Led_SpellCheckEngine_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ***************************** SpellCheckEngine ***************************
     ********************************************************************************
     */
    /*
    @METHOD:        SpellCheckEngine::LookupWord
    @DESCRIPTION:   <p>Lookup the given word and see if its found in the system (dictionaries). Return true
                if found and set (optional - can be nullptr) OUT argument 'matchedWordResult'
                to indicate the original word in the dictionary matched
                (due to case mapping, this may not be the same as the 'checkWord').</p>
                    <p>Also note that just because a word doesn't appear in the dictionary, (i.e. isn't returned
                from LookupWord) - doesn't mean it will be found as a missing (misspelled) word. Some words
                are ignored like possibly all upper case words, punctuation, numbers, and others algorithmicly checked
                words.</p>
    */
    inline bool SpellCheckEngine::LookupWord (const Led_tString& checkWord, Led_tString* matchedWordResult)
    {
        return LookupWord_ (checkWord, matchedWordResult);
    }
    inline void SpellCheckEngine::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }

#if qDebug
    /*
@METHOD:        SpellCheckEngine::Invariant_
@DESCRIPTION:   <p>Default implementaiton for calls to @'SpellCheckEngine::Invariant'. Overridden in subclasses
        to check validity of SpellCheck engine.</p>
*/
    inline void SpellCheckEngine::Invariant_ () const
    {
    }
#endif

}

#endif /*_Stroika_Framework_Led_SpellCheckEngine_inl_*/
