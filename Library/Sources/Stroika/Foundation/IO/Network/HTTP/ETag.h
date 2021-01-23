/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ETag_h_
#define _Stroika_Foundation_IO_Network_HTTP_ETag_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

	using Characters::String;

	/**
	 *	ETag is formatted with surrounding quotes (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/ETag)
	 */
	struct ETag {

		ETag (const String& value, bool weak = false);

		/**
		 *	If ill-format or missing, return nullopt, no exception.
		 */
        static optional <ETag> Parse (const String& wireFormat);

        String fValue;
        bool   fWeak{false};

		/**
		 *	This As<> encodes the ETag as specified in HTTP SPEC (URL)
		 *
		 *	T can be among these:
		 *		o	String
		 */
		template <typename T>
        T As () const;
	};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ETag.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_ETag_h_*/
