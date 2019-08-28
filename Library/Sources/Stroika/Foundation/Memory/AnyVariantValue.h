/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_AnyVariantValue_h_
#define _Stroika_Foundation_Memory_AnyVariantValue_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <optional>
#include <type_traits>

#include "../Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Document (in some overall design doc) - the choice of As<> versus
 *              explicit conversion operator. I THINK the rationale is that:
 *                  static_cast<T> (n) is about the same as n.As<T> () - except that overload
 *                  semantics work better for static_cast<T> (n) - THINK ABOUT THIS - verus
 *                  .As<T> () is somewhat shorter, and more clear, and works with compilers that
 *                  don't have explicit operators.
 *
 *              But I think its too easy to accidentally get conversions with the conversion/cast operator.
 *              That maybe due to bugs/incomplete implementation of explicit conversion operators? But we can always
 *              add explicit conversion operators later, and gradually phase-out As<> usage.
 *
 *      @todo   consider using stuff like remove_references and remove_const to 'normalize' the T type
 *              used here. Might make it a little easier to use.
 *
 *      @todo   Consider making operator T () less restrictive - and work if conertable. But then not
 *              sure how to get the right type (can I do decltype (GetType())??? - in other words make a T out of
 *              the type_info? I think not...
 *
 *
 *  Awknolwedgements:
 *      I came up with this idea before looking at the BOOST any code, but when I thought of it
 *      I first checked out what boost had that was similar. Before implementing, I looked at
 *      http://www.boost.org/doc/libs/1_54_0/doc/html/any.html
 */

namespace Stroika::Foundation::Memory {

    /**
     * \brief
     *
     *  Simple variant-value object, but unlike @VariantValue, this can contain ANY type. That both
     *  limits its usefulness (cannot use it for stuff like streaming to XML because not enough is
     *  known about the constituent types), but expands cases of usefulness - like for example - to
     *  implement a @BlockingQueue<> of Command objects, where you have a bunch of different kinds of
     *  commands (objects). The party enquing messages needs to know the types, and the party dequing them
     *  does, but nobody else.
     *
     *  Example Rationale:
     *      Often in more complex applications, one wants to have a producer - consumer system, with type safety
     *      and some kind of generic message routing. For example - consider the idea of the SOAP Envelope / Body
     *      distinction.
     *
     *      This also can easily be constructed with the Stroika BlockingQueue<>.
     *
     *      If you want to have type safety on construction of the shared objects to be safely know about on
     *      two ends (the producer and the consumer) but want to have a layer of software in between that handles
     *      the routing (as with SOAP) - having the ability to genericify an object, and then undo that (could also use
     *      serializaiton) - can be handy.
     *
     *      Note - this CAN be accomplished using templates as the in-between routing layer. But that approach has a
     *      number of defects including:
     *          o   Very complex message routing libraries - all in tempaltes - are hard to use because of bad
     *              compiler diagnostics when there is some mis-match.
     *
     *          o   Code bloat from complex/duplicative middle layers (routing layers).
     *
     *          o   Lack of modularity/hiding - all the template code needs to be in header files to be expanded.
     *
     *      Using AnyVariantValue can be a simple remedy to this. At the message routing (between producer/consumer) layer
     *      you wrap your objects in AnyVariantValue, and then (type safely) unpack them on the other end.
     *
     *  \note   Design Note - Equals and less/greater not supported
     *          Supporting this generically would require a virtual method, and as such, would
     *          be tricky todo without requiring the underlying 'T' type to have an equals method. For now
     *          we can just avoid that.
     *
     *  @see VariantValue
     */
    class [[deprecated ("This class is essentially the same as std::any; not worth the extra class - deprecate as of Stroika v2.1d1")]] AnyVariantValue
    {
    public:
        /**
         *  Note that its is important that the AnyVariantValue (T) CTOR is explicit, because otherwise its too easy to
         *  accidentally assign the wrong type and get surprising results.
         */
        AnyVariantValue ()                            = default;
        AnyVariantValue (const AnyVariantValue& from) = default;
        AnyVariantValue (AnyVariantValue && from);
        AnyVariantValue& operator= (const AnyVariantValue& rhs) = default;
        template <typename T>
        explicit AnyVariantValue (T val);

    public:
        /**
         *  Note - its legal to call this even if empty: in that case it returns typeid(void).
         */
        nonvirtual const type_info& GetType () const;

    public:
        /**
         */
        nonvirtual bool empty () const;

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         *  @see IfAs<>
         */
        template <typename RETURNTYPE>
        nonvirtual RETURNTYPE As () const;

    private:
        struct IRep_;

    private:
        shared_ptr<IRep_> fVal_;

    private:
        template <typename T>
        struct TIRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "AnyVariantValue.inl"

#endif /*_Stroika_Foundation_Memory_AnyVariantValue_h_*/
