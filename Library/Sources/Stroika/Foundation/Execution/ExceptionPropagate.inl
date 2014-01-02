/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Propagate_inl_
#define _Stroia_Foundation_Execution_Propagate_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Memory/Common.h"
#include    "../Memory/LeakChecker.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


#if     qPlatform_Windows
#define CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)\
    catch (const Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException& e) {\
        (CE).fHRESULTErrorException = std::shared_ptr<Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException> (DEBUG_NEW Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException (e));\
    }\
    catch (const Stroika::Foundation::Execution::Platform::Windows::Exception& e) {\
        (CE).fWin32ErrorException = std::shared_ptr<Stroika::Foundation::Execution::Platform::Windows::Exception> (DEBUG_NEW Stroika::Foundation::Execution::Platform::Windows::Exception (e));\
    }
#else
#define CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)
#endif

#define CATCH_AND_CAPTURE_CATCH_BLOCK(CE)\
    catch (const Stroika::Foundation::Execution::RequiredComponentMissingException& e) {\
        (CE).fRequiredComponentMissingException = std::shared_ptr<Stroika::Foundation::Execution::RequiredComponentMissingException> (DEBUG_NEW Stroika::Foundation::Execution::RequiredComponentMissingException (e));\
    }\
    CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)\
    catch (const Stroika::Foundation::IO::FileFormatException& e) {\
        (CE).fFileFormatException = std::shared_ptr<Stroika::Foundation::IO::FileFormatException> (DEBUG_NEW Stroika::Foundation::IO::FileFormatException (e));\
    }\
    catch (const Stroika::Foundation::IO::FileBusyException& e) {\
        (CE).fFileBusyException = std::shared_ptr<Stroika::Foundation::IO::FileBusyException> (DEBUG_NEW Stroika::Foundation::IO::FileBusyException (e));\
    }\
    catch (const Stroika::Foundation::Execution::SilentException& e) {\
        (CE).fSilentException = std::shared_ptr<Stroika::Foundation::Execution::SilentException> (DEBUG_NEW Stroika::Foundation::Execution::SilentException (e));\
    }\
    catch (const Stroika::Foundation::Execution::StringException& e) {\
        (CE).fStringException = std::shared_ptr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (e));\
    }\
    catch (...) {\
        (CE).fStringException = std::shared_ptr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (L"Unknown Exception"));\
    }\
     

        }
    }
}

#endif  /*_Stroia_Foundation_Execution_Propagate_inl_*/
