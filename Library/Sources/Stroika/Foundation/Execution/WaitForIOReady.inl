/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_inl_
#define _Stroika_Foundation_Execution_WaitForIOReady_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ************************** Execution::WaitForIOReady ***************************
             ********************************************************************************
             */
            inline  WaitForIOReady::WaitForIOReady (const Set<FileDescriptorType>& fds)
                : fFDs_ ( fds )
            {
            }
            inline  WaitForIOReady::WaitForIOReady (const Set<Socket>& s)
            {
                AddAll (s);
            }
            inline  void    WaitForIOReady::Add (FileDescriptorType fd)
            {
                fFDs_->Add (fd);
            }
            inline  void    WaitForIOReady::Add (Socket s)
            {
                Add (cvt_ (s.GetNativeSocket ()));
            }
            inline  void    WaitForIOReady::AddAll (const Set<FileDescriptorType>& fds)
            {
                fFDs_->AddAll (fds);
            }
            inline  void    WaitForIOReady::AddAll (const Set<Socket>& s)
            {
                Set<FileDescriptorType> fds;
                for (Socket si : s) {
                    fds.Add (cvt_ (si.GetNativeSocket ()));
                }
                fFDs_->AddAll (fds);
            }
            inline  void    WaitForIOReady::Remove (FileDescriptorType fd)
            {
                fFDs_->Remove (fd);
            }
            inline  void    WaitForIOReady::Remove (Socket s)
            {
                Remove (cvt_ (s.GetNativeSocket ()));
            }
            inline  void    WaitForIOReady::RemoveAll (const Set<FileDescriptorType>& fds)
            {
                fFDs_->RemoveAll (fds);
            }
            inline  void    WaitForIOReady::RemoveAll (const Set<Socket>& s)
            {
                Set<FileDescriptorType> fds;
                for (Socket si : s) {
                    fds.Add (cvt_ (si.GetNativeSocket ()));
                }
                fFDs_->RemoveAll (fds);
            }
            inline  auto WaitForIOReady::GetDescriptors () const -> Set<FileDescriptorType> {
                return fFDs_;
            }
            inline  void        WaitForIOReady::SetDescriptors (const Set<FileDescriptorType>& fds)
            {
                fFDs_ = fds;
            }
            inline  void        WaitForIOReady::SetDescriptors (const Set<Socket>& s)
            {
                Set<FileDescriptorType> fds;
                for (Socket si : s) {
                    fds.Add (cvt_ (si.GetNativeSocket ()));
                }
                SetDescriptors (fds);
            }
            inline  void        WaitForIOReady::clear ()
            {
                fFDs_->clear ();
            }
            inline  auto     WaitForIOReady::Wait (Time::DurationSecondsType waitFor) -> Set<FileDescriptorType> {
                return WaitUntil (waitFor + Time::GetTickCount ());
            }
            inline  WaitForIOReady::FileDescriptorType  WaitForIOReady::cvt_ (Socket::PlatformNativeHandle pnh)
            {
                return reinterpret_cast<FileDescriptorType> (pnh);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitForIOReady_inl_*/
