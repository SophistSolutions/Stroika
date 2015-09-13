/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/SDKString.h"
#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BlockAllocated.h"

#include    "../ResourceNotFoundException.h"

#include    "WindowsResourceManager.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Execution::Resources;
using   namespace   Stroika::Foundation::Execution::Resources::Concrete;





class    WindowsResourceManager::IRep_ : public Manager::_IRep {
private:
    HMODULE fModule_;
public:
    IRep_ (HMODULE hModule)
        : fModule_ (hModule)
    {
    }
    virtual Accessor    ReadResource (const Name& name) const override
    {
        HRSRC   hres = ::FindResource (fModule_, name.GetSDKString (), name.GetType ());
        if (hres != nullptr) {
            HGLOBAL lglbl   =   ::LoadResource (fModule_, hres);
            if (lglbl != nullptr) {
                const void* lr  =   ::LockResource (lglbl);
                AssertNotNull (lr);
                const Byte* start = reinterpret_cast<const Byte*> (lr);
                return Manager::_mkAccessor (start, start + ::SizeofResource (fModule_, hres));
            }
        }
        DoThrow (ResourceNotFoundException ());
    }
public:
    DECLARE_USE_BLOCK_ALLOCATION (IRep_);
};




/*
********************************************************************************
***************************** WindowsResourceManager ***************************
********************************************************************************
*/
WindowsResourceManager::WindowsResourceManager (HMODULE hModule)
    : Manager (_SharedRep (new IRep_ (hModule)))
{
}
