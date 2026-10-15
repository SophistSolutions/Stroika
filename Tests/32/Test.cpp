/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Reader/Writers(7z/CharacterDelimitedLines/INI/JSON/XML/Zip)
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <random>
#include <sstream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Common/Locale.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/Archive/Reader.h"
#include "Stroika/Foundation/DataExchange/Archive/Writer.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#if qStroika_HasComponent_zlib
#include "Stroika/Foundation/DataExchange/Archive/Zip/Reader.h"
#include "Stroika/Foundation/DataExchange/Archive/Zip/Writer.h"
#include "Stroika/Foundation/DataExchange/Compression/Zip/Reader.h"
#endif
#include "Stroika/Foundation/DataExchange/Compression/Deflate.h"
#include "Stroika/Foundation/DataExchange/Compression/GZip.h"
#include "Stroika/Foundation/DataExchange/Compression/ZStd.h"
#if qStroika_HasComponent_LZMA
#include "Stroika/Foundation/DataExchange/Archive/7z/Reader.h"
#endif
#include "Stroika/Foundation/DataExchange/JSON/Patch.h"
#include "Stroika/Foundation/DataExchange/JSON/Pointer.h"
#include "Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/CharacterDelimitedLines/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/FormURLEncoded/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/FormURLEncoded/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/INI/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/INI/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Writer.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;

using Characters::String;
using DataExchange::VariantValue;

///// @todo ADD SEPEARET MODULE TO TEST VARIANTVALUE!!!

