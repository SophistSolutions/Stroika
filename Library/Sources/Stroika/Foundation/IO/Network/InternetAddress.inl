/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_inl_
#define _Stroika_Foundation_IO_Network_InternetAddress_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /*
                 ********************************************************************************
                 *********************** IO::Network::InternetAddress ***************************
                 ********************************************************************************
                 */
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress ()
                    : fAddressFamily_ (AddressFamily::UNKNOWN)
                    , fV4_ {} {
                }
#if     qPlatform_POSIX
                inline
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (const in_addr_t& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ { i } {
                }
                inline  InternetAddress::InternetAddress (const in_addr_t& i, ByteOrder byteOrder)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ { i } {
                    if (byteOrder == ByteOrder::Host)
                    {
                        fV4_.s_addr = htonl (fV4_.s_addr);
                    }
                }
#endif
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (const in_addr& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                }
                inline InternetAddress::InternetAddress (const in_addr& i, ByteOrder byteOrder)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                    if (byteOrder == ByteOrder::Host) {
                        fV4_.s_addr = htonl (fV4_.s_addr);
                    }
                }
                inline
#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                constexpr
#endif
                InternetAddress::InternetAddress (const in6_addr& i)
                    : fAddressFamily_ (AddressFamily::V6)
                    , fV6_ (i)
                {
                }
                inline  bool    InternetAddress::empty () const
                {
                    return fAddressFamily_ == AddressFamily::UNKNOWN;
                }
                inline  void    InternetAddress::clear ()
                {
                    fAddressFamily_ = AddressFamily::UNKNOWN;
                }
                inline  InternetAddress::AddressFamily  InternetAddress::GetAddressFamily () const
                {
                    return fAddressFamily_;
                }
                template    <>
                String InternetAddress::As<String> () const;
                template    <typename T>
                T   InternetAddress::As () const
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }
#if     qPlatform_POSIX
                template    <>
                inline  in_addr_t   InternetAddress::As<in_addr_t> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return fV4_.s_addr;
                }
#endif
                template    <>
                inline  in_addr InternetAddress::As<in_addr> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return fV4_;
                }
                template    <>
                inline  in6_addr    InternetAddress::As<in6_addr> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V6);
                    return fV6_;
                }
                template    <>
                inline  in_addr InternetAddress::As<in_addr> (ByteOrder byteOrder) const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    if (byteOrder == ByteOrder::Network) {
                        return fV4_;
                    }
                    else {
                        in_addr tmp =   fV4_;
                        tmp.s_addr = ntohl (tmp.s_addr);
                        return tmp;
                    }
                }
                template    <typename T>
                T   InternetAddress::As (ByteOrder byteOrder) const
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }
                inline  bool    InternetAddress::operator< (const InternetAddress& rhs) const
                {
                    return Compare (rhs) < 0;
                }
                inline  bool    InternetAddress::operator<= (const InternetAddress& rhs) const
                {
                    return Compare (rhs) <= 0;
                }
                inline  bool    InternetAddress::operator> (const InternetAddress& rhs) const
                {
                    return Compare (rhs) > 0;
                }
                inline  bool    InternetAddress::operator>= (const InternetAddress& rhs) const
                {
                    return Compare (rhs) >= 0;
                }
                inline  bool    InternetAddress::operator== (const InternetAddress& rhs) const
                {
                    return Compare (rhs) == 0;
                }
                inline  bool    InternetAddress::operator!= (const InternetAddress& rhs) const
                {
                    return Compare (rhs) != 0;
                }


#if     !qCompilerAndStdLib_constexpr_union_variants_Buggy
                namespace V4 {
                    constexpr   InternetAddress kAddrAny    { in_addr { } };
#if     qPlatform_POSIX
                    constexpr   InternetAddress kLocalhost  { in_addr { INADDR_LOOPBACK } };
#elif   qPlatform_Windows
                    constexpr   InternetAddress kLocalhost  { in_addr { { { 0x1, 0x0, 0x0, 0x7f } } } };
#endif

                }
                namespace V6 {
                    constexpr   InternetAddress kAddrAny    { in6_addr { } };
                    constexpr   InternetAddress kLocalhost  { in6_addr IN6ADDR_LOOPBACK_INIT };
                }
#endif


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_InternetAddress_inl_*/
