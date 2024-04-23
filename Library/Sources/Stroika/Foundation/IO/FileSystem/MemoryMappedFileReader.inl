/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ******************* IO::FileSystem::MemoryMappedFileReader *********************
     ********************************************************************************
     */
    inline const byte* MemoryMappedFileReader::begin () const
    {
        return fSpan_.data ();
    }
    inline const byte* MemoryMappedFileReader::end () const
    {
        return fSpan_.data () + fSpan_.size ();
    }
    inline size_t MemoryMappedFileReader::size () const
    {
        return fSpan_.size ();
    }
    inline span<const byte> MemoryMappedFileReader::AsSpan () const
    {
        return fSpan_;
    }
    inline span<const byte> MemoryMappedFileReader::operator() () const
    {
        return fSpan_;
    }

}
