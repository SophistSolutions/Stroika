/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

namespace {
    String ExtractReasonFromResponse_ (const Response& response)
    {
        using namespace Streams;
        //DbgTrace ("response headers={}"_f, response.GetHeaders ());
        //DbgTrace ("response body={}"_f, Streams::BinaryToText::Reader::New (response.GetData()).ReadAll());
        if (auto ctHdr = response.GetHeaders ().Lookup ("Content-Type")) {
            InternetMediaType ct{*ctHdr};
            if (InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::Wildcards::kText, ct)) {
                if (ct == InternetMediaTypes::kText_PLAIN) {
                    return Streams::BinaryToText::Reader::New (response.GetData ()).ReadLine ();
                }
                else {
                    // The sample JSON - I have in mind:
                    //      {   "error": "unsupported_grant_type",   "error_description": "Invalid grant_type: "
                    // Try to just grab the first few 'words', and hope it makes some sense in an error message
                    Character                   buf[512];
                    InputStream::Ptr<Character> textStream = BinaryToText::Reader::New (response.GetData ());
                    String                      roughText  = textStream.ReadAll (span{buf});
                    //static const RegularExpression kBunchaWords_{"([\\w]+)"};
                    static const RegularExpression kBunchaWordsOrTags_{"(<?[\\w]+/?>?)"};
                    auto                           words = roughText.FindEachMatch (kBunchaWordsOrTags_);
                    if (not textStream.IsAtEOF ()) {
                        if (words.size () > 2) {
                            words.Remove (words.size () - 1); // last word cut-off, so don't include
                        }
                    }
                    StringBuilder    sb;
                    Iterable<String> wordText = words.Map<Iterable<String>> ([] (const auto& i) { return i.GetFullMatch (); });
                    for (const String& w : wordText) {
                        // remove html tags
                        if (not w.StartsWith ("<"sv)) {
                            sb << w << " ";
                        }
                    }
                    sb = sb.str ().RTrim ();
                    if (not textStream.IsAtEOF ()) {
                        sb += "..."sv;
                    }
                    return sb;
                }
            }
        }
        return String{};
    }

}

/*
 ********************************************************************************
 ***************************** Transfer::Exception ******************************
 ********************************************************************************
 */
Exception::Exception (const Response& response)
    : HTTP::Exception{response.GetStatus (), ExtractReasonFromResponse_ (response)}
    , fResponse_{response}
{
}

Response Exception::GetResponse () const
{
    return fResponse_;
}
