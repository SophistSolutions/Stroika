/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SpellCheckEngine_Basic_h_
#define _Stroika_Frameworks_Led_SpellCheckEngine_Basic_h_ 1

/*
@MODULE:    SpellCheckEngine_Basic
@DESCRIPTION:   <p></p>

 */

#include "../StroikaPreComp.h"

#include <memory>
#include <set>

#include "SpellCheckEngine.h"
#include "Support.h"
#include "TextBreaks.h"

namespace Stroika::Frameworks::Led {

#if qFailToCompileLargeDataInitializedArraysBug
#define qIncludeBakedInDictionaries 0
#endif

/*
    @CONFIGVAR:     qIncludeBakedInDictionaries
    @DESCRIPTION:   <p>Turning this on (its on by default) includes into the binary - pre-built dictionaries (currently just US-English).
                Including this allows direct access to the US-English dictionary. However - its large - adding about 2-3MB (depending
                on if you use UNICODE or single-byte - among other things) to the size of your binary.</p>
        */
#ifndef qIncludeBakedInDictionaries
// DISABLE BY DEFAULT UNTIL WE PUT IN PLACE BETTER MECHANISM FOR 'RESOURCES' - as we do in HealthFrame for stuff like XSDs...
#define qIncludeBakedInDictionaries 0
#endif

    /*
    @CLASS:         SpellCheckEngine_Basic
    @DESCRIPTION:   <p>
                </p>
    */
    class SpellCheckEngine_Basic : public SpellCheckEngine {
    private:
        using inherited = SpellCheckEngine;

    public:
        class Dictionary;
        class EditableDictionary;
        class CompiledDictionary;

    public:
        SpellCheckEngine_Basic (const Dictionary* mainDictionary = NULL);
        ~SpellCheckEngine_Basic ();

    public:
        virtual bool ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                                           const Led_tChar** wordStartResult, const Led_tChar** wordEndResult) override;

    protected:
        virtual bool LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult) override;

    private:
        nonvirtual bool LookupWordHelper_ (const Led_tString& checkWord, Led_tString* matchedWordResult) const;

    protected:
        virtual bool    OtherStringToIgnore (const Led_tString& checkWord);
        nonvirtual bool OtherStringToIgnore_AllPunctuation (const Led_tString& checkWord);
        nonvirtual bool OtherStringToIgnore_Sentinals (const Led_tString& checkWord);
        nonvirtual bool OtherStringToIgnore_Number (const Led_tString& checkWord);

    public:
        virtual vector<Led_tString> GenerateSuggestions (const Led_tString& misspelledWord) override;

    public:
        virtual UDInterface* GetUDInterface () override;

    public:
        virtual TextBreaks* PeekAtTextBreaksUsed () override;

    private:
        nonvirtual float Heuristic (const Led_tString& misspelledWord, const Led_tString& candidateWord, float atLeast);

    private:
        nonvirtual bool ScanForWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                                     const Led_tChar** wordStartResult, const Led_tChar** wordEndResult);

    public:
        nonvirtual shared_ptr<TextBreaks> GetTextBreaker () const;
        nonvirtual void                   SetTextBreaker (const shared_ptr<TextBreaks>& textBreaker);

    private:
        mutable shared_ptr<TextBreaks> fTextBreaker;

    public:
        struct InfoBlock {
            unsigned int fIndex : 22;
            unsigned int fWordLen : 8;
            unsigned int fXXX : 2; // some flags - I forget the design - but I'll need this...
        };

#if qIncludeBakedInDictionaries
    public:
        static const CompiledDictionary kDictionary_US_English;
#endif

    public:
        nonvirtual vector<const Dictionary*> GetDictionaries () const;
        nonvirtual void                      SetDictionaries (const vector<const Dictionary*>& dictionaries);

    private:
        vector<const Dictionary*> fDictionaries;

#if qDebug
    protected:
        virtual void Invariant_ () const override;
#endif

#if qDebug
    public:
        /*
        @METHOD:        SpellCheckEngine_Basic::RegressionTest
        @DESCRIPTION:   <p>This function only exists if @'qDebug' is on. When run, it performs a basic regression test.</p>
        */
        static void RegressionTest ();

    private:
        static void RegressionTest_1 ();
