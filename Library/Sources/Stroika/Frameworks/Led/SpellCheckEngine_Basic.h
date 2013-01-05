/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef __SpellCheckEngine_Basic_h__
#define __SpellCheckEngine_Basic_h__    1

/*
@MODULE:    SpellCheckEngine_Basic
@DESCRIPTION:   <p></p>

 */

#include    "../../Foundation/StroikaPreComp.h"

#include    <memory>
#include    <set>

#include    "Support.h"
#include    "SpellCheckEngine.h"
#include    "TextBreaks.h"



namespace   Stroika {
    namespace   Frameworks {
        namespace   Led {


#if     qFailToCompileLargeDataInitializedArraysBug
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
#define qIncludeBakedInDictionaries     0
#endif




            /*
            @CLASS:         SpellCheckEngine_Basic
            @DESCRIPTION:   <p>
                        </p>
            */
            class   SpellCheckEngine_Basic : public SpellCheckEngine {
            private:
                typedef SpellCheckEngine    inherited;

            public:
                class   Dictionary;
                class   EditableDictionary;
                class   CompiledDictionary;

            public:
                SpellCheckEngine_Basic (const Dictionary* mainDictionary = NULL);
                ~SpellCheckEngine_Basic ();

            public:
                virtual    bool    ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                        const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
                                                        ) override;

            protected:
                virtual    bool    LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult) override;

            private:
                nonvirtual  bool    LookupWordHelper_ (const Led_tString& checkWord, Led_tString* matchedWordResult) const;

            protected:
                virtual     bool    OtherStringToIgnore (const Led_tString& checkWord);
                nonvirtual  bool    OtherStringToIgnore_AllPunctuation (const Led_tString& checkWord);
                nonvirtual  bool    OtherStringToIgnore_Sentinals (const Led_tString& checkWord);
                nonvirtual  bool    OtherStringToIgnore_Number (const Led_tString& checkWord);

            public:
                virtual    vector<Led_tString> GenerateSuggestions (const Led_tString& misspelledWord) override;

            public:
                virtual    UDInterface*    GetUDInterface () override;

            public:
                virtual    TextBreaks*     PeekAtTextBreaksUsed () override;


            private:
                nonvirtual  float   Heuristic (const Led_tString& misspelledWord, const Led_tString& candidateWord, float atLeast);

            private:
                nonvirtual  bool    ScanForWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
                                                 const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
                                                );

            public:
                nonvirtual  shared_ptr<TextBreaks>  GetTextBreaker () const;
                nonvirtual  void                    SetTextBreaker (const shared_ptr<TextBreaks>& textBreaker);
            private:
                mutable shared_ptr<TextBreaks>  fTextBreaker;

            public:
                struct  InfoBlock {
                    unsigned    int fIndex: 22;
                    unsigned    int fWordLen: 8;
                    unsigned    int fXXX: 2; // some flags - I forget the design - but I'll need this...
                };


#if     qIncludeBakedInDictionaries
            public:
                static  const   CompiledDictionary      kDictionary_US_English;
#endif

