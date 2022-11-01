/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_UniformResourceIdentification_
#define _Stroika_Foundation_IO_Network_URL_UniformResourceIdentification_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/Optional.h"

namespace Stroika::Foundation::IO::Network::UniformResourceIdentification {

    /*
     ********************************************************************************
     ******************* UniformResourceIdentification::SchemeType ******************
     ********************************************************************************
     */
    inline SchemeType::SchemeType (const wchar_t* s)
        : inherited{s}
    {
    }
    inline SchemeType::SchemeType (const wstring_view& s)
        : inherited{s}
    {
    }
    inline SchemeType::SchemeType (const String& s)
        : inherited{s}
    {
    }
    inline strong_ordering SchemeType::operator<=> (const SchemeType& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool SchemeType::operator== (const SchemeType& rhs) const
    {
        return TWC_ (*this, rhs) == 0;
    }

    /*
     ********************************************************************************
     ************************************* Host *************************************
     ********************************************************************************
     */
    inline Host::Host (const String& registeredName)
        : fEncodedName_{EncodeAsRawURL_ (registeredName)}
        , fRegisteredName_{registeredName}
    {
        Require (not registeredName.empty ());
    }
    inline Host::Host (const InternetAddress& addr)
        : fEncodedName_{EncodeAsRawURL_ (addr)}
        , fInternetAddress_{addr}
    {
    }
    inline Host Host::Parse (const String& rawURLHostnameText)
    {
        Require (not rawURLHostnameText.empty ());
        pair<optional<String>, optional<InternetAddress>> tmp{ParseRaw_ (rawURLHostnameText)};
        Host                                              h;
        h.fEncodedName_     = rawURLHostnameText;
        h.fRegisteredName_  = tmp.first;
        h.fInternetAddress_ = tmp.second;
        return h;
    }
    inline optional<String> Host::AsRegisteredName () const
    {
        return fRegisteredName_;
    }
    inline optional<InternetAddress> Host::AsInternetAddress () const
    {
        return fInternetAddress_;
    }
    inline String Host::AsDecoded () const
    {
        if (fRegisteredName_) {
            return *fRegisteredName_;
        }
        Assert (fInternetAddress_);
        return fInternetAddress_->As<String> ();
    }
    template <>
    inline String Host::AsEncoded () const
    {
        return fEncodedName_;
    }
    template <>
    inline string Host::AsEncoded () const
    {
        return fEncodedName_.AsASCII<string> ();
    }
    inline strong_ordering Host::operator<=> (const Host& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool Host::operator== (const Host& rhs) const
    {
        return TWC_ (*this, rhs) == 0;
    }
    inline strong_ordering Host::TWC_ (const Host& lhs, const Host& rhs)
    {
        if (strong_ordering cmp = compare_three_way{}(lhs.AsInternetAddress (), rhs.AsInternetAddress ()); cmp != strong_ordering::equal) { // Use BWA from Common/Compare.h
            return cmp;
        }
        return Common::OptionalThreeWayComparer<String, String::ThreeWayComparer>{String::ThreeWayComparer{Characters::CompareOptions::eCaseInsensitive}}(lhs.AsRegisteredName (), rhs.AsRegisteredName ());
    }

    /*
     ********************************************************************************
     ********************************* UserInfo *************************************
     ********************************************************************************
     */
    inline UserInfo::UserInfo (const String& decodedUserInfo)
        : fEncodedUserInfo_{EncodeAsRawURL_ (decodedUserInfo)}
        , fUserInfo_{decodedUserInfo}
    {
        Require (not decodedUserInfo.empty ());
    }
    inline UserInfo UserInfo::Parse (const String& rawURLUserInfo)
    {
        Require (not rawURLUserInfo.empty ());
        UserInfo h;
        h.fEncodedUserInfo_ = rawURLUserInfo;
        h.fUserInfo_        = ParseRaw_ (rawURLUserInfo);
        return h;
    }
    inline String UserInfo::AsDecoded () const
    {
        return fUserInfo_;
    }
    template <>
    inline String UserInfo::AsEncoded () const
    {
        return fEncodedUserInfo_;
    }
    template <>
    inline string UserInfo::AsEncoded () const
    {
        return fEncodedUserInfo_.AsASCII<string> ();
    }
    inline strong_ordering UserInfo::operator<=> (const UserInfo& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool UserInfo::operator== (const UserInfo& rhs) const
    {
        return TWC_ (*this, rhs) == 0;
    }
    inline strong_ordering UserInfo::TWC_ (const UserInfo& lhs, const UserInfo& rhs)
    {
        return lhs.AsDecoded () <=> rhs.AsDecoded ();
    }

    /*
     ********************************************************************************
     ********************************* Authority ************************************
     ********************************************************************************
     */
    inline Authority::Authority (const optional<Host>& h, const optional<PortType>& port, const optional<UserInfo>& userInfo)
        : fHost_{h}
        , fPort_{port}
        , fUserInfo_{userInfo}
    {
    }
    inline optional<Host> Authority::GetHost () const
    {
        return fHost_;
    }
    inline void Authority::SetHost (const optional<Host>& host)
    {
        fHost_ = host;
    }
    inline optional<PortType> Authority::GetPort () const
    {
        return fPort_;
    }
    inline void Authority::SetPort (const optional<PortType>& port)
    {
        fPort_ = port;
    }
    inline optional<UserInfo> Authority::GetUserInfo () const
    {
        return fUserInfo_;
    }
    inline void Authority::SetUserInfo (const optional<UserInfo>& userInfo)
    {
        fUserInfo_ = userInfo;
    }
    inline strong_ordering Authority::operator<=> (const Authority& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool Authority::operator== (const Authority& rhs) const
    {
        return TWC_ (*this, rhs) == 0;
    }
    inline strong_ordering Authority::TWC_ (const Authority& lhs, const Authority& rhs)
    {
        if (auto cmp = compare_three_way{}(lhs.GetHost (), rhs.GetHost ()); cmp != strong_ordering::equal) {            // Use BWA from Common/Compare.h
            return cmp;
        }
        if (auto cmp = compare_three_way{}(lhs.GetUserInfo (), rhs.GetUserInfo ()); cmp != strong_ordering::equal) {    // Use BWA from Common/Compare.h
            return cmp;
        }
        return compare_three_way{}(lhs.GetPort (), rhs.GetPort ());                                                     // Use BWA from Common/Compare.h
    }

    /*
     ********************************************************************************
     ************************************ Query *************************************
     ********************************************************************************
     */
    inline const Containers::Mapping<String, String>& Query::GetMap () const
    {
        return fMap_;
    }
    inline String Query::operator() (const String& idx) const
    {
        return Memory::NullCoalesce (fMap_.Lookup (idx));
    }
    inline String Query::operator() (const string& idx) const
    {
        return operator() (Characters::UTF8StringToWide (idx));
    }
    inline bool Query::HasField (const String& idx) const
    {
        return fMap_.ContainsKey (idx);
    }
    inline bool Query::HasField (const string& idx) const
    {
        return HasField (Characters::UTF8StringToWide (idx));
    }
    inline void Query::AddField (const String& idx, const String& value)
    {
        fMap_.Add (idx, value);
    }
    inline void Query::RemoveFieldIfAny (const string& idx)
    {
        RemoveFieldIfAny (Characters::UTF8StringToWide (idx));
    }
    inline strong_ordering Query::operator<=> (const Query& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool Query::operator== (const Query& rhs) const
    {
        return TWC_ (*this, rhs) == 0;
    }

}

#endif /*_Stroika_Foundation_IO_Network_URL_UniformResourceIdentification_*/
