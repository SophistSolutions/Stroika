/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_ServiceDescription_h_
#define _Stroika_Frameworks_UPnP_ServiceDescription_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::UPnP {

    using Foundation::Characters::String;
    using Foundation::Containers::Sequence;

    /**
     *  \brief A service's description - its SCPD (UPnP Device Architecture 1.1, section 2.5): its actions, each with its
     *         arguments, and its state variables - what the SCPDURL in its device's description serves.
     *
     *  Its standard - SwitchPower:1, say - gives a standard service's; a device serves that, and a control point reads it to
     *  learn what it can ask (SOAP::ActionRequest).
     *
     *  \par Example Usage
     *      \code
     *          // SwitchPower:1's (https://upnp.org/specs/ha/UPnP-ha-SwitchPower-v1-Service.pdf), served at its SCPDURL
     *          using SD = ServiceDescription;
     *          const SD kSwitchPower{
     *              .fActions = {SD::Action{.fName = "SetTarget"sv, .fArguments = {{.fName = "newTargetValue"sv, .fRelatedStateVariable = "Target"sv}}},
     *                           SD::Action{.fName      = "GetStatus"sv,
     *                                      .fArguments = {{.fName = "ResultStatus"sv, .fDirection = SD::Argument::Direction::eOut,
     *                                                      .fRelatedStateVariable = "Status"sv}}}},
     *              .fStateVariables = {SD::StateVariable{.fName = "Target"sv, .fDataType = "boolean"sv, .fDefaultValue = "0"sv, .fSendEvents = false},
     *                                  SD::StateVariable{.fName = "Status"sv, .fDataType = "boolean"sv, .fDefaultValue = "0"sv}}};
     *          ...
     *          response.write (Serialize (kSwitchPower));
     *      \endcode
     */
    struct ServiceDescription {
        /**
         *  One of an action's arguments: an in argument (sent in its request), or an out one (sent back in its response).
         */
        struct Argument {
            String fName;
            enum class Direction {
                eIn,
                eOut,
            };
            Direction fDirection{Direction::eIn};
            /**
             *  An out argument only: it is the action's return value - at most one, which is its first out argument.
             */
            bool fRetval{false};
            /**
             *  The state variable whose type it has - its name.
             */
            String fRelatedStateVariable;

            bool operator== (const Argument&) const = default;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         */
        struct Action {
            String fName;
            /**
             *  In order - the order a request's and a response's arguments go in: its in arguments, then its out ones.
             */
            Sequence<Argument> fArguments;

            bool operator== (const Action&) const = default;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         *  The values a number state variable may take: minimum to maximum, in steps of fStep (if given).
         */
        struct AllowedValueRange {
            String           fMinimum;
            String           fMaximum;
            optional<String> fStep;

            bool operator== (const AllowedValueRange&) const = default;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         *  A piece of the service's state: what an action's argument has the type of, and - if evented - what GENA tells each
         *  subscriber the changes of.
         */
        struct StateVariable {
            String fName;
            /**
             *  Its type, as UPnP names it: boolean, ui4, string, ... (UPnP Device Architecture 1.1, section 2.5)
             */
            String           fDataType;
            optional<String> fDefaultValue;
            /**
             *  Evented: each change is told to each subscriber (GENA).
             */
            bool fSendEvents{true};
            /**
             *  A string's values, if only these: empty, any.
             */
            Sequence<String> fAllowedValues;
            /**
             *  A number's values, if only these.
             */
            optional<AllowedValueRange> fAllowedValueRange;

            bool operator== (const StateVariable&) const = default;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        Sequence<Action>        fActions;
        Sequence<StateVariable> fStateVariables;

        bool operator== (const ServiceDescription&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief The SCPD's XML: what its SCPDURL serves - as UPnP's XML (text/xml), UTF-8
     */
    Foundation::Memory::BLOB Serialize (const ServiceDescription& sd);

    /**
     *  \brief An SCPD read - what Serialize writes, and any other's: what it does not know - a vendor's own elements, say - is
     *         skipped. Throws if it is not XML, or the build has no XML parser (qStroika_Foundation_DataExchange_XML_SupportParsing).
     */
    void DeSerialize (const Foundation::Memory::BLOB& b, ServiceDescription* sd);

}

#endif /*_Stroika_Frameworks_UPnP_ServiceDescription_h_*/
