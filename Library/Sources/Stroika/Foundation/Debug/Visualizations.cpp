/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Database/SQL/Statement.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/TimeOfDay.h"

#include "Visualizations.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;

namespace {

    Visualizations::StringRep<char8_t>  kFake_8_;
    Visualizations::StringRep<char16_t> kFake_16_;
    Visualizations::StringRep<char32_t> kFake_32_;

}
/**
 *  Only purpose of this is for debugger, since most debuggers support natively visualizing wstring
 */

namespace Stroika::Foundation::Debug::Visualizations {
    bool Private_::gMagicSoIncluded = true;

    void ForceInclude ()
    {
        // automatically 'forced include' if you #include header and are building qDebug. But to include otherwise (e.g. release build) - call this method.
    }
    u8string ToStdString (const Stroika::Foundation::Characters::StringBuilder<>& t)
    {
        return t.str ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::Common::GUID& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::IO::Network::CIDR& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::IO::Network::InternetAddress& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::IO::Network::SocketAddress& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::IO::Network::URI& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::Database::SQL::Connection::Ptr& t)
    {
        // @todo improve this so shows dereferenced ToString() data
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::Database::SQL::Statement& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::Time::Date& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::Time::DateTime& t)
    {
        return t.ToString ().AsUTF8 ();
    }
    u8string ToStdString (const Stroika::Foundation::Time::TimeOfDay& t)
    {
        return t.ToString ().AsUTF8 ();
    }
}