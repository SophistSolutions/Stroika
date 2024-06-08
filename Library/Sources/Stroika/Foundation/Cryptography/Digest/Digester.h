/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_h_
#define _Stroika_Foundation_Cryptography_Digest_Digester_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdint>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/Algorithm.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Cryptography::Digest {

    using Memory::BLOB;

    /**
     *  \brief ComputeDigest<ALGORITHM> () is the simplest and generally best way to call Digest algorithms
     * 
     *  A Digest is an algorithm that takes a stream of bytes and computes a series of bits
     *  (can be thought of as a number, or string, or seqeunce of bits) which hopefully as
     *  nearly as possible (given the length of the digest) uniquely identifies the input.
     *
     *  A digest is generally of fixed length - often 4, or 16, or 20 bytes long.
     *
     *  RETURN_TYPE is typically uint32_t, uint64_t, Common::GUID, or Result128BitType, Result128BitType etc,
     *  but could in principle be anything.
     *
     *  \note Endianness - these algorithms logically operate on bytes, so if you use RETURN_TYPE=uin32_t (or anything but byte array) -
     *          expect the actual numerical value will depend on endianness.
     *
     *  \par Example Usage
     *      \code
     *          string digestStr = Format (ComputeDigest<Algorithm::MD5> (s, e));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          SourceDefinition    tmp;    // some struct which defines ostream operator>>
     *          string  digestStr = Format (ComputeDigest<Algorithm::MD5> (Streams::iostream::SerializeItemToBLOB (tmp)));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          return ComputeDigest<Algorithm::MD5> (Memory::BLOB::FromRaw (sb.AsUTF8 ())));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // to convert to string
     *          return Format<String> (ComputeDigest<Algorithm::MD5> (Memory::BLOB::FromRaw (sb)));
     *          // OR
     *          return ComputeDigest<Algorithm::MD5, String> (Memory::BLOB::FromRaw (sb)));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          using namespace IO::Network;
     *          Memory::BLOB value2Hash     = DefaultSerializer<InternetAddress>{}(InternetAddress{"192.168.244.33"});
     *          auto         h1             = ComputeDigest<Digest::Algorithm::SuperFastHash> (value2Hash);
     *          uint8_t      h2             = ComputeDigest<Digest::Algorithm::SuperFastHash, uint8_t> (value2Hash);
     *          EXPECT_TRUE (h1 == 2512011991); // experimentally derived values but they shouldn't float (actually may depend on endiannesss?)
     *          EXPECT_TRUE (h2 == 215);
     *          std::array<byte, 40> h3 = ComputeDigest<Digest::Algorithm::SuperFastHash, std::array<byte, 40>> (value2Hash);
     *          EXPECT_TRUE (h3[0] == 215_b and h3[1] == 66_b and h3[39] == 0_b);
     *          if (Configuration::GetEndianness () == Configuration::Endian::eX86) {
     *              EXPECT_TRUE ((ComputeDigest<Digest::Algorithm::SuperFastHash, string>(value2Hash) == "0x2512011991"));
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          String s1 = "abc";
     *          auto   r1 = Digest::ComputeDigest<Digest::Algorithm::MD5> (s1);
     *      \endcode
     *
     *  @see  Hash ()
     *  @see  IncrementalDigester ()
     *  @see  Digester ()
     *    
     */
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    RETURN_TYPE ComputeDigest (const Streams::InputStream::Ptr<byte>& from);
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    RETURN_TYPE ComputeDigest (const byte* from, const byte* to);
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    RETURN_TYPE ComputeDigest (span<const byte> from);
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    RETURN_TYPE ComputeDigest (const BLOB& from);
    template <typename ALGORITHM, typename TRIVIALLY_COPYABLE_T, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    RETURN_TYPE ComputeDigest (const Traversal::Iterable<TRIVIALLY_COPYABLE_T>& from)
        requires (is_trivially_copyable_v<TRIVIALLY_COPYABLE_T>);

    /**
     *  \brief IncrementalDigester<ALGORITHM> () is the low level way to call Digest algorithms, appropriate for streamed sources of data (because it a stateful object you can call Write on multiple times before extracting the digest)
     * 
     *  A Digest is an algorithm that takes a stream of bytes and computes a series of bits
     *  (can be thought of as a number, or string, or seqeunce of bits) which hopefully as
     *  nearly as possible (given the length of the digest) uniquely identifies the input.
     *
     *  A digest is generally of fixed length - often 4, or 16, or 20 bytes long.
     *
     *  RETURN_TYPE is typically uint32_t, uint64_t, or Result128BitType, Result128BitType etc,
     *  but could in principle be anything.
     *
     *  \note Endianness - these algorithms logically operate on bytes, so if you use RETURN_TYPE=uin32_t (or anything but byte array) -
     *          expect the actual numerical value will depend on endianness.
     *
     *  \par Example Usage
     *      \code
     *          IncrementalDigester<Algorithm:CRC32> ctx;
     *          ctx.Write (from, to);
     *          uint32_t result = ctx.Complete ();
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          IncrementalDigester<Algorithm:CRC32, string> ctx;
     *          ctx.Write (from, to);
     *          string result = ctx.Complete ();
     *      \endcode
     * 
     *  See Also:
     *      @see ComputeDigest () - this is usually the simplest way to access the digest algorithms.
     */
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    class IncrementalDigester {
    public:
        using ReturnType = RETURN_TYPE;

    public:
        /**
         */
        IncrementalDigester ()                               = default;
        IncrementalDigester (const IncrementalDigester& src) = default;

    public:
        nonvirtual IncrementalDigester& operator= (const IncrementalDigester& rhs) = default;

    public:
        /**
         * After you construct an IncrementalDigester, call Write() zero or more times, before calling Complete() to retrieve the result.
         * 
         *  \req not CompleteHasBeenCalled ();
         */
        nonvirtual void Write (const byte* from, const byte* to);
        nonvirtual void Write (span<const byte> from);
        nonvirtual void Write (const BLOB& from);
        nonvirtual void Write (const Streams::InputStream::Ptr<byte>& from);
        template <typename TRIVIALLY_COPYABLE_T>
        nonvirtual void Write (const Traversal::Iterable<TRIVIALLY_COPYABLE_T>& from)
            requires (is_trivially_copyable_v<TRIVIALLY_COPYABLE_T>);

    public:
        /**
         * When all the data has been accumulated, call Complete () once to retrieve the converted result.
         * 
         *  \req not CompleteHasBeenCalled ();
         */
        nonvirtual ReturnType Complete ();

    private:
        Algorithm::DigesterAlgorithm<ALGORITHM> fDigesterAlgorithm_;
#if qDebug
        bool fCompleted_{false};
#endif
    };

    /**
     *  \brief Digester<ALGORITHM> is a function-object way to access the digest algorithm. Its generally almost the same as calling ComputeDigest (just a little more complicated) - but works with overloading properly (which is why it exists) - when you make a separate choice of algorithm from where you call
     * 
     *  A Digest is an algorithm that takes a stream of bytes and computes a series of bits
     *  (can be thought of as a number, or string, or seqeunce of bits) which hopefully as
     *  nearly as possible (given the length of the digest) uniquely identifies the input.
     *
     *  A digest is generally of fixed length - often 4, or 16, or 20 bytes long.
     *
     *  RETURN_TYPE is typically uint32_t, uint64_t, or Result128BitType, Result128BitType etc,
     *  but could in principle be anything.
     *
     *  \note Endianness - these algorithms logically operate on bytes, so if you use RETURN_TYPE=uin32_t (or anything but byte array) -
     *          expect the actual numerical value will depend on endianness.
     *
     *  \par Example Usage
     *      \code
     *          string digestStr = Format (Digester<Algorithm::MD5>{} (s, e));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          SourceDefinition    tmp;    // some struct which defines ostream operator>>
     *          string  digestStr = Format (Digester<Algorithm::MD5>{} (Streams::iostream::SerializeItemToBLOB (tmp)));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          return Digester<Algorithm::MD5>{} (Memory::BLOB::FromRaw (sb.AsASCII ())));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // to convert to string
     *          return Format<String> (Digester<Algorithm::MD5>{} (Memory::BLOB::FromRaw (sb.AsASCII ())));
     *          // OR
     *          return Digester<Algorithm::MD5, String>{} (Memory::BLOB::FromRaw (sb.AsASCII ())));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          using namespace IO::Network;
     *          auto         digesterWithDefaultResult  = Digester<Algorithm::SuperFastHash>{};
     *          auto         digesterWithResult_uint8_t = Digester<Algorithm::SuperFastHash, uint8_t>{};
     *          Memory::BLOB value2Hash                 = DefaultSerializer<InternetAddress>{}(InternetAddress{"192.168.244.33"});
     *          auto         h1                         = digesterWithDefaultResult (value2Hash);
     *          uint8_t      h2                         = digesterWithResult_uint8_t (value2Hash);
     *          EXPECT_TRUE (h1 == 2512011991); // experimentally derived values but they shouldn't float (actually may depend on endiannesss?)
     *          EXPECT_TRUE (h2 == 215);
     *          auto                 digesterWithResult_array40 = Digester<Algorithm::SuperFastHash, std::array<byte, 40>>{};
     *          std::array<byte, 40> h3                         = digesterWithResult_array40 (value2Hash);
     *          EXPECT_TRUE (h3[0] == byte{215} and h3[1] == byte{66} and h3[39] == byte{0});
     *          if (Configuration::GetEndianness () == Configuration::Endian::eX86) {
     *              EXPECT_TRUE ((Digester<Digest::Algorithm::SuperFastHash, string>{}(value2Hash) == "0x2512011991"));
     *          }
     *      \endcode
     *
     *  @see  Hash ()
     *
     */
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    class Digester {
    public:
        using ReturnType = RETURN_TYPE;

    public:
        constexpr Digester ()          = default;
        Digester (const Digester& src) = delete;

    public:
        /**
         *  The reason we support the overload BLOB, is that this is the most convenient and univeral argument
         *  to a Digester.
         *
         *  The reason to provide an InputStream<> parameter, is that this allows operating a digest on some data
         *  without loading it all into RAM at once.
         *
         *  The reason to provide a const byte* / const byte* overload is - well - maybe pointless - since
         *  that can be quite efficiently turned into a BLOB, but its potentially a little more efficient for
         *  perhaps important cases.
         * 
         *  \note many of these overloads are redundant, but provided as a speed tweak for this situations.
         */
        nonvirtual ReturnType operator() (const Streams::InputStream::Ptr<byte>& from) const;
        nonvirtual ReturnType operator() (const byte* from, const byte* to) const;
        nonvirtual ReturnType operator() (span<const byte> from) const;
        nonvirtual ReturnType operator() (const BLOB& from) const;
        template <typename TRIVIALLY_COPYABLE_T>
        nonvirtual ReturnType operator() (const Traversal::Iterable<TRIVIALLY_COPYABLE_T>& from) const
            requires (is_trivially_copyable_v<TRIVIALLY_COPYABLE_T>);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Digester.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Digester_h_*/
