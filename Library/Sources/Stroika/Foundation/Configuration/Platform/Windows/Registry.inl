/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Platform_Windows_Registry_inl_
#define _Stroika_Foundation_Configuration_Platform_Windows_Registry_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Configuration::Platform::Windows {

	constexpr RegistryKey::RegistryKey (HKEY hkey, bool owned)
		: fKey_ (hkey)
		, fOwned_{ owned }
	{
		static_assert (nullptr != INVALID_HANDLE_VALUE);
		Require (hkey != nullptr);
		Require (hkey != INVALID_HANDLE_VALUE);
	}
	inline RegistryKey::~RegistryKey ()
	{
		Assert (nullptr != INVALID_HANDLE_VALUE);
		Assert (fKey_ != nullptr);
		Assert (fKey_ != INVALID_HANDLE_VALUE);
		::RegCloseKey (fKey_);
	}
	inline RegistryKey::operator HKEY () const
	{
		Ensure (fKey_ != nullptr);
		Ensure (fKey_ != INVALID_HANDLE_VALUE);
		return fKey_;
	}


}


#endif /*_Stroika_Foundation_Configuration_Platform_Windows_Registry_inl_*/
