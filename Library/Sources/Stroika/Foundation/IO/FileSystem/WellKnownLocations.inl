/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ********************* FileSystem::ThroughTmpFileWriter *************************
     ********************************************************************************
     */
    inline filesystem::path FileSystem::WellKnownLocations::GetTemporary ()
    {
        return filesystem::temp_directory_path ();
    }

}
