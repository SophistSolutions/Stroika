/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Common/Common.h"
#include "../../../Memory/BlockAllocated.h"

#include "../ResourceNotFoundException.h"

#include "WindowsResourceManager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Resources;
using namespace Stroika::Foundation::Execution::Resources::Concrete;

class WindowsResourceManager::Rep_ final : public Manager::_IRep {
private:
    HMODULE fModule_;

public:
    Rep_ (HMODULE hModule)
        : fModule_{hModule}
    {
    }
    virtual Accessor ReadResource (const Name& name) const override
    {
        HRSRC hres = ::FindResource (fModule_, name.GetSDKString (), name.GetType ());
        if (hres != nullptr) {
            HGLOBAL lglbl = ::LoadResource (fModule_, hres);
            if (lglbl != nullptr) {
                const void* lr = ::LockResource (lglbl);
                AssertNotNull (lr);
                const byte* start = reinterpret_cast<const byte*> (lr);
                return Manager::_mkAccessor (span{start, ::SizeofResource (fModule_, hres)});
            }
        }
        Throw (ResourceNotFoundException::kThe);
    }
};

/*
********************************************************************************
***************************** WindowsResourceManager ***************************
********************************************************************************
*/
WindowsResourceManager::WindowsResourceManager (HMODULE hModule)
    : Manager{make_shared<Rep_> (hModule)}
{
}
