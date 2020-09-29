/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Hash_h_
#define _Stroika_Foundation_Cryptography_Digest_Hash_h_ 1

#include "../../StroikaPreComp.h"

#include <cstdint>

#include "../../Characters/String.h"
#include "../../Memory/BLOB.h"

#include "Digester.h"

#include "Algorithm/SuperFastHash.h" // for default algorithm

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *  \brief use std::hash<T> to digest a type t. AKA  stdhash_Digester
     * 
     *  This is not generally useful, as the default hash<> function is really not generally very good as a cryptographic digest,
     *  but it can be used as such (and for some implementations it maybe quite good or fast).
     * 
     *  The more pressing reason to want to use it for a digest, is that many people support std::hash<T> for their types T, and
     *  this allows using those hash<> specializations.
     */
    template <typename T>
    struct SystemHashDigester {
        using ReturnType = size_t;

        ReturnType operator() (const Streams::InputStream<std::byte>::Ptr& from) const;
        ReturnType operator() (const std::byte* from, const std::byte* to) const;
        ReturnType operator() (const BLOB& from) const;
    };

    /**
     *  The Hash<> template takes an optional parameter of the digester. This simply provides a default
     *  which allows the Hash<> template to be used with the same parameters as std::hash<>, except provides a an avenue to
     *  revise/enhance hashing to use a differt digest algorithm.
     */
    using DefaultHashDigester = Digest::Digester<Digest::Algorithm::SuperFastHash>;

    /**
     *  \brief Simple wrapper on (cryptographic) digest algorithms with fewer knobs, and easier construcion- mimicing std::hash<T>
     *
     *  A Hash is very much like a Digest - it takes a series of bytes and produces a
     *  series of bits which as closely as practical uniquely bijectively maps between
     *  the series of input bytes and the series of output bits.
     *
     *  The main difference between a Digest and a Hash, is more a focus on how its used. A Hash
     *  is typically required to be more quick, and often works on a variety of input types (int, string
     *  etc), and maps to often smaller sequences of bits (say 32-bit number).
     *
     *  A hash makes NO EFFORT to rem the results (though frequently the caller with % the result before use).
     *
     *  So this class uses the Digest mechanism to allow users to easily map different types to
     *  a sequence of 'bytes' in normalized form, and then allows them to be digested, and then the digest
     *  mapped to a (typically) small number (32-bit integer for example).
     *
     *  This function applies any (argument) Hash function (DIGESTER given data type, and
     *  returns the argument hash value.
     *
     *  Hash algorithms work on BLOBs, and generate return (often longish) integers, often encoded
     *  as strings and such.
     *
     *  This Adpater takes care of the general part of mapping the inputs and outputs to/from
     *  common forms, and then makes generic the actual hash computing algorithm.
     *
     *  \note   Endianness - the digest algorithms logically returns an array of bytes, so in a typical use case
     *          HASH_RETURN_TYPE is a numeric type like uin32_t, then the hash numerical value will depend on
     *          integer endianness of the machine.
     *
     *          Use the digest directly if you need a portable, externalizable digest value.
     *
     *  Mimic the behavior of std::hash<> - except hopefully pick a better default algorithm than gcc did.
     *  (https://news.ycombinator.com/item?id=13745383)
     * 
     *  Generally replaces the same types 'T' as defined https://en.cppreference.com/w/cpp/utility/hash specialized,
     *  but just in case, the default algorithm / implementation just defaults to hash<T>{} ();
     * 
     *  This template is NOT defined for all T, and must be explicitly specialized for many user types, because we dont
     *  know in general how to compute the digest of an arbitrary C++ type.
     * 
     *  This class is 'pre-specialized' for:
     *      o   all builtin numeric types, int, char, unsigned int, long etc...
     *      o   many other Stroika types, but see docs for that type to see if there is a specialization
     *
     *  To use with types for which hash<> is already defined, you can use DIGESTER=SystemHashDigester (but then this
     *  class provides little additional value over direct use of std::hash)
     *
     *  If you use the overload with 'salt', if the SALT is not a BLOB, it will be turned into a BLOB with the same
     *  normalizing function used on data2Hash. The resulting BLOB salt will be combined with the serialized data2Hash
     *  producing a different output hash value. This can be used for cryptographic salt (as with passwords) or
     *  with rehashing, for example.
     *
     *  For now, this works with TYPE_TO_COMPUTE_HASH_OF:
     *      o   is_arithmetic (e.g. int, float, uint32_t, etc)
     *      o   const char*
     *      o   std::string
     *      o   String (or anything promotable to string)
     *      o   Memory::BLOB (just passed throgh, not adpated)
     * ((PRELIMINARY - WROKING ON DEFINING THIS))
     *
     *  Other types should generate compile-time error.
     *
     *  Supported values for HASH_RETURN_TYPE, depend on the DIGESTER::ReturnType. This can be any type
     *  cast convertible into HASH_RETURN_TYPE (typically an unsigned int), or std::string, or Characters::String.
     *
     *  \par Example Usage
     *      \code
     *          using   USE_DIGESTER_     =   Digester<Algorithm::Jenkins>;
     *          VerifyTestResult (Hash<int, USE_DIGESTER_>{} (1) == 10338022);
     *          VerifyTestResult (Hash<string, USE_DIGESTER_>{} ("1") == 2154528969);
     *          VerifyTestResult (Hash<String, USE_DIGESTER_>{} (L"1") == 2154528969);
     *      \endcode
     */
    template <typename T, typename DIGESTER = DefaultHashDigester, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
    struct Hash {
        /**
         *  Some algorithms respect a 'seed' parameter if provided.
         */
        constexpr Hash () = default;
        constexpr Hash (const T& seed);

        /**
         */
        HASH_RETURN_TYPE operator() (const T& t) const;

        optional<HASH_RETURN_TYPE> fSeed;
    };

    /**
     *  NOT intended to be used directly, except to simplify boilerplate for implementation of Hash<> specializations.
     *  
     *  First 3 template arguments identical to Hash<> but the CVT argument converts from T to something that can be
     *  passed to the Digest function.
     */
    template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE, typename CONVERT_T_2_DIGEST_ARG>
    struct HashImplHelper {
        constexpr HashImplHelper () = default;
        constexpr HashImplHelper (const T& seed);

        /**
         */
        HASH_RETURN_TYPE operator() (const T& t) const;

        optional<HASH_RETURN_TYPE> fSeed;
    };

    /**
     *  \brief combine two hash values to produce a new hash value (in a hopefully very random/uncorrelated way)
     * 
     *  inspired by https://en.cppreference.com/w/cpp/utility/hash - return h1 ^ (h2 << 1); // or use boost::hash_combine
     */
    template <typename RESULT_TYPE>
    RESULT_TYPE HashValueCombine (RESULT_TYPE lhs, RESULT_TYPE rhs);

#if 0
    /**
     * 
     * 
     * &&&& CLEANUP DOCS BELOW AND MAKE CORRECT FOR NEW DESIGN AND INCORPORATE WHEN CORRECT/FINALIZED/FIGURED OUT
     * 
     * 
     *  @note   This concept is similar to, and interoperable with the std::hash ().
     *          These Hash () functions take generic digest algorithms and types of things to hash and
     *          return generic return types. And they allow the flexability for a given set of input and
     *          return types to use different hash functions in the same program. std::hash() lacks this flexability.
     *
     *          But for a given type (a std::hash () limitation) - you can define:
     *
     *          namespace std {
     *              template <>
     *              struct hash<YOUR_TYPE> {
     *                  size_t operator ()(YOUR_TYPE value) const {
     *                      // note the restrictions on TYPE_TO_COMPUTE_HASH_OF above
     *                      return Hash<PICK_ANY_DIGESTER, YOUR_TYPE, size_t> (value);
     *                  }
     *              };
     *          }
     */
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Hash.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Hash_h_*/
