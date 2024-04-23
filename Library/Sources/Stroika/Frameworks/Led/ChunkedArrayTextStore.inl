/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ********************** ChunkedArrayTextStore::ChunkAndOffset *******************
     ********************************************************************************
     */
    inline ChunkedArrayTextStore::ChunkAndOffset::ChunkAndOffset (size_t chunk, size_t offset)
        : fChunk (chunk)
        , fOffset (offset)
    {
    }

    /*
     ********************************************************************************
     **************************** ChunkedArrayTextStore *****************************
     ********************************************************************************
     */
    inline size_t ChunkedArrayTextStore::GetLength () const noexcept
    {
        return fLength;
    }
    inline ChunkedArrayTextStore::ChunkAndOffset ChunkedArrayTextStore::FindChunkIndex (size_t charPos) const
    {
        // No cache implemented yet - later check cache - and only call _ version if need be...
        return (FindChunkIndex_ (charPos));
    }
    inline void ChunkedArrayTextStore::InvalCachedChunkIndexes ()
    {
        fCachedChunkIndex         = 0;
        fCachedChunkIndexesOffset = 0;
    }

}
