/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
     ***************************** StandardStyledTextInteractor ***************************
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
#if qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
        return sCommandNames ();
#else
        return sCommandNames;
#endif
    }
    /*
    @METHOD:        StandardStyledTextInteractor::SetCommandNames
    @DESCRIPTION:   <p>See @'StandardStyledTextInteractor::GetCommandNames'.</p>
    */
    inline void StandardStyledTextInteractor::SetCommandNames (const StandardStyledTextInteractor::CommandNames& cmdNames)
    {
#if qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
        sCommandNames () = cmdNames;
#else
        sCommandNames = cmdNames;
#endif
    }

    //  class   StandardStyledTextInteractor::StandardStyledTextIOSinkStream
    inline TextStore& StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetTextStore () const
    {
        EnsureNotNull (fTextStore);
        return *fTextStore;
    }
    inline StandardStyledTextImager::StyleDatabasePtr StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetStyleDatabase () const
    {
        return fStyleRunDatabase;
    }
    /*
    @METHOD:        StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart
    @DESCRIPTION:   <p>Returns where (in TextStore marker coordinates - not relative to the sinkstream) where the next character
                will be inserted. See also @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart'</p>
    */
    inline size_t StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart () const
    {
        return fInsertionStart;
    }
    /*
    @METHOD:        StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart
    @DESCRIPTION:   <p>See also @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart'</p>
    */
    inline void StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart (size_t insertionStart)
    {
        fInsertionStart = insertionStart;
    }
    inline size_t StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetOriginalStart () const
    {
        return fOriginalStart;
    }
    inline size_t StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetCachedTextSize () const
    {
        return fCachedText.size ();
    }
    inline const vector<Led_tChar>& StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetCachedText () const
    {
        return fCachedText;
    }

    //  class   StandardStyledTextInteractor::StandardStyledTextIOSrcStream
    inline size_t StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetCurOffset () const
    {
        return fCurOffset;
    }
    inline size_t StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetSelStart () const
    {
        return fSelStart;
    }
    inline size_t StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetSelEnd () const
    {
        return fSelEnd;
    }

}

#endif /*_Stroika_Framework_Led_StandardStyledTextInteractor_inl_*/
