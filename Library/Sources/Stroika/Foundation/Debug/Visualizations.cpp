/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/StringBuilder.h"
#include "../Common/GUID.h"
#include "../Database/SQL/Connection.h"
#include "../Database/SQL/Statement.h"
#include "../IO/Network/CIDR.h"
#include "../IO/Network/InternetAddress.h"
#include "../IO/Network/SocketAddress.h"
#include "../IO/Network/URI.h"
#include "../Time/Date.h"
#include "../Time/DateTime.h"
#include "../Time/TimeOfDay.h"

#include "Visualizations.h"

using namespace std;

using namespace Stroika::Foundation::Characters;

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