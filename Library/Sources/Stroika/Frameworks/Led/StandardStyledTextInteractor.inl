/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_StandardStyledTextInteractor_inl_
#define _Stroika_Framework_Led_StandardStyledTextInteractor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     *********************** StandardStyledTextIOSrcStream **************************
     ********************************************************************************
     */
    inline size_t StandardStyledTextIOSrcStream::GetCurOffset () const
    {
        return fCurOffset;
    }
    inline size_t StandardStyledTextIOSrcStream::GetSelStart () const
    {
        return fSelStart;
    }
    inline size_t StandardStyledTextIOSrcStream::GetSelEnd () const
    {
        return fSelEnd;
    }

    /*
     ********************************************************************************
     *********************** StandardStyledTextIOSrcStream **************************
     ********************************************************************************
     */
    inline TextStore& StandardStyledTextIOSinkStream::GetTextStore () const
    {
        EnsureNotNull (fTextStore);
        return *fTextStore;
    }
    inline shared_ptr<AbstractStyleDatabaseRep> StandardStyledTextIOSinkStream::GetStyleDatabase () const
    {
        return fStyleRunDatabase;
    }
    /*
    @METHOD:        StandardStyledTextIOSinkStream::GetInsertionStart
    @DESCRIPTION:   <p>Returns where (in TextStore marker coordinates - not relative to the sinkstream) where the next character
                will be inserted. See also @'StandardStyledTextIOSinkStream::SetInsertionStart'</p>
    */
    inline size_t StandardStyledTextIOSinkStream::GetInsertionStart () const
    {
        return fInsertionStart;
    }
    /*
    @METHOD:        StandardStyledTextIOSinkStream::SetInsertionStart
    @DESCRIPTION:   <p>See also @'StandardStyledTextIOSinkStream::GetInsertionStart'</p>
    */
    inline void StandardStyledTextIOSinkStream::SetInsertionStart (size_t insertionStart)
    {
        fInsertionStart = insertionStart;
    }
    inline size_t StandardStyledTextIOSinkStream::GetOriginalStart () const
    {
        return fOriginalStart;
    }
    inline size_t StandardStyledTextIOSinkStream::GetCachedTextSize () const
    {
        return fCachedText.size ();
    }
    inline const vector<Led_tChar>& StandardStyledTextIOSinkStream::GetCachedText () const
    {
        return fCachedText;
    }

#if qStroika_Frameworks_Led_SupportGDI
    /*
     ********************************************************************************
     *********************** StandardStyledTextInteractor ***************************
     ********************************************************************************
     */
    /*
    @METHOD:        StandardStyledTextInteractor::GetCommandNames
    @DESCRIPTION:   <p>Returns command name for each of the user-visible commands produced by this module.
        This name is used used in the constructed Undo command name, as
        in, "Undo Change Font". You can replace this name with whatever you like.You change this value with
        WordProcessor::SetCommandNames.</p>
            <p> The point of this is to allow for different UI-language localizations,
                without having to change Led itself.</p>
            <p>See also @'StandardStyledTextInteractor::CommandNames'.</p>
    */
    inline const StandardStyledTextInteractor::CommandNames& StandardStyledTextInteractor::GetCommandNames ()
    {
        return sCommandNames;
    }
    /*
    @METHOD:        StandardStyledTextInteractor::SetCommandNames
    @DESCRIPTION:   <p>See @'StandardStyledTextInteractor::GetCommandNames'.</p>
    */
    inline void StandardStyledTextInteractor::SetCommandNames (const StandardStyledTextInteractor::CommandNames& cmdNames)
    {
        sCommandNames = cmdNames;
    }
#endif

}

#endif /*_Stroika_Framework_Led_StandardStyledTextInteractor_inl_*/
