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
                inline  InternetAddress::InternetAddress ()
                    : fAddressFamily_ (AddressFamily::UNKNOWN)
                {
                }
#if     qPlatform_POSIX
                inline  InternetAddress::InternetAddress (const in_addr_t& i)
                    : fAddressFamily_ (AddressFamily::V4)
                {
                    fV4_.s_addr = i;
                }
#endif
                inline  InternetAddress::InternetAddress (const in_addr& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                {
                }
                inline  InternetAddress::InternetAddress (const in6_addr& i)
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


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_InternetAddress_inl_*/
