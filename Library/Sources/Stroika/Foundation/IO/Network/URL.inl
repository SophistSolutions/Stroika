/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_URL_inl_
#define _Stroika_Foundation_IO_Network_URL_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Characters/CodePage.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                //  class   URLQueryDecoder
                inline  const map<String, String>& URLQueryDecoder::GetMap () const
                {
                    return fMap;
                }
                inline  String     URLQueryDecoder::operator () (const String& idx) const
                {
                    map<String, String>::const_iterator    i   =   fMap.find (idx);
                    if (i == fMap.end ()) {
                        return String ();
                    }
                    else {
                        return (*i).second;
                    }
                }
                inline  String     URLQueryDecoder::operator () (const string& idx) const
                {
                    return operator () (Characters::UTF8StringToWide (idx));
                }
                inline  bool        URLQueryDecoder::HasField (const String& idx) const
                {
                    map<String, String>::const_iterator    i   =   fMap.find (idx);
                    return (i != fMap.end ());
                }
                inline  bool        URLQueryDecoder::HasField (const string& idx) const
                {
                    return HasField (Characters::UTF8StringToWide (idx));
                }
                inline  void    URLQueryDecoder::AddField (const String& idx, const String& value)
                {
                    fMap.insert (map<String, String>::value_type (idx, value));
                }
                inline  void    URLQueryDecoder::RemoveFieldIfAny (const string& idx)
                {
                    RemoveFieldIfAny (Characters::UTF8StringToWide (idx));
                }


                //  class   URL
                inline  int     URL::GetEffectivePortNumber () const
                {
                    if (fPort == kDefaultPort) {
                        return GetDefaultPortForProtocol (fProtocol);
                    }
                    else {
                        return fPort;
                    }
                }
                inline  int     URL::GetPortNumber () const
                {
                    return fPort;
                }
                inline  void    URL::SetPortNumber (int portNum)
                {
                    fPort = portNum;
                }
                inline  bool    operator!= (const URL& lhs, const URL& rhs)
                {
                    return not (lhs == rhs);
                }



                //  class   LabeledURL
                inline  LabeledURL::LabeledURL (const String& url, const String& label)
                    : fURL (url)
                    , fLabel (label)
                {
                }

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_URL_inl_*/