#if qStroika_HasComponent_googletest
/*
 * Validating JSON parse results:
 *      http://json.parser.online.fr/
 */

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, Test1_7zArchive_)
    {
        Debug::TraceContextBumper ctx{"Test1_7zArchive_"};
        // od sample_zip.7z -t x1 -w32 | sed 's/ /,0x/g'
        // then strip first column manually, and leading ,
        [[maybe_unused]] static constexpr uint8_t ksample_zip_7z_[] = {
            0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c, 0x00, 0x03, 0xad, 0xd3, 0x3a, 0xb4, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xd9, 0x4d, 0x4c, 0x00, 0x2d, 0x8c, 0x02, 0x26, 0x10, 0xd9, 0x9e, 0x69, 0xe9,
            0xf2, 0xc8, 0xac, 0x29, 0x7c, 0xec, 0x89, 0x1d, 0x9e, 0x5d, 0xaf, 0x2f, 0x82, 0x5d, 0x47, 0x3b, 0x79, 0x78, 0x47, 0x44, 0xf8,
            0x55, 0xee, 0xb7, 0xc7, 0x3d, 0x8b, 0x6a, 0x2f, 0xc5, 0x53, 0x4d, 0x57, 0x4a, 0x1a, 0xb0, 0x16, 0x86, 0x3b, 0xb5, 0x33, 0xa9,
            0x9c, 0xcc, 0x0a, 0xf0, 0xc6, 0xb8, 0xe7, 0x69, 0xf8, 0x6a, 0xc7, 0x90, 0x20, 0xf1, 0xe0, 0x7c, 0x99, 0x2c, 0x89, 0xc6, 0x8c,
            0x8b, 0x97, 0x02, 0x6e, 0x46, 0xc4, 0x4c, 0x53, 0x48, 0x09, 0x0e, 0x66, 0x88, 0x0e, 0xfc, 0x76, 0x15, 0xe6, 0xbe, 0x73, 0x62,
            0x74, 0x8a, 0x82, 0x4f, 0x0e, 0xdf, 0x7c, 0xde, 0x7c, 0x4d, 0xaf, 0x15, 0x71, 0xbc, 0x98, 0x74, 0x9c, 0x85, 0x38, 0xa6, 0x69,
            0x0d, 0xe3, 0xa8, 0x98, 0xfb, 0x70, 0xf4, 0x57, 0x96, 0x6c, 0x99, 0x25, 0x44, 0x2e, 0x58, 0x66, 0x89, 0x0c, 0x94, 0x66, 0x96,
            0x74, 0x64, 0x12, 0x82, 0xc7, 0x32, 0xec, 0x30, 0xd2, 0xec, 0x4b, 0xfa, 0xc6, 0xbc, 0x86, 0x4e, 0xf3, 0x58, 0xa7, 0x62, 0xb6,
            0xbe, 0xc6, 0x80, 0x61, 0x1e, 0x38, 0xc5, 0xb2, 0xd0, 0xcb, 0x3f, 0x46, 0xff, 0xf2, 0x5a, 0xd8, 0xd2, 0xb2, 0xa8, 0xf5, 0xdc,
            0xb6, 0xea, 0xfe, 0x89, 0xfc, 0xa8, 0x98, 0x14, 0x58, 0x72, 0xf1, 0x41, 0x77, 0x8b, 0xec, 0xb8, 0x71, 0x4c, 0xf0, 0xd3, 0xed,
            0x99, 0x70, 0x04, 0xbe, 0xcf, 0x3b, 0x46, 0x22, 0x60, 0x41, 0x65, 0xf9, 0x24, 0x60, 0x82, 0x6d, 0x2f, 0xc6, 0x6e, 0x18, 0xf1,
            0x23, 0xd2, 0x3f, 0x02, 0x89, 0x9e, 0x91, 0x98, 0x7a, 0x11, 0x5f, 0x9d, 0xf9, 0x08, 0xd6, 0xfd, 0xee, 0xaa, 0x57, 0xb9, 0x80,
            0x7a, 0xde, 0x48, 0x56, 0xd8, 0x6a, 0x41, 0x04, 0x39, 0xc7, 0x27, 0xb0, 0xcc, 0xe4, 0xcc, 0x0a, 0x75, 0x64, 0x09, 0xdf, 0x04,
            0x86, 0x75, 0x36, 0x2e, 0xbe, 0x26, 0xac, 0x20, 0x20, 0x94, 0x37, 0xe1, 0xd8, 0x01, 0x7c, 0x70, 0x34, 0x6b, 0x6d, 0xdd, 0x3f,
            0xeb, 0x29, 0x41, 0x85, 0xb3, 0x5d, 0xfa, 0x17, 0xad, 0xba, 0xf1, 0x83, 0x8a, 0x59, 0x41, 0x94, 0xb4, 0xed, 0x45, 0xdd, 0x1d,
            0xd8, 0x54, 0xc4, 0xd0, 0x2a, 0xf3, 0x06, 0x34, 0xec, 0xa8, 0x82, 0x00, 0xf4, 0x6b, 0xa0, 0x67, 0xf6, 0x07, 0x39, 0xb7, 0x0a,
            0xf6, 0x4a, 0xa6, 0x13, 0x71, 0x28, 0x5c, 0x50, 0x2a, 0xe9, 0x3f, 0xf3, 0x95, 0xa2, 0xdf, 0x86, 0x76, 0x5d, 0xf2, 0xdd, 0xea,
            0x20, 0x5d, 0xed, 0x21, 0xcd, 0xb6, 0xa5, 0x7d, 0x22, 0x92, 0x76, 0x5e, 0x06, 0x50, 0x94, 0x2c, 0xf5, 0xce, 0xff, 0x09, 0x68,
            0xa0, 0xe5, 0xaf, 0x11, 0x19, 0x1b, 0x47, 0x6d, 0x35, 0x5a, 0xc6, 0x99, 0x5d, 0xbf, 0x98, 0xe9, 0x12, 0xbe, 0x0a, 0xda, 0x6d,
            0x5e, 0x32, 0x7d, 0xf2, 0x61, 0x83, 0x03, 0xaf, 0xa9, 0xe1, 0x71, 0x7b, 0x95, 0xa1, 0x34, 0xf8, 0xd6, 0xa7, 0x81, 0x5b, 0x7b,
            0xe5, 0x9b, 0xda, 0x72, 0x01, 0xea, 0x93, 0x47, 0x95, 0xe6, 0xf0, 0xd7, 0xf5, 0x03, 0xa5, 0x12, 0x81, 0xec, 0xc4, 0x73, 0x53,
            0xdf, 0xaa, 0xc0, 0x44, 0x31, 0x83, 0x22, 0x91, 0x48, 0x23, 0x67, 0x74, 0x04, 0x93, 0xb3, 0x20, 0x6d, 0x66, 0x7a, 0xd5, 0x33,
            0xac, 0x99, 0x5e, 0xa4, 0x8c, 0xf4, 0x32, 0x38, 0x6f, 0xf2, 0x3a, 0x87, 0x4e, 0x93, 0x89, 0x31, 0xc0, 0xe0, 0xdd, 0xf4, 0x27,
            0xec, 0x54, 0xf4, 0x4a, 0x91, 0x49, 0x28, 0x22, 0x71, 0x7b, 0x99, 0xd7, 0xd8, 0x55, 0xcc, 0xd3, 0xbb, 0x1f, 0x03, 0x61, 0xcd,
            0xf3, 0x25, 0xdf, 0xbe, 0x65, 0x1b, 0x01, 0xe4, 0x3d, 0x6d, 0xdf, 0x43, 0xac, 0xcf, 0xfc, 0x76, 0xb0, 0x73, 0x78, 0x61, 0x7c,
            0x4c, 0x3d, 0x91, 0xbb, 0x3e, 0x68, 0x93, 0xc5, 0x01, 0x61, 0x4b, 0xb8, 0x71, 0xd1, 0x74, 0x96, 0xf3, 0x86, 0x37, 0x5f, 0x82,
            0x5d, 0x79, 0xd0, 0xb3, 0xd4, 0xf7, 0x12, 0x78, 0x6a, 0x65, 0x7e, 0xe6, 0x2f, 0xd5, 0xb7, 0xa3, 0x2d, 0xd7, 0xb6, 0x81, 0xcc,
            0xb5, 0xd4, 0x5a, 0xf2, 0x4e, 0x84, 0xcd, 0xd4, 0x78, 0x8e, 0x4e, 0xed, 0x0e, 0x99, 0x1b, 0x14, 0xd3, 0x03, 0x6c, 0x1c, 0x88,
            0xd0, 0x5c, 0xe8, 0xbc, 0x6c, 0x8c, 0x51, 0xfb, 0x5e, 0xcb, 0xa3, 0x87, 0xad, 0x30, 0x54, 0x3d, 0xed, 0x25, 0xa4, 0xa4, 0x9f,
            0xfa, 0x4f, 0x4c, 0x83, 0x84, 0xb6, 0x73, 0xe6, 0x84, 0xa0, 0x33, 0xd0, 0x72, 0x4b, 0xaf, 0xbb, 0x25, 0xed, 0x0d, 0xa3, 0xef,
            0xdd, 0xc0, 0x0b, 0x2b, 0x64, 0x58, 0xb3, 0x80, 0x48, 0xaa, 0x95, 0x38, 0x3e, 0x25, 0x40, 0x8b, 0xd5, 0x61, 0xd0, 0xf0, 0x36,
            0xca, 0xde, 0x32, 0x55, 0xeb, 0x16, 0xc0, 0x3f, 0xb0, 0xb7, 0x9c, 0x34, 0x36, 0x75, 0x52, 0x47, 0x53, 0xb7, 0x63, 0x2d, 0x6b,
            0x5b, 0x17, 0xd5, 0xe5, 0x3b, 0xa4, 0xd0, 0xf0, 0x92, 0x3c, 0xb6, 0x48, 0x9a, 0xd1, 0x24, 0x0b, 0x5f, 0x96, 0x85, 0x9c, 0x60,
            0x1b, 0x8f, 0x1e, 0x4d, 0xa0, 0x15, 0x8f, 0x81, 0x2d, 0xcc, 0x86, 0x5e, 0xba, 0xba, 0xba, 0x8f, 0xd7, 0xf7, 0x41, 0x38, 0x38,
            0x20, 0xe5, 0xa6, 0x4e, 0x73, 0xea, 0xc5, 0x72, 0xcf, 0x5e, 0x6d, 0xae, 0x5c, 0x29, 0x6e, 0x3a, 0x4c, 0xcb, 0x90, 0x2c, 0xc4,
            0xa7, 0x93, 0xd4, 0x86, 0xe8, 0xc3, 0xb6, 0x0d, 0xc7, 0xaa, 0x40, 0x3f, 0x00, 0xc3, 0xa5, 0x47, 0xd5, 0xc7, 0xb1, 0x87, 0xf4,
            0x05, 0x3b, 0x6d, 0x89, 0x8f, 0x8b, 0x30, 0x05, 0x11, 0x56, 0x50, 0xab, 0x9a, 0x0e, 0x7e, 0xa8, 0xf9, 0x85, 0x2c, 0xb6, 0x27,
            0x26, 0xe5, 0xb4, 0xe9, 0xab, 0x96, 0x87, 0x72, 0xbd, 0x09, 0x93, 0x3d, 0x86, 0x65, 0x6c, 0x72, 0xc3, 0x87, 0xf2, 0x1b, 0xe8,
            0x17, 0x59, 0x17, 0xe5, 0xad, 0x22, 0x9d, 0x6e, 0x02, 0x7c, 0x70, 0xe9, 0xf2, 0x00, 0xc5, 0xac, 0xd7, 0x5d, 0x92, 0x8e, 0x15,
            0x90, 0xe0, 0x29, 0xb5, 0xfd, 0x4a, 0x92, 0x2f, 0xfc, 0xa7, 0xdc, 0x55, 0xff, 0x60, 0xab, 0xf3, 0x8a, 0x61, 0x98, 0x80, 0x68,
            0x7b, 0xfc, 0xb0, 0x08, 0x07, 0x12, 0xbb, 0x32, 0x3f, 0xf7, 0x8d, 0x7a, 0xeb, 0x2b, 0x3b, 0x79, 0xc8, 0xe9, 0x14, 0xe8, 0xc5,
            0x7e, 0x83, 0xf6, 0xb9, 0x95, 0xfa, 0xad, 0x3c, 0x9d, 0xf5, 0xec, 0xe8, 0xd9, 0x13, 0x8f, 0x32, 0xc2, 0xbe, 0x4c, 0x2e, 0x28,
            0x5d, 0x4f, 0x1d, 0xe2, 0xe7, 0xa0, 0xda, 0xf8, 0x96, 0xb0, 0x2f, 0x15, 0xc1, 0xc8, 0xc0, 0x51, 0xa1, 0xe1, 0xfb, 0x42, 0x05,
            0xee, 0xa5, 0x66, 0x9d, 0x38, 0x87, 0x61, 0x72, 0xbc, 0x69, 0x36, 0xa3, 0x89, 0xbc, 0x43, 0x1b, 0x14, 0x64, 0xe9, 0xad, 0x79,
            0xc8, 0x62, 0xdf, 0x3f, 0xc4, 0xf8, 0x52, 0xfc, 0x5c, 0xae, 0xa1, 0x1a, 0x95, 0xd4, 0x9f, 0xe8, 0x28, 0x6d, 0xc2, 0xbe, 0x9a,
            0xc9, 0xa1, 0x20, 0x0e, 0x67, 0x6a, 0x90, 0x53, 0x49, 0x2f, 0xfd, 0x12, 0x69, 0xf8, 0xac, 0xf5, 0x40, 0x19, 0x53, 0x1f, 0x7f,
            0x74, 0x89, 0xf5, 0x44, 0xa3, 0x36, 0x4f, 0x19, 0x01, 0x9d, 0x10, 0xaa, 0x81, 0xc2, 0x67, 0x6e, 0x50, 0xab, 0xe2, 0xfb, 0x7a,
            0x82, 0xe8, 0x0d, 0x4f, 0xd9, 0x65, 0x37, 0x88, 0x83, 0xa9, 0x83, 0xe3, 0x5c, 0x89, 0x38, 0xfe, 0xf5, 0xac, 0x5a, 0x6f, 0x22,
            0x3a, 0xcf, 0x6c, 0x6d, 0x48, 0xd2, 0x1b, 0xf8, 0x18, 0x2c, 0xf0, 0xbb, 0xc3, 0xcb, 0x8b, 0x71, 0xca, 0xbb, 0xa5, 0xdc, 0x6e,
            0x14, 0x45, 0x75, 0x1b, 0x19, 0x60, 0xdc, 0x5b, 0xc7, 0x1c, 0x96, 0xba, 0x80, 0x63, 0x6a, 0x55, 0x6d, 0x07, 0xe2, 0xf0, 0x7a,
            0x2c, 0x6b, 0xa8, 0xc2, 0x7f, 0xcc, 0xdd, 0x26, 0xb4, 0x20, 0x79, 0x83, 0x74, 0x55, 0x90, 0xbe, 0x8f, 0x9a, 0x4f, 0xd4, 0x9f,
            0x65, 0xd4, 0x20, 0x34, 0x1f, 0x5d, 0x2b, 0xc5, 0x18, 0x69, 0xa8, 0xbc, 0x69, 0xbe, 0xa5, 0xb0, 0xde, 0x27, 0xcf, 0x1f, 0x99,
            0x75, 0x6c, 0x66, 0xd3, 0x84, 0x89, 0x27, 0x79, 0xe7, 0x22, 0x56, 0xe8, 0x4f, 0x04, 0x2f, 0x4c, 0xf7, 0xde, 0x43, 0x1b, 0x75,
            0xe0, 0xbb, 0x86, 0x1f, 0xa0, 0x01, 0xc1, 0x2c, 0x9b, 0x4d, 0xf6, 0x92, 0xdb, 0x96, 0x1e, 0x9c, 0x04, 0x32, 0x62, 0xd6, 0xb2,
            0x47, 0xdd, 0xbb, 0x19, 0xde, 0x78, 0x72, 0x00, 0x62, 0x3f, 0xa4, 0x27, 0xf8, 0x78, 0x2b, 0x58, 0x63, 0xd2, 0x93, 0x1c, 0x1d,
            0xd1, 0x98, 0x89, 0x4a, 0x69, 0x43, 0x5a, 0x5e, 0x05, 0x93, 0x75, 0x6f, 0x2c, 0x70, 0xb7, 0x05, 0x04, 0xf5, 0xb9, 0x3b, 0xf9,
            0xab, 0x8c, 0x3d, 0xc6, 0xbe, 0x06, 0x54, 0xe9, 0x06, 0xb1, 0x5f, 0xdd, 0x31, 0xec, 0xd8, 0x67, 0x6f, 0x38, 0xf5, 0x84, 0xf5,
            0x75, 0xf3, 0xa1, 0x58, 0xfa, 0x22, 0x6b, 0x4c, 0xf4, 0xc3, 0xe5, 0xea, 0x25, 0xf6, 0x9f, 0x4b, 0x82, 0x62, 0x50, 0x27, 0x02,
            0x8a, 0xf4, 0x00, 0xc4, 0xbc, 0x23, 0x5c, 0x0f, 0x03, 0xed, 0xa1, 0x37, 0x94, 0xcd, 0xcd, 0x33, 0x7a, 0xf8, 0x93, 0x81, 0x21,
            0xac, 0x53, 0x35, 0xba, 0x27, 0x0e, 0x11, 0xb6, 0x9d, 0xba, 0x0e, 0x8b, 0x14, 0x6a, 0x71, 0xfc, 0x22, 0x20, 0x5a, 0xe4, 0x3e,
            0xbc, 0xf3, 0xd5, 0x60, 0xa2, 0x52, 0xbc, 0x4d, 0xab, 0x06, 0x32, 0x32, 0xb4, 0xd7, 0x41, 0xfa, 0x4f, 0x7d, 0x05, 0xb5, 0x78,
            0xa8, 0x3d, 0xaf, 0x9b, 0xef, 0x5e, 0x02, 0xb5, 0x56, 0x20, 0xe0, 0x08, 0xc0, 0x33, 0x49, 0xa8, 0x0d, 0xc0, 0x41, 0x2f, 0x1c,
            0x7a, 0xa8, 0x21, 0xbb, 0x3b, 0x76, 0x8b, 0x85, 0x8f, 0x3d, 0x2c, 0x9c, 0x18, 0x84, 0xcd, 0x63, 0x4f, 0x7f, 0xde, 0x31, 0xb1,
            0xe6, 0x50, 0xe0, 0xdb, 0xa9, 0xed, 0x89, 0xd4, 0xee, 0xa6, 0x3f, 0xd6, 0x08, 0x29, 0x45, 0xf7, 0x10, 0xbd, 0x4c, 0x04, 0x22,
            0xcc, 0x1d, 0x68, 0x54, 0x98, 0xf2, 0x9d, 0x7d, 0x03, 0xdc, 0xff, 0x80, 0xd3, 0x93, 0x48, 0xbe, 0x72, 0x97, 0x4f, 0x60, 0xa8,
            0xe4, 0x11, 0x33, 0xfb, 0xf8, 0x15, 0x21, 0x1e, 0xb0, 0x22, 0x7c, 0x20, 0xd2, 0x6e, 0x01, 0xaf, 0x90, 0xd9, 0x47, 0xa9, 0xb5,
            0xce, 0xb1, 0xdf, 0xd8, 0xd2, 0xaf, 0x62, 0x80, 0x22, 0x58, 0x9a, 0x39, 0x6c, 0xbb, 0x21, 0x56, 0xf1, 0x1b, 0x1c, 0xba, 0x13,
            0x8e, 0x8d, 0xea, 0xc2, 0x5b, 0x69, 0x66, 0xe7, 0x95, 0x75, 0x57, 0x1e, 0xf4, 0x31, 0x97, 0xd5, 0x29, 0x19, 0x5c, 0xf5, 0x98,
            0x09, 0x86, 0x40, 0xbf, 0xdd, 0xb6, 0x4d, 0xde, 0x98, 0xc1, 0xa3, 0x92, 0x92, 0xc0, 0x5d, 0xd5, 0x30, 0xc9, 0xab, 0x80, 0x09,
            0x7f, 0x14, 0x05, 0x9d, 0x33, 0x11, 0x53, 0xfc, 0xba, 0x9d, 0xbe, 0x1d, 0x33, 0xda, 0xa3, 0x98, 0x6b, 0x74, 0x95, 0x88, 0xba,
            0x80, 0x57, 0xc0, 0x07, 0xd9, 0x08, 0x3f, 0x3a, 0x39, 0x5f, 0xb8, 0xe9, 0x61, 0x9a, 0xdd, 0x59, 0xb2, 0x8e, 0xa1, 0xeb, 0x4b,
            0x7c, 0xd5, 0x0e, 0x07, 0xed, 0x24, 0x41, 0x34, 0x03, 0x23, 0x75, 0x8e, 0x7e, 0xb7, 0xdb, 0x95, 0xd2, 0xd7, 0x54, 0x56, 0x34,
            0xc9, 0x5d, 0xf8, 0xdf, 0xc1, 0x94, 0xaa, 0x59, 0xcc, 0x60, 0xfa, 0xe8, 0x08, 0x2a, 0x1a, 0xdc, 0x03, 0xab, 0xf7, 0xf3, 0x1b,
            0xfa, 0x3a, 0xf8, 0x8e, 0x02, 0x8a, 0x2e, 0x2b, 0x50, 0x4b, 0x7d, 0x92, 0x98, 0x81, 0x35, 0x75, 0xba, 0xa3, 0xd5, 0xe9, 0x70,
            0x27, 0xce, 0xc2, 0xe9, 0xe8, 0x96, 0xff, 0x94, 0x7c, 0x53, 0xe0, 0x38, 0x0b, 0xcc, 0x8d, 0xfe, 0xce, 0x3a, 0x94, 0x82, 0xc2,
            0xca, 0x78, 0xa7, 0x3e, 0x62, 0x32, 0x9b, 0x61, 0xb0, 0x90, 0x1e, 0x09, 0xa0, 0x75, 0x04, 0x43, 0x3a, 0x70, 0x65, 0x04, 0x0c,
            0x9f, 0x22, 0xac, 0x0a, 0x7c, 0xe1, 0x62, 0x39, 0x13, 0xab, 0xe4, 0xfc, 0x1d, 0x1b, 0xcf, 0x8f, 0x10, 0x31, 0x00, 0x75, 0xfd,
            0x6a, 0x60, 0x8d, 0x67, 0x9d, 0x58, 0x31, 0x1a, 0x74, 0x37, 0x98, 0x19, 0x46, 0x47, 0xb9, 0xa8, 0x8f, 0xbd, 0x39, 0x0e, 0xbd,
            0xda, 0x86, 0xbc, 0x97, 0xc4, 0x68, 0x9c, 0xca, 0x82, 0x2c, 0x74, 0xd7, 0x78, 0xa4, 0x69, 0xd1, 0x34, 0x8f, 0x57, 0x43, 0xb9,
            0x47, 0x02, 0xe5, 0x29, 0x58, 0xda, 0x51, 0x0c, 0x13, 0x16, 0x24, 0x24, 0x79, 0xbe, 0xb4, 0x6e, 0xf5, 0x5d, 0xc7, 0xd7, 0xe9,
            0xab, 0x1a, 0x13, 0x19, 0x97, 0x06, 0x97, 0x8b, 0xf9, 0xd0, 0x37, 0xcb, 0x17, 0x44, 0x85, 0xa9, 0x18, 0x4d, 0x85, 0x76, 0xee,
            0x9b, 0xd6, 0x5c, 0x8e, 0x24, 0x9f, 0x5c, 0xf3, 0xc2, 0xed, 0x6e, 0xc5, 0x18, 0xfa, 0xc1, 0x14, 0x5a, 0xf0, 0xe1, 0xc0, 0xdb,
            0x26, 0x1f, 0xe4, 0xd9, 0x7d, 0x79, 0xcc, 0xb3, 0x02, 0x72, 0x99, 0x60, 0xa4, 0xec, 0x2f, 0x78, 0xb8, 0xa0, 0xd1, 0xf6, 0x90,
            0x03, 0x08, 0x09, 0xde, 0xc4, 0xf9, 0xa6, 0x6f, 0x7f, 0x40, 0x62, 0xa7, 0x40, 0x10, 0xce, 0xd6, 0x90, 0x51, 0x2e, 0x8c, 0x81,
            0x57, 0xc6, 0x62, 0x04, 0x87, 0x83, 0xb7, 0x01, 0x11, 0xff, 0xa8, 0xc1, 0xc7, 0x8a, 0xcd, 0x2a, 0xed, 0x96, 0x57, 0xd5, 0x08,
            0xd9, 0xbf, 0xd3, 0x66, 0x37, 0xb0, 0x3f, 0x2e, 0x66, 0x1f, 0x4d, 0xc9, 0xdd, 0x95, 0x2d, 0x37, 0x8e, 0xc0, 0xf2, 0xdc, 0xba,
            0x66, 0xea, 0x70, 0x3a, 0x71, 0x21, 0x49, 0x00, 0x9c, 0x0c, 0xcd, 0xb3, 0x34, 0x29, 0x11, 0xb6, 0x90, 0xd2, 0x37, 0x0b, 0xfe,
            0x5f, 0x5d, 0x5e, 0x5f, 0x7a, 0x7b, 0x21, 0xb9, 0x28, 0x87, 0x0b, 0xd8, 0xc2, 0x41, 0xed, 0x93, 0x64, 0x32, 0xde, 0xbd, 0x6a,
            0xaa, 0x1e, 0x35, 0x6a, 0x95, 0xde, 0x53, 0x4d, 0xb9, 0x05, 0xec, 0xba, 0xeb, 0xb1, 0x41, 0x4c, 0xb4, 0xa5, 0xa5, 0x87, 0x9b,
            0x54, 0x53, 0xc1, 0x2c, 0x01, 0x86, 0x11, 0x94, 0x5f, 0xfe, 0xc1, 0x45, 0x8d, 0x9d, 0x3d, 0x9d, 0x13, 0x00, 0x00, 0x81, 0x33,
            0x07, 0xae, 0x0f, 0xd5, 0x7b, 0xab, 0xa9, 0xd7, 0x24, 0xd3, 0xfe, 0xb3, 0x7e, 0x2f, 0x89, 0x92, 0xbe, 0xbe, 0x3e, 0x27, 0x92,
            0xcd, 0x04, 0x52, 0x61, 0x82, 0x2a, 0x95, 0x6e, 0xe4, 0x7b, 0xdc, 0x16, 0x19, 0x3a, 0x88, 0xd2, 0xe7, 0xdf, 0xf7, 0xae, 0x3a,
            0xd5, 0x66, 0x91, 0x94, 0x5b, 0x54, 0xa7, 0x9e, 0xf2, 0xd3, 0x51, 0x25, 0x12, 0xf1, 0xd2, 0x1d, 0x8b, 0xd9, 0xf3, 0xf3, 0x75,
            0xc4, 0xce, 0xde, 0x09, 0xe4, 0xd5, 0x23, 0x4e, 0x1e, 0xf4, 0x99, 0x60, 0x3d, 0x5d, 0x82, 0xb0, 0x31, 0x2a, 0xbf, 0xf6, 0x10,
            0xea, 0xb6, 0xae, 0x18, 0xb8, 0xfb, 0x50, 0xc7, 0x96, 0x28, 0x7e, 0x5f, 0x6f, 0xd5, 0xf5, 0x6d, 0x6b, 0xfe, 0x24, 0x82, 0xe2,
            0xbd, 0xee, 0x55, 0xd0, 0x27, 0xc8, 0x28, 0x02, 0x98, 0x41, 0x8d, 0x04, 0x2b, 0x55, 0xc0, 0x57, 0x4c, 0x87, 0x6f, 0x36, 0x84,
            0xfb, 0x71, 0x10, 0xae, 0xed, 0x71, 0x06, 0x81, 0x03, 0xcc, 0x90, 0x26, 0xa4, 0xd3, 0x3a, 0x30, 0xe8, 0x60, 0xbf, 0x3c, 0xa7,
            0xe1, 0x0d, 0x42, 0x0a, 0xf4, 0x21, 0xbb, 0xf4, 0x82, 0xe5, 0x83, 0x5d, 0x85, 0xa6, 0xe3, 0x2f, 0x9c, 0xfe, 0x36, 0xc6, 0xcb,
            0x67, 0x27, 0xcf, 0x86, 0x2d, 0x37, 0xe0, 0x27, 0x20, 0x16, 0x34, 0x6a, 0x5e, 0xd5, 0x56, 0x27, 0x4a, 0x18, 0x95, 0xb8, 0xd5,
            0x0c, 0xdc, 0xf1, 0x2e, 0x53, 0xf1, 0x48, 0x6e, 0xdb, 0xe9, 0x62, 0x2e, 0x67, 0x53, 0x1f, 0xca, 0x65, 0x93, 0xab, 0x99, 0xca,
            0x00, 0x17, 0x06, 0x87, 0x53, 0x01, 0x09, 0x80, 0xd7, 0x00, 0x07, 0x0b, 0x01, 0x00, 0x01, 0x23, 0x03, 0x01, 0x01, 0x05, 0x5d,
            0x00, 0x10, 0x00, 0x00, 0x0c, 0x81, 0x9b, 0x0a, 0x01, 0xa0, 0xee, 0xa0, 0x06, 0x00, 0x00};
        Assert (sizeof (ksample_zip_7z_) == 2157);
#if qStroika_HasComponent_LZMA
        Archive::Reader::Ptr reader = Archive::_7z::Reader::New (
            Streams::ExternallyOwnedSpanInputStream::New<byte> (Memory::SpanBytesCast<span<const byte>> (span{ksample_zip_7z_})));
        EXPECT_EQ (reader.GetContainedFiles (), (Containers::Set<String>{"sample_zip/BlockAllocation-Valgrind.supp", "sample_zip/Common-Valgrind.supp",
                                                                         "sample_zip/TODO.txt", "sample_zip/Tests-Description.txt"}));

        {
            EXPECT_EQ (reader.GetData ("sample_zip/TODO.txt").size (), 243u);
            EXPECT_EQ (reader.GetData ("sample_zip/BlockAllocation-Valgrind.supp").size (), 4296u);
            EXPECT_EQ (reader.GetData ("sample_zip/Common-Valgrind.supp").size (), 1661u);
            EXPECT_EQ (reader.GetData ("sample_zip/Tests-Description.txt").size (), 1934u);
            EXPECT_TRUE (
                BinaryToText::Reader::New (reader.GetData ("sample_zip/TODO.txt").As<InputStream::Ptr<byte>> ())
                    .ReadAll ()
                    .Contains ("Once any of the ThreadSafetyBuiltinObject tests work - with the locking stuff - add more concrete tyeps"));
            EXPECT_TRUE (
                BinaryToText::Reader::New (reader.GetData ("sample_zip/Tests-Description.txt").As<InputStream::Ptr<byte>> ()).ReadAll ().Contains ("[30]\tFoundation::DataExchange::Other"));
            try {
                auto i = reader.GetData ("file-not-found");
                EXPECT_TRUE (false);
            }
            catch (...) {
                // good
            }
        }
#else
        GTEST_SKIP () << "built without LZMA";
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, Test2_ZipArchive_)
    {
        Debug::TraceContextBumper ctx{"Test2_ZipArchive_"};
        // od sample_zip.zip -t x1 -w32 | sed 's/ /,0x/g'
        // then strip first column manually, and leading ,
        [[maybe_unused]] static constexpr uint8_t ksample_zip_[] = {
            0x50, 0x4b, 0x03, 0x04, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x50,
            0x4b, 0x03, 0x04, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0xb9, 0x17, 0x5d, 0x58, 0x3f, 0x03, 0x00, 0x00,
            0xc8, 0x10, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x42, 0x6c,
            0x6f, 0x63, 0x6b, 0x41, 0x6c, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x56, 0x61, 0x6c, 0x67, 0x72, 0x69, 0x6e,
            0x64, 0x2e, 0x73, 0x75, 0x70, 0x70, 0xd5, 0x57, 0x4b, 0x6f, 0xdb, 0x30, 0x0c, 0xbe, 0x17, 0xe8, 0x7f, 0xe8, 0x39, 0xc5, 0x82,
            0xd8, 0xcb, 0xc3, 0xdd, 0xcd, 0x49, 0xdc, 0xce, 0xa8, 0xf3, 0x40, 0x9c, 0xa1, 0xc0, 0x2e, 0x84, 0x6a, 0x33, 0x8d, 0x10, 0xcb,
            0xf2, 0x24, 0x39, 0x5b, 0x36, 0xec, 0xbf, 0x8f, 0x72, 0x1e, 0x6d, 0x83, 0x6c, 0xe8, 0xb6, 0xb6, 0xcb, 0x7c, 0x88, 0x65, 0x4a,
            0x94, 0x3e, 0x92, 0x9f, 0x48, 0xe6, 0xdb, 0xe9, 0xc9, 0xd9, 0xd9, 0xd9, 0x74, 0xce, 0x35, 0x84, 0x1a, 0xa6, 0x12, 0xe2, 0xb2,
            0x28, 0x14, 0x6a, 0x0d, 0x97, 0x2c, 0xd3, 0x08, 0x37, 0x4c, 0xe5, 0x3c, 0xbf, 0xd3, 0xe0, 0xdf, 0xca, 0xd2, 0x40, 0x37, 0x93,
            0xc9, 0x02, 0xfc, 0x8c, 0x5e, 0xcc, 0x70, 0x99, 0x57, 0xda, 0x03, 0x14, 0xc9, 0x1c, 0x93, 0xc5, 0xbb, 0x08, 0xd9, 0xa2, 0x92,
            0xcc, 0xca, 0xfc, 0x9d, 0x60, 0x76, 0xd5, 0xee, 0x13, 0x3e, 0x0e, 0x3b, 0xb1, 0x51, 0x92, 0x2f, 0x98, 0xd3, 0xb8, 0x94, 0x65,
            0x9e, 0x56, 0x1b, 0xb4, 0x49, 0x59, 0xaa, 0x95, 0x37, 0x56, 0x7c, 0xc9, 0x0c, 0x82, 0xe3, 0x5e, 0xa1, 0x21, 0x19, 0x7c, 0x30,
            0x3c, 0x83, 0x40, 0x9c, 0x9e, 0x7c, 0x3f, 0x3d, 0xf9, 0x4b, 0x90, 0xee, 0xb3, 0xa3, 0xac, 0x55, 0x47, 0xdc, 0x9f, 0x30, 0x96,
            0x32, 0x83, 0xda, 0x03, 0xac, 0xdd, 0x68, 0xd4, 0xbb, 0x06, 0x3f, 0xa2, 0x17, 0x8c, 0xc6, 0xd3, 0x70, 0x10, 0x7e, 0x0c, 0xfa,
            0x70, 0x75, 0x7e, 0x0e, 0x3f, 0xc1, 0x22, 0x98, 0x49, 0xe6, 0x6f, 0x32, 0xfa, 0x7c, 0xb3, 0xe0, 0x79, 0xaa, 0xdf, 0x9d, 0x15,
            0x52, 0x6b, 0x7e, 0x9b, 0xe1, 0xcf, 0xa0, 0x3e, 0xf4, 0xd3, 0xef, 0xe2, 0x77, 0x1b, 0x07, 0x0d, 0xa8, 0x73, 0xad, 0x58, 0xbd,
            0xf6, 0x1c, 0x3e, 0x87, 0x51, 0x61, 0xb8, 0xe0, 0x5f, 0x51, 0xc1, 0x5f, 0x7b, 0xdf, 0x69, 0xf4, 0xe6, 0x4c, 0xb1, 0xc4, 0xa0,
            0xd2, 0x6d, 0x9a, 0xa7, 0x63, 0xdf, 0x8a, 0x05, 0x04, 0xe3, 0xeb, 0xcf, 0x71, 0x13, 0x8e, 0x10, 0xad, 0xcc, 0x0d, 0xe3, 0x39,
            0xa1, 0xf5, 0x68, 0x98, 0x28, 0x34, 0xe8, 0x78, 0x03, 0x56, 0x14, 0x84, 0x00, 0x22, 0x9e, 0x2f, 0x30, 0x8d, 0xb8, 0x36, 0xa1,
            0x10, 0xc3, 0xd8, 0x81, 0xda, 0x04, 0x0b, 0x78, 0xeb, 0xa7, 0x69, 0x20, 0x9e, 0x85, 0xed, 0x2f, 0x6e, 0x8b, 0xdb, 0x8a, 0xf1,
            0x53, 0x89, 0x79, 0x82, 0xd0, 0x97, 0xe5, 0x6d, 0xb6, 0xba, 0x37, 0xe9, 0x7f, 0xa0, 0x8e, 0xd3, 0x88, 0xcb, 0xdb, 0xf5, 0x10,
            0x82, 0xc9, 0xf5, 0x30, 0x76, 0xc1, 0x69, 0x43, 0xcc, 0x66, 0x48, 0x81, 0xf0, 0x93, 0x84, 0x70, 0x4a, 0x15, 0x88, 0xe3, 0x0b,
            0xc6, 0xc5, 0x54, 0xb1, 0x25, 0xd9, 0xc1, 0x32, 0xa7, 0xd5, 0x53, 0x48, 0xf7, 0xf8, 0x0a, 0x29, 0x30, 0xcc, 0x48, 0x15, 0x0e,
            0xe3, 0x06, 0x1c, 0xb0, 0x35, 0x08, 0x02, 0x4b, 0x31, 0x2f, 0x24, 0x19, 0xa3, 0x5c, 0x12, 0x4e, 0x21, 0x20, 0x9b, 0x63, 0xe3,
            0xd1, 0x39, 0x89, 0xdd, 0x34, 0xbc, 0xb4, 0xaa, 0xdb, 0xfc, 0x30, 0x2a, 0xac, 0x8c, 0x65, 0x61, 0xdc, 0x86, 0x61, 0x7c, 0x01,
            0xae, 0xbb, 0x95, 0x40, 0x1f, 0x67, 0xac, 0xcc, 0x0c, 0x61, 0xe0, 0x46, 0xdb, 0xf9, 0x80, 0x9e, 0x65, 0x10, 0x1c, 0x9b, 0x97,
            0xac, 0x0c, 0x99, 0xd0, 0x1e, 0x97, 0xba, 0x1a, 0xbd, 0x6d, 0x75, 0x79, 0xce, 0xd4, 0x2a, 0xcc, 0x8b, 0xd2, 0xac, 0x27, 0x2f,
            0x95, 0x14, 0xe1, 0x7a, 0xe8, 0xa7, 0xac, 0x20, 0xe7, 0xf4, 0x9c, 0x60, 0x12, 0xf3, 0x63, 0xb3, 0xe5, 0x70, 0x2a, 0x69, 0xc4,
            0x68, 0x40, 0x9b, 0x54, 0xa3, 0x09, 0xb9, 0xa5, 0xaf, 0xdb, 0xbc, 0x97, 0xec, 0xc5, 0x89, 0x57, 0xd1, 0xed, 0x49, 0x21, 0x68,
            0x37, 0x8f, 0xde, 0x05, 0x53, 0xa8, 0x6e, 0xb8, 0x99, 0x07, 0x9f, 0x4a, 0x96, 0xd1, 0x02, 0xcb, 0x90, 0x16, 0xb8, 0xce, 0xc3,
            0xb9, 0x1b, 0xcc, 0xb2, 0x91, 0x4a, 0x51, 0xd1, 0xb4, 0x7d, 0xc8, 0x3b, 0xcb, 0x63, 0xf6, 0x4d, 0x67, 0x53, 0xd7, 0x5c, 0x77,
            0x6c, 0xcb, 0x28, 0x21, 0xe8, 0x33, 0xc3, 0x48, 0xa9, 0x4c, 0x4c, 0x49, 0xf0, 0xdc, 0x66, 0x25, 0xb7, 0x37, 0x60, 0xa7, 0xf4,
            0x1e, 0xb3, 0x82, 0x0c, 0xac, 0xae, 0x7f, 0xf3, 0xf1, 0xf2, 0x7f, 0x9a, 0xc5, 0x1c, 0x17, 0xae, 0xa2, 0x51, 0xd7, 0x8f, 0x00,
            0x86, 0xe0, 0x38, 0x5e, 0x5f, 0x4e, 0xf0, 0xce, 0x1e, 0x4b, 0x7b, 0x4e, 0x51, 0x1b, 0x0d, 0xc7, 0x17, 0x8b, 0x8b, 0xe0, 0x0b,
            0x26, 0x65, 0x15, 0x95, 0xa6, 0x58, 0x84, 0x93, 0x32, 0xcf, 0xad, 0xaf, 0xc7, 0x46, 0xed, 0x27, 0x9b, 0xe5, 0x11, 0x66, 0x8c,
            0x43, 0x4c, 0x72, 0x2e, 0x42, 0xb3, 0x49, 0xad, 0xa2, 0xc8, 0xd6, 0x5c, 0x81, 0xda, 0x6e, 0x1d, 0x55, 0x8a, 0x88, 0x00, 0x59,
            0xda, 0x58, 0xdc, 0x52, 0x99, 0x47, 0xdd, 0xdf, 0x5e, 0x73, 0x05, 0x49, 0xc6, 0xf2, 0xbb, 0xf3, 0xf3, 0x67, 0x6b, 0xfd, 0x9e,
            0x50, 0xec, 0x76, 0xe9, 0x62, 0x67, 0x51, 0xa7, 0x5b, 0xce, 0x66, 0xa8, 0x30, 0x5d, 0x97, 0x06, 0x32, 0xa1, 0x09, 0xf4, 0xd3,
            0x21, 0xa7, 0xa3, 0x5a, 0x62, 0x20, 0x5e, 0xd7, 0x80, 0xc7, 0x3c, 0x77, 0x5b, 0x9b, 0x8a, 0xbc, 0x05, 0xe9, 0x2b, 0xc5, 0x56,
            0x16, 0x1f, 0xe4, 0x9f, 0x5f, 0x1b, 0xda, 0x9f, 0x77, 0xd0, 0x61, 0x24, 0x6a, 0xde, 0x46, 0xf6, 0xea, 0x1e, 0xfd, 0x05, 0x6c,
            0xb7, 0xe3, 0x97, 0x46, 0x0a, 0x12, 0x25, 0xa4, 0xb6, 0x7a, 0x08, 0x06, 0xd3, 0x27, 0x30, 0x17, 0xe4, 0xf6, 0xba, 0xd5, 0xeb,
            0xf5, 0x97, 0xc6, 0x7c, 0xe8, 0x3e, 0xee, 0xa7, 0x68, 0xa7, 0x71, 0xdf, 0x74, 0xfe, 0x4b, 0xfc, 0x7b, 0xd9, 0xba, 0xb3, 0x61,
            0x31, 0x75, 0x98, 0x7a, 0x3d, 0x6a, 0x0f, 0x56, 0x4f, 0x27, 0xf1, 0x4b, 0xc1, 0xfc, 0xed, 0x7e, 0xb2, 0xb6, 0xdf, 0x10, 0xd6,
            0x06, 0xab, 0x6d, 0x0f, 0x49, 0xe6, 0x1c, 0xc3, 0x3f, 0xdd, 0x2d, 0x7d, 0x2b, 0x28, 0xa7, 0x27, 0x3f, 0x00, 0x50, 0x4b, 0x03,
            0x04, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0xbc, 0xd6, 0x66, 0x07, 0x57, 0x02, 0x00, 0x00, 0x7d, 0x06,
            0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x43, 0x6f, 0x6d, 0x6d,
            0x6f, 0x6e, 0x2d, 0x56, 0x61, 0x6c, 0x67, 0x72, 0x69, 0x6e, 0x64, 0x2e, 0x73, 0x75, 0x70, 0x70, 0xcd, 0x54, 0x4b, 0x4f, 0xdb,
            0x40, 0x10, 0xbe, 0x23, 0xf1, 0x1f, 0x38, 0xb6, 0x97, 0xc8, 0x2e, 0x01, 0x02, 0x37, 0x1e, 0x46, 0x8a, 0x12, 0x20, 0xc5, 0xb4,
            0x95, 0x5a, 0x55, 0xa3, 0xf5, 0xee, 0xc4, 0xb1, 0xb2, 0x0f, 0x77, 0x1f, 0x09, 0x51, 0xd5, 0xff, 0xde, 0x59, 0x27, 0x84, 0xbc,
            0x50, 0xda, 0x53, 0xeb, 0x83, 0x2d, 0xcf, 0xf3, 0xdb, 0xef, 0x9b, 0x9d, 0x9f, 0x87, 0x07, 0x47, 0x47, 0x47, 0x97, 0x75, 0xcd,
            0x2c, 0x6a, 0x2f, 0x67, 0x70, 0x65, 0xca, 0xe0, 0xe0, 0x0b, 0xb3, 0xba, 0xd2, 0x25, 0x5c, 0x16, 0x26, 0x78, 0xc8, 0xbd, 0x7d,
            0x32, 0x37, 0x70, 0x6d, 0x04, 0x02, 0x67, 0x52, 0x46, 0x4f, 0xdf, 0x98, 0xb1, 0x83, 0x87, 0x5e, 0x93, 0x7f, 0x87, 0x8a, 0x8f,
            0x90, 0x8f, 0x2f, 0xae, 0x8d, 0x16, 0x8d, 0x65, 0x18, 0xf4, 0x05, 0xd0, 0xe3, 0xbc, 0xf5, 0x46, 0x80, 0x84, 0x4a, 0x7b, 0xb4,
            0x9a, 0xc9, 0xc3, 0x83, 0x5f, 0x87, 0x07, 0x5b, 0x5d, 0x8b, 0x50, 0x6e, 0x34, 0xa5, 0x4c, 0x11, 0xb3, 0xa3, 0x41, 0xa1, 0x92,
            0xc8, 0xc6, 0x4d, 0x6f, 0x14, 0x30, 0xb4, 0x46, 0x81, 0x08, 0x96, 0xf9, 0xca, 0x68, 0xe0, 0x84, 0x6a, 0x1d, 0x44, 0x9f, 0x62,
            0x97, 0x20, 0x14, 0xe5, 0x18, 0xfe, 0x8a, 0xe9, 0xab, 0x9e, 0xaa, 0x95, 0xbf, 0xfb, 0x73, 0x80, 0x52, 0x07, 0xe0, 0xcf, 0xcf,
            0xe9, 0xb1, 0xc6, 0x29, 0x34, 0xe1, 0xcc, 0x1b, 0xdb, 0xe5, 0x59, 0x67, 0xf1, 0x83, 0x99, 0x1a, 0xf4, 0x26, 0xab, 0x59, 0xb9,
            0x6b, 0xc3, 0x23, 0xd6, 0xe7, 0x90, 0x03, 0xb7, 0xd8, 0x44, 0xa8, 0xc7, 0x5e, 0xce, 0x28, 0x69, 0x47, 0x58, 0x07, 0xee, 0x80,
            0x4b, 0xa3, 0x31, 0x5b, 0xc4, 0xa8, 0xf5, 0xa0, 0x33, 0x8b, 0x0e, 0xed, 0x04, 0x37, 0xed, 0xa7, 0xac, 0xae, 0x51, 0x8b, 0x6c,
            0xd0, 0xe3, 0x6f, 0x7b, 0xd6, 0x1d, 0x75, 0x7f, 0xd3, 0x76, 0x46, 0xea, 0x99, 0x6a, 0xcc, 0xd2, 0xe4, 0xd6, 0x04, 0x2d, 0x1a,
            0xd2, 0xda, 0x4f, 0x95, 0xc2, 0xce, 0xcd, 0x82, 0xc2, 0xf4, 0xc3, 0x27, 0x3d, 0x60, 0xd6, 0x61, 0xb4, 0x42, 0x26, 0x76, 0x48,
            0x04, 0x57, 0xa1, 0xec, 0xea, 0xbe, 0x21, 0x05, 0x30, 0x8e, 0xc1, 0x55, 0xf0, 0x03, 0x6b, 0x0a, 0x56, 0xc8, 0xd9, 0xc7, 0x50,
            0xd9, 0xf1, 0xc3, 0xf0, 0x33, 0x93, 0x25, 0x89, 0x25, 0xd6, 0xa5, 0xb8, 0x14, 0xc2, 0x76, 0x96, 0x68, 0xa6, 0xdc, 0x71, 0x55,
            0xaf, 0x01, 0xf6, 0x69, 0xa2, 0x88, 0x98, 0x59, 0x1d, 0x34, 0xf7, 0xdd, 0x69, 0xbf, 0x48, 0xb3, 0xec, 0x26, 0xcd, 0x26, 0xfb,
            0x83, 0x92, 0xcd, 0xa0, 0x53, 0xd9, 0xc0, 0x3b, 0x81, 0xae, 0xaa, 0xe5, 0xa2, 0xc6, 0xb6, 0x3b, 0x3a, 0xf6, 0xd1, 0x93, 0x1e,
            0xd3, 0x1c, 0x0f, 0xab, 0xf2, 0x85, 0x9f, 0xf6, 0x2d, 0x1d, 0x6c, 0x7e, 0xf8, 0x7b, 0xa6, 0xa2, 0x8a, 0xf7, 0x79, 0x02, 0x69,
            0x72, 0x3d, 0x62, 0x96, 0x71, 0x9a, 0x6b, 0x77, 0x9a, 0x37, 0x93, 0x9a, 0xe5, 0x27, 0xf0, 0x4f, 0xd9, 0x4b, 0xfe, 0x84, 0xbd,
            0xe4, 0xbf, 0x66, 0xef, 0xef, 0x8a, 0x9f, 0xc4, 0xe2, 0xb1, 0xec, 0xbc, 0xc3, 0xde, 0xea, 0xa4, 0xcd, 0x8b, 0x3a, 0x73, 0x3e,
            0x21, 0xde, 0x24, 0x1a, 0x7e, 0xf0, 0x06, 0x4a, 0xd4, 0x48, 0x75, 0x11, 0x86, 0x4c, 0x3a, 0x84, 0xe9, 0x7c, 0x1f, 0x39, 0x28,
            0x39, 0x6f, 0x77, 0x20, 0x14, 0x41, 0xfb, 0xb0, 0xbd, 0xef, 0x76, 0x09, 0xd4, 0x6a, 0xb5, 0x56, 0xe6, 0x40, 0x56, 0x05, 0x0f,
            0x56, 0x42, 0xcd, 0x9c, 0x43, 0x07, 0xce, 0xd0, 0x35, 0x2b, 0x98, 0x80, 0x09, 0x93, 0x01, 0xe7, 0x8d, 0x3d, 0x13, 0x82, 0x50,
            0x0e, 0x0d, 0x28, 0x36, 0x2b, 0x36, 0x16, 0xda, 0x80, 0x8e, 0x32, 0x5f, 0x00, 0x8e, 0x2e, 0xbd, 0x72, 0xe5, 0x3b, 0x45, 0xaf,
            0x6f, 0xc9, 0xf7, 0x16, 0x7d, 0x60, 0x24, 0xec, 0xfb, 0x25, 0x86, 0x18, 0x10, 0x9d, 0x2b, 0x4b, 0x38, 0x76, 0x07, 0x4b, 0x7d,
            0x75, 0x74, 0xee, 0x74, 0xfc, 0x08, 0x68, 0x67, 0x6f, 0xa4, 0x30, 0xcb, 0x47, 0x4b, 0x17, 0x19, 0x1c, 0x08, 0xed, 0x22, 0xe2,
            0x91, 0x71, 0xbe, 0x98, 0x69, 0xe2, 0xbe, 0x0d, 0x76, 0x19, 0x51, 0xb2, 0x6a, 0x44, 0xcb, 0x1e, 0xfd, 0xab, 0xe5, 0xf5, 0x70,
            0x2f, 0xdc, 0x2c, 0x74, 0xf8, 0x0d, 0x50, 0x4b, 0x03, 0x04, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0x73,
            0x89, 0x39, 0xc3, 0x1c, 0x02, 0x00, 0x00, 0x8e, 0x07, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65,
            0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x54, 0x65, 0x73, 0x74, 0x73, 0x2d, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f,
            0x6e, 0x2e, 0x74, 0x78, 0x74, 0xa5, 0x94, 0x4f, 0x73, 0xda, 0x30, 0x10, 0xc5, 0xcf, 0xcd, 0x4c, 0xbe, 0x47, 0x7b, 0xc2, 0x18,
            0x48, 0xc1, 0x37, 0x12, 0xc8, 0x94, 0x0e, 0xff, 0x8a, 0x3d, 0x4d, 0x67, 0x32, 0x3d, 0x6c, 0x84, 0xb0, 0x55, 0x8c, 0x44, 0xd7,
            0x52, 0x8a, 0xfb, 0xe9, 0xab, 0xb5, 0x4b, 0x28, 0xea, 0x58, 0x1c, 0xb8, 0x09, 0xed, 0x6f, 0x9f, 0xd6, 0xef, 0x49, 0x3c, 0x07,
            0xed, 0xef, 0xef, 0x1e, 0x95, 0x91, 0x6b, 0xd0, 0x42, 0xc9, 0x28, 0x7a, 0x00, 0x96, 0x09, 0x99, 0xde, 0xde, 0x3c, 0x07, 0xa1,
            0x53, 0xc9, 0x00, 0x81, 0x69, 0x8e, 0x45, 0x14, 0xc5, 0x1a, 0x2d, 0x54, 0x10, 0xd5, 0x71, 0x28, 0x25, 0x37, 0x22, 0x35, 0x58,
            0xfd, 0xa4, 0x7a, 0xf7, 0xbf, 0xba, 0x06, 0x21, 0x2b, 0x95, 0xf1, 0xc1, 0xaa, 0x49, 0xc8, 0xf3, 0x32, 0x2e, 0x25, 0xcb, 0x50,
            0x49, 0xf1, 0x9b, 0xaf, 0x47, 0xa0, 0xc1, 0xea, 0x1b, 0xa6, 0x0d, 0x72, 0x4b, 0x0d, 0x11, 0xa1, 0x24, 0xa5, 0xde, 0xb5, 0x4a,
            0x53, 0x21, 0xb7, 0x7c, 0x3d, 0x15, 0x85, 0x26, 0xb9, 0xbb, 0x6b, 0xe5, 0x46, 0xca, 0xbc, 0xe4, 0xe5, 0xb9, 0xe8, 0xc7, 0x66,
            0xd1, 0x25, 0x8a, 0x57, 0xd0, 0xdc, 0xf6, 0x39, 0x3a, 0x71, 0x32, 0x7d, 0xe3, 0x9e, 0x10, 0xf6, 0x7b, 0x8e, 0x24, 0xd5, 0xbf,
            0x76, 0xbe, 0x78, 0x2b, 0xf6, 0xc7, 0xb9, 0x06, 0xcd, 0x62, 0xc3, 0xa2, 0x50, 0x4c, 0x1c, 0xf3, 0x6a, 0x07, 0xcd, 0xe4, 0x3d,
            0xa4, 0x44, 0xb4, 0x3d, 0x84, 0xf8, 0xc1, 0xd9, 0x51, 0x29, 0x6c, 0xe2, 0x68, 0x9d, 0xe7, 0x27, 0xb0, 0xd3, 0x0c, 0x8e, 0xf8,
            0x4f, 0xc3, 0x89, 0xf1, 0x5c, 0xa3, 0x99, 0xb5, 0xac, 0xbe, 0xb2, 0xed, 0x9e, 0xd7, 0x7e, 0x85, 0x42, 0x97, 0x5f, 0x0c, 0xaf,
            0x15, 0x3d, 0xf9, 0xbf, 0x31, 0x9e, 0x38, 0x63, 0x9a, 0x4c, 0xb2, 0x0a, 0xeb, 0xfb, 0x30, 0x4d, 0xc4, 0xc0, 0x43, 0x28, 0xd4,
            0x7c, 0x7d, 0x9e, 0x42, 0x18, 0x5c, 0xe2, 0xcf, 0x1c, 0x0c, 0xdb, 0x97, 0xf0, 0x93, 0x47, 0x61, 0x78, 0x91, 0x35, 0xb9, 0x16,
            0xf5, 0xdc, 0x61, 0xe7, 0x12, 0xfc, 0x97, 0xeb, 0x7a, 0x38, 0x0d, 0x6c, 0x4b, 0x8c, 0x27, 0x9c, 0x7f, 0x8f, 0x74, 0x73, 0xc1,
            0x72, 0xaf, 0x55, 0x6a, 0xdf, 0x45, 0x56, 0x52, 0xf9, 0x2c, 0x92, 0xfa, 0x31, 0x8d, 0x0f, 0x2c, 0x03, 0x99, 0xda, 0xa7, 0xb5,
            0xe2, 0xb0, 0xe6, 0xd8, 0x7a, 0xb2, 0x41, 0x5b, 0xdd, 0xf7, 0x93, 0xf9, 0xa4, 0xf5, 0x39, 0x5e, 0xcc, 0x5b, 0xdf, 0x66, 0xd3,
            0x0f, 0xd4, 0xdc, 0xf7, 0x35, 0x2f, 0x5e, 0xe8, 0xf2, 0x7e, 0x05, 0x14, 0x20, 0xf5, 0xec, 0xf8, 0x0e, 0xc3, 0x81, 0xaf, 0xc7,
            0x0a, 0xdb, 0x2f, 0x84, 0xc3, 0x12, 0xb0, 0xa8, 0xf0, 0x4e, 0xe0, 0x3d, 0x42, 0x67, 0x35, 0xe5, 0x04, 0x36, 0x3e, 0x70, 0x66,
            0x8e, 0x4b, 0xc6, 0xf7, 0xb4, 0x2c, 0x88, 0x0b, 0x1b, 0xb9, 0x25, 0x2a, 0xc6, 0x8b, 0x62, 0x65, 0xa4, 0xac, 0x25, 0x3b, 0x8d,
            0x68, 0x2c, 0x52, 0xfb, 0x67, 0x51, 0xe9, 0x75, 0x1b, 0xa1, 0x24, 0x43, 0x6b, 0x5e, 0x05, 0xf5, 0x2e, 0x40, 0x31, 0x6c, 0xb8,
            0x2e, 0xef, 0x8d, 0xb0, 0xa1, 0xc9, 0xda, 0x35, 0x6a, 0xbb, 0x6b, 0x6a, 0x3b, 0x7d, 0xb6, 0x13, 0xde, 0x64, 0x11, 0x45, 0x73,
            0xae, 0x7f, 0x29, 0xdc, 0x52, 0xb5, 0xdf, 0x58, 0x8d, 0xa2, 0x4f, 0x49, 0xb2, 0x24, 0x66, 0xe0, 0x61, 0x12, 0x04, 0x59, 0x6c,
            0xaa, 0x93, 0xba, 0x81, 0xcb, 0x9d, 0xa6, 0xe8, 0x3a, 0xe6, 0xcf, 0x40, 0x67, 0xb4, 0x1d, 0x3a, 0xdb, 0x7c, 0xa7, 0xb0, 0xa4,
            0x82, 0xe3, 0x6c, 0xac, 0xad, 0x09, 0xbb, 0x82, 0x2a, 0x8e, 0x9d, 0x89, 0xd8, 0x71, 0xda, 0x3e, 0x33, 0xb0, 0x9a, 0xeb, 0xd5,
            0x5e, 0x47, 0xc8, 0xa9, 0xe6, 0xb8, 0xb4, 0x1c, 0xaf, 0x1e, 0x17, 0xab, 0xd9, 0x70, 0xfe, 0x30, 0xbe, 0xbd, 0xf9, 0x03, 0x50,
            0x4b, 0x03, 0x04, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0x94, 0x3c, 0x0b, 0xa0, 0x82, 0x00, 0x00, 0x00,
            0xf3, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x54, 0x4f,
            0x44, 0x4f, 0x2e, 0x74, 0x78, 0x74, 0xb5, 0xce, 0xc1, 0x0d, 0x83, 0x30, 0x14, 0x03, 0xd0, 0x33, 0x48, 0xec, 0xe0, 0x05, 0xba,
            0x44, 0xd5, 0x7b, 0x0e, 0x65, 0x81, 0xdf, 0xf0, 0x53, 0x52, 0x20, 0x1f, 0xe5, 0x1b, 0x21, 0xb6, 0x6f, 0xd4, 0x1d, 0x7a, 0xf2,
            0xc1, 0x96, 0x9e, 0xc7, 0xf0, 0x08, 0x43, 0xdf, 0x59, 0x17, 0x4a, 0x54, 0x48, 0xb9, 0x60, 0x09, 0x9c, 0x15, 0xe3, 0x5c, 0x55,
            0xa6, 0xa7, 0x24, 0xe5, 0x75, 0x3f, 0xf2, 0xca, 0x5c, 0xc2, 0xeb, 0xa3, 0x91, 0xa0, 0x3a, 0x1d, 0xa7, 0xd5, 0x05, 0x37, 0x9c,
            0x99, 0xf3, 0x6f, 0xbf, 0x5a, 0x5c, 0x72, 0x79, 0xc3, 0x79, 0xa4, 0xd4, 0x0a, 0x99, 0x26, 0x6c, 0x56, 0x15, 0xd1, 0x4a, 0xac,
            0x4a, 0x05, 0x2f, 0xdd, 0x7d, 0xe8, 0xff, 0xa6, 0xd9, 0xae, 0x55, 0x98, 0xad, 0x38, 0x68, 0x10, 0xf7, 0xa3, 0xe9, 0x2e, 0xa9,
            0x41, 0x6c, 0xb9, 0xb5, 0x07, 0x79, 0xd3, 0xa1, 0xff, 0x02, 0x50, 0x4b, 0x01, 0x02, 0x3f, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xd1, 0xa2, 0x4b, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x24, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f,
            0x7a, 0x69, 0x70, 0x2f, 0x0a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0xf5, 0xbf, 0xd7, 0x16, 0x84,
            0x04, 0xd1, 0x01, 0xf5, 0xbf, 0xd7, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x7c, 0xcf, 0xd5, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x50, 0x4b,
            0x01, 0x02, 0x3f, 0x00, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0xb9, 0x17, 0x5d, 0x58, 0x3f, 0x03, 0x00,
            0x00, 0xc8, 0x10, 0x00, 0x00, 0x28, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x29, 0x00,
            0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x41, 0x6c, 0x6c,
            0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2d, 0x56, 0x61, 0x6c, 0x67, 0x72, 0x69, 0x6e, 0x64, 0x2e, 0x73, 0x75, 0x70, 0x70,
            0x0a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x09, 0x42, 0xd6, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x7c,
            0xcf, 0xd5, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x7c, 0xcf, 0xd5, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x50, 0x4b, 0x01, 0x02, 0x3f, 0x00,
            0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0xbc, 0xd6, 0x66, 0x07, 0x57, 0x02, 0x00, 0x00, 0x7d, 0x06, 0x00,
            0x00, 0x1f, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xae, 0x03, 0x00, 0x00, 0x73, 0x61,
            0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x43, 0x6f, 0x6d, 0x6d, 0x6f, 0x6e, 0x2d, 0x56, 0x61, 0x6c, 0x67, 0x72,
            0x69, 0x6e, 0x64, 0x2e, 0x73, 0x75, 0x70, 0x70, 0x0a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x01,
            0x01, 0xd7, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x69, 0x8e, 0xd6, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x69, 0x8e, 0xd6, 0x16, 0x84, 0x04,
            0xd1, 0x01, 0x50, 0x4b, 0x01, 0x02, 0x3f, 0x00, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0x73, 0x89, 0x39,
            0xc3, 0x1c, 0x02, 0x00, 0x00, 0x8e, 0x07, 0x00, 0x00, 0x20, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
            0x00, 0x00, 0x42, 0x06, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x54, 0x65, 0x73, 0x74,
            0x73, 0x2d, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x74, 0x78, 0x74, 0x0a, 0x00, 0x20, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x55, 0x0c, 0xd8, 0x16, 0x84, 0x04, 0xd1, 0x01, 0xf5, 0xbf, 0xd7, 0x16, 0x84,
            0x04, 0xd1, 0x01, 0xf5, 0xbf, 0xd7, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x50, 0x4b, 0x01, 0x02, 0x3f, 0x00, 0x14, 0x00, 0x00, 0x00,
            0x08, 0x00, 0xd1, 0xa2, 0x4b, 0x47, 0x94, 0x3c, 0x0b, 0xa0, 0x82, 0x00, 0x00, 0x00, 0xf3, 0x00, 0x00, 0x00, 0x13, 0x00, 0x24,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x9c, 0x08, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65,
            0x5f, 0x7a, 0x69, 0x70, 0x2f, 0x54, 0x4f, 0x44, 0x4f, 0x2e, 0x74, 0x78, 0x74, 0x0a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x18, 0x00, 0x8e, 0x73, 0xd7, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x2d, 0x27, 0xd7, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x2d,
            0x27, 0xd7, 0x16, 0x84, 0x04, 0xd1, 0x01, 0x50, 0x4b, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x1f, 0x02,
            0x00, 0x00, 0x4f, 0x09, 0x00, 0x00, 0x00, 0x00};
        Assert (sizeof (ksample_zip_) == 2948);
#if qStroika_HasComponent_zlib
        {
            Archive::Reader::Ptr reader = Archive::Zip::Reader::New (
                Streams::ExternallyOwnedSpanInputStream::New<byte> (Memory::SpanBytesCast<span<const byte>> (span{ksample_zip_})));

            EXPECT_EQ (reader.GetContainedFiles (), (Containers::Set<String>{"sample_zip/BlockAllocation-Valgrind.supp", "sample_zip/Common-Valgrind.supp",
                                                                             "sample_zip/TODO.txt", "sample_zip/Tests-Description.txt"}));
            EXPECT_EQ (reader.GetData ("sample_zip/TODO.txt").size (), 243u);
            EXPECT_EQ (reader.GetData ("sample_zip/BlockAllocation-Valgrind.supp").size (), 4296u);
            EXPECT_EQ (reader.GetData ("sample_zip/Common-Valgrind.supp").size (), 1661u);
            EXPECT_EQ (reader.GetData ("sample_zip/Tests-Description.txt").size (), 1934u);
            EXPECT_TRUE (
                BinaryToText::Reader::New (reader.GetData ("sample_zip/TODO.txt").As<InputStream::Ptr<byte>> ())
                    .ReadAll ()
                    .Contains ("Once any of the ThreadSafetyBuiltinObject tests work - with the locking stuff - add more concrete tyeps"));
            EXPECT_TRUE (
                BinaryToText::Reader::New (reader.GetData ("sample_zip/Tests-Description.txt").As<InputStream::Ptr<byte>> ()).ReadAll ().Contains ("[30]\tFoundation::DataExchange::Other"));
            try {
                auto i = reader.GetData ("file-not-found");
                EXPECT_TRUE (false);
            }
            catch (...) {
                // good
            }
        }
        {
            MemoryStream::Ptr<byte> store = MemoryStream::New<byte> ();
            {
                Archive::Writer::Ptr writer = Archive::Zip::Writer::New (store);
                writer.Add ("file-a.zip", Memory::BLOB{ksample_zip_});
            }
            // now see if we can read it back
            {
                Archive::Reader::Ptr reader = Archive::Zip::Reader::New (store);
                EXPECT_EQ (reader.GetContainedFiles (), (Containers::Set<String>{"file-a.zip"}));
                EXPECT_EQ (reader.GetData ("file-a.zip"), Memory::BLOB{ksample_zip_});
            }
        }
#else
        GTEST_SKIP () << "built without zlib";
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, INI_ONLY_)
    {
        Debug::TraceContextBumper ctx{"INI_ONLY_::DoAll_"};
        auto                      DoBasicReader1_ = [] () {
            stringstream tmp;
            tmp << "NAME=\"Ubuntu\"" << endl;
            tmp << "VERSION=\"13.10, Saucy Salamander\"" << endl;
            tmp << "ID=ubuntu" << endl;
            tmp << "ID_LIKE=debian" << endl;
            tmp << "PRETTY_NAME=\"Ubuntu 13.10\"" << endl;
            tmp << "VERSION_ID=\"13.10\"" << endl;
            tmp << "HOME_URL=\"http://www.ubuntu.com/\"" << endl;
            tmp << "SUPPORT_URL=\"http://help.ubuntu.com/\"" << endl;
            tmp << "BUG_REPORT_URL=\"http://bugs.launchpad.net/ubuntu/\"" << endl;
            Variant::INI::Profile p = Variant::INI::Reader{}.ReadProfile (tmp);
            EXPECT_TRUE (p.fNamedSections.empty ());
            EXPECT_TRUE (p.fUnnamedSection.fProperties.LookupValue ("NAME") == "Ubuntu");
            EXPECT_TRUE (p.fUnnamedSection.fProperties.LookupValue ("SUPPORT_URL") == "http://help.ubuntu.com/");
        };
        auto DoReadWriteTest2_ = [] () {
            {
                struct Case_ {
                    DataExchange::Variant::INI::Profile data;
                    string                              dataAsFile;
                };
                const Case_ kCase1_{[] () {
                                        DataExchange::Variant::INI::Section section{{
                                            {"NAME", "Ubuntu"},
                                            {"VERSION", "13.10, Saucy Salamander"},
                                            {"ID", "ubuntu"},
                                            {"ID_LIKE", "debian"},
                                            {"PRETTY_NAME", "Ubuntu 13.10"},
                                            {"VERSION_ID", "13.10"},
                                            {"HOME_URL", "http://www.ubuntu.com/"},
                                            {"SUPPORT_URL", "http://help.ubuntu.com/"},
                                            {"BUG_REPORT_URL", "http://bugs.launchpad.net/ubuntu/"},
                                        }};
                                        return DataExchange::Variant::INI::Profile{section};
                                    }(),
                                    [] () {
                                        stringstream tmp;
                                        tmp << "NAME=\"Ubuntu\"" << Characters::kEOL<char>;
                                        tmp << "VERSION=\"13.10, Saucy Salamander\"" << Characters::kEOL<char>;
                                        tmp << "ID=ubuntu" << Characters::kEOL<char>;
                                        tmp << "ID_LIKE=debian" << Characters::kEOL<char>;
                                        tmp << "PRETTY_NAME=\"Ubuntu 13.10\"" << Characters::kEOL<char>;
                                        tmp << "VERSION_ID=\"13.10\"" << Characters::kEOL<char>;
                                        tmp << "HOME_URL=\"http://www.ubuntu.com/\"" << Characters::kEOL<char>;
                                        tmp << "SUPPORT_URL=\"http://help.ubuntu.com/\"" << Characters::kEOL<char>;
                                        tmp << "BUG_REPORT_URL=\"http://bugs.launchpad.net/ubuntu/\"" << Characters::kEOL<char>;
                                        return tmp.str ();
                                    }()};

                // Cannot compare that serialized formats equal, but we can assure that when we serialize and then deserialize that
                // is the same as the original data
                String                serialized   = Variant::INI::Writer{}.WriteAsString (kCase1_.data);
                Variant::INI::Profile deserialized = Variant::INI::Reader{}.ReadProfile (serialized);
                EXPECT_EQ (deserialized, kCase1_.data);
                EXPECT_EQ ((Variant::INI::Reader{}.ReadProfile (Memory::BLOB{kCase1_.dataAsFile})), kCase1_.data);
            }
        };
        DoBasicReader1_ ();
        DoReadWriteTest2_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, CharacterDelimitedLines_ONLY_)
    {
        Debug::TraceContextBumper ctx{"CharacterDelimitedLines_ONLY_::DoAll_"};

        auto DoBasicReader1_ = [] () {
            using Traversal::Iterable;
            stringstream tmp;
            tmp << "3,4" << endl;
            tmp << "4,5" << endl;
            Iterable<Sequence<String>> s = Variant::CharacterDelimitedLines::Reader{{','}}.ReadMatrix (tmp);
            EXPECT_TRUE (s.size () == 2);
            EXPECT_TRUE (s.Nth (1)[1] == "5");
        };

        struct Case_ {
            Sequence<Sequence<String>> data;
            string                     dataAsFile;
        };
        const Case_ kCase1_spaceSep_{[] () {
                                         return Sequence<Sequence<String>>{
                                             Sequence<String>{"3", "4"},
                                             Sequence<String>{"4", "5"},
                                         };
                                     }(),
                                     [] () {
                                         stringstream tmp;
                                         tmp << "3, 4" << Characters::kEOL<char>;
                                         tmp << "4, 5" << Characters::kEOL<char>;
                                         return tmp.str ();
                                     }()};
        const Case_ kCase2_noSpace_{[] () {
                                        return Sequence<Sequence<String>>{
                                            Sequence<String>{"a", "b"},
                                            Sequence<String>{"1", "2", "3"}, // Can be different lengths
                                            Sequence<String>{"x", "7"},
                                        };
                                    }(),
                                    [] () {
                                        stringstream tmp;
                                        tmp << "a,b" << Characters::kEOL<char>;
                                        tmp << "1,2,3" << Characters::kEOL<char>;
                                        tmp << "x,7" << Characters::kEOL<char>;
                                        return tmp.str ();
                                    }()};

        auto DoBasicWriterAndReader1_ = [&] () {
            using WriterOptions = Variant::CharacterDelimitedLines::Writer::Options;
            {
                auto serialized =
                    Variant::CharacterDelimitedLines::Writer{WriterOptions{.fSpaceSeparate = true}}.WriteAsString (kCase1_spaceSep_.data);
                String aaa = String{kCase1_spaceSep_.dataAsFile};
                EXPECT_EQ (serialized.AsASCII (), kCase1_spaceSep_.dataAsFile);
                stringstream tmp{kCase1_spaceSep_.dataAsFile};
                EXPECT_EQ (kCase1_spaceSep_.data, (Sequence<Sequence<String>>{Variant::CharacterDelimitedLines::Reader{{','}}.ReadMatrix (tmp)}));
            }
            {
                auto serialized =
                    Variant::CharacterDelimitedLines::Writer{WriterOptions{.fSpaceSeparate = false}}.WriteAsString (kCase2_noSpace_.data);
                EXPECT_EQ (serialized.AsASCII (), kCase2_noSpace_.dataAsFile);
                stringstream tmp{kCase2_noSpace_.dataAsFile};
                EXPECT_EQ (kCase2_noSpace_.data, Sequence<Sequence<String>>{Variant::CharacterDelimitedLines::Reader{{','}}.ReadMatrix (tmp)});
            }
        };

        DoBasicReader1_ ();
        DoBasicWriterAndReader1_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, FormURLEncoded)
    {
        using namespace Memory;
        using Memory::BLOB;
        Debug::TraceContextBumper ctx{"FormURLEncoded"};

        auto t1 = [] () {
            BLOB                        encoded_data = "name=John+Doe&age=30&city=New%20York"_blob;
            Association<String, String> a            = Variant::FormURLEncoded::Reader{}.ReadAssociation (encoded_data);
            DbgTrace ("a={}"_f, a);
            EXPECT_EQ (a.LookupOneValue ("name"), "John Doe");
            EXPECT_EQ (a.LookupOneValue ("age"), "30");
            EXPECT_EQ (a.LookupOneValue ("city"), "New York");
            BLOB roundTripped = Variant::FormURLEncoded::Writer{}.WriteAsBLOB (a);
            DbgTrace ("roundTripped={}"_f, roundTripped);
            EXPECT_EQ (a, Variant::FormURLEncoded::Reader{}.ReadAssociation (roundTripped));
        };

        t1 ();
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_01_BasicWriterTests_)
    {
        Debug::TraceContextBumper ctx{"JSON_ONLY_::DoAll_"};
        auto                      CheckMatchesExpected_WRITER_ = [] (const VariantValue& v, const string& expected) {
            Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
            using Writer                         = DataExchange::Variant::JSON::Writer;
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            Writer{Writer::Options{.fLineTermination = "\n"sv}}.Write (v, out);
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            string x = out.As<string> ();
            // not quite true, but almost: EXPECT_TRUE (out.As<string> () == expected);
            if (x != expected) {
                // Might be a bug, but probably not; before Stroika v3.0d1, we used Mapping_stdmap<> so
                // the maps were really ordered. But now they are not, so the text representation can
                // vary. If diff, reverse the parse, and see if OK.
                VariantValue vvv = DataExchange::Variant::JSON::Reader{}.Read (Memory::BLOB{x});
                if (vvv != v) {
                    Stroika::Frameworks::Test::WarnTestIssue (string{"x: " + x});
                    Stroika::Frameworks::Test::WarnTestIssue (string{"expected: " + expected});
                    EXPECT_TRUE (false);
                }
            }
        };
        {
            VariantValue v1 = "hello world";
            CheckMatchesExpected_WRITER_ (v1, "\"hello world\"\n");
        }
        {
            VariantValue v1 = 3;
            CheckMatchesExpected_WRITER_ (v1, "3\n");
        }
        {
            // Sterl's bug report email dated 2015-10-15 - a backslash must be followed by one of “\/bfnrtu
            {
                VariantValue v1 = "test\?";
                CheckMatchesExpected_WRITER_ (v1, "\"test?\"\n");
            }
            {
                VariantValue v1 = "test\\?";
                CheckMatchesExpected_WRITER_ (v1, "\"test\\\\?\"\n");
            }
            {
                Mapping<String, VariantValue> m{pair<String, VariantValue>{"fCmdLine", "test\\?"}};
                VariantValue                  v1{m};
                CheckMatchesExpected_WRITER_ (v1, "{\n    \"fCmdLine\" : \"test\\\\?\"\n}\n");
            }
        }
        {
            // Check (real issue behind Sterl's bug report email dated 2015-10-15) - proper control char handling
            // No control characters allowed directly in string
            {
                VariantValue v1 = "\t";
                CheckMatchesExpected_WRITER_ (v1, "\"\\t\"\n");
            }
            {
                VariantValue v1 = L"\x3";
                CheckMatchesExpected_WRITER_ (v1, "\"\\u0003\"\n");
            }
        }
        {
            VariantValue v1 = 4.7;
            CheckMatchesExpected_WRITER_ (v1, "4.7\n");
        }
        {
            VariantValue v1 = "\"";
            CheckMatchesExpected_WRITER_ (v1, "\"\\\"\"\n");
        }
        {
            // array
            vector<VariantValue> v;
            v.push_back (3);
            v.push_back (7);
            v.push_back ("cookie");
            VariantValue v1 = VariantValue{v};
            CheckMatchesExpected_WRITER_ (v1, "[\n    3,\n    7,\n    \"cookie\"\n]\n");
        }
        {
            // object
            map<wstring, VariantValue> v;
            v[L"Arg1"]      = 32;
            v[L"Arg2"]      = L"Cookies";
            v[L"Arg3"]      = Containers::Sequence<VariantValue> ({19});
            VariantValue v1 = VariantValue{v};
            CheckMatchesExpected_WRITER_ (v1,
                                          "{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}\n");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_02_BasicReaderTests_)
    {
        auto CheckMatchesExpected_READER_ = [] (const string& v, const VariantValue& expected) {
            stringstream tmp;
            tmp << v;
            VariantValue v1 = DataExchange::Variant::JSON::Reader{}.Read (tmp);
            EXPECT_TRUE (v1 == expected);
        };
        {
            VariantValue v1 = "hello world";
            CheckMatchesExpected_READER_ ("\"hello world\"", v1);
        }
        {
            VariantValue v1 = 3;
            CheckMatchesExpected_READER_ ("3", v1);
        }
        {
            VariantValue v1 = L"\uFDD0";
            CheckMatchesExpected_READER_ ("\"\\uFDD0\"", v1);
        }
        {
            VariantValue v1 = 4.7;
            CheckMatchesExpected_READER_ ("4.7", v1);
        }
        {
            // array
            vector<VariantValue> v;
            v.push_back (3);
            v.push_back (7);
            v.push_back ("cookie");
            VariantValue v1 = VariantValue{v};
            CheckMatchesExpected_READER_ ("[\n    3,\n    7,\n    \"cookie\"\n]", v1);
        }
        {
            // object
            map<wstring, VariantValue> v;
            v[L"Arg1"]      = 32;
            v[L"Arg2"]      = L"Cookies";
            v[L"Arg3"]      = Containers::Sequence<VariantValue> ({19});
            VariantValue v1 = VariantValue{v};
            CheckMatchesExpected_READER_ ("{\n    \"Arg1\" : 32,\n    \"Arg2\" : \"Cookies\",\n    \"Arg3\" : [\n        19\n    ]\n}", v1);
        }
        {
            // Bug found in another JSON reader (sent me by Ryan - 2011-07-27)
            const string kExample =
                "{\"nav_items\":[{\"main_link\":{\"href\":\"/about/index.html\",\"text\":\"Who We "
                "Are\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/"
                "company-management.html\",\"text\":\"Management\"}},{\"header\":{\"href\":\"/about/"
                "mission-statement.html\",\"text\":\"Mission\"}},{\"header\":{\"href\":\"/about/company-history.html\",\"text\":\" "
                "History\"}},{\"header\":{\"href\":\"/about/headquarters.html\",\"text\":\"Corporate "
                "Headquarters\"}},{\"header\":{\"href\":\"/about/diversity.html\",\"text\":\"Diversity\"}},{\"header\":{\"href\":\"/"
                "about/"
                "supplier-diversity.html\",\"text\":\"Supplier "
                "Diversity\"}}]}]},{\"main_link\":{\"href\":\"http://investor.compuware.com\",\"text\":\"Investor "
                "Relations\"}},{\"main_link\":{\"href\":\"/about/newsroom.html\",\"text\":\"News "
                "Room\"},\"column\":[{\"link_list\":[{},{\"header\":{\"href\":\"/about/analyst-reports\",\"text\":\"Analyst "
                "Reports\"}},{\"header\":{\"href\":\"/about/awards-recognition.html\",\"text\":\"Awards and "
                "Recognition\"}},{\"header\":{\"href\":\"/about/blogs.html\",\"text\":\"Blog "
                "Home\"}},{\"header\":{\"href\":\"/about/press-analyst-contacts.html\",\"text\":\"Contact "
                "Us\"}},{\"header\":{\"href\":\"/about/customers.html\",\"text\":\"Customers\"}},{\"header\":{\"href\":\"/about/"
                "press-mentions\",\"text\":\"Press Mentions\"}},{\"header\":{\"href\":\"/about/press-releases\",\"text\":\"Press "
                "Releases\"}},{\"header\":{\"href\":\"/about/press-resources.html\",\"text\":\"Press "
                "Resources\"}}]}]},{\"main_link\":{\"href\":\"#top\",\"text\":\"Sponsorships\"},\"column\":[{\"link_list\":[{"
                "\"header\":{"
                "\"href\":\"/about/lemans-sponsorship.html\",\"text\":\"Le "
                "Mans\"}},{\"header\":{\"href\":\"/about/nhl-sponsorship.html\",\"text\":\"NHL\"}},{}]}]},{\"main_link\":{\"href\":\"/"
                "about/community-involvement.html\",\"text\":\"Community "
                "Involvement\"},\"column\":[{\"link_list\":[{\"header\":{\"href\":\"http://"
                "communityclicks.compuware.com\",\"text\":\"Community Clicks "
                "Blog\"}},{\"header\":{\"href\":\"javascript:securenav('/forms/grant-eligibility-form.html')\",\"text\":\"Grant "
                "Eligibility Form\"}},{}]}]},{\"main_link\":{\"href\":\"/government/\",\"text\":\"Government\"}}]}";
            stringstream tmp;
            tmp << kExample;
            VariantValue v1 = DataExchange::Variant::JSON::Reader{}.Read (tmp);
            EXPECT_TRUE (v1.GetType () == VariantValue::eMap);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_03_CheckCanReadFromSmallBadSrc_)
    {
        auto VerifyThisStringFailsToParse_ = [] (const string& s) {
            stringstream tmp;
            tmp << s;
            try {
                VariantValue v1 = DataExchange::Variant::JSON::Reader{}.Read (tmp);
                EXPECT_TRUE (false); // should get exception
            }
            catch (const DataExchange::BadFormatException&) {
                // GOOD
            }
            catch (...) {
                EXPECT_TRUE (false); // should get BadFormatException
            }
        };
        VerifyThisStringFailsToParse_ ("n");
        VerifyThisStringFailsToParse_ ("'");
        VerifyThisStringFailsToParse_ ("\"");
        VerifyThisStringFailsToParse_ ("[");
        VerifyThisStringFailsToParse_ ("}");
        VerifyThisStringFailsToParse_ ("]");
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_04_CheckStringQuoting_)
    {
        Debug::TraceContextBumper ctx{"Test_04_CheckStringQuoting_::DoAll_"};
        auto                      CheckRoundtrip_encode_decode_unchanged = [] (const VariantValue& v) {
            string encodedRep;
            {
                Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
                DataExchange::Variant::JSON::Writer{}.Write (v, out);
                encodedRep = out.As<string> ();
            }
            {
                stringstream tmp;
                tmp << encodedRep;
                VariantValue vOut = DataExchange::Variant::JSON::Reader{}.Read (tmp);
                EXPECT_TRUE (vOut == v);
            }
        };
        CheckRoundtrip_encode_decode_unchanged (VariantValue{L"\t\r\n\f\x3"}); // proper read/write control characters
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"test\?"});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"test\\?"});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"cookie"});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"c:\\"});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"'"});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"\""});
        //this test is OK, but makes no sense, and isn't testing what it appears to--- CheckRoundtrip_encode_decode_unchanged (VariantValue{L"\\u20a9")); //  ₩
        CheckRoundtrip_encode_decode_unchanged (VariantValue{L"\u20a9"}); //  ₩
        CheckRoundtrip_encode_decode_unchanged (VariantValue{"\"apple\""});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<int>::min ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<int>::max ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<long int>::min ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<long int>::max ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<long long int>::min ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<long long int>::max ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<unsigned int>::min ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<unsigned int>::max ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<unsigned long int>::min ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<unsigned long int>::max ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<unsigned long long int>::min ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<unsigned long long int>::max ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{true});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{Memory::BLOB::FromHex ("aa1234abcd01010102030405")});
    }

    /*
     *  A BLOB as text - so in JSON - is base64 on one line, as RFC 4648 (section 3.1) has it unless a format says otherwise. Before
     *  Stroika v3.0d25 it had a CRLF every 76 characters, which strict readers refuse.
     */
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_BLOBAsBase64OnOneLine_)
    {
        Debug::TraceContextBumper ctx{"JSON_ONLY_BLOBAsBase64OnOneLine_"};
        vector<byte>              bytes;
        for (int i = 0; i < 100; ++i) {
            bytes.push_back (static_cast<byte> (i));
        }
        const Memory::BLOB blob{bytes};
        const String       text = VariantValue{blob}.As<String> ();
        EXPECT_FALSE (text.Contains ("\r"sv) or text.Contains ("\n"sv)) << text.AsNarrowSDKString ();
        const String json = DataExchange::Variant::JSON::Writer{}.WriteAsString (VariantValue{blob});
        EXPECT_FALSE (json.Contains ("\\r"sv) or json.Contains ("\\n"sv)) << json.AsNarrowSDKString (); // JSON's escapes for them
        EXPECT_EQ (DataExchange::Variant::JSON::Reader{}.Read (json).As<Memory::BLOB> (), blob);
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_05_ParseRegressionTest_1_)
    {
        Debug::TraceContextBumper ctx{"Test_05_ParseRegressionTest_1_::DoAll_"};
        {
            const char   kJSONExample_[] = "{"
                                           "    \"Automated Backups\" : {"
                                           "        \"From\" : {"
                                           "            \"CurrentHRWildcard\" : true,"
                                           "            \"PrintName\" : \"{Current HR}\""
                                           "        },"
                                           "        \"LastRanAt\" : {"
                                           "            \"ID-ca22f72c-9ff5-4082-82d0-d9763c64ddd6\" : \"2013-03-03T13:53:05-05:00\""
                                           "        },"
                                           "        \"Operation\" : 0,"
                                           "        \"Output\" : {"
                                           "            \"AttachmentPolicy\" : 2,"
                                           "            \"Format\" : \"application/x-healthframe-snapshotphr-3\","
                                           "            \"MaxFiles\" : 0,"
                                           "            \"NamePolicy\" : 1,"
                                           "            \"Password\" : \"\""
                                           "        },"
                                           "        \"PolicyName\" : \"Automated Backups\","
                                           "        \"Schedule\" : 2,"
                                           "        \"To\" : {"
                                           "            \"DefaultBackupDirectory\" : true,"
                                           "            \"PrintName\" : \"{Default Backup Directory}\""
                                           "        }"
                                           "    }"
                                           "}";
            VariantValue v               = DataExchange::Variant::JSON::Reader{}.Read (
                Streams::ExternallyOwnedSpanInputStream::New<byte> (Memory::SpanBytesCast<span<const byte>> (span{kJSONExample_})));
            map<wstring, VariantValue> mv = v.As<map<wstring, VariantValue>> ();
            EXPECT_EQ (mv[L"Automated Backups"].GetType (), VariantValue::eMap);
            map<wstring, VariantValue> outputMap = v.As<map<wstring, VariantValue>> ()[L"Output"].As<map<wstring, VariantValue>> ();
            outputMap[L"MaxFiles"]               = 123456789;
            mv[L"Output"]                        = outputMap;
            v                                    = mv;

            string jsonExampleWithUpdatedMaxFilesReference;
            {
                Streams::MemoryStream::Ptr<byte> tmpStrm = Streams::MemoryStream::New<byte> ();
                DataExchange::Variant::JSON::Writer{}.Write (v, tmpStrm);
                jsonExampleWithUpdatedMaxFilesReference = tmpStrm.As<string> ();
            }
            {
                // Verify change of locale has no effect on results
                locale                           prevLocale = locale::global (locale{"C"});
                Streams::MemoryStream::Ptr<byte> tmpStrm    = Streams::MemoryStream::New<byte> ();
                DataExchange::Variant::JSON::Writer{}.Write (v, tmpStrm);
                EXPECT_EQ (jsonExampleWithUpdatedMaxFilesReference, tmpStrm.As<string> ());
                locale::global (prevLocale);
            }
            try {
                // Verify change of locale has no effect on results
                Common::ScopedUseLocale          tmpLocale{Common::FindNamedLocale ("en"sv, "us"sv)};
                Streams::MemoryStream::Ptr<byte> tmpStrm = Streams::MemoryStream::New<byte> ();
                DataExchange::Variant::JSON::Writer{}.Write (v, tmpStrm);
                EXPECT_EQ (jsonExampleWithUpdatedMaxFilesReference, tmpStrm.As<string> ());
            }
            catch ([[maybe_unused]] const Common::LocaleNotFoundException& e) {
                SkipTestPart ("en_US locale not installed");
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_06_ParseRegressionTest_2_)
    {
        Debug::TraceContextBumper ctx{"Test_06_ParseRegressionTest_2_::DoAll_"};
        auto                      f = [] () {
            map<wstring, VariantValue> mv;
            mv[L"MaxFiles"] = VariantValue{405};
            VariantValue v  = VariantValue{mv};

            string encoded;
            {
                stringstream tmpStrm;
                DataExchange::Variant::JSON::Writer{}.Write (v, tmpStrm);
                encoded = tmpStrm.str ();
            }
            stringstream tnmStrStrm{encoded};
            VariantValue v1 = DataExchange::Variant::JSON::Reader{}.Read (tnmStrStrm);
            EXPECT_EQ (v1, v);
        };
        f ();
        try {
            Common::ScopedUseLocale tmpLocale{Common::FindNamedLocale ("en", "us")};
            f ();
        }
        catch ([[maybe_unused]] const Common::LocaleNotFoundException& e) {
            SkipTestPart ("en_US locale not installed");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_05_ParseRegressionTest_3_)
    {
        Debug::TraceContextBumper ctx{"Test_05_ParseRegressionTest_3_::DoAll_"};
        {
            const char kJSONExample_[] = R"({
                                            "T1" : "",
                                            "T2" : null,
                                            "T3" : {
                                                 "DefaultBackupDirectory" : true,
                                                 "PrintName" : "{Default Backup Directory}"
                                             }
                                         })";
            VariantValue v = DataExchange::Variant::JSON::Reader{}.Read (Streams::ExternallyOwnedSpanInputStream::New<byte> (span{kJSONExample_}));
            Mapping<String, VariantValue> mv = v.As<Mapping<String, VariantValue>> ();
            EXPECT_EQ (mv["T1"].GetType (), VariantValue::eString);
            EXPECT_EQ (mv["T1"], String{});
            EXPECT_EQ (mv["T2"].GetType (), VariantValue::eNull);
            EXPECT_EQ (mv["T3"].GetType (), VariantValue::eMap);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_07_ParserTestReadWriteBasictypes_)
    {
        Debug::TraceContextBumper ctx{"Test_07_ParserTestReadWriteBasictypes_::DoAll_"};
        using namespace Time;
        auto f = [] (VariantValue v) {
            string encoded;
            {
                stringstream tmpStrm;
                DataExchange::Variant::JSON::Writer{}.Write (v, tmpStrm);
                encoded = tmpStrm.str ();
            }
            stringstream tnmStrStrm (encoded);
            VariantValue v1 = DataExchange::Variant::JSON::Reader{}.Read (tnmStrStrm);
            // JSON reader comes back with strings - because date/datetime are not native types
            if (v.GetType () == VariantValue::eDate and v1.GetType () == VariantValue::eString) {
                v1 = VariantValue{v1.As<Time::Date> ()};
            }
            if (v.GetType () == VariantValue::eDateTime and v1.GetType () == VariantValue::eString) {
                v1 = VariantValue{v1.As<Time::DateTime> ()};
            }
            if (v.GetType () == VariantValue::eFloat) {
                EXPECT_TRUE (Math::NearlyEquals (v1.As<double> (), v.As<double> (), 0.11));
            }
            else {
                EXPECT_EQ (v1, v);
            }
        };
        auto doAll = [f] () {
            f (VariantValue{405});
            f (VariantValue{4405});
            f (VariantValue{44905});
            f (VariantValue{405.1});
            f (VariantValue{4405.2});
            f (VariantValue{44905.3});
            f (VariantValue{"'"});
            f (VariantValue{Date{Year{1933}, February, day{12}}});
            f (VariantValue{DateTime{Date{Year{1933}, February, day{12}}, TimeOfDay{432}}});

            {
                stringstream tmpStrm;
                DataExchange::Variant::JSON::Writer{}.Write (VariantValue{44905.3}, tmpStrm);
                string tmp = tmpStrm.str ();
                EXPECT_TRUE (tmp.find (",") == string::npos);
            }
        };
        try {
            doAll ();
            Common::ScopedUseLocale tmpLocale{Common::FindNamedLocale ("en"sv, "us"sv)};
            doAll ();
        }
        catch ([[maybe_unused]] const Common::LocaleNotFoundException& e) {
            SkipTestPart ("en_US locale not installed");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSON_ONLY_Test_08_ReadEmptyStreamShouldFail__)
    {
        Debug::TraceContextBumper ctx{"Test_08_ReadEmptyStreamShouldFail_::DoAll_"};
        try {
            VariantValue vOut = DataExchange::Variant::JSON::Reader{}.Read (Streams::MemoryStream::New<byte> ());
            EXPECT_TRUE (false);
        }
        catch (const DataExchange::BadFormatException&) {
            // Good - this should fail
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSONONLY_Test_09_ReadWriteNANShouldNotFail_)
    {
        Debug::TraceContextBumper ctx{"Test_09_ReadWriteNANShouldNotFail_::DoAll_"};
        auto                      CheckRoundtrip_encode_decode_unchanged = [] (const VariantValue& v) {
            string encodedRep;
            {
                Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
                DataExchange::Variant::JSON::Writer{}.Write (v, out);
                encodedRep = out.As<string> ();
            }
            {
                stringstream tmp;
                tmp << encodedRep;
                VariantValue vOut = DataExchange::Variant::JSON::Reader{}.Read (tmp);
                if (Debug::IsRunningUnderValgrind () and vOut != v) {
                    Stroika::Frameworks::Test::WarnTestIssue (
                        "JSONONLY_Test_09_ReadWriteNANShouldNotFail_ (valgrind holds long double in 64 bits): v={}, encodedRep={}, vOut={}"_f(
                            v, String::FromUTF8 (encodedRep), vOut)
                            .ReplaceAll ("[\\r\\n]"_RegEx, ""));
                    SkipTestPart ("NaN round trip not checked under valgrind (valgrind holds long double in 64 bits)");
                    return;
                }
                EXPECT_EQ (vOut, v);
            }
        };
        CheckRoundtrip_encode_decode_unchanged (VariantValue{Math::nan<double> ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{-numeric_limits<double>::infinity ()});
        CheckRoundtrip_encode_decode_unchanged (VariantValue{numeric_limits<double>::infinity ()});
    }
}

namespace {
    /*
     *  This section is for using the direct - XML-only APIs, and verifying the results look like good XML
     */
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, XML_ONLY_)
    {
        using DataExchange::Variant::XML::Binding;
        using DataExchange::Variant::XML::Writer;
        // what the writer makes of a value, as a String (so no XML declaration - @see Variant::XML::Writer)
        auto written = [] (const VariantValue& v, const Binding& binding) { return Writer{{.fBinding = binding}}.WriteAsString (v); };
        {
            // the Binding names the document element, and a scalar is its text
            const Binding maxFiles{.fName = "MaxFiles"sv};
            EXPECT_EQ (written (VariantValue{405}, maxFiles), "<MaxFiles>405</MaxFiles>\n"sv);
            EXPECT_EQ (written (VariantValue{true}, maxFiles), "<MaxFiles>true</MaxFiles>\n"sv);
            EXPECT_EQ (written (VariantValue{}, maxFiles), "<MaxFiles></MaxFiles>\n"sv);                     // null: an empty element
            EXPECT_EQ (written (VariantValue{"<&>"sv}, maxFiles), "<MaxFiles>&lt;&amp;&gt;</MaxFiles>\n"sv); // quoted
        }
        {
            // with none named - the default - the value is an object of one member, and that member is the document element
            EXPECT_EQ (written (VariantValue{Mapping<String, VariantValue>{{"a"sv, VariantValue{1}}}}, Binding{}), "<a>1</a>\n"sv);
        }
        {
            // an array: an <Array> per item, so an array of one still reads as an array
            VariantValue v{Sequence<VariantValue>{VariantValue{"a"sv}, VariantValue{"b"sv}}};
            EXPECT_EQ (written (v, Binding{.fName = "list"sv}), "<list>\n    <Array>a</Array>\n    <Array>b</Array>\n</list>\n"sv);
            // or items of another name, from the default array items
            EXPECT_EQ (written (v, Binding{.fName = "list"sv, .fDefaultArrayItems = make_shared<Binding> (Binding{.fName = "item"sv})}),
                       "<list>\n    <item>a</item>\n    <item>b</item>\n</list>\n"sv);
            // or, where those name none, the member's own element repeated - how a schema of someone else's usually has it
            EXPECT_EQ (written (VariantValue{Mapping<String, VariantValue>{{"service"sv, v}}},
                                Binding{.fName = "serviceList"sv, .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)}),
                       "<serviceList>\n    <service>a</service>\n    <service>b</service>\n</serviceList>\n"sv);
        }
        {
            // nested objects, indented; members in the order the Mapping gives them (none in particular; a Binding gives one)
            VariantValue v{Mapping<String, VariantValue>{
                {"specVersion"sv, VariantValue{Mapping<String, VariantValue>{{"major"sv, VariantValue{1}}, {"minor"sv, VariantValue{0}}}}},
                {"UDN"sv, VariantValue{"uuid:abc"sv}}}};
            EXPECT_EQ (written (v, Binding{.fName = "device"sv}), "<device>\n"
                                                                  "    <UDN>uuid:abc</UDN>\n"
                                                                  "    <specVersion>\n"
                                                                  "        <major>1</major>\n"
                                                                  "        <minor>0</minor>\n"
                                                                  "    </specVersion>\n"
                                                                  "</device>\n"sv);
        }
        {
            // Variant::XML::Writer's own Example Usage, verbatim - here so the documentation cannot drift from what is written
            VariantValue v{Mapping<String, VariantValue>{
                {"specVersion"sv, VariantValue{Mapping<String, VariantValue>{{"major"sv, VariantValue{1}}}}},
                {"serviceList"sv, VariantValue{Mapping<String, VariantValue>{
                                      {"service"sv, VariantValue{Sequence<VariantValue>{VariantValue{"a"sv}, VariantValue{"b"sv}}}}}}}}};
            // <service> repeated, not <Array>s
            Binding device{.fName = "device"sv, .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
            EXPECT_EQ (Writer{{.fBinding = device}}.WriteAsString (v), "<device>\n"
                                                                       "    <serviceList>\n"
                                                                       "        <service>a</service>\n"
                                                                       "        <service>b</service>\n"
                                                                       "    </serviceList>\n"
                                                                       "    <specVersion>\n"
                                                                       "        <major>1</major>\n"
                                                                       "    </specVersion>\n"
                                                                       "</device>\n"sv);
        }
        {
            // the byte overload writes a document, so it declares its encoding
            Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
            Writer{{.fBinding = Binding{.fName = "a"sv}}}.Write (VariantValue{1}, out);
            EXPECT_EQ (out.As<string> (), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<a>1</a>\n");
        }
        {
            // not pretty printed: no whitespace between elements, nor after the declaration, nor at the end; or another indent
            VariantValue  v{Mapping<String, VariantValue>{
                {"UDN"sv, VariantValue{"uuid:abc"sv}}, {"specVersion"sv, VariantValue{Mapping<String, VariantValue>{{"major"sv, VariantValue{1}}}}}}};
            const Binding device{.fName = "device"sv};
            EXPECT_EQ ((Writer{{.fBinding = device, .fPrettyPrint = false}}.WriteAsString (v)),
                       "<device><UDN>uuid:abc</UDN><specVersion><major>1</major></specVersion></device>"sv);
            Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
            Writer{{.fBinding = device, .fPrettyPrint = false}}.Write (v, out);
            EXPECT_EQ (out.As<string> (),
                       "<?xml version=\"1.0\" "
                       "encoding=\"UTF-8\"?><device><UDN>uuid:abc</UDN><specVersion><major>1</major></specVersion></device>");
            EXPECT_EQ ((Writer{{.fBinding = device, .fSpacesPerIndent = 1}}.WriteAsString (v)),
                       "<device>\n <UDN>uuid:abc</UDN>\n <specVersion>\n  <major>1</major>\n </specVersion>\n</device>\n"sv);
        }
        {
            // a key that is not an XML name cannot be written as one - it would make malformed XML - so the Writer throws
            using DataExchange::XML::NameWithNamespace;
            using IO::Network::URI;
            using VVMap = Mapping<String, VariantValue>;
            EXPECT_THROW (written (VariantValue{VVMap{{"my key"sv, VariantValue{"x"sv}}}}, Binding{}), BadFormatException);
            EXPECT_THROW (written (VariantValue{VVMap{{"1a"sv, VariantValue{"x"sv}}}}, Binding{}), BadFormatException);
            EXPECT_THROW (written (VariantValue{VVMap{{"a"sv, VariantValue{VVMap{{"b:c"sv, VariantValue{1}}}}}}}, Binding{}), BadFormatException);
            EXPECT_THROW (written (VariantValue{VVMap{{""sv, VariantValue{1}}}}, Binding{}), BadFormatException);
            // ... nor can a Binding's prefix that is not one
            const URI kNS_{"urn:a"sv};
            EXPECT_THROW (written (VariantValue{1}, Binding{.fName = NameWithNamespace{kNS_, "a"sv}, .fNamespacePrefixes = {{kNS_, "1p"sv}}}),
                          BadFormatException);
            // but a name need not be ASCII
            EXPECT_EQ (written (VariantValue{VVMap{{String{L"caf\u00e9"}, VariantValue{1}}}}, Binding{}),
                       String{L"<caf\u00e9>1</caf\u00e9>\n"});
        }
        {
            // a document has exactly one root element: with none named, a value that is not an object of exactly one member
            // has nothing to be it - and a VariantValue is data, not code, so writing it throws, as any unwritable value does
            using VVMap = Mapping<String, VariantValue>;
            EXPECT_THROW (written (VariantValue{3}, Binding{}), BadFormatException);
            EXPECT_THROW (written (VariantValue{Sequence<VariantValue>{VariantValue{1}}}, Binding{}), BadFormatException);
            EXPECT_THROW (written (VariantValue{VVMap{{"a"sv, VariantValue{1}}, {"b"sv, VariantValue{2}}}}, Binding{}), BadFormatException);
            EXPECT_THROW (written (VariantValue{VVMap{}}, Binding{}), BadFormatException);
            // ... nor can the document element be repeated, for an array whose items have no name of their own
            const Binding repeated{.fName = "a"sv, .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
            EXPECT_THROW (written (VariantValue{Sequence<VariantValue>{VariantValue{1}, VariantValue{2}}}, repeated), BadFormatException);
            EXPECT_EQ (written (VariantValue{Sequence<VariantValue>{VariantValue{1}}}, repeated), "<a>1</a>\n"sv); // one item: one root
        }
    }
}

namespace {
    /*
     *  A Binding - what a schema of someone else's needs, and the default shape cannot say: an attribute rather than an
     *  element, a namespace, a name of its own, and an order of its own (a VariantValue object has none; XML cares).
     */
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, XML_ONLY_Binding_)
    {
        using DataExchange::Variant::XML::Binding;
        using DataExchange::Variant::XML::Writer;
        using DataExchange::XML::NameWithNamespace;
        using IO::Network::URI;
        using VVMap  = Mapping<String, VariantValue>;
        auto written = [] (const VariantValue& v, const Binding& binding) { return Writer{{.fBinding = binding}}.WriteAsString (v); };
        {
            // UPnP's <stateVariable sendEvents="yes">: an attribute, and name before dataType - which the value, a Mapping,
            // has no say in (it would give dataType, name, sendEvents, sorted)
            const Binding stateVariable{.fName = "stateVariable"sv,
                                        .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute}}, {"name"sv}, {"dataType"sv}}};
            VariantValue v{VVMap{{"dataType"sv, VariantValue{"boolean"sv}}, {"name"sv, VariantValue{"Status"sv}}, {"sendEvents"sv, VariantValue{"yes"sv}}}};
            EXPECT_EQ (written (v, stateVariable), "<stateVariable sendEvents=\"yes\">\n"
                                                   "    <name>Status</name>\n"
                                                   "    <dataType>boolean</dataType>\n"
                                                   "</stateVariable>\n"sv);
        }
        {
            // a namespace, written as a default one on the element that has it - and inherited, so written once
            const Binding root{.fName    = NameWithNamespace{URI{"urn:schemas-upnp-org:device-1-0"sv}, "root"sv},
                               .fMembers = {{"specVersion"sv}, {"URLBase"sv}}};
            VariantValue  v{VVMap{{"URLBase"sv, VariantValue{"http://1.2.3.4/"sv}},
                                  {"specVersion"sv, VariantValue{VVMap{{"major"sv, VariantValue{1}}}}}}};
            EXPECT_EQ (written (v, root), "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                                          "    <specVersion>\n"
                                          "        <major>1</major>\n"
                                          "    </specVersion>\n"
                                          "    <URLBase>http://1.2.3.4/</URLBase>\n"
                                          "</root>\n"sv);
        }
        {
            // a member named something else, a member that is the element's text, and an array whose item Binding names no item
            // element - so the member's own element, repeated
            const Binding serviceList{
                .fName    = "serviceList"sv,
                .fMembers = {{"services"sv, Binding{.fName  = "service"sv,
                                                    .fItems = make_shared<Binding> (
                                                        Binding{.fMembers = {{"id"sv, Binding{.fKind = Binding::Kind::eAttribute}},
                                                                             {"type"sv, Binding{.fKind = Binding::Kind::eText}}}})}}}};
            VariantValue v{VVMap{{"services"sv, VariantValue{Sequence<VariantValue>{
                                                    VariantValue{VVMap{{"id"sv, VariantValue{1}}, {"type"sv, VariantValue{"SwitchPower"sv}}}},
                                                    VariantValue{VVMap{{"id"sv, VariantValue{2}}, {"type"sv, VariantValue{"Dimming"sv}}}}}}}}};
            EXPECT_EQ (written (v, serviceList), "<serviceList>\n"
                                                 "    <service id=\"1\">SwitchPower</service>\n"
                                                 "    <service id=\"2\">Dimming</service>\n"
                                                 "</serviceList>\n"sv);
        }
        {
            // a prefixed namespace: declared on the element whose Binding gives it, and used by every element and attribute in
            // that namespace there and inside - the only way to write an attribute in a namespace at all. SOAP's envelope, as in
            // Binding::fNamespacePrefixes' Example Usage
            const URI     kSOAP_{"http://schemas.xmlsoap.org/soap/envelope/"sv};
            const Binding envelope{
                .fName = NameWithNamespace{kSOAP_, "Envelope"sv},
                .fMembers = {{"encodingStyle"sv, Binding{.fKind = Binding::Kind::eAttribute, .fName = NameWithNamespace{kSOAP_, "encodingStyle"sv}}},
                             {"Body"sv, Binding{.fName = NameWithNamespace{kSOAP_, "Body"sv}}}},
                .fNamespacePrefixes = {{kSOAP_, "s"sv}}};
            VariantValue v{VVMap{{"encodingStyle"sv, VariantValue{"http://schemas.xmlsoap.org/soap/encoding/"sv}},
                                 {"Body"sv, VariantValue{VVMap{{"GetStatus"sv, VariantValue{""sv}}}}}}};
            EXPECT_EQ (written (v, envelope), "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                                              "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n"
                                              "    <s:Body>\n"
                                              "        <GetStatus></GetStatus>\n"
                                              "    </s:Body>\n"
                                              "</s:Envelope>\n"sv);
            // a prefix declared on an inner element: UPnP's action, in its service's namespace - <u:GetStatus xmlns:u="...">
            const URI     kSwitchPower_{"urn:schemas-upnp-org:service:SwitchPower:1"sv};
            const Binding withAction{
                .fName = NameWithNamespace{kSOAP_, "Envelope"sv},
                .fMembers = {{"Body"sv, Binding{.fName = NameWithNamespace{kSOAP_, "Body"sv},
                                                .fMembers = {{"GetStatus"sv, Binding{.fName = NameWithNamespace{kSwitchPower_, "GetStatus"sv},
                                                                                     .fNamespacePrefixes = {{kSwitchPower_, "u"sv}}}}}}}},
                .fNamespacePrefixes = {{kSOAP_, "s"sv}}};
            EXPECT_EQ (written (VariantValue{VVMap{{"Body"sv, VariantValue{VVMap{{"GetStatus"sv, VariantValue{""sv}}}}}}}, withAction),
                       "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
                       "    <s:Body>\n"
                       "        <u:GetStatus xmlns:u=\"urn:schemas-upnp-org:service:SwitchPower:1\"></u:GetStatus>\n"
                       "    </s:Body>\n"
                       "</s:Envelope>\n"sv);
            // with no document element named, the top level Binding's prefixes are declared on the one element it has
            const Binding document{.fMembers           = {{"Envelope"sv, Binding{.fName = NameWithNamespace{kSOAP_, "Envelope"sv}}}},
                                   .fNamespacePrefixes = {{kSOAP_, "s"sv}}};
            EXPECT_EQ (written (VariantValue{VVMap{{"Envelope"sv, VariantValue{""sv}}}}, document),
                       "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"></s:Envelope>\n"sv);
        }
        {
            // a prefix declared again inside, for another namespace, names only that one there - so an element inside it, in the
            // first namespace, has a default namespace instead
            const URI     kA_{"urn:a"sv};
            const URI     kB_{"urn:b"sv};
            const Binding a{.fName              = NameWithNamespace{kA_, "a"sv},
                            .fMembers           = {{"b"sv, Binding{.fName              = NameWithNamespace{kB_, "b"sv},
                                                                   .fMembers           = {{"c"sv, Binding{.fName = NameWithNamespace{kA_, "c"sv}}}},
                                                                   .fNamespacePrefixes = {{kB_, "p"sv}}}}},
                            .fNamespacePrefixes = {{kA_, "p"sv}}};
            EXPECT_EQ (written (VariantValue{VVMap{{"b"sv, VariantValue{VVMap{{"c"sv, VariantValue{1}}}}}}}, a),
                       "<p:a xmlns:p=\"urn:a\">\n"
                       "    <p:b xmlns:p=\"urn:b\">\n"
                       "        <c xmlns=\"urn:a\">1</c>\n"
                       "    </p:b>\n"
                       "</p:a>\n"sv);
        }
        {
            // Binding::fItems' own example, verbatim: items with a name of their own, wrapped in the member's element
            // {"allowedValueList": ["yes", "no"]}
            const Binding b =
                Binding{.fMembers = {{"allowedValueList"sv, Binding{.fItems = make_shared<Binding> (Binding{.fName = "allowedValue"sv})}}}};
            VariantValue v{VVMap{{"allowedValueList"sv, VariantValue{Sequence<VariantValue>{VariantValue{"yes"sv}, VariantValue{"no"sv}}}}}};
            EXPECT_EQ (written (v, b), "<allowedValueList>\n"
                                       "    <allowedValue>yes</allowedValue>\n"
                                       "    <allowedValue>no</allowedValue>\n"
                                       "</allowedValueList>\n"sv);
        }
        {
            // a member the Binding says nothing about is written after those it does, in the value's own order
            VariantValue v{VVMap{{"m"sv, VariantValue{1}}, {"z"sv, VariantValue{2}}}};
            EXPECT_EQ (written (v, Binding{.fName = "a"sv, .fMembers = {{"z"sv}}}), "<a>\n    <z>2</z>\n    <m>1</m>\n</a>\n"sv);
        }
        {
            // an item Binding with a name of its own wraps that array, each item of that name - so one document can wrap two
            // arrays differently. UPnP's service description
            auto          itemsNamed = [] (const String& itemName) { return make_shared<Binding> (Binding{.fName = itemName}); };
            const Binding scpd{.fName    = "scpd"sv,
                               .fMembers = {{"actionList"sv, Binding{.fItems = itemsNamed ("action"sv)}},
                                            {"allowedValueList"sv, Binding{.fItems = itemsNamed ("allowedValue"sv)}}}};
            VariantValue  v{
                VVMap{{"actionList"sv, VariantValue{Sequence<VariantValue>{VariantValue{VVMap{{"name"sv, VariantValue{"SetTarget"sv}}}},
                                                                           VariantValue{VVMap{{"name"sv, VariantValue{"GetTarget"sv}}}}}}},
                      {"allowedValueList"sv, VariantValue{Sequence<VariantValue>{VariantValue{"yes"sv}, VariantValue{"no"sv}}}}}};
            EXPECT_EQ (written (v, scpd), "<scpd>\n"
                                          "    <actionList>\n"
                                          "        <action>\n"
                                          "            <name>SetTarget</name>\n"
                                          "        </action>\n"
                                          "        <action>\n"
                                          "            <name>GetTarget</name>\n"
                                          "        </action>\n"
                                          "    </actionList>\n"
                                          "    <allowedValueList>\n"
                                          "        <allowedValue>yes</allowedValue>\n"
                                          "        <allowedValue>no</allowedValue>\n"
                                          "    </allowedValueList>\n"
                                          "</scpd>\n"sv);
            // an empty array is still its wrapper, and a one-item array is still an array - which is what a Binding settles
            VariantValue one{VVMap{{"actionList"sv, VariantValue{Sequence<VariantValue>{VariantValue{VVMap{{"name"sv, VariantValue{"X"sv}}}}}}},
                                   {"allowedValueList"sv, VariantValue{Sequence<VariantValue>{}}}}};
            EXPECT_EQ (written (one, scpd), "<scpd>\n"
                                            "    <actionList>\n"
                                            "        <action>\n"
                                            "            <name>X</name>\n"
                                            "        </action>\n"
                                            "    </actionList>\n"
                                            "    <allowedValueList></allowedValueList>\n"
                                            "</scpd>\n"sv);
        }
        {
            // default array items hold at and inside the element whose Binding gives them, until another's does - and kDefaultArrayItems
            // gives back the default shape's
            const Binding a{.fName    = "a"sv,
                            .fMembers = {{"inner"sv, Binding{.fDefaultArrayItems = make_shared<Binding> (Binding::kDefaultArrayItems)}}},
                            .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
            VariantValue v{VVMap{{"x"sv, VariantValue{Sequence<VariantValue>{VariantValue{1}, VariantValue{2}}}},
                                 {"inner"sv, VariantValue{VVMap{{"y"sv, VariantValue{Sequence<VariantValue>{VariantValue{3}, VariantValue{4}}}}}}}}};
            EXPECT_EQ (written (v, a), "<a>\n"
                                       "    <inner>\n"
                                       "        <y>\n"
                                       "            <Array>3</Array>\n"
                                       "            <Array>4</Array>\n"
                                       "        </y>\n"
                                       "    </inner>\n"
                                       "    <x>1</x>\n"
                                       "    <x>2</x>\n"
                                       "</a>\n"sv);
        }
    }
}

#if qStroika_Foundation_DataExchange_XML_SupportParsing
namespace {
    /*
     *  Variant::XML::Reader - the Writer's inverse: what it wrote reads back, allowing that XML says nothing of types, so every
     *  leaf reads as a String (VariantValue::As, and so ObjectVariantMapper::ToObject, coerce).
     */
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, XML_ONLY_RoundTrip_)
    {
        using DataExchange::Variant::XML::Binding;
        using DataExchange::Variant::XML::Reader;
        using DataExchange::Variant::XML::Writer;
        using VVMap    = Mapping<String, VariantValue>; // a comma in a template argument would end an EXPECT_EQ's first argument
        auto roundTrip = [] (const VariantValue& v, const Binding& binding) {
            return Reader{binding}.Read (Writer{{.fBinding = binding}}.WriteAsString (v));
        };
        {
            // Binding's own Example Usage, verbatim - here so the documentation cannot drift from what is written and read back
            // UPnP's <stateVariable>: sendEvents an attribute, and name before dataType (a VariantValue object has no order)
            VariantValue v{Mapping<String, VariantValue>{
                {"name"sv, VariantValue{"Status"sv}}, {"dataType"sv, VariantValue{"boolean"sv}}, {"sendEvents"sv, VariantValue{"yes"sv}}}};
            Binding      b{.fName    = "stateVariable"sv,
                           .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute}}, {"name"sv}, {"dataType"sv}}};
            String       xml  = Variant::XML::Writer{{.fBinding = b}}.WriteAsString (v);
            VariantValue back = Variant::XML::Reader{b}.Read (xml); // == v

            // With no fName, the document element is the value's one member: its name moves from the Binding into the value.
            // So the same XML again, from v wrapped in a member of that name - whose Binding is b:
            VariantValue w{Mapping<String, VariantValue>{{"stateVariable"sv, v}}};
            Binding      d{.fMembers = {{"stateVariable"sv, b}}};
            String       xml2  = Variant::XML::Writer{{.fBinding = d}}.WriteAsString (w); // == xml
            VariantValue back2 = Variant::XML::Reader{d}.Read (xml2);                     // == w
            VariantValue plain = Variant::XML::Reader{}.Read (xml);                       // == w too: read with no Binding at all

            EXPECT_EQ (xml, "<stateVariable sendEvents=\"yes\">\n"
                            "    <name>Status</name>\n"
                            "    <dataType>boolean</dataType>\n"
                            "</stateVariable>\n"sv);
            EXPECT_EQ (back, v);
            EXPECT_EQ (xml2, xml);
            EXPECT_EQ (back2, w);
            EXPECT_EQ (plain, w);
        }
        {
            // an object, and its leaves - each back as a String
            VariantValue v{VVMap{{"UDN"sv, VariantValue{"uuid:abc"sv}}, {"port"sv, VariantValue{80}}}};
            VariantValue back = roundTrip (v, Binding{.fName = "device"sv});
            EXPECT_EQ (back.As<VVMap> ().Lookup ("UDN"sv)->As<String> (), "uuid:abc"sv);
            EXPECT_EQ (back.As<VVMap> ().Lookup ("port"sv)->As<int> (), 80); // XML has no types: "80"
            EXPECT_EQ (back.As<VVMap> ().Lookup ("port"sv)->GetType (), VariantValue::eString);
        }
        {
            // an array, an <Array> per item - so an array of ONE still reads as an array
            for (const Sequence<VariantValue>& items :
                 {Sequence<VariantValue>{VariantValue{"a"sv}, VariantValue{"b"sv}}, Sequence<VariantValue>{VariantValue{"a"sv}}}) {
                VariantValue back = roundTrip (VariantValue{items}, Binding{.fName = "list"sv});
                EXPECT_EQ (back.GetType (), VariantValue::eArray);
                EXPECT_EQ (back.As<Sequence<VariantValue>> ().size (), items.size ());
                EXPECT_EQ (back.As<Sequence<VariantValue>> ()[0].As<String> (), "a"sv);
            }
        }
        {
            // with the member's own element repeated, a repeated element name is the array - how someone else's schema has it; but
            // then an array of one is indistinguishable from a scalar, and reads back as one
            const Binding serviceList{.fName = "serviceList"sv, .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
            VariantValue  two{VVMap{{"service"sv, VariantValue{Sequence<VariantValue>{VariantValue{"a"sv}, VariantValue{"b"sv}}}}}};
            EXPECT_EQ (roundTrip (two, serviceList), two);
            VariantValue one{VVMap{{"service"sv, VariantValue{Sequence<VariantValue>{VariantValue{"a"sv}}}}}};
            EXPECT_EQ (roundTrip (one, serviceList).As<VVMap> ().Lookup ("service"sv)->As<String> (), "a"sv);
            // ... unless a Binding says that member is an array: an item Binding naming no item element, so repeated too
            const Binding bound{.fName    = "serviceList"sv,
                                .fMembers = {{"service"sv, Binding{.fItems = make_shared<Binding> (Binding::kRepeatedArrayItems)}}}};
            EXPECT_EQ (roundTrip (one, bound), one);
            EXPECT_EQ (roundTrip (two, bound), two);
        }
        {
            // reading someone else's XML: an attribute reads as a member too, so nothing is silently dropped
            VVMap m = Reader{Binding{.fName = "stateVariable"sv}}
                          .Read (String{"<stateVariable sendEvents=\"yes\"><name>Status</name><dataType>boolean</dataType></stateVariable>"sv})
                          .As<VVMap> ();
            EXPECT_EQ (m.Lookup ("sendEvents"sv)->As<String> (), "yes"sv);
            EXPECT_EQ (m.Lookup ("name"sv)->As<String> (), "Status"sv);
            EXPECT_EQ (m.Lookup ("dataType"sv)->As<String> (), "boolean"sv);
        }
        {
            // through a Binding, both ways: an attribute, a renamed member, a member that is the element's text, a member that
            // is an array however few elements there are, and an order of its own - its items repeated, or wrapped
            const Sequence<Binding::Member> serviceMembers{{"id"sv, Binding{.fKind = Binding::Kind::eAttribute}},
                                                           {"type"sv, Binding{.fKind = Binding::Kind::eText}}};
            const Binding                   repeated{
                .fName = "serviceList"sv,
                .fMembers = {{"services"sv, Binding{.fName = "service"sv, .fItems = make_shared<Binding> (Binding{.fMembers = serviceMembers})}}}};
            const Binding wrapped{
                .fName = "serviceList"sv,
                .fMembers = {{"services"sv, Binding{.fItems = make_shared<Binding> (Binding{.fName = "service"sv, .fMembers = serviceMembers})}}}};
            for (const Sequence<VariantValue>& services :
                 {Sequence<VariantValue>{VariantValue{VVMap{{"id"sv, VariantValue{"1"sv}}, {"type"sv, VariantValue{"SwitchPower"sv}}}},
                                         VariantValue{VVMap{{"id"sv, VariantValue{"2"sv}}, {"type"sv, VariantValue{"Dimming"sv}}}}},
                  Sequence<VariantValue>{VariantValue{VVMap{{"id"sv, VariantValue{"1"sv}}, {"type"sv, VariantValue{"SwitchPower"sv}}}}}}) {
                VariantValue v{VVMap{{"services"sv, VariantValue{services}}}};
                EXPECT_EQ (roundTrip (v, repeated), v) << services.size (); // including the array of one
                EXPECT_EQ (roundTrip (v, wrapped), v) << services.size ();
            }
        }
        {
            // mixed content - text beside child elements - cannot be held, and reading on would drop the text silently
            Reader p{Binding{.fName = "p"sv}};
            EXPECT_THROW (p.Read (String{"<p>Hello <b>world</b>!</p>"sv}), BadFormatException);
            // ... array items among them too, as the default shape's <Array>s or a Binding's
            EXPECT_THROW (Reader{Binding{.fName = "list"sv}}.Read (String{"<list>hello<Array>a</Array></list>"sv}), BadFormatException);
            const Binding scpd{.fName    = "scpd"sv,
                               .fMembers = {{"actionList"sv, Binding{.fItems = make_shared<Binding> (Binding{.fName = "action"sv})}}}};
            EXPECT_THROW (Reader{scpd}.Read (String{"<scpd><actionList>hello<action>a</action></actionList></scpd>"sv}), BadFormatException);
            // but the whitespace a pretty-printed document has between elements is not mixed content
            EXPECT_NO_THROW (p.Read (String{"<p>\n    <b>world</b>\n</p>"sv}));
            // nor is text beside attributes alone: a Binding saying which member the text is reads it, and with none there is
            // no member to read it into - which is what the exception says
            EXPECT_EQ ((Reader{Binding{.fName = "p"sv, .fMembers = {{"text"sv, Binding{.fKind = Binding::Kind::eText}}}}}.Read (String{"<p id=\"1\">hello</p>"sv})),
                       (VariantValue{VVMap{{"id"sv, VariantValue{"1"sv}}, {"text"sv, VariantValue{"hello"sv}}}}));
            try {
                p.Read (String{"<p id=\"1\">hello</p>"sv});
                ADD_FAILURE () << "should have thrown";
            }
            catch (const BadFormatException& e) {
                const String what = Characters::ToString (e);
                EXPECT_EQ (what.Find ("child elements"sv), nullopt) << what.AsNarrowSDKString ();
                EXPECT_NE (what.Find ("attributes"sv), nullopt) << what.AsNarrowSDKString ();
            }
        }
        {
            // with no document element named - the default - the document element is the value's one member, as the Writer
            // takes it: named, and bound, as any member is
            EXPECT_EQ (Reader{}.Read (String{"<a><b>1</b></a>"sv}),
                       (VariantValue{VVMap{{"a"sv, VariantValue{VVMap{{"b"sv, VariantValue{"1"sv}}}}}}}));
            VariantValue dev{VVMap{{"dev"sv, VariantValue{VVMap{{"UDN"sv, VariantValue{"uuid:abc"sv}}}}}}};
            EXPECT_EQ (roundTrip (dev, Binding{.fMembers = {{"dev"sv, Binding{.fName = "device"sv}}}}), dev);
            VariantValue list{VVMap{{"list"sv, VariantValue{Sequence<VariantValue>{VariantValue{"a"sv}}}}}};
            EXPECT_EQ (roundTrip (list, Binding{.fDefaultArrayItems = make_shared<Binding> (Binding{.fName = "item"sv})}), list);
        }
        {
            // a Binding naming the document element reads only a document with that element - another is not what it describes
            const Binding stateVariable{.fName = "stateVariable"sv};
            EXPECT_THROW (Reader{stateVariable}.Read (String{"<scpd><x>1</x></scpd>"sv}), BadFormatException);
            // ... matched by its local name, as every member is, whatever namespace and prefix the document gives it
            EXPECT_EQ (Reader{stateVariable}.Read (String{"<s:stateVariable xmlns:s=\"urn:x\"><x>1</x></s:stateVariable>"sv}),
                       (VariantValue{VVMap{{"x"sv, VariantValue{"1"sv}}}}));
            // and with none named, any document element reads - as the value's one member
            EXPECT_EQ (Reader{}.Read (String{"<scpd><x>1</x></scpd>"sv}),
                       (VariantValue{VVMap{{"scpd"sv, VariantValue{VVMap{{"x"sv, VariantValue{"1"sv}}}}}}}));
        }
        {
            // array items BESIDE other members: the array is one member among them, and not lost
            VVMap m = Reader{Binding{.fName = "a"sv, .fDefaultArrayItems = make_shared<Binding> (Binding{.fName = "item"sv})}}
                          .Read (String{"<a><item>1</item><item>2</item><other>x</other></a>"sv})
                          .As<VVMap> ();
            EXPECT_EQ (m.Lookup ("other"sv)->As<String> (), "x"sv);
            ASSERT_TRUE (m.Lookup ("item"sv).has_value ());
            EXPECT_EQ (m.Lookup ("item"sv)->As<Sequence<VariantValue>> ().size (), 2u);
        }
        {
            // an array each item Binding names: its wrapper reads back as that member's array, two differently named wrappers in
            // the one document - and still an array where the document holds one item, or none. UPnP's service description
            auto          itemsNamed = [] (const String& itemName) { return make_shared<Binding> (Binding{.fName = itemName}); };
            const Binding scpd{.fName    = "scpd"sv,
                               .fMembers = {{"actionList"sv, Binding{.fItems = itemsNamed ("action"sv)}},
                                            {"allowedValueList"sv, Binding{.fItems = itemsNamed ("allowedValue"sv)}}}};
            for (const Sequence<VariantValue>& actions :
                 {Sequence<VariantValue>{VariantValue{VVMap{{"name"sv, VariantValue{"SetTarget"sv}}}},
                                         VariantValue{VVMap{{"name"sv, VariantValue{"GetTarget"sv}}}}},
                  Sequence<VariantValue>{VariantValue{VVMap{{"name"sv, VariantValue{"SetTarget"sv}}}}}, Sequence<VariantValue>{}}) {
                VariantValue v{VVMap{{"actionList"sv, VariantValue{actions}},
                                     {"allowedValueList"sv, VariantValue{Sequence<VariantValue>{VariantValue{"yes"sv}}}}}};
                EXPECT_EQ (roundTrip (v, scpd), v) << actions.size (); // including the arrays of one, and of none
            }
            // someone else's document, read through the same Binding
            VariantValue read = Reader{scpd}.Read (String{"<scpd><actionList><action><name>A</name></action></actionList></scpd>"sv});
            EXPECT_EQ (read.As<VVMap> ().Lookup ("actionList"sv)->GetType (), VariantValue::eArray);
        }
        {
            // default array items read back as they were written, at and inside the element whose Binding gives them
            const Binding a{.fName    = "a"sv,
                            .fMembers = {{"inner"sv, Binding{.fDefaultArrayItems = make_shared<Binding> (Binding::kDefaultArrayItems)}}},
                            .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
            VariantValue  v{VVMap{
                {"x"sv, VariantValue{Sequence<VariantValue>{VariantValue{"1"sv}, VariantValue{"2"sv}}}},
                {"inner"sv, VariantValue{VVMap{{"y"sv, VariantValue{Sequence<VariantValue>{VariantValue{"3"sv}, VariantValue{"4"sv}}}}}}}}};
            EXPECT_EQ (roundTrip (v, a), v);
        }
        {
            // an element with the item name in scope (<Array>) is a member, not an item, where the object's Binding names a member
            // written with that name - so any value reads back, given a Binding that says enough
            const Binding named{.fName = "a"sv, .fMembers = {{"x"sv, Binding{.fMembers = {{"Array"sv}}}}}};
            VariantValue  alone{VVMap{{"x"sv, VariantValue{VVMap{{"Array"sv, VariantValue{"1"sv}}}}}}};
            EXPECT_EQ (roundTrip (alone, named), alone);
            VariantValue beside{VVMap{{"x"sv, VariantValue{VVMap{{"Array"sv, VariantValue{"1"sv}}, {"b"sv, VariantValue{"2"sv}}}}}}};
            EXPECT_EQ (roundTrip (beside, named), beside);
            // ... a member renamed to it too
            const Binding renamed{.fName = "a"sv, .fMembers = {{"x"sv, Binding{.fMembers = {{"y"sv, Binding{.fName = "Array"sv}}}}}}};
            VariantValue  y{VVMap{{"x"sv, VariantValue{VVMap{{"y"sv, VariantValue{"1"sv}}}}}}};
            EXPECT_EQ (roundTrip (y, renamed), y);
            // ... while an array beside it, in its own element, still reads back as one
            VariantValue withArray{VVMap{{"x"sv, VariantValue{VVMap{{"Array"sv, VariantValue{"1"sv}},
                                                                    {"list"sv, VariantValue{Sequence<VariantValue>{VariantValue{"a"sv}}}}}}}}};
            EXPECT_EQ (roundTrip (withArray, named), withArray);
        }
    }
}

namespace {
    using DataExchange::Variant::XML::Binding;

    // equal, and of the same type all the way down - VariantValue's == calls 3 and "3" equal (it compares them Normalize ()d)
    bool SameTyped_ (const VariantValue& a, const VariantValue& b)
    {
        if (a.GetType () != b.GetType ()) {
            return false;
        }
        switch (a.GetType ()) {
            case VariantValue::eArray: {
                Sequence<VariantValue> l = a.As<Sequence<VariantValue>> ();
                Sequence<VariantValue> r = b.As<Sequence<VariantValue>> ();
                if (l.size () != r.size ()) {
                    return false;
                }
                for (size_t i = 0; i < l.size (); ++i) {
                    if (not SameTyped_ (l[i], r[i])) {
                        return false;
                    }
                }
                return true;
            }
            case VariantValue::eMap: {
                Mapping<String, VariantValue> l = a.As<Mapping<String, VariantValue>> ();
                Mapping<String, VariantValue> r = b.As<Mapping<String, VariantValue>> ();
                if (l.size () != r.size ()) {
                    return false;
                }
                for (const auto& i : l) {
                    optional<VariantValue> o = r.Lookup (i.fKey);
                    if (not o or not SameTyped_ (i.fValue, *o)) {
                        return false;
                    }
                }
                return true;
            }
            default:
                return a == b;
        }
    }

    /*
     *  Random complete Bindings, and random values of the type each declares - so that what IsComplete promises (every such value
     *  read back exactly as written) is tested over many Bindings and values, not only the few written out by hand. Seeded, so a
     *  failure repeats.
     */
    struct CompleteBindings_ {
        mt19937      fRandom;
        unsigned int fNextName{0};

        explicit CompleteBindings_ (unsigned int seed)
            : fRandom{seed}
        {
        }

        unsigned int Pick (size_t n)
        {
            return uniform_int_distribution<unsigned int>{0, static_cast<unsigned int> (n - 1)}(fRandom);
        }
        String NewName ()
        {
            return "n{}"_f(fNextName++);
        }
        Binding Leaf (Binding::Kind kind = Binding::Kind::eElement)
        {
            static constexpr VariantValue::Type kLeaves_[] = {
                VariantValue::eString, VariantValue::eBoolean, VariantValue::eInteger,  VariantValue::eUnsignedInteger,
                VariantValue::eFloat,  VariantValue::eDate,    VariantValue::eDateTime, VariantValue::eBLOB};
            return Binding{.fKind = kind, .fType = kLeaves_[Pick (size (kLeaves_))]};
        }
        // a Binding for a value depth deep; a member's may be an array of its own element repeated - an item's, or the top level one,
        // has no element of its own to repeat
        Binding Make (unsigned int depth, bool member)
        {
            switch (depth == 0 ? 0 : Pick (3)) {
                case 0:
                    return Leaf ();
                case 1: {
                    Binding b{.fType = VariantValue::eMap};
                    if (Pick (4) == 0) {
                        // text beside attributes, as UPnP's <service id="1">SwitchPower</service>
                        for (unsigned int i = Pick (3); i > 0; --i) {
                            b.fMembers.Append (Binding::Member{NewName (), Leaf (Binding::Kind::eAttribute)});
                        }
                        b.fMembers.Append (Binding::Member{NewName (), Leaf (Binding::Kind::eText)});
                    }
                    else {
                        for (unsigned int i = Pick (5); i > 0; --i) {
                            Binding m = Pick (3) == 0   ? Leaf (Binding::Kind::eAttribute)
                                        : Pick (6) == 0 ? Binding{.fKind = Binding::Kind::ePresenceMarker, .fType = VariantValue::eBoolean}
                                                        : Make (depth - 1, true);
                            if (Pick (4) == 0) {
                                m.fName = NewName (); // a name of its own, not its key
                            }
                            b.fMembers.Append (Binding::Member{NewName (), m});
                        }
                    }
                    if (Pick (3) == 0) {
                        b.fUndeclaredMembers = Binding::UndeclaredMembers::eSkip; // still complete
                    }
                    return b;
                }
                default: {
                    Binding item = Make (depth - 1, false);
                    if (not member or Pick (2) == 0) {
                        item.fName = NewName (); // wrapped in the member's element, each item an element of this name
                    }
                    return Binding{.fType = VariantValue::eArray, .fItems = make_shared<Binding> (item)};
                }
            }
        }

        // a string XML carries both ways today - not CR or another control character, nor a character outside the BMP (on
        // Windows), nor in an attribute tab or LF (@todo widen with the escaping fixes)
        String RandomString (bool attribute)
        {
            static constexpr char32_t kChars_[]        = {U'a', U'Z', U'0', U' ', U'-', U'&', U'<', U'>', U'"', char32_t (39), U';',
                                                          char32_t (0xE9), char32_t (0x4E2D),
                                                          // element text only
                                                          char32_t (10), char32_t (9)};
            constexpr size_t          kAttributeChars_ = 13;
            u32string                 s;
            for (unsigned int i = Pick (7); i > 0; --i) {
                s += kChars_[Pick (attribute ? kAttributeChars_ : size (kChars_))];
            }
            return String{s};
        }
        VariantValue Value (const Binding& b)
        {
            switch (*b.fType) {
                case VariantValue::eString:
                    return VariantValue{RandomString (b.fKind == Binding::Kind::eAttribute)};
                case VariantValue::eBoolean:
                    return VariantValue{Pick (2) == 0};
                case VariantValue::eInteger:
                    return VariantValue{
                        uniform_int_distribution<long long>{numeric_limits<long long>::min (), numeric_limits<long long>::max ()}(fRandom)};
                case VariantValue::eUnsignedInteger:
                    return VariantValue{uniform_int_distribution<unsigned long long>{0, numeric_limits<unsigned long long>::max ()}(fRandom)};
                case VariantValue::eFloat: {
                    static const long double kSome_[] = {0.0L,
                                                         -1.5L,
                                                         1e300L,
                                                         numeric_limits<long double>::max (),
                                                         numeric_limits<long double>::denorm_min (),
                                                         numeric_limits<long double>::infinity ()};
                    return VariantValue{Pick (2) == 0 ? kSome_[Pick (size (kSome_))] : uniform_real_distribution<long double>{-1e10L, 1e10L}(fRandom)};
                }
                case VariantValue::eDate:
                    return VariantValue{Time::Date{chrono::year{static_cast<int> (1900 + Pick (300))}, chrono::month{1 + Pick (12)},
                                                   chrono::day{1 + Pick (28)}}};
                case VariantValue::eDateTime: {
                    Time::Date d{chrono::year{static_cast<int> (1900 + Pick (300))}, chrono::month{1 + Pick (12)}, chrono::day{1 + Pick (28)}};
                    return VariantValue{Time::DateTime{d, Time::TimeOfDay{Pick (86400)}, Pick (2) == 0 ? Time::Timezone::kUTC : Time::Timezone::kUnknown}};
                }
                case VariantValue::eBLOB: {
                    vector<byte> bytes;
                    for (unsigned int i = Pick (12); i > 0; --i) {
                        bytes.push_back (static_cast<byte> (Pick (256)));
                    }
                    return VariantValue{Memory::BLOB{bytes}};
                }
                case VariantValue::eMap: {
                    Mapping<String, VariantValue> m;
                    for (const Binding::Member& i : b.fMembers) {
                        switch (Pick (8)) {
                            case 0:
                                break; // absent
                            case 1:
                                m.Add (i.fKey, VariantValue{}); // null
                                break;
                            default:
                                m.Add (i.fKey, Value (*i.fBinding));
                                break;
                        }
                    }
                    return VariantValue{m};
                }
                case VariantValue::eArray: {
                    Sequence<VariantValue> items;
                    for (unsigned int i = Pick (4); i > 0; --i) {
                        items.Append (Value (*b.fItems));
                    }
                    return VariantValue{items};
                }
                default:
                    AssertNotReached ();
                    return VariantValue{};
            }
        }
        // what v reads back as: v, less what is written as nothing - a null member, an empty array of repeated elements, and text
        // (Kind::eText) written as none
        static VariantValue Expected (const VariantValue& v, const Binding& b)
        {
            switch (v.GetType ()) {
                case VariantValue::eMap: {
                    Mapping<String, VariantValue> m;
                    for (const auto& i : v.As<Mapping<String, VariantValue>> ()) {
                        const Binding& mb = *b.MemberBinding (i.fKey);
                        if (i.fValue.GetType () == VariantValue::eNull or
                            (mb.fType == VariantValue::eArray and not mb.fItems->fName and i.fValue.As<Sequence<VariantValue>> ().empty ()) or
                            (mb.fKind == Binding::Kind::eText and i.fValue.As<String> ().empty ())) {
                            continue;
                        }
                        m.Add (i.fKey, Expected (i.fValue, mb));
                    }
                    // a presence marker absent - or null, or false: each written as nothing - reads back false
                    for (const Binding::Member& i : b.fMembers) {
                        if (i.fBinding->fKind == Binding::Kind::ePresenceMarker and not m.ContainsKey (i.fKey)) {
                            m.Add (i.fKey, VariantValue{false});
                        }
                    }
                    return VariantValue{m};
                }
                case VariantValue::eArray: {
                    Sequence<VariantValue> items;
                    for (const VariantValue& i : v.As<Sequence<VariantValue>> ()) {
                        items.Append (Expected (i, *b.fItems));
                    }
                    return VariantValue{items};
                }
                default:
                    return v;
            }
        }
    };

    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, XML_ONLY_CompleteBinding_)
    {
        using DataExchange::Variant::XML::Reader;
        using DataExchange::Variant::XML::Writer;
        using DataExchange::XML::NameWithNamespace;
        using IO::Network::URI;
        using VVMap = Mapping<String, VariantValue>;
        auto leaf = [] (VariantValue::Type t, Binding::Kind kind = Binding::Kind::eElement) { return Binding{.fKind = kind, .fType = t}; };
        auto incomplete = [] (const Binding& b, const char* why) {
            EXPECT_FALSE (b.IsComplete ()) << why;
            String s = b.ToString ();
            EXPECT_TRUE (s.Contains ("Incomplete: "sv) and s.Contains (String{why})) << s.AsUTF8<string> ();
        };
        auto roundTrip = [] (const VariantValue& v, const Binding& b) { return Reader{b}.Read (Writer{{.fBinding = b}}.WriteAsString (v)); };
        // UPnP's <stateVariable sendEvents="yes">, every value's type declared
        const Binding stateVariable{
            .fName    = "stateVariable"sv,
            .fType    = VariantValue::eMap,
            .fMembers = {{"sendEvents"sv, leaf (VariantValue::eBoolean, Binding::Kind::eAttribute)},
                         {"name"sv, leaf (VariantValue::eString)},
                         {"allowedValueList"sv,
                          Binding{.fType  = VariantValue::eArray,
                                  .fItems = make_shared<Binding> (Binding{.fName = "allowedValue"sv, .fType = VariantValue::eInteger})}}}};
        {
            // complete, and so read back exactly - each value of the type it was written as
            EXPECT_TRUE (stateVariable.IsComplete ()) << stateVariable.ToString ().AsUTF8<string> ();
            EXPECT_TRUE (stateVariable.ToString ().Contains (", Complete}"sv));
            VariantValue v{VVMap{{"sendEvents"sv, VariantValue{true}},
                                 {"name"sv, VariantValue{"Status"sv}},
                                 {"allowedValueList"sv, VariantValue{Sequence<VariantValue>{VariantValue{1}, VariantValue{2}}}}}};
            VariantValue back = roundTrip (v, stateVariable);
            EXPECT_TRUE (SameTyped_ (back, v)) << Characters::ToString (back).AsUTF8<string> ();
            EXPECT_EQ (back.As<VVMap> ().Lookup ("sendEvents"sv)->GetType (), VariantValue::eBoolean);
            // an empty object and an empty array read back as themselves, not as ""
            const Binding empty{
                .fName    = "e"sv,
                .fType    = VariantValue::eMap,
                .fMembers = {{"o"sv, Binding{.fType = VariantValue::eMap}},
                             {"a"sv, Binding{.fType  = VariantValue::eArray,
                                             .fItems = make_shared<Binding> (Binding{.fName = "i"sv, .fType = VariantValue::eString})}}}};
            VariantValue nothing{VVMap{{"o"sv, VariantValue{VVMap{}}}, {"a"sv, VariantValue{Sequence<VariantValue>{}}}}};
            EXPECT_TRUE (SameTyped_ (roundTrip (nothing, empty), nothing));
            // a null member is written as none, so reads back absent
            VariantValue withNull{VVMap{{"name"sv, VariantValue{}}}};
            EXPECT_TRUE (SameTyped_ (roundTrip (withNull, stateVariable), VariantValue{VVMap{}}));
            // a member keyed as the default shape's array items are named is just a member, given a complete Binding
            const Binding arrayKey{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"Array"sv, leaf (VariantValue::eString)}}};
            VariantValue  a{VVMap{{"Array"sv, VariantValue{"x"sv}}}};
            EXPECT_TRUE (SameTyped_ (roundTrip (a, arrayKey), a));
            // an attribute and an element may have the same name
            const Binding sameName{.fName = "a"sv,
                                   .fType = VariantValue::eMap,
                                   .fMembers = {{"id"sv, Binding{.fKind = Binding::Kind::eAttribute, .fName = "x"sv, .fType = VariantValue::eInteger}},
                                                {"x"sv, leaf (VariantValue::eString)}}};
            EXPECT_TRUE (sameName.IsComplete ()) << sameName.ToString ().AsUTF8<string> ();
            VariantValue s{VVMap{{"id"sv, VariantValue{7}}, {"x"sv, VariantValue{"seven"sv}}}};
            EXPECT_TRUE (SameTyped_ (roundTrip (s, sameName), s)) << Characters::ToString (roundTrip (s, sameName)).AsUTF8<string> ();
        }
        {
            // the Writer writes only values of the type declared - a value is data, so it throws
            auto write = [&] (const VariantValue& v) { return Writer{{.fBinding = stateVariable}}.WriteAsString (v); };
            EXPECT_THROW (write (VariantValue{VVMap{{"name"sv, VariantValue{3}}}}), BadFormatException);
            EXPECT_THROW (write (VariantValue{VVMap{{"other"sv, VariantValue{"x"sv}}}}), BadFormatException);
            EXPECT_THROW (write (VariantValue{VVMap{{"sendEvents"sv, VariantValue{"yes"sv}}}}), BadFormatException);
            // ... and the Reader reads only documents of that type: an element or attribute not declared, a leaf not of its type
            Reader r{stateVariable};
            EXPECT_THROW (r.Read (String{"<stateVariable><other>x</other></stateVariable>"sv}), BadFormatException);
            EXPECT_THROW (r.Read (String{"<stateVariable other=\"x\"></stateVariable>"sv}), BadFormatException);
            EXPECT_THROW (r.Read (String{"<stateVariable sendEvents=\"yes\"></stateVariable>"sv}), BadFormatException);
            EXPECT_THROW (r.Read (String{"<stateVariable><allowedValueList><allowedValue>x</allowedValue></allowedValueList></stateVariable>"sv}),
                          BadFormatException);
            EXPECT_THROW (r.Read (String{"<stateVariable><name><b>x</b></name></stateVariable>"sv}), BadFormatException);
            // ... nor a member repeated that its Binding declares as one value - not silently made an array
            EXPECT_THROW (r.Read (String{"<stateVariable><name>a</name><name>b</name></stateVariable>"sv}), BadFormatException);
            // but someone else's document of that type reads, as XML Schema reads it - whitespace around a number dropped, 1 a boolean
            VariantValue read =
                r.Read (String{"<stateVariable sendEvents=\"1\"><allowedValueList><allowedValue> 3 </allowedValue></allowedValueList></stateVariable>"sv});
            EXPECT_TRUE (SameTyped_ (read, VariantValue{VVMap{{"sendEvents"sv, VariantValue{true}},
                                                              {"allowedValueList"sv, VariantValue{Sequence<VariantValue>{VariantValue{3LL}}}}}}))
                << Characters::ToString (read).AsUTF8<string> ();
        }
        {
            // Variant/XML/ReadMe.md's example, verbatim
            Binding b{.fName    = "stateVariable"sv,
                      .fType    = VariantValue::eMap,
                      .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute, .fType = VariantValue::eBoolean}},
                                   {"name"sv, Binding{.fType = VariantValue::eString}}}};
            EXPECT_TRUE (b.IsComplete ()) << b.ToString ().AsUTF8<string> ();
        }
        {
            // the same document read with no fType, and with it - Binding::fType's table
            const String  xml{"<a><n>3</n><o></o><l></l></a>"sv};
            const Binding loose{.fName = "a"sv};
            const Binding typed{
                .fName    = "a"sv,
                .fType    = VariantValue::eMap,
                .fMembers = {{"n"sv, leaf (VariantValue::eInteger)},
                             {"o"sv, Binding{.fType = VariantValue::eMap}},
                             {"l"sv, Binding{.fType  = VariantValue::eArray,
                                             .fItems = make_shared<Binding> (Binding{.fName = "i"sv, .fType = VariantValue::eString})}}}};
            EXPECT_TRUE (SameTyped_ (Reader{loose}.Read (xml),
                                     VariantValue{VVMap{{"n"sv, VariantValue{"3"sv}}, {"o"sv, VariantValue{""sv}}, {"l"sv, VariantValue{""sv}}}}));
            EXPECT_TRUE (SameTyped_ (
                Reader{typed}.Read (xml),
                VariantValue{VVMap{{"n"sv, VariantValue{3LL}}, {"o"sv, VariantValue{VVMap{}}}, {"l"sv, VariantValue{Sequence<VariantValue>{}}}}}));
            // a member it does not name: read, or thrown on
            EXPECT_NO_THROW (Reader{loose}.Read (String{"<a><x>1</x></a>"sv}));
            EXPECT_THROW (Reader{typed}.Read (String{"<a><x>1</x></a>"sv}), BadFormatException);
            // a null member: an empty element, or none
            VariantValue nullN{VVMap{{"n"sv, VariantValue{}}}};
            EXPECT_EQ (Writer{{.fBinding = loose}}.WriteAsString (nullN), "<a>\n    <n></n>\n</a>\n"sv);
            EXPECT_EQ (Writer{{.fBinding = typed}}.WriteAsString (nullN), "<a></a>\n"sv);
        }
        {
            // a declared array its Binding gives no fItems takes the default items in scope, as the Writer does - here its own
            // element repeated: so an array however few elements there are
            const Binding repeated{.fName              = "a"sv,
                                   .fType              = VariantValue::eMap,
                                   .fMembers           = {{"k"sv, Binding{.fType = VariantValue::eArray}}},
                                   .fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)};
            for (const Sequence<VariantValue>& items :
                 {Sequence<VariantValue>{VariantValue{"x"sv}}, Sequence<VariantValue>{VariantValue{"x"sv}, VariantValue{"y"sv}}}) {
                VariantValue v{VVMap{{"k"sv, VariantValue{items}}}};
                try {
                    EXPECT_TRUE (SameTyped_ (roundTrip (v, repeated), v)) << items.size ();
                }
                catch (const std::exception& e) {
                    ADD_FAILURE () << items.size () << " item(s): " << e.what ();
                }
            }
        }
        {
            // undeclared members, as XSD's xs:any says: rejected (the default where fType is declared), skipped unread, or kept
            const Binding rejecting{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"n"sv, leaf (VariantValue::eInteger)}}};
            Binding       skipping      = rejecting;
            skipping.fUndeclaredMembers = Binding::UndeclaredMembers::eSkip;
            Binding keeping             = rejecting;
            keeping.fUndeclaredMembers  = Binding::UndeclaredMembers::eKeep;
            // a vendor's elements and attributes - one with mixed content, which could not be read into a VariantValue at all
            const String vendor{"<a xmlns:v=\"urn:v\" v:x=\"1\" y=\"2\"><n>3</n><v:note>a <b>vendor</b>'s</v:note><other k=\"v\">t</other></a>"sv};
            EXPECT_THROW (Reader{rejecting}.Read (vendor), BadFormatException);
            EXPECT_TRUE (SameTyped_ (Reader{skipping}.Read (vendor), VariantValue{VVMap{{"n"sv, VariantValue{3LL}}}}));
            EXPECT_THROW (Reader{keeping}.Read (vendor), BadFormatException); // kept, so read - and mixed content cannot be
            EXPECT_TRUE (SameTyped_ (Reader{keeping}.Read (String{"<a y=\"2\"><n>3</n><other>t</other></a>"sv}),
                                     VariantValue{VVMap{{"n"sv, VariantValue{3LL}}, {"y"sv, VariantValue{"2"sv}}, {"other"sv, VariantValue{"t"sv}}}}));
            // writing, a member it does not declare would not read back - unless kept
            auto         write = [] (const VariantValue& v, const Binding& b) { return Writer{{.fBinding = b}}.WriteAsString (v); };
            VariantValue extra{VVMap{{"n"sv, VariantValue{3}}, {"other"sv, VariantValue{"t"sv}}}};
            EXPECT_THROW (write (extra, rejecting), BadFormatException);
            EXPECT_THROW (write (extra, skipping), BadFormatException);
            EXPECT_EQ (write (extra, keeping), "<a>\n    <n>3</n>\n    <other>t</other>\n</a>\n"sv);
            // skipping still round-trips every value of its type - keeping does not, so is not complete
            EXPECT_TRUE (skipping.IsComplete ()) << skipping.ToString ().AsUTF8<string> ();
            incomplete (keeping, "keeps the members it does not declare");
        }
        {
            // a presence marker: an empty element, true by being there - as UPnP's <retval/>
            const Binding argument{.fName    = "argument"sv,
                                   .fType    = VariantValue::eMap,
                                   .fMembers = {{"name"sv, leaf (VariantValue::eString)},
                                                {"retval"sv, leaf (VariantValue::eBoolean, Binding::Kind::ePresenceMarker)}}};
            EXPECT_TRUE (argument.IsComplete ()) << argument.ToString ().AsUTF8<string> ();
            VariantValue yes{VVMap{{"name"sv, VariantValue{"r"sv}}, {"retval"sv, VariantValue{true}}}};
            VariantValue no{VVMap{{"name"sv, VariantValue{"r"sv}}, {"retval"sv, VariantValue{false}}}};
            EXPECT_EQ (Writer{{.fBinding = argument}}.WriteAsString (yes), "<argument>\n    <name>r</name>\n    <retval></retval>\n</argument>\n"sv);
            EXPECT_EQ (Writer{{.fBinding = argument}}.WriteAsString (no), "<argument>\n    <name>r</name>\n</argument>\n"sv);
            EXPECT_TRUE (SameTyped_ (roundTrip (yes, argument), yes));
            EXPECT_TRUE (SameTyped_ (roundTrip (no, argument), no)); // absent: false
            EXPECT_TRUE (SameTyped_ (Reader{argument}.Read (String{"<argument><retval/><name>r</name></argument>"sv}), yes));
            // ... and only empty
            EXPECT_THROW (Reader{argument}.Read (String{"<argument><retval>no</retval></argument>"sv}), BadFormatException);
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"f"sv, leaf (VariantValue::eString, Binding::Kind::ePresenceMarker)}}},
                        "a presence marker (ePresenceMarker) that is not a Boolean");
        }
        {
            // each way a Binding can fall short of complete - and ToString says why
            const auto kString = leaf (VariantValue::eString);
            incomplete (Binding{.fType = VariantValue::eMap}, "not named");
            incomplete (Binding{.fName = "a"sv}, "a: declares no fType");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"x"sv}}}, "a/x: no Binding");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"x"sv, Binding{}}}}, "a/x: declares no fType");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eArray}, "no fItems");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eArray, .fItems = make_shared<Binding> (kString)},
                        "not an object's member");
            incomplete (Binding{.fName  = "a"sv,
                                .fType  = VariantValue::eArray,
                                .fItems = make_shared<Binding> (
                                    Binding{.fName = "i"sv, .fType = VariantValue::eArray, .fItems = make_shared<Binding> (kString)})},
                        "a[]: an array of repeated elements");
            incomplete (Binding{.fName    = "a"sv,
                                .fType    = VariantValue::eMap,
                                .fMembers = {{"x"sv, kString}, {"y"sv, Binding{.fName = "x"sv, .fType = VariantValue::eString}}}},
                        "local name too");
            incomplete (Binding{.fName    = "a"sv,
                                .fType    = VariantValue::eMap,
                                .fMembers = {{"t"sv, leaf (VariantValue::eString, Binding::Kind::eText)}, {"x"sv, kString}}},
                        "mixed content");
            incomplete (Binding{.fName    = "a"sv,
                                .fType    = VariantValue::eMap,
                                .fMembers = {{"t"sv, leaf (VariantValue::eString, Binding::Kind::eText)},
                                             {"u"sv, leaf (VariantValue::eString, Binding::Kind::eText)}}},
                        "a second text member");
            incomplete (Binding{.fName    = "a"sv,
                                .fType    = VariantValue::eMap,
                                .fMembers = {{"x"sv, Binding{.fKind = Binding::Kind::eAttribute, .fType = VariantValue::eMap}}}},
                        "not a leaf");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"my key"sv, kString}}}, "not an XML name");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"x"sv, kString}, {"x"sv, kString}}},
                        "listed twice");
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eString, .fMembers = {{"x"sv, kString}}},
                        "a leaf (String) with fMembers");
            // an attribute in a namespace needs a prefix in scope - declared where it is, or on an element it is inside
            const URI kNS{"urn:x"sv};
            const Binding attrInNS{.fKind = Binding::Kind::eAttribute, .fName = NameWithNamespace{kNS, "x"sv}, .fType = VariantValue::eString};
            incomplete (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"x"sv, attrInNS}}}, "no prefix in scope");
            EXPECT_TRUE (
                (Binding{.fName = "a"sv, .fType = VariantValue::eMap, .fMembers = {{"x"sv, attrInNS}}, .fNamespacePrefixes = {{kNS, "p"sv}}}.IsComplete ()));
        }
        {
            // the claim itself - every value of a complete Binding's type reads back exactly - over random complete Bindings, and
            // random values of each
            CompleteBindings_ g{20261015};
            for (unsigned int i = 0; i < 300; ++i) {
                Binding b = g.Make (3, false);
                b.fName   = "root"sv;
                ASSERT_TRUE (b.IsComplete ()) << b.ToString ().AsUTF8<string> ();
                for (unsigned int j = 0; j < 4; ++j) {
                    VariantValue v   = g.Value (b);
                    String       xml = Writer{{.fBinding = b}}.WriteAsString (v);
                    try {
                        VariantValue back = Reader{b}.Read (xml);
                        EXPECT_TRUE (SameTyped_ (back, CompleteBindings_::Expected (v, b)))
                            << "Binding: " << b.ToString ().AsUTF8<string> () << endl
                            << "value:   " << Characters::ToString (v).AsUTF8<string> () << endl
                            << "read:    " << Characters::ToString (back).AsUTF8<string> () << endl
                            << "xml:     " << xml.AsUTF8<string> ();
                    }
                    catch (const std::exception& e) {
                        ADD_FAILURE () << "read threw " << e.what () << endl
                                       << "Binding: " << b.ToString ().AsUTF8<string> () << endl
                                       << "xml:     " << xml.AsUTF8<string> ();
                    }
                }
            }
        }
    }
}
#endif

