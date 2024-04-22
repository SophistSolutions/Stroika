/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
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

/*
 ********************************************************************************
 ********************** XPath::XPathExpressionNotSupported **********************
 ********************************************************************************
 */
XPath::XPathExpressionNotSupported::XPathExpressionNotSupported ()
    : Execution::RuntimeErrorException<> ("XPath implementation doesn't support that XML expression"_k)
{
}

/*
 ********************************************************************************
 ************************** XPath::Expression ***********************************
 ********************************************************************************
 */
namespace {
    struct XPathExpRep_ : XPath::Expression ::IRep {
        String                     fExpression_;
        XPath::Expression::Options fOptions_;
        XPathExpRep_ (String&& e, const XPath::Expression::Options& o)
            : fExpression_{move (e)}
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
XPath::Expression::Expression (String&& e, const Options& o)
    : fRep_{make_shared<XPathExpRep_> (move (e), o)}
{
}

String XPath::Expression::ToString () const
{
    AssertNotNull (fRep_);
    StringBuilder sb;
    sb << "{"sv;
    sb << "expression: "sv << fRep_->GetExpression ();
    // @todo add options
    sb << "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 ****************************** Element::IRep ***********************************
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
    if (PeekRep () == nullptr) {
        return "nullptr"sv;
    }
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
 *************************** DOM::Document::Ptr *********************************
 ********************************************************************************
 */
void Document::Ptr::Validate (const Schema::Ptr& schema) const
{
    RequireNotNull (schema);
    RequireNotNull (fRep_);
    fRep_->Validate (schema);
}

String Document::Ptr::ToString () const
{
    if (fRep_ == nullptr) {
        return "nullptr"sv;
    }
    Streams::MemoryStream::Ptr m = Streams::MemoryStream::New<byte> ();
    fRep_->Write (m, DOM::SerializationOptions{});
    return Streams::TextReader::New (m).ReadAll ();
}

/*
 ********************************************************************************
 *************************** DOM::Document::New *********************************
 ********************************************************************************
 */
Document::Ptr Document::New (const Providers::IDOMProvider& p)
{
    return Document::Ptr{p.DocumentFactory (nullptr, nullptr)};
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const NameWithNamespace& documentElementName)
{
    Document::Ptr doc{New (p)};
    doc.ReplaceRootElement (documentElementName);
    return doc;
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in)
{
    return Document::Ptr{p.DocumentFactory (in, nullptr)};
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading)
{
    return Document::Ptr{p.DocumentFactory (in, schemaToValidateAgainstWhileReading)};
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const String& in)
{
    return New (p, Streams::TextToByteReader::New (in), nullptr);
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading)
{
    return New (p, Streams::TextToByteReader::New (in), schemaToValidateAgainstWhileReading);
}

Document::Ptr Document::New (const Providers::IDOMProvider& p, const Ptr& clone)
{
    // provider-specific impl COULD possibly be done more efficiently...
    Streams::MemoryStream::Ptr m = Streams::MemoryStream::New<byte> ();
    clone.Write (m);
    return New (p, m);
}

#if qStroika_Foundation_DataExchange_XML_SupportDOM
Document::Ptr Document::New ()
{
    static const XML::Providers::IDOMProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return New (*kDefaultProvider_);
}

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

Document::Ptr Document::New (const Ptr& clone)
{
    static const XML::Providers::IDOMProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return New (*kDefaultProvider_, clone);
}
#endif
