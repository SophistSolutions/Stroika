/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_h_ 1

#include "../../StroikaPreComp.h"

#include "../String.h"

/**
 *  \file
 *
 *      @todo   PROBABLY DEPRECATE - 2014-03-18. This IS a good idea as optimization (e.g. for stuff like
 *              callbacks from xerces/sax parser) - but issue is the strings if copied to another thread - you cannot
 *              replace out the rep safely if in the middle of access in another thread.
 *
 *              DOCUMENT idea (in todo section) in case I someday come up with a better way, but I think as of
 *              2014-03-18 we must deprecate this.
 *
 *              String_ExternalMemoryOwnership_StackLifetime
 *
 *
 *      @todo   Redo implementation of String_StackLifetime - using high-performance algorithm described in the documentation.
 *
 *      @todo   Make another pass over String_ExternalMemoryOwnership_StackLifetime/ReadWrite
 *              documentation, and make clearer, and document the tricky bits loosely
 *              alluded to in the appropriate place if the API is truely DOABLE.
 *
 *      @todo   Review this criticism (below). Not sure still valid:
 *
 ** SERIOUS - NOT sure what todo about stuff like c_str() - as doc says below - no obvious limitation on lifetime! I GUESS we must simply
 ** force such APIs to 'breakreferences' - as the note below says. THAT does appear to fix the problem - but at a cost - maybe too huge cost?
 *
 *     o   Seriously reconsider design / semantics of String_ExternalMemoryOwnership_StackLifetime/String_ExternalMemoryOwnership_StackLifetime_ReadWrite classes.
 *         There maybe a serious / hopeless bug having todo with threads and c_str(). Suppose in one thread we do:
 *             void    f()
 *                 {
 *                     wchar_t buf[1024] = L"fred";
 *                     String_ExternalMemoryOwnership_StackLifetime tmp (buf);
 *
 *                     // Then call some function F() with tmp - where F() passes the value (as a String) to another thread, and that other thread runs for a very long time with the string - just READING
 *                     // the string - never MODIFYING it.
 *                     //
 *                     // THEN - because of second thread owning that string - the refCount is bumped up. And suppose that other string calls s.c_str() - and is ever so briefly using the s.c_str() results - say
 *                     // in a printf() call.
 *                     //
 *                     // Now when String_ExternalMemoryOwnership_StackLifetime goes out of scope - and it notices its refCount != 0, it must somehow MORPH the underlying representation into
 *                     // something SAFE (copy all the strings). But thats tricky - ESPECIALLY in light of threads and someone else doing a c_str().
 *                     //
 *                     // I guess its not IMPOSSIBLE to fix all this. You could
 *                     //          (o)     On c_str() calls - at least for this class rep - maybe always - BREAKREFERENCES() - so the lifetime of your ptr is garuanteed friendly
 *                     //          (o)     Put in enuf flags and CriticalSection code - so on String_ExternalMemoryOwnership_StackLifetime () - we block until we can
 *                     //                  Copy all the data safely and set a flag saying to free it at the end - as a regular string - not a String_ExternalMemoryOwnership_StackLifetime::Rep (would be nice
 *                     //                  to transform the rep object but thats trickier).
 *                 }

 *
 *
 */

namespace Stroika::Foundation::Characters::Concrete {

    /*
        *  String_ExternalMemoryOwnership_StackLifetime is a subtype of String you can use to construct a String object, so long as the memory pointed to
        * in the argument has a
        *      o   Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime envelope class
        *      o   and buffer data never changes value externally to this String represenation
        *
    ///REVIEW - PRETTY SURE THIS IS WRONG!!!! - UNSAFE - READONLY SHOULD mean pointer passed in is CONST - so memory may NOT be modified in this case -- LGP 2012-03-28
    /// DOBLE CHECK NO ASSIMPTIONS BELOW - WRONG - LINE NOT ASSUMED ANYWHERE
        *  Note that the memory passed in must be READ/WRITE - and may be modified by the String_ExternalMemoryOwnership_StackLifetime ()!
        *
        *  Strings constructed with this String_ExternalMemoryOwnership_StackLifetime maybe treated like normal strings - passed anywhere, and even modified via the
        *  String APIs. However, the underlying implementation may cache the argument const wchar_t* cString for as long as the lifetime of the envelope class,
        *  and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
        *
        *  This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
        *  call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
        *  association will be broken when the String_ExternalMemoryOwnership_StackLifetime goes out of scope.
        *
        *  This means its EVEN safe to use in cases where the String object might get assigned to long-lived String variables (the internal data will be
        *  copied in that case).
        *
        *  For example
        *
        *      extern String saved;
        *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
        *
        *
        *      void f (const wchar_t* cs)
        *          {
        *              F(L"FRED";);
        *              F(String (L"FRED"));
        *              F(String_ExternalMemoryOwnership_StackLifetime (cs));
        *          }
        *
        *  These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime()
        *  based memory maybe more efficient than the previous two, because the string pointed to be 'cs' never needs to be copied (now malloc/copy needed).
        *
        *      <<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
        *          real optimized implementation >>
        *
        *
        *  TODO::::COOPY SOME OF THIS - CLEANUP THESE DCOS
        *
    *       This class looks and acts like a regular String object, but with the performance advantage
    *   that it requires no (significant) free-store allocation. It allocates a 'rep' object from
    *   block-allocation, and re-uses its argument pointers for actual string character storage.
    *
    *       Also important, it avoids having todo any copying of the original string.
    *
    *       It can avoid these costs under MANY - but not ALL circumstances. This underlying String
    *   object may remain highly efficient (working off stack memory) only so long as its original
    *   String_ExternalMemoryOwnership_StackLifetime exsts. Once that goes out of scope
    *   the underlying StringRep must be 'morphed' effectively into a regular string-rep (if there
    *   remain any references.
    *
    *       Also - SOME APIS (still TBD, but perhaps including c_str()) - will force that morphing
    *   /copying.
    *
    *       This can STILL be a HUGE performance benefit. Consider a shim API between Xerces and
    *   Stroika - for SAX - where lots of strings are efficiently passed by Xerces - and are forwarded
    *   to Stroika 'SAX' APIs. But consider a usage, where several of the strings are never used.
    *
    *       The shim would be copying/converting these strings, all for no purpose, since they never
    *   got used. This cost can be almost totally eliminated.
    *
    *       Plus - many - perhaps even most String API methods can be applied to these ligher cost
    *   strings in that 'SAX Callback' scenario without ever constructing high-cost String objects.
    *
    *       But - if anyone ever does allocate one of those objects - no biggie. It just gets morphed
    *   and the cost paid then.
    *
    *       THATS THE PLAN ANYHOW....
        */
    class String_ExternalMemoryOwnership_StackLifetime : public String {
    private:
        using inherited = String;

    public:
        explicit String_ExternalMemoryOwnership_StackLifetime (const wchar_t* cString);
        // DOCUMENT THESE NEW EXTRA CTORS!!! NYI
        explicit String_ExternalMemoryOwnership_StackLifetime (const wchar_t* start, const wchar_t* end);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String_ExternalMemoryOwnership_StackLifetime.inl"

#endif /*_Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_h_*/
