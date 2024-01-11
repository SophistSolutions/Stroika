/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Streams/TextToByteReader.h"

#include "Providers/IProvider.h"
#include "Schema.h"

#include "DOM.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::DOM;
using namespace Stroika::Foundation::DataExchange::XML::DOM::Document;

namespace {
    struct XPathExpRep_ : XPath::Expression ::IRep {
        String                     fExpression_;
        XPath::Expression::Options fOptions_;
        XPathExpRep_ (const String& e, const XPath::Expression::Options& o)
            : fExpression_{e}
            , fOptions_{o}
        {
        }
        virtual String GetExpression () const override
        {
            return fExpression_;
        }
        virtual XPath::Expression::Options GetOptions () const override
        {
            return fOptions_;
        }
    };
}
XPath::Expression::Expression (const String& e, const Options& o)
    : fRep_{make_shared<XPathExpRep_> (e, o)}
{
}

/*
 ********************************************************************************
 ********************************* Element::IRep ***********************************
 ********************************************************************************
 */
Element::Ptr Element::IRep::GetChildElementByID (const String& id) const
{
    for (Node::Ptr c : this->GetChildren ()) {
        static const String kID_ = "id"s;
        if (Element::Ptr e = Element::Ptr{c}; e != nullptr) {
            if (e.GetAttribute (kID_) == id) {
                return e;
            }
        }
    }
    return Element::Ptr{nullptr};
}

/*
 ********************************************************************************
 ********************************* Node::Ptr ************************************
 ********************************************************************************
 */
String Node::Ptr::ToString () const
{
    Streams::MemoryStream::Ptr m = Streams::MemoryStream::New<byte> ();
    GetRep ()->Write (m, DOM::SerializationOptions{});
    return Streams::TextReader::New (m).ReadAll ();
}

auto Element::Ptr::GetChild (const NameWithNamespace& eltName) const -> Ptr
{
    for (Element::Ptr c : GetChildElements ()) {
        if (c.GetName () == eltName) {
            return c;
        }
    }
    return Ptr{nullptr};
}

/*
 ********************************************************************************
 *********************************** Document ***********************************
 ********************************************************************************
 */
void Document::Ptr::Validate (const Schema::Ptr& schema) const
{
    RequireNotNull (schema);
    RequireNotNull (fRep_);
    fRep_->Validate (schema);
}

/*
 ********************************************************************************
 *************************** DOM::Document::New *********************************
 ********************************************************************************
 */
Document::Ptr Document::New (const Providers::IDOMProvider& p, const NameWithNamespace& documentElementName)
{
    return Document::Ptr{p.DocumentFactory (documentElementName)};
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in)
{
    return Document::Ptr{p.DocumentFactory (in, nullptr)};
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReadingr)
{
    return Document::Ptr{p.DocumentFactory (in, schemaToValidateAgainstWhileReadingr)};
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const String& in)
{
    return New (p, Streams::TextToByteReader::New (in), nullptr);
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading)
{
    return New (p, Streams::TextToByteReader::New (in), schemaToValidateAgainstWhileReading);
}

#if qStroika_Foundation_DataExchange_XML_SupportDOM
Document::Ptr Document::New (const NameWithNamespace& documentElementName)
{
    static const XML::Providers::IDOMProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return New (*kDefaultProvider_, documentElementName);
}

Document::Ptr Document::New (const Streams::InputStream::Ptr<byte>& in)
{
    return New (in, nullptr);
}

Document::Ptr Document::New (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schema)
{
    static const XML::Providers::IDOMProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return New (*kDefaultProvider_, in, schema);
}

Document::Ptr Document::New (const String& in)
{
    return New (Streams::TextToByteReader::New (in), nullptr);
}

Document::Ptr Document::New (const String& in, const Schema::Ptr& schema)
{
    return New (Streams::TextToByteReader::New (in), schema);
}
#endif
