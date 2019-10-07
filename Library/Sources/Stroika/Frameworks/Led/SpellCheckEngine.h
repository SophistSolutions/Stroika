/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SpellCheckEngine_h_
#define _Stroika_Frameworks_Led_SpellCheckEngine_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    SpellCheckEngine
@DESCRIPTION:   <p></p>

 */

#include "Support.h"
#include "TextBreaks.h"

namespace Stroika::Frameworks::Led {

    /*
    @CLASS:         SpellCheckEngine
    @DESCRIPTION:   <p>Abstract spellchecker low-level API. This just defines the basic functionality used for looking for misspelled
                    words, and for finding guesses.
                </p>
    */
    class SpellCheckEngine {
    protected:
        SpellCheckEngine () = default;

    public:
        virtual ~SpellCheckEngine () = default;

    public:
        /*
        @METHOD:        SpellCheckEngine::ScanForUndefinedWord
        @DESCRIPTION:   <p>Look in the given buffer input buffer, starting at 'cursor' for the next undefined
                    word Set wordStartResult/wordEndResult according to what is found. Note that if
                    '*cursor' is nullptr, then it is treated as being 'startBuf' instead. Return true if something is
                    found (setting only in this case wordStartResult/wordEndResult) and return false if no undefined
                    words are found. In either case - set 'cursor' on output to reflect how far we scanned ahead.
                    It is intended that this function be used iteratively and that you repeatedly pass IN
                    the same cursor that was passed out from the last call. On the first call - set the cursor value
                    to nullptr. Please don't confuse the cursor value with the POINTER TO the cursor value which is
                    what is passed in.
                    </p>
                        <p>
        <code>
            const int nChars    =   100;
            Led_tChar   textToSearch [nChars];
            // fill in textToSearch from someplace
            const Led_tChar*    cursor          =   nullptr;
            const Led_tChar*    wordStartResult =   nullptr;
            const Led_tChar*    wordEndResult   =   nullptr;
            while (ScanForUndefinedWord (textToSearch, textToSearch + nChars, &cursor,
                                            &wordStartResult, &wordEndResult
                                            )
                    ) {
                // we found a possible undefined word.
                Led_tString word    =   Led_tString (wordStartResult, wordEndResult);
            }
            // no more undefined words.
        </code>
                    </p>
                        <p>Note that in the case where you haven't examined an entire buffer, but just bits at a time
                    (chunking) - you may find apparant undefined words at the edges of the buffer. It is up to you to
                    overlap your calls to check for undefined words in such a way that you verify any found undefined
                    words truely are undefined, by examining a larger surrounding region.
                    </p>
        */
        virtual bool ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                                           const Led_tChar** wordStartResult, const Led_tChar** wordEndResult) = 0;

    public:
        nonvirtual bool LookupWord (const Led_tString& checkWord, Led_tString* matchedWordResult = nullptr);

    protected:
        /*
        @METHOD:        SpellCheckEngine::LookupWord_
        @ACCESS:        protected
        @DESCRIPTION:   <p>pure virtual method of @'SpellCheckEngine' - called internally by @'SpellCheckEngine::LookupWord'.
                    This method is overriden in subclasses to to the actual word lookup - returning true (and setting matchedWordResult) if the
                    given word is found (considered legitimate).</p>
                        <p>This function may do much more than just lookikng the word up in a dictionary. It may perform
                    all sorts of lingustic manipulations (stripping trailing 's' from nouns etc) to look for a match.</p>
        */
        virtual bool LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult) = 0;

    public:
        /*
        @METHOD:        SpellCheckEngine::GenerateSuggestions
        @DESCRIPTION:   <p>This generates a list of words which are close to the given misspelled word. (ORDER
                ALPHA OR MOST LIKELY FIRST????)</p>
        */
        virtual vector<Led_tString> GenerateSuggestions (const Led_tString& misspelledWord) = 0;

    public:
        /*
        @METHOD:        SpellCheckEngine::PeekAtTextBreaksUsed
        @DESCRIPTION:   <p>This method can return nullptr. If it returns non-nullptr, you can use this @'TextBreaks' object
                    to break the source text at word boundaries in much the same manner as the spellcheck engine does.</p>
                        <p>This can be useful if you provide a UI which breaks the input into chunks - but wants
                    to make sure the chunks correspond at the edges to word boundaries.</p>
                        <p>Note - the lifetime of the PeekAt call is short. It is at least til the next
                    call to the spellcheck engine (should I gaurantee longer?).
        */
        virtual TextBreaks* PeekAtTextBreaksUsed () = 0;

    public:
        class UDInterface;
        /*
        @METHOD:        SpellCheckEngine::GetUDInterface
        @DESCRIPTION:   <p>This method can return nullptr if there is no UDInterface supported.</p>
        */
        virtual UDInterface* GetUDInterface () = 0;

    public:
        /*
        @METHOD:        SpellCheckEngine::Invariant
        @DESCRIPTION:   <p>if @'qDebug' is on, this called the virtual @'SpellCheckEngine::Invariant' () method
            (in subclasses) to check the state. It can be called freely of @'qDebug' is off - it will have no effect. If
            it is on, however, the time used by this funcion could be significant.</p>
        */
        nonvirtual void Invariant () const;

#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

    /*
    @CLASS:         SpellCheckEngine::UDInterface
    @DESCRIPTION:   <p>
                </p>
    */
    class SpellCheckEngine::UDInterface {
    protected:
        UDInterface () = default;

    public:
        virtual ~UDInterface () = default;

    public:
        /*
        @METHOD:        SpellCheckEngine::UDInterface::AddWordToUserDictionarySupported
        @DESCRIPTION:   <p>This method allows for the UDInterface interface to be supported by a spellcheck engine, but still to dynamically
                    turn on/off UD support (say if the UD is loaded or not).</p>
        */
        virtual bool AddWordToUserDictionarySupported () const = 0;

    public:
        /*
        @METHOD:        SpellCheckEngine::UDInterface::AddWordToUserDictionary
        @DESCRIPTION:   <p>Add the given word the current open (or primary) user dictionary. This is typically called from
                    the 'add word to dictionary' button in the spellcheck dialog.</p>
        */
        virtual void AddWordToUserDictionary (const Led_tString& word) = 0;
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    // class SpellCheckEngine
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
void SpellCheckEngine::Invariant_ () const
{
}
#endif


}

#endif /*_Stroika_Frameworks_Led_SpellCheckEngine_h_*/