namespace {
    /// @TODO MOVE ELSEWHERE
    template <typename T>
    void Test3_VariantValue_Helper_MinMax_ ()
    {
        {
            VariantValue v   = numeric_limits<T>::lowest ();
            VariantValue vs  = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            if (Debug::IsRunningUnderValgrind () and v != vrt) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "Test3_VariantValue_Helper_MinMax_ (valgrind holds long double in 64 bits): v={}, vs={}, vrt={}"_f(v, vs, vrt).ReplaceAll ("[\r\n]"_RegEx, ""));
            }
            else {
                EXPECT_EQ (v, vrt);
            }
        }
        {
            VariantValue v   = numeric_limits<T>::min ();
            VariantValue vs  = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            EXPECT_EQ (v, vrt);
        }
        {
            VariantValue v   = numeric_limits<T>::max ();
            VariantValue vs  = v.As<String> ();
            VariantValue vrt = vs.As<T> ();
            if (Debug::IsRunningUnderValgrind () and v != vrt) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    "Test3_VariantValue_Helper_MinMax_ (valgrind holds long double in 64 bits): v={}, vs={}, vrt={}"_f(v, vs, vrt).ReplaceAll ("[\r\n]"_RegEx, ""));
            }
            else {
                EXPECT_EQ (v, vrt);
            }
        }
    }
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, Test3_VariantValue)
    {
        using Characters::String;
        {
            VariantValue v;
            EXPECT_TRUE (v.empty ());
            v = String{"hi"};
            EXPECT_TRUE (v == "hi");
        }
        Test3_VariantValue_Helper_MinMax_<int> ();
        Test3_VariantValue_Helper_MinMax_<unsigned int> ();
        Test3_VariantValue_Helper_MinMax_<long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long> ();
        Test3_VariantValue_Helper_MinMax_<long long> ();
        Test3_VariantValue_Helper_MinMax_<unsigned long long> ();
        Test3_VariantValue_Helper_MinMax_<float> ();
        Test3_VariantValue_Helper_MinMax_<double> ();
        Test3_VariantValue_Helper_MinMax_<long double> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, ValueReaderReadFromString)
    {
        using Characters::Character;
        using Characters::String;

        auto roundTripCheck = [] (const VariantValue& vv) {
            String       inputAsJSON = Variant::JSON::Writer{}.WriteAsString (vv);
            VariantValue v           = Variant::JSON::Reader{}.Read (inputAsJSON);
            EXPECT_EQ (v, vv);
        };
        roundTripCheck (VariantValue{3});
        roundTripCheck (VariantValue{"x"});
        roundTripCheck (
            VariantValue (Mapping<String, VariantValue>{pair<String, VariantValue>{"a", 3}, pair<String, VariantValue>{"n", "34"}}));
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, CompressionTests_)
    {
        if constexpr (not Compression::Deflate::kSupported) {
            SkipTestPart ("Deflate not built in");
        }
        if constexpr (not Compression::GZip::kSupported) {
            SkipTestPart ("GZip not built in");
        }
        if constexpr (not Compression::ZStd::kSupported) {
            SkipTestPart ("ZStd not built in");
        }
        auto RoundTripCompressTest_ = [] (const Memory::BLOB& b) {
            // Compression::Deflate
            if constexpr (Compression::Deflate::kSupported) {
                Memory::BLOB compressed = Compression::Deflate::Compress::New ().Transform (b);
                EXPECT_EQ (b, Compression::Deflate::Decompress::New ().Transform (compressed));
#if qStroika_HasComponent_zlib
                {
                    DISABLE_COMPILER_MSC_WARNING_START (4996);
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
                    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
                    EXPECT_EQ (Compression::Zip::Reader{}.Compress (b), Compression::Deflate::Compress::New ().Transform (b));
                    EXPECT_EQ (Compression::Zip::Reader{}.Decompress (compressed), Compression::Deflate::Decompress::New ().Transform (compressed));
                    DISABLE_COMPILER_MSC_WARNING_END (4996);
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
                    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
                }
#endif
            }
            // Compression::GZip
            if constexpr (Compression::GZip::kSupported) {
                Memory::BLOB compressed = Compression::GZip::Compress::New ().Transform (b);
                EXPECT_EQ (b, Compression::GZip::Decompress::New ().Transform (compressed));
            }
            // Compression::ZStd
            if constexpr (Compression::ZStd::kSupported) {
                Memory::BLOB compressed = Compression::ZStd::Compress::New ().Transform (b);
                EXPECT_EQ (b, Compression::ZStd::Decompress::New ().Transform (compressed));
            }
        };
        RoundTripCompressTest_ (Memory::BLOB::FromHex ("aa1234abcd01010102030405"));
        RoundTripCompressTest_ (Memory::BLOB::FromHex ("aa1234abcd01010102030405"sv));
        {
            Memory::BLOB bigBlob = Memory::BLOB::FromHex ("00112233445566778899aabbccddeeff");
            Assert (bigBlob.size () == 16);
            while (bigBlob.size () < 256 * 1024) {
                bigBlob = bigBlob + bigBlob;
            }
            RoundTripCompressTest_ (bigBlob);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, Write2JSONSThenRead2JSONsWithSharedStream_)
    {
        static VariantValue kTestVariant_{Mapping<String, VariantValue>{pair<String, VariantValue>{"a", 3}, pair<String, VariantValue>{"b", 99}}};
        auto WriteJSON_ = [] (const Streams::OutputStream::Ptr<byte>& out) {
            using namespace DataExchange::Variant::JSON;
            const Writer::Options kOptions_{false};
            Writer{kOptions_}.Write (kTestVariant_, out);
        };
        auto ReadJSON_ = [] (const Streams::InputStream::Ptr<byte>& in) {
            using namespace DataExchange::Variant::JSON;
            EXPECT_TRUE (kTestVariant_ == Reader{}.Read (in));
        };
        {
            Streams::MemoryStream::Ptr<byte> memStream = Streams::MemoryStream::New<byte> ();
            WriteJSON_ (memStream);
            ReadJSON_ (memStream);
            WriteJSON_ (memStream);
            ReadJSON_ (memStream);
            EXPECT_TRUE (memStream.IsAtEOF ()); // mem-stream is at EOF because we checked - it reads/advances read pointer
        }
        {
            Streams::SharedMemoryStream::Ptr<byte> sharedMemStream = Streams::SharedMemoryStream::New<byte> ();
            WriteJSON_ (sharedMemStream);
            ReadJSON_ (sharedMemStream);
            WriteJSON_ (sharedMemStream);
            ReadJSON_ (sharedMemStream);
            EXPECT_TRUE (not sharedMemStream.AvailableToRead ().has_value ()); // would be at EOF, but not KNOWN at EOF til writing side closed.
            sharedMemStream.CloseWrite ();
            EXPECT_TRUE (sharedMemStream.IsAtEOF ()); // now at EOF because input closed
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSONWriterNumberPrecision)
    {
        {
            Variant::JSON::Writer w{};
            EXPECT_EQ (w.WriteAsString (VariantValue{numbers::pi}), "3.14159"); // defaults to 6 digits of precision
        }
        {
            using namespace Characters;
            Variant::JSON::Writer w{
                Variant::JSON::Writer::Options{.fFloatOptions = FloatConversion::ToStringOptions{FloatConversion::SignificantFigures{10}}}};
            EXPECT_EQ (w.WriteAsString (VariantValue{numbers::pi}), "3.141592654"); // so try 10 digits (not tricky case cuz number is 3.14159265 then 358 so not truncate but round))
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, ReadAsBLOBVsReadAsString_)
    {
        using namespace Characters::Literals;
        using namespace Memory::Literals;
        // Saw assertion error cuz not seekable here, but doesn't seem to happen anymore - not sure what was happening? --LGP 2024-05-22
        // I THINK an issue with code in DataExchange::JSON::Reader::BoostRep_::Read - but cannot see anything wrong and cannot repro issue
        VariantValue readAsString = DataExchange::Variant::JSON::Reader{}.Read (R"({
                                                                                "foo" : [ "bar", "baz" ]
                                                                                ,"" : 0
                                                                                ,"a/b" : 1
                                                                                ,"c%d" : 2
                                                                                ,"e^f" : 3
                                                                                ,"g|h" : 4
                                                                                ,"i\\j" : 5
                                                                                ,"k\"l": 6
                                                                                ," ": 7
                                                                                ,"m~n" : 8
                                                                                })"_k);
        VariantValue readAsBLOB   = DataExchange::Variant::JSON::Reader{}.Read (R"({
                                                                                "foo" : [ "bar", "baz" ]
                                                                                ,"" : 0
                                                                                ,"a/b" : 1
                                                                                ,"c%d" : 2
                                                                                ,"e^f" : 3
                                                                                ,"g|h" : 4
                                                                                ,"i\\j" : 5
                                                                                ,"k\"l": 6
                                                                                ," ": 7
                                                                                ,"m~n" : 8
                                                                                })"_blob);
        EXPECT_EQ (readAsString, readAsBLOB);
    }
}

namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSONPointer_)
    {
        using namespace Characters::Literals;
        using namespace Memory::Literals;
        // test case from https://datatracker.ietf.org/doc/html/rfc6901
        static VariantValue kTestVariant_ = DataExchange::Variant::JSON::Reader{}.Read (R"({
                                                                                "foo" : [ "bar", "baz" ]
                                                                                ,"" : 0
                                                                                ,"a/b" : 1
                                                                                ,"c%d" : 2
                                                                                ,"e^f" : 3
                                                                                ,"g|h" : 4
                                                                                ,"i\\j" : 5
                                                                                ,"k\"l": 6
                                                                                ," ": 7
                                                                                ,"m~n" : 8
                                                                                })"_blob);
        EXPECT_EQ (JSON::PointerType{""}.Apply (kTestVariant_), kTestVariant_);
        EXPECT_EQ (JSON::PointerType{"/foo"}.Apply (kTestVariant_), (VariantValue{Sequence<VariantValue>{"bar", "baz"}}));
        EXPECT_EQ (JSON::PointerType{"/foo/0"}.Apply (kTestVariant_), (VariantValue{"bar"}));
        EXPECT_EQ (JSON::PointerType{"/"}.Apply (kTestVariant_), (VariantValue{0}));
        EXPECT_EQ (JSON::PointerType{"/a~1b"}.Apply (kTestVariant_), (VariantValue{1}));
        EXPECT_EQ (JSON::PointerType{"/c%d"}.Apply (kTestVariant_), (VariantValue{2}));
        EXPECT_EQ (JSON::PointerType{"/e^f"}.Apply (kTestVariant_), (VariantValue{3}));
        EXPECT_EQ (JSON::PointerType{"/g|h"}.Apply (kTestVariant_), (VariantValue{4}));
        EXPECT_EQ (JSON::PointerType{"/i\\j"}.Apply (kTestVariant_), (VariantValue{5}));
        EXPECT_EQ (JSON::PointerType{"/k\"l"}.Apply (kTestVariant_), (VariantValue{6}));
        EXPECT_EQ (JSON::PointerType{"/ "}.Apply (kTestVariant_), (VariantValue{7}));
        EXPECT_EQ (JSON::PointerType{"/m~0n"}.Apply (kTestVariant_), (VariantValue{8}));
    }
}
namespace {
    GTEST_TEST (Foundation_Foundation_DataExchange_Reader_Writers, JSONPPatch1_)
    {
        using namespace Memory::Literals;
        // test case from https://datatracker.ietf.org/doc/html/rfc6901
        static VariantValue kTestVariant_ = DataExchange::Variant::JSON::Reader{}.Read (R"({
                                                                                "foo" : [ "bar", "baz" ]
                                                                                ,"" : 0
                                                                                ,"a/b" : 1
                                                                                ,"c%d" : 2
                                                                                ,"e^f" : 3
                                                                                ,"g|h" : 4
                                                                                ,"i\\j" : 5
                                                                                ,"k\"l": 6
                                                                                ," ": 7
                                                                                ,"m~n" : 8
                                                                                })"_blob);

