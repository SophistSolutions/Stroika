/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SOAP_Action_h_
#define _Stroika_Frameworks_UPnP_SOAP_Action_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Memory/BLOB.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

/**
 *  UPnP's control (UPnP Device Architecture 1.1, section 3): a control point asks a service to do one of its actions - an
 *  ActionRequest, POSTed to the service's controlURL as SOAP - and the service answers with the action's out arguments, an
 *  ActionResponse, or says why it could not, an ActionError. Each written (Serialize) and read (DeSerialize) here, so neither
 *  side writes or reads XML of its own.
 *
 *  \par Example Usage
 *      \code
 *          // a control point: switch a light on
 *          SOAP::ActionRequest request{.fServiceType = "urn:schemas-upnp-org:service:SwitchPower:1"sv,
 *                                      .fAction      = "SetTarget"sv,
 *                                      .fArguments   = {{"newTargetValue"sv, "1"sv}}};
 *          Transfer::Request r;
 *          r.fMethod               = HTTP::Methods::kPost;
 *          r.fAuthorityRelativeURL = controlURL.GetAuthorityRelativeResource<URI> ();
 *          r.fOverrideHeaders      = {{"SOAPACTION"sv, request.GetSOAPAction ()}};
 *          r.SetTypedBLOB ({.fData = Serialize (request), .fType = SOAP::kContentType});
 *          Transfer::Connection::Ptr c = Transfer::Connection::New ();
 *          c.SetSchemeAndAuthority (controlURL.GetSchemeAndAuthority ());
 *          Transfer::Response answer = c.Send (r); // not SendAndThrowOnFailure: a 500's body says why
 *          if (answer.GetSucceeded ()) {
 *              SOAP::ActionResponse response;
 *              DeSerialize (answer.GetData (), &response);
 *          }
 *          else {
 *              SOAP::ActionError error;
 *              DeSerialize (answer.GetData (), &error);
 *          }
 *
 *          // the light: its controlURL's handler
 *          SOAP::ActionRequest request;
 *          DeSerialize (m.rwRequest ().GetBody (), &request);
 *          m.rwResponse ().contentType = SOAP::kContentType;
 *          if (request.fAction == "GetStatus"sv) {
 *              m.rwResponse ().write (Serialize (SOAP::ActionResponse{.fServiceType = request.fServiceType,
 *                                                                     .fAction      = request.fAction,
 *                                                                     .fArguments   = {{"ResultStatus"sv, on ? "1"sv : "0"sv}}}));
 *          }
 *          else {
 *              m.rwResponse ().status = HTTP::StatusCodes::kInternalError; // how SOAP says it failed
 *              m.rwResponse ().write (Serialize (SOAP::ActionError{.fErrorCode = SOAP::ActionError::kInvalidAction, .fErrorDescription = "Invalid Action"sv}));
 *          }
 *      \endcode
 */
namespace Stroika::Frameworks::UPnP::SOAP {

    using Foundation::Characters::String;
    using Foundation::Common::KeyValuePair;
    using Foundation::Containers::Sequence;
    using Foundation::DataExchange::InternetMediaType;

    /**
     *  An action's arguments, each its name and value - in order: the order its service's description lists them in, which is
     *  the order they are sent in (UPnP Device Architecture 1.1, section 3.2.1). Values as text, as UPnP's types are written.
     */
    using Arguments = Sequence<KeyValuePair<String, String>>;

    /**
     *  What a control request, and its answer, are sent as: text/xml, UTF-8 (UPnP Device Architecture 1.1, section 3.2.1)
     */
    inline const InternetMediaType kContentType{"text/xml; charset=\"utf-8\""sv};

    /**
     *  \brief A control point's request that a service do one of its actions.
     */
    struct ActionRequest {
        /**
         *  The service's type: urn:schemas-upnp-org:service:SwitchPower:1, say
         */
        String fServiceType;
        /**
         *  The action's name: SetTarget, say
         */
        String fAction;
        /**
         *  Its in arguments
         */
        Arguments fArguments;

        /**
         *  The SOAPACTION header it is sent with: its service type, # and action, in quotes (UPnP Device Architecture 1.1,
         *  section 3.2.1)
         */
        nonvirtual String GetSOAPAction () const;

        /**
         *  The value of the argument called name; nullopt if none is
         */
        nonvirtual optional<String> LookupArgument (const String& name) const;

        bool operator== (const ActionRequest&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief A service's answer to an ActionRequest, it having done the action: the action's out arguments.
     */
    struct ActionResponse {
        /**
         *  The service's type, as in the request
         */
        String fServiceType;
        /**
         *  The action's name, as in the request
         */
        String fAction;
        /**
         *  Its out arguments
         */
        Arguments fArguments;

        /**
         *  The value of the argument called name; nullopt if none is
         */
        nonvirtual optional<String> LookupArgument (const String& name) const;

        bool operator== (const ActionResponse&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief A service's answer to an ActionRequest, it having not done the action: why not (UPnP Device Architecture 1.1,
     *         section 3.2.2). Sent as a SOAP fault, in an HTTP response whose status is 500 (Internal Server Error).
     */
    struct ActionError {
        /**
         *  The error codes UPnP Device Architecture 1.1, section 3.2.2, gives - each with its errorDescription there: 401 Invalid
         *  Action (none of that name), 402 Invalid Args (too few or too many, misnamed, or a value of the wrong type), 501 Action
         *  Failed (could not, as things are), ...
         */
        static constexpr unsigned int kInvalidAction                = 401;
        static constexpr unsigned int kInvalidArgs                  = 402;
        static constexpr unsigned int kActionFailed                 = 501;
        static constexpr unsigned int kArgumentValueInvalid         = 600;
        static constexpr unsigned int kArgumentValueOutOfRange      = 601;
        static constexpr unsigned int kOptionalActionNotImplemented = 602;
        static constexpr unsigned int kOutOfMemory                  = 603;
        static constexpr unsigned int kHumanInterventionRequired    = 604;
        static constexpr unsigned int kStringArgumentTooLong        = 605;

        /**
         *  One of those, or one the service's standard gives (606-799), or its vendor's (800-899)
         */
        unsigned int fErrorCode{kActionFailed};
        String       fErrorDescription;

        bool operator== (const ActionError&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief The SOAP message - its XML, UTF-8 - which sends it: a request, a response, or an error (a SOAP fault)
     */
    Foundation::Memory::BLOB Serialize (const ActionRequest& request);
    Foundation::Memory::BLOB Serialize (const ActionResponse& response); ///< \brief The SOAP message - its XML, UTF-8 - which sends it
    Foundation::Memory::BLOB Serialize (const ActionError& error);       ///< \brief The SOAP message - its XML, UTF-8 - which sends it

    /**
     *  \brief The SOAP message read: what Serialize writes, and any other's. Throws DataExchange::BadFormatException if it is not one
     *         - an ActionResponse read from a fault, say.
     *
     *  With the XML parser the build has, if any (qStroika_Foundation_DataExchange_XML_SupportDOM); else read as text, which is
     *  enough for the usual SOAP message, though not for one with comments or CDATA.
     */
    void DeSerialize (const Foundation::Memory::BLOB& b, ActionRequest* request);
    void DeSerialize (const Foundation::Memory::BLOB& b, ActionResponse* response); ///< \brief The SOAP message read
    void DeSerialize (const Foundation::Memory::BLOB& b, ActionError* error);       ///< \brief The SOAP message read

}

#endif /*_Stroika_Frameworks_UPnP_SOAP_Action_h_*/
