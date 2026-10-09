/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SOAP_Action_h_
#define _Stroika_Frameworks_UPnP_SOAP_Action_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>
#include <stdexcept>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Stroika/Frameworks/UPnP/DataTypes.h"
#include "Stroika/Frameworks/WebServer/Message.h"
#include "Stroika/Frameworks/WebService/SOAP.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

/**
 *  UPnP's control (UPnP Device Architecture 1.1, section 3): a control point asks a service to do one of its actions - an
 *  ActionRequest, POSTed to the service's controlURL (Invoke) - and the service answers (HandleAction) with the action's out
 *  arguments, an ActionResponse, or says why it could not, an ActionError.
 *
 *  It is SOAP 1.1's RPC (WebService::SOAP) with UPnP's conventions on top: a method's namespace is its service's type, the
 *  SOAPACTION header is that type, # and the action, and an error is a UPnPError in a fault's detail.
 *
 *  \par Example Usage
 *      \code
 *          // a control point: switch a light on, then ask it its status
 *          const URI controlURL = location.Combine (service.fControlURL);
 *          SOAP::Invoke (controlURL, {.fServiceType = kSwitchPower, .fAction = "SetTarget"sv, .fArguments = {{"newTargetValue"sv, true}}});
 *          optional<bool> on = SOAP::Invoke (controlURL, {.fServiceType = kSwitchPower, .fAction = "GetStatus"sv}).LookupArgument<bool> ("ResultStatus"sv);
 *
 *          // the light: its controlURL's route
 *          Route{HTTP::MethodsRegEx::kPost, "SwitchPower/control"_RegEx, [&] (Message& m) {
 *              SOAP::HandleAction (m, kSwitchPower, [&] (const SOAP::ActionRequest& request) -> SOAP::Arguments {
 *                  if (request.fAction == "GetStatus"sv) {
 *                      return {{"ResultStatus"sv, on}};
 *                  }
 *                  Execution::Throw (SOAP::ActionException{SOAP::ActionError::kInvalidAction});
 *              });
 *          }}
 *      \endcode
 */
namespace Stroika::Frameworks::UPnP::SOAP {

    using Foundation::Characters::String;
    using Foundation::IO::Network::URI;

    /**
     *  rief One of an action's arguments: its name, and its value as text, as UPnP writes it (DataTypes::ToText) - made from a
     *         value of any of UPnP's data types, as its C++ type: {"ResultStatus"sv, true}
     */
    struct Argument : Foundation::Common::KeyValuePair<String, String> {
        /**
         *  A value of any of UPnP's data types - written as UPnP writes it; or, given text, that text: a string's value, say
         */
        template <DataTypes::IValue T>
        Argument (const String& name, const T& value);
        Argument (const String& name, const String& text); ///< rief A value of any of UPnP's data types - written as UPnP writes it; or, given text, that text
        Argument (const Foundation::Common::KeyValuePair<String, String>& nameAndText); ///< rief As SOAP has it: its name, and its text
    };

    /**
     *  An action's arguments - in order: the order its service's description lists them in, which is the order they are sent in
     *  (UPnP Device Architecture 1.1, section 3.2.1).
     */
    using Arguments = Foundation::Containers::Sequence<Argument>;

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
         *  The value of the argument called name, read as a T (DataTypes::FromText) - its text, by default; nullopt if no argument
         *  has that name, or its text is not a T's
         */
        template <DataTypes::IValue T = String>
        nonvirtual optional<T> LookupArgument (const String& name) const;

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
         *  The value of the argument called name, read as a T (DataTypes::FromText) - its text, by default; nullopt if no argument
         *  has that name, or its text is not a T's
         */
        template <DataTypes::IValue T = String>
        nonvirtual optional<T> LookupArgument (const String& name) const;

        bool operator== (const ActionResponse&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief A service's answer to an ActionRequest, it having not done the action: why not (UPnP Device Architecture 1.1,
     *         section 3.2.2) - a UPnPError, in a SOAP fault's detail.
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

        /**
         *  \brief As SOAP sends it: a Client fault, UPnPError, its detail a UPnPError element
         */
        nonvirtual WebService::SOAP::Fault AsFault () const;

        /**
         *  \brief The UPnPError a SOAP fault's detail holds; nullopt if it holds none
         */
        static optional<ActionError> FromFault (const WebService::SOAP::Fault& fault);

        bool operator== (const ActionError&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief An ActionError, thrown: Invoke throws the one a service answers with, and a HandleAction doAction throws one to
     *         answer with it.
     */
    class ActionException : public Foundation::Execution::Exception<runtime_error> {
    public:
        /**
         *  The error; or, given just one of ActionError's codes, the errorDescription UPnP gives it
         */
        ActionException (const ActionError& error);
        ActionException (unsigned int errorCode); ///< \brief The error; or, given just one of ActionError's codes, the errorDescription UPnP gives it

    public:
        /**
         */
        nonvirtual ActionError GetError () const;

    private:
        ActionError fError_;
    };

    /**
     *  \brief The SOAP message - its XML, UTF-8 - which sends it: a request, a response, or an error (a SOAP fault)
     */
    Foundation::Memory::BLOB Serialize (const ActionRequest& request);
    Foundation::Memory::BLOB Serialize (const ActionResponse& response); ///< \brief The SOAP message - its XML, UTF-8 - which sends it
    Foundation::Memory::BLOB Serialize (const ActionError& error);       ///< \brief The SOAP message - its XML, UTF-8 - which sends it

    /**
     *  \brief The SOAP message read: what Serialize writes, and any other's. Throws DataExchange::BadFormatException if it is not one
     *         - an ActionResponse read from a fault, say; and throws in a build with no XML parser.
     */
    void DeSerialize (const Foundation::Memory::BLOB& b, ActionRequest* request);
    void DeSerialize (const Foundation::Memory::BLOB& b, ActionResponse* response); ///< \brief The SOAP message read
    void DeSerialize (const Foundation::Memory::BLOB& b, ActionError* error);       ///< \brief The SOAP message read

    /**
     *  \brief Ask the service at controlURL to do request's action - and return its answer. Throws ActionException if the service
     *         answers with a UPnPError; otherwise as WebService::SOAP::Invoke.
     */
    ActionResponse Invoke (const URI& controlURL, const ActionRequest& request);

    /**
     *  \brief Answer a control request, at the controlURL's route of the service of type serviceType: doAction's out arguments,
     *         or the ActionError it throws (as ActionException) - and 401 (Invalid Action) for another service type's request.
     *         Otherwise as WebService::SOAP::HandleRequest: 415 if not text/xml, and a Client fault if not a SOAP call.
     */
    void HandleAction (WebServer::Message& m, const String& serviceType, const function<Arguments (const ActionRequest&)>& doAction);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Action.inl"

#endif /*_Stroika_Frameworks_UPnP_SOAP_Action_h_*/
