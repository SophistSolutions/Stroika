/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_ServiceDescription_h_
#define _Stroika_Frameworks_UPnP_ServiceDescription_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/Options.h"
#include "Stroika/Foundation/DataExchange/XML/Schema.h"
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

        /**
         *  \brief What Serialize and DeSerialize of an SCPD are told - defaulted, so a caller says only what it wants different.
         *         As DeviceDescription::Options, for the SCPD's own schema: by default what is written is checked, and what is
         *         read is not
         */
        struct Options {
            /**
             *  \brief The XML provider that reads, and that checks what is written - by default, Stroika's default provider;
             *         nearly always left as is (@see XML::Options)
             */
            Foundation::DataExchange::XML::Options fXMLOptions;

            /**
             *  \brief Check an SCPD being read against this schema - ServiceDescriptionSchema - throwing where it does not
             *         hold; nullptr, the default, checks nothing
             *
             *  Off by default, as for a device description: devices in the field are loose, and whether to see fewer of them
             *  is the application's choice, not the library's (@see DeviceDescription::Options::fValidateReadAgainst).
             */
            Foundation::DataExchange::XML::Schema::Ptr fValidateReadAgainst{nullptr};

            /**
             *  \brief Check what Serialize writes against ServiceDescriptionSchema, throwing BadFormatException where it does
             *         not hold - true, the default
             *
             *  On by default, as for a device description (@see DeviceDescription::Options::fValidateWrite): an SCPD a device
             *  writes is its own, and one that does not hold - a state variable whose fDataType is not one of UPnP's (a
             *  vendor's own, or a misspelling), or no state variables at all, where service-1-0 requires one - is a mistake
             *  better found by its author than by its control points.
             */
            bool fValidateWrite{true};
        };

        bool operator== (const ServiceDescription&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief The SCPD's XML: what its SCPDURL serves - as UPnP's XML (text/xml), UTF-8
     *
     *  The text is written directly (Variant::XML::Writer); an XML parser only checks it, as by default it does - throwing
     *  BadFormatException where it does not hold (@see ServiceDescription::Options::fValidateWrite).
     */
    Foundation::Memory::BLOB Serialize (const ServiceDescription& sd, const ServiceDescription::Options& options = {});

    /**
     *  \brief An SCPD read - what Serialize writes, and any other's, leniently: what it does not know - a vendor's own
     *         elements, say - is skipped, and an element given twice keeps its last. Throws if it is not XML, its document
     *         element is not <scpd>, or the build has no XML parser (qStroika_Foundation_DataExchange_XML_SupportParsing).
     *         @see Variant::XML::Reader::Options::fLenient
     */
    void DeSerialize (const Foundation::Memory::BLOB& b, ServiceDescription* sd, const ServiceDescription::Options& options = {});

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    /**
     *  \brief UPnP's own SCPD schema, compiled by the provider o names - what Serialize checks what it writes against
     *         (ServiceDescription::Options::fValidateWrite), and what to give fValidateReadAgainst
     *
     *  @see DeviceDescriptionSchema, which this matches in every other way.
     *
     *  \note The schema is https://upnp.org/schemas/service-1-0.xsd as it stood 2026-10-13 - BUT WITH ONE CORRECTION: as
     *        published it restricts stateVariable's sendEvents and multicast to {"1", "0"}, where the specification it
     *        describes says sendEvents="yes"/"no" (UPnP Device Architecture 1.0: "non-evented state variables have
     *        sendEvents="no"; default is sendEvents="yes""), and every implementation writes both that way. Left as
     *        published it would refuse every real device, Stroika's own output included. The two enumerations are
     *        widened to accept yes and no as well; nothing else is touched. @see ServiceDescriptionSchema.cpp.
     */
    Foundation::DataExchange::XML::Schema::Ptr ServiceDescriptionSchema (const Foundation::DataExchange::XML::Options& o = {});
#endif

}

#endif /*_Stroika_Frameworks_UPnP_ServiceDescription_h_*/
