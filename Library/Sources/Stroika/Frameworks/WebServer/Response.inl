/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Response_inl_
#define _Stroika_Frameworks_WebServer_Response_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Containers/Common.h"

namespace   Stroika {
    namespace   Frameworks  {
        namespace   WebServer {


            //  class   Response
            inline  Response::State Response::GetState () const
            {
                return fState_;
            }
            inline  IO::Network::HTTP::Status   Response::GetStatus () const
            {
                return fStatus_;
            }
            inline  InternetMediaType   Response::GetContentType () const
            {
                return fContentType_;
            }
            inline  Characters::CodePage    Response::GetCodePage () const
            {
                return fCodePage_;
            }
            inline  void    Response::write (const wchar_t* e)
            {
                RequireNotNull (e);
                write (e, e + ::wcslen (e));
            }
            inline  void    Response::write (const wstring& e)
            {
                if (not e.empty ()) {
                    write (Containers::Start (e), Containers::End (e));
                }
            }
            inline  void    Response::writeln (const wchar_t* e)
            {
                RequireNotNull (e);
                const   wchar_t kEOL[]  =   L"\r\n";
                write (e, e + ::wcslen (e));
                write (std::begin (kEOL), std::end (kEOL));
            }
            inline  void    Response::writeln (const wstring& e)
            {
                const   wchar_t kEOL[]  =   L"\r\n";
                if (not e.empty ()) {
                    write (Containers::Start (e), Containers::End (e));
                }
                write (std::begin (kEOL), std::end (kEOL));
            }
            inline  void    Response::clear ()
            {
                fBytes_.clear ();
            }
            inline  bool    Response::empty () const
            {
                return fBytes_.empty ();
            }
            inline  const vector<Byte>& Response::GetBytes () const
            {
                return fBytes_;
            }
            inline  Response::ContentSizePolicy Response::GetContentSizePolicy () const
            {
                return fContentSizePolicy_;
            }

        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_Response_inl_*/