        using namespace DataExchange::JSON::Patch;
        {
            OperationItemType o{.op = OperationType::eAdd, .path = JSON::PointerType{"/foo/-"sv}, .value = "newItem"sv};
            VariantValue      vv = o.Apply (kTestVariant_);
            EXPECT_EQ (JSON::PointerType{"/foo"sv}.Apply (vv), (VariantValue{Sequence<VariantValue>{"bar"sv, "baz"sv, "newItem"sv}}));
            EXPECT_EQ (JSON::PointerType{"/ "}.Apply (vv), (VariantValue{7})); // only changed what we wanted to
        }
        {
            OperationItemType o{.op = OperationType::eAdd, .path = JSON::PointerType{"/foo/0"sv}, .value = "newItem"sv};
            VariantValue      vv = o.Apply (kTestVariant_);

            DbgTrace ("1={}"_f, JSON::PointerType{"/foo"sv}.Apply (vv));
            DbgTrace ("2={}"_f, (VariantValue{Sequence<VariantValue>{
                                    "newItem"sv,
                                    "bar"sv,
                                    "baz"sv,
                                }}));
            EXPECT_EQ (JSON::PointerType{"/foo"sv}.Apply (vv), (VariantValue{Sequence<VariantValue>{"newItem"sv, "bar"sv, "baz"sv}}));
            EXPECT_EQ (JSON::PointerType{"/ "}.Apply (vv), (VariantValue{7})); // only changed what we wanted to
        }
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
