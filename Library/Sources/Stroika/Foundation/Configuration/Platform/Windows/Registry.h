/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Platform_Windows_Registry_h_
#define _Stroika_Foundation_Configuration_Platform_Windows_Registry_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../Common.h"
#include "../../../Characters/String.h"
#include "../../../DataExchange/VariantValue.h"

namespace Stroika::Foundation::Configuration::Platform::Windows {


	/**
	 *	Simple utility class to allow (read-only) access to the windows registry.
	 *
	 *	I considered making this support modification, but as JSON configuraiton files seem a better path than using
	 *	the registry, and this is mainly for backward compatability with older systems, I chose to keep this read-only.
	 */
	class RegistryKey {
	public:
		/**
		 *	Require HKEY argument (parent or actual) to be != null, and != INVALID_HANDLE_VALUE
		 *
		 *	If the KEY/String overload is used, and the key cannot be opened, and exception is raised: you can
		 *	never get an invalid RegistryKey object.
		 *
		 *	The overload taking 'owned' argument determines if the key is destroyed at the end of ownership.
		 *
		 *	'samDesired' must be KEY_READ or some other permission that doesn't cause creation.
		 */
		constexpr RegistryKey (HKEY hkey, bool owned = false);
		RegistryKey (HKEY parentKey, const Characters::String& path, REGSAM samDesired = KEY_READ);
		RegistryKey () = delete;
		RegistryKey (const RegistryKey&) = delete;

	public:
		/**
		 */
		const RegistryKey& operator= (const RegistryKey&) const = delete;

	public:
		/**
		 */
		~RegistryKey ();

	public:
		nonvirtual operator HKEY () const;

	public:
		/**
		 *	Returns empty object (null) if missing. Can also return String type, or numeric for DWORDs.
		 *
		 *	\note @todo - should return empty VariantValue if missing, but for permissions issues, raise exception
		 */
		nonvirtual DataExchange::VariantValue LookupPref (const Characters::String& prefName) const;

	private:
		/*
		 * Walk the given path (in segments) - and make sure each exists, and create each segment if it doesn't
		 * exist.Finally - do a regular registry open with access permissions 'samDesired'.< / p>
		 */
		static HKEY OpenWithCreateAlongPath_ (HKEY parentKey, const Characters::String& path, REGSAM samDesired);

	private:
		bool fOwned_;
		HKEY fKey_;
	};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Registry.inl"

#endif /*_Stroika_Foundation_Configuration_Platform_Windows_Registry_h_*/