            public:
                nonvirtual  vector<const Dictionary*>   GetDictionaries () const;
                nonvirtual  void                        SetDictionaries (const vector<const Dictionary*>& dictionaries);
            private:
                vector<const Dictionary*>   fDictionaries;


#if     qDebug
            protected:
                virtual    void    Invariant_ () const override;
#endif

#if     qDebug
            public:
                /*
                @METHOD:        SpellCheckEngine_Basic::RegressionTest
                @DESCRIPTION:   <p>This function only exists if @'qDebug' is on. When run, it performs a basic regression test.</p>
                */
                static  void    RegressionTest ();
            private:
                static  void    RegressionTest_1 ();
#endif
            };


            class   SpellCheckEngine_Basic::Dictionary {
            public:
                typedef SpellCheckEngine_Basic::InfoBlock   InfoBlock;

            protected:
                Dictionary ();
            public:
                virtual ~Dictionary ();

            public:
                virtual const Led_tChar*    GetTextBase () const    =   0;
                virtual const Led_tChar*    GetTextEnd () const =   0;
                virtual void                GetInfoBlocks (const InfoBlock** start, const InfoBlock** end) const =   0;
            };

            class   SpellCheckEngine_Basic::EditableDictionary : public SpellCheckEngine_Basic::Dictionary {
            private:
                typedef SpellCheckEngine_Basic::Dictionary  inherited;

            public:
                EditableDictionary ();
            public:
                virtual ~EditableDictionary ();

            public:
                nonvirtual  void    AddWordToUserDictionary (const Led_tString& word);

            public:
                virtual    const Led_tChar*    GetTextBase () const override;
                virtual    const Led_tChar*    GetTextEnd () const override;
                virtual    void                GetInfoBlocks (const InfoBlock** start, const InfoBlock** end) const override;

            public:
                nonvirtual  void                ReadFromBuffer (const Led_tChar* readOnlyRAMDictStart, const Led_tChar* readOnlyRAMDictEnd);
                nonvirtual  vector<Led_tChar>   SaveToBuffer () const;

            private:
                nonvirtual  void    ConstructInfoBlocksEtcFromWordList ();

            private:
                set<Led_tString>    fSortedWordList;
                Led_tChar*          fDictBufStart;
                Led_tChar*          fDictBufEnd;
                vector<InfoBlock>   fInfoBlocks;
            };

            class   SpellCheckEngine_Basic::CompiledDictionary : public SpellCheckEngine_Basic::Dictionary {
            private:
                typedef SpellCheckEngine_Basic::Dictionary  inherited;

            public:
                struct  CompiledDictionaryData {
                    const Led_tChar*    fTextDataStart;
                    const Led_tChar*    fTextDataEnd;
                    const InfoBlock*    fInfoBlocksStart;
                    const InfoBlock*    fInfoBlocksEnd;
                };

            public:
                CompiledDictionary (const CompiledDictionaryData& data);

            public:
                virtual    const Led_tChar*    GetTextBase () const override;
                virtual    const Led_tChar*    GetTextEnd () const override;
                virtual    void                GetInfoBlocks (const InfoBlock** start, const InfoBlock** end) const override;

            private:
                CompiledDictionaryData  fData;
            };




            /*
            @CLASS:         TextBreaks_SpellChecker
            @BASES:         @'TextBreaks_Basic'
            @DESCRIPTION:   <p>Special purpose text-break implementation uses inside spell-checker. Not recomended for use elsewhere, but
                        it could be.</p>
            */
            class   TextBreaks_SpellChecker : public TextBreaks_Basic {
            private:
                typedef TextBreaks_Basic    inherited;
            public:
                TextBreaks_SpellChecker ();

            protected:
                virtual    CharacterClasses    CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const override;

#if     qDebug
            private:
                nonvirtual  void    RegressionTest ();
#endif
            };




            /*
            @CLASS:         SpellCheckEngine_Basic_Simple
            @DESCRIPTION:   <p>This is a simple wrapper providing most pre-packaged spellcheck functionality a spellchekcing application would typically want.
                        You easily specify one pre-built 'system' dictionary, and one optional user-dictionary (you just specify the file name
                        and the rest is taken care of automatically).
                        </p>
            */
            class   SpellCheckEngine_Basic_Simple : public SpellCheckEngine_Basic, private SpellCheckEngine::UDInterface {
            private:
                typedef SpellCheckEngine_Basic  inherited;

            public:
                SpellCheckEngine_Basic_Simple ();
                ~SpellCheckEngine_Basic_Simple ();

            public:
#if     qMixinDisambiguatingNameInBothBug
                typedef inherited::UDInterface  UDInterface;
#endif
                virtual    UDInterface*    GetUDInterface () override;

                // From SpellCheckEngine::UDInterface
            public:
                virtual    bool    AddWordToUserDictionarySupported () const override;
                virtual    void    AddWordToUserDictionary (const Led_tString& word) override;

            public:
                nonvirtual  const Dictionary*   GetMainDictionary () const;
                nonvirtual  void                SetMainDictionary (const Dictionary* mainDictionary);
            private:
                const Dictionary*   fMainDictionary;

            public:
#if     qPlatform_MacOS
                typedef FSSpec  UDDictionaryName;
#else
                typedef Led_SDK_String  UDDictionaryName;
#endif
                nonvirtual  UDDictionaryName    GetUserDictionary () const;
                nonvirtual  void                SetUserDictionary (const UDDictionaryName& userDictionary);
            private:
                UDDictionaryName    fUDName;
                EditableDictionary* fUD;

            private:
                nonvirtual  void    ReadFromUD ();
                nonvirtual  void    WriteToUD ();
            };




            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */

// class SpellCheckEngine_Basic
            inline  bool    SpellCheckEngine_Basic::OtherStringToIgnore_Sentinals (const Led_tString& checkWord)
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
            inline  shared_ptr<TextBreaks>  SpellCheckEngine_Basic::GetTextBreaker () const
            {
                if (fTextBreaker.get () == nullptr) {
                    fTextBreaker = shared_ptr<TextBreaks> (new TextBreaks_SpellChecker ());
                }
                return fTextBreaker;
            }
            /*
            @METHOD:        SpellCheckEngine_Basic::SetTextBreaker
            @DESCRIPTION:   <p>See @'SpellCheckEngine_Basic::GetTextBreaker'.</p>
            */
            inline  void    SpellCheckEngine_Basic::SetTextBreaker (const shared_ptr<TextBreaks>& textBreaker)
            {
                fTextBreaker = textBreaker;
            }



// class SpellCheckEngine_Basic
            inline  SpellCheckEngine_Basic::Dictionary::Dictionary ()
            {
            }
            inline  SpellCheckEngine_Basic::Dictionary::~Dictionary ()
            {
            }



        }
    }
}



#endif  /*__SpellCheckEngine_Basic_h__*/
