/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_TransferEncoding_h_
#define _Stroika_Foundation_IO_Network_HTTP_TransferEncoding_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Containers/Set.h"

/**
 */

namespace Stroika::Foundation::IO::Network::HTTP {

	using Characters::String;
	using Containers::Set;

	/**
	 *	\brief ransfer-Encoding is a hop-by-hop header, that is applied to a message between two nodes, not to a resource itself. Each segment of a multi-node connection can use different Transfer-Encoding values. If you want to compress data over the whole connection, use the end-to-end Content-Encoding header instead
	 * 
	 *	TransferEncoding is typically identity (in which case omitted) or chucked (@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding)
	 *
	 *	This is (probably mostly) osboelted in HTTP 2.0(@todo read up); and we dont currently intned to support compress etc here 
	 *	@see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Encoding
	 */
	enum class TransferEncoding {
        eChunked,
		eCompress,
		eDeflate,
		eGZip,
		eIdentity,

		Stroika_Define_Enum_Bounds (eChunked, eIdentity)
	};


	/**
	 */
    class TransferEncodings : public Set<TransferEncoding> {
    public:
        TransferEncodings (const Traversal::Iterable<TransferEncoding>& src);
        TransferEncodings (TransferEncoding tc);

	public:
        nonvirtual TransferEncodings& operator= (const TransferEncodings& rhs) = default;

	public:
        /**
		 */
        nonvirtual String Encode () const;

    public:
        /**
		 */
        static TransferEncodings Decode (const String& headerValue);
    };
	
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TransferEncoding.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_TransferEncoding_h_*/
