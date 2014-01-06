/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_inl_
#define _Stroika_Foundation_Execution_SignalHandlers_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ******************************** SignalHandlerType *****************************
             ********************************************************************************
             */
            inline  SignalHandlerType::SignalHandlerType (void (*signalHandler)(SignalIDType), Type type)
                : fType_ (type)
                , fCall_ (shared_ptr<function<void(SignalIDType)>> (new function<void(SignalIDType)> (signalHandler)))
            {
            }
            inline                SignalHandlerType::SignalHandlerType (const function<void(SignalIDType)>& signalHandler, Type type)
                : fType_ (type)
                , fCall_ (shared_ptr<function<void(SignalIDType)>> (new function<void(SignalIDType)> (signalHandler)))
            {
            }
            inline  SignalHandlerType::Type SignalHandlerType::GetType () const
            {
                return fType_;
            }
            inline  void SignalHandlerType::operator () (SignalIDType i) const
            {
                Require (fCall_.get () != nullptr);
                (*fCall_) (i);
            }
            inline  bool SignalHandlerType::operator== (const SignalHandlerType& rhs) const
            {
                return fCall_.get () == rhs.fCall_.get ();
            }
            inline  bool SignalHandlerType::operator!= (const SignalHandlerType& rhs) const
            {
                return fCall_.get () != rhs.fCall_.get ();
            }
            inline  bool SignalHandlerType::operator< (const SignalHandlerType& rhs) const
            {
                // technically not quite real... - compute address of ptr...
                return fCall_.get () < rhs.fCall_.get ();
            }


            /*
             ********************************************************************************
             ****************************** SignalHandlerRegistry ***************************
             ********************************************************************************
             */
#if 0
            inline  void    SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, const function<void(SignalIDType)>& handler)
            {
                SetSignalHandlers (signal, SignalHandlerType (handler));
            }
            inline  void    SignalHandlerRegistry::AddSignalHandler (SignalIDType signal, const function<void(SignalIDType)>& handler)
            {
                AddSignalHandler (signal, SignalHandlerType (handler));
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SignalHandlers_inl_*/
