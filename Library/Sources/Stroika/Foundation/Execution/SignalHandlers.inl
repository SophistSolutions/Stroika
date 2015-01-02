/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_inl_
#define _Stroika_Foundation_Execution_SignalHandlers_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/ModuleInit.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ******************************** SignalHandler *********************************
             ********************************************************************************
             */
            inline  SignalHandler::SignalHandler (void (*signalHandler)(SignalID), Type type)
                : fType_ (type)
                , fCall_ (signalHandler)
            {
            }
            inline                SignalHandler::SignalHandler (const Function<void(SignalID)>& signalHandler, Type type)
                : fType_ (type)
                , fCall_ (signalHandler)
            {
            }
            inline  SignalHandler::Type SignalHandler::GetType () const
            {
                return fType_;
            }
            inline  void SignalHandler::operator () (SignalID i) const
            {
                fCall_ (i);
            }
            inline  bool SignalHandler::operator== (const SignalHandler& rhs) const
            {
                return fType_ == rhs.fType_ and fCall_ == rhs.fCall_;
            }
            inline  bool SignalHandler::operator!= (const SignalHandler& rhs) const
            {
                return fType_ != rhs.fType_ or fCall_ != rhs.fCall_;
            }
            inline  bool SignalHandler::operator< (const SignalHandler& rhs) const
            {
                if (fType_ == rhs.fType_) {
                    // technically not quite real... - compute address of ptr...
                    return fCall_ < rhs.fCall_;
                }
                else {
                    return fType_ < rhs.fType_;
                }
            }


            namespace Private_ {
                struct SignalHandlers_ModuleData_ {
                    mutex                               fMutex;
                    shared_ptr<SignalHandlerRegistry>   fTheRegistry;
                };
            }


        }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Execution::Private_::SignalHandlers_ModuleData_>    _Stroika_Foundation_ExecutionSignalHandlers_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Execution_SignalHandlers_inl_*/
