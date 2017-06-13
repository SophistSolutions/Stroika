/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_inl_
#define _Stroika_Foundation_IO_Network_URL_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Characters/CodePage.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 ************************************ URLQuery **********************************
                 ********************************************************************************
                 */
                inline const Containers::Mapping<String, String>& URLQuery::GetMap () const
                {
                    return fMap_;
                }
                inline String URLQuery::operator() (const String& idx) const
                {
                    return fMap_.Lookup (idx).Value ();
                }
                inline String URLQuery::operator() (const string& idx) const
                {
                    return operator() (Characters::UTF8StringToWide (idx));
                }
                inline bool URLQuery::HasField (const String& idx) const
                {
                    return fMap_.ContainsKey (idx);
                }
                inline bool URLQuery::HasField (const string& idx) const
                {
                    return HasField (Characters::UTF8StringToWide (idx));
                }
                inline void URLQuery::AddField (const String& idx, const String& value)
                {
                    fMap_.Add (idx, value);
                }
                inline void URLQuery::RemoveFieldIfAny (const string& idx)
                {
                    RemoveFieldIfAny (Characters::UTF8StringToWide (idx));
                }

                /*
                 ********************************************************************************
                 *************************************** URL ************************************
                 ********************************************************************************
                 */
                inline URL::PortType URL::GetPortValue (PortType defaultValue) const
                {
                    if (fPort_) {
                        return *fPort_;
                    }
                    else {
                        return GetDefaultPortForScheme (GetSchemeValue ()).Value (defaultValue);
                    }
                }
                inline Memory::Optional<URL::PortType> URL::GetPortNumber () const
                {
                    return fPort_;
                }
                inline void URL::SetPortNumber (const Optional<PortType>& portNum)
                {
                    fPort_ = portNum;
                }
                inline Optional<URL::SchemeType> URL::GetScheme () const
                {
                    return fScheme_;
                }
                inline String URL::GetHost () const
                {
                    return fHost_;
                }
                inline void URL::SetHost (const String& host)
                {
                    fHost_ = host;
                }
                inline void URL::SetScheme (const Optional<SchemeType>& scheme)
                {
                    if (scheme) {
                        SetScheme (*scheme); //validates value
                    }
                    else {
                        fScheme_.clear ();
                    }
                }
                inline String URL::GetHostRelativePath () const
                {
                    return fRelPath_;
                }
                inline void URL::SetHostRelativePath (const String& hostRelativePath)
                {
                    fRelPath_ = hostRelativePath;
                }
                inline URLQuery URL::GetQuery () const
                {
                    return URLQuery (fQuery_);
                }
                inline void URL::SetQuery (const URLQuery& query)
                {
                    fQuery_ = query.ComputeQueryString ();
                }
                inline String URL::GetQueryString () const
                {
                    Ensure (not fQuery_.StartsWith (L"?"));
                    return fQuery_;
                }
                inline void URL::SetQueryString (const String& queryString)
                {
                    Require (not queryString.StartsWith (L"?")); // should validate that it parses
                    fQuery_ = queryString;
                }
                inline String URL::GetFragment () const
                {
                    return fFragment_;
                }
                inline void URL::SetFragment (const String& frag)
                {
                    fFragment_ = frag;
                }

                /*
                 ********************************************************************************
                 ****************************** URL operators ***********************************
                 ********************************************************************************
                 */
                inline bool operator< (const URL& lhs, const URL& rhs)
                {
                    return lhs.Compare (rhs) < 0;
                }
                inline bool operator<= (const URL& lhs, const URL& rhs)
                {
                    return lhs.Compare (rhs) <= 0;
                }
                inline bool operator== (const URL& lhs, const URL& rhs)
                {
                    return lhs.Equals (rhs);
                }
                inline bool operator!= (const URL& lhs, const URL& rhs)
                {
                    return not lhs.Equals (rhs);
                }
                inline bool operator> (const URL& lhs, const URL& rhs)
                {
                    return lhs.Compare (rhs) > 0;
                }
                inline bool operator>= (const URL& lhs, const URL& rhs)
                {
                    return lhs.Compare (rhs) >= 0;
                }

                /*
                 ********************************************************************************
                 ******************************** LabeledURL ************************************
                 ********************************************************************************
                 */
                inline LabeledURL::LabeledURL (const URL& url, const String& label)
                    : fURL (url)
                    , fLabel (label)
                {
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_URL_inl_*/
