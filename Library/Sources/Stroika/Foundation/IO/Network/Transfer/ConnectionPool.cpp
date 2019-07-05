/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "ConnectionPool.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

namespace {

    using Ptr     = Connection::Ptr;
    using Options = Connection::Options;

    struct Rep_ : Connection::IRep {

        Ptr fDelegateTo;

        Rep_ (const Ptr& delegateTo)
            : fDelegateTo (delegateTo)
        {
        }
        Rep_ (const Rep_&) = delete;
        virtual ~Rep_ ()
        {
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    public:
        virtual Options GetOptions () const override
        {
            return fDelegateTo.GetOptions ();
        }
        virtual DurationSecondsType GetTimeout () const override
        {
            return fDelegateTo.GetTimeout ();
        }
        virtual void SetTimeout (DurationSecondsType timeout) override
        {
            fDelegateTo.SetTimeout (timeout);
        }
        virtual URI DeprecatedGetAuthorityRelativeURL () const override
        {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            return fDelegateTo.GetURL ().GetAuthorityRelativeResource ();
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
        virtual void DeprecatedSetAuthorityRelativeURL (const URI& url) override
        {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            fDelegateTo.SetURL (fDelegateTo.GetURL ().Combine (url));
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
        virtual URI GetSchemeAndAuthority () const override
        {
            return fDelegateTo.GetSchemeAndAuthority ();
        }
        virtual void SetSchemeAndAuthority (const URI& schemeAndAuthority) override
        {
            fDelegateTo.SetSchemeAndAuthority (schemeAndAuthority);
        }
        virtual void Close () override
        {
            fDelegateTo.Close ();
        }
        virtual Response Send (const Request& request) override
        {
            return fDelegateTo.Send (request);
        }
    };
}
/*
 ********************************************************************************
 ************************ Transfer::CreateConnection ****************************
 ********************************************************************************
 */