#endif
    };

    class SpellCheckEngine_Basic::Dictionary {
    public:
        using InfoBlock = SpellCheckEngine_Basic::InfoBlock;

    protected:
        Dictionary ();

    public:
        virtual ~Dictionary ();

    public:
        virtual const Led_tChar* GetTextBase () const                                                 = 0;
        virtual const Led_tChar* GetTextEnd () const                                                  = 0;
        virtual void             GetInfoBlocks (const InfoBlock** start, const InfoBlock** end) const = 0;
    };

    class SpellCheckEngine_Basic::EditableDictionary : public SpellCheckEngine_Basic::Dictionary {
    private:
        using inherited = SpellCheckEngine_Basic::Dictionary;

    public:
        EditableDictionary ();

    public:
        virtual ~EditableDictionary ();

    public:
        nonvirtual void AddWordToUserDictionary (const Led_tString& word);

    public:
        virtual const Led_tChar* GetTextBase () const override;
        virtual const Led_tChar* GetTextEnd () const override;
        virtual void             GetInfoBlocks (const InfoBlock** start, const InfoBlock** end) const override;

    public:
        nonvirtual void ReadFromBuffer (const Led_tChar* readOnlyRAMDictStart, const Led_tChar* readOnlyRAMDictEnd);
        nonvirtual vector<Led_tChar> SaveToBuffer () const;

    private:
        nonvirtual void ConstructInfoBlocksEtcFromWordList ();

    private:
        set<Led_tString>  fSortedWordList;
        Led_tChar*        fDictBufStart;
        Led_tChar*        fDictBufEnd;
        vector<InfoBlock> fInfoBlocks;
    };

    class SpellCheckEngine_Basic::CompiledDictionary : public SpellCheckEngine_Basic::Dictionary {
    private:
        using inherited = SpellCheckEngine_Basic::Dictionary;

    public:
        struct CompiledDictionaryData {
            const Led_tChar* fTextDataStart;
            const Led_tChar* fTextDataEnd;
            const InfoBlock* fInfoBlocksStart;
            const InfoBlock* fInfoBlocksEnd;
        };

    public:
        CompiledDictionary (const CompiledDictionaryData& data);

    public:
        virtual const Led_tChar* GetTextBase () const override;
        virtual const Led_tChar* GetTextEnd () const override;
        virtual void             GetInfoBlocks (const InfoBlock** start, const InfoBlock** end) const override;

    private:
        CompiledDictionaryData fData;
    };

    /*
    @CLASS:         TextBreaks_SpellChecker
    @BASES:         @'TextBreaks_Basic'
    @DESCRIPTION:   <p>Special purpose text-break implementation uses inside spell-checker. Not recomended for use elsewhere, but
                it could be.</p>
    */
    class TextBreaks_SpellChecker : public TextBreaks_Basic {
    private:
        using inherited = TextBreaks_Basic;

    public:
        TextBreaks_SpellChecker ();

    protected:
        virtual CharacterClasses CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const override;

#if qDebug
    private:
        nonvirtual void RegressionTest ();
#endif
    };

    /*
    @CLASS:         SpellCheckEngine_Basic_Simple
    @DESCRIPTION:   <p>This is a simple wrapper providing most pre-packaged spellcheck functionality a spellchekcing application would typically want.
                You easily specify one pre-built 'system' dictionary, and one optional user-dictionary (you just specify the file name
                and the rest is taken care of automatically).
                </p>
    */
    class SpellCheckEngine_Basic_Simple : public SpellCheckEngine_Basic, private SpellCheckEngine::UDInterface {
    private:
        using inherited = SpellCheckEngine_Basic;

    public:
        SpellCheckEngine_Basic_Simple ();
        ~SpellCheckEngine_Basic_Simple ();

    public:
        virtual UDInterface* GetUDInterface () override;

        // From SpellCheckEngine::UDInterface
    public:
        virtual bool AddWordToUserDictionarySupported () const override;
        virtual void AddWordToUserDictionary (const Led_tString& word) override;

    public:
        nonvirtual const Dictionary* GetMainDictionary () const;
        nonvirtual void              SetMainDictionary (const Dictionary* mainDictionary);

    private:
        const Dictionary* fMainDictionary;

    public:
#if qPlatform_MacOS
        using UDDictionaryName = FSSpec;
#else
        using UDDictionaryName = Led_SDK_String;
#endif
        nonvirtual UDDictionaryName GetUserDictionary () const;
        nonvirtual void             SetUserDictionary (const UDDictionaryName& userDictionary);

    private:
        UDDictionaryName    fUDName;
        EditableDictionary* fUD;

    private:
        nonvirtual void ReadFromUD ();
        nonvirtual void WriteToUD ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SpellCheckEngine_Basic.inl"

#endif /*_Stroika_Frameworks_Led_SpellCheckEngine_Basic_h_*/
