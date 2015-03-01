
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Trace_inl_
#define _Stroika_Foundation_Debug_Trace_inl_    1

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /*
             ********************************************************************************
             ******************************* Trace::Emitter *********************************
             ********************************************************************************
             */
            template    <typename   CHARTYPE>
            inline  void    Emitter::EmitUnadornedText (const CHARTYPE* p)
            {
                DoEmit_ (p);
            }


            /*
             ********************************************************************************
             ******************************* TraceContextBumper *****************************
             ********************************************************************************
             */
            inline  TraceContextBumper::TraceContextBumper ()
#if     qDefaultTracingOn
                : fDoEndMarker (false)
                //,fSavedContextName_ ()
#endif
            {
#if     qDefaultTracingOn
                IncCount ();
#endif
            }

#if     !qDefaultTracingOn
            inline  TraceContextBumper::TraceContextBumper (const SDKChar* contextName)
            {
            }
            inline  TraceContextBumper::~TraceContextBumper ()
            {
            }
#endif


            /*
             ********************************************************************************
             ******************************** TimingTrace ***********************************
             ********************************************************************************
             */
            inline TimingTrace::TimingTrace (Time::DurationSecondsType warnIfLongerThan)
                : fStartedAt_ (Time::GetTickCount ())
                , fMinTime2Report_ (warnIfLongerThan)
            {
            }
            inline   TimingTrace::TimingTrace (const SDKChar* msg, Time::DurationSecondsType warnIfLongerThan)
                : fStartedAt_ (Time::GetTickCount ())
                , fMinTime2Report_ (warnIfLongerThan)
            {
            }
            inline    TimingTrace::~TimingTrace ()
            {
                Time::DurationSecondsType timeTaken = Time::GetTickCount () - fStartedAt_;
                if (timeTaken >= fMinTime2Report_) {
                    DbgTrace ("(timeTaken=%f seconds)", timeTaken);
                }
            }




            Execution::ModuleDependency MakeModuleDependency_Trace ();

            namespace Private_ {
                struct  TraceModuleData_ {
                    TraceModuleData_ ();
                    ~TraceModuleData_ ();

                    Emitter                     fEmitter;
                    Execution::ModuleDependency fStringDependency;
#if     qTraceToFile
                    SDKString                   fTraceFileName;
#endif
                };
            }


        }
    }
}


namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Debug::Private_::TraceModuleData_> _Stroika_Foundation_Debug_Trace_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}


inline  Stroika::Foundation::Debug::Emitter&    Stroika::Foundation::Debug::Emitter::Get ()
{
    return Execution::ModuleInitializer<Private_::TraceModuleData_>::Actual ().fEmitter;
}
#endif  /*_Stroika_Foundation_Debug_Trace_inl_*/
