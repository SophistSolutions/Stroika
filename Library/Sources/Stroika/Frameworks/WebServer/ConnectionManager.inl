/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_ConnectionManager_inl_
#define _Stroika_Frameworks_WebServer_ConnectionManager_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Frameworks  {
        namespace   WebServer {


#if     !qCompilerAndStdLib_regex_Buggy
            /*
             ********************************************************************************
             ******************************** ConnectionManager *****************************
             ********************************************************************************
             */
            inline  Optional<String> ConnectionManager::GetServerHeader () const
            {
                return fServerHeader_;
            }
            inline  void ConnectionManager::SetServerHeader (Optional<String> server)
            {
                fServerHeader_ = server;
            }
            inline      bool ConnectionManager::GetIgnoreCORS () const
            {
                return fIgnoreSillyCORS_;
            }
            inline    void ConnectionManager::SetIgnoreCORS (bool ignoreCORS)
            {
                fIgnoreSillyCORS_ = ignoreCORS;
            }
#endif


        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_ConnectionManager_inl_*/
