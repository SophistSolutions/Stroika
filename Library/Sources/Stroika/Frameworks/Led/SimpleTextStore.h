/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SimpleTextStore_h_
#define _Stroika_Frameworks_Led_SimpleTextStore_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    SimpleTextStore
@DESCRIPTION:
        <p>SimpleTextStore is a module for the implementation of the class - @'SimpleTextStore' -
    a very simple implementation of the @'TextStore' class.</p>
 */

#include "TextStore.h"

namespace Stroika::Frameworks::Led {

    /*
    @CLASS:         SimpleTextStore
    @BASES:         @'TextStore'
    @DESCRIPTION:
            <p>SimpleTextStore is a very simple implementation of the TextStore API.
        It is useful as a starting point for any new TextStore implementations, because of
        its simplicity. And it is useful for understanding.</p>
    */
    class SimpleTextStore : public TextStore {
    public:
        SimpleTextStore ();
        virtual ~SimpleTextStore ();

    public:
        virtual TextStore* ConstructNewTextStore () const override;

    public:
        virtual size_t GetLength () const noexcept override;
        virtual void   CopyOut (size_t from, size_t count, Led_tChar* buffer) const noexcept override;
        virtual void   ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount) override;

    protected:
        nonvirtual void InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after);
        nonvirtual void DeleteAfter_ (size_t howMany, size_t after);

    public:
        virtual void AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner) override;
        virtual void RemoveMarkers (Marker* const markerArray[], size_t markerCount) override;
        virtual void PreRemoveMarker (Marker* marker) override;
        virtual void SetMarkerRange (Marker* m, size_t start, size_t end) noexcept override;
        virtual void CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const override;

    private:
        size_t          fLength;
        Led_tChar*      fBuffer;
        vector<Marker*> fMarkers;

#if qDebug
    protected:
        virtual void Invariant_ () const override;
#endif
    };

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    inline size_t SimpleTextStore::GetLength () const noexcept
    {
        return (fLength);
    }

}

#endif /*_Stroika_Frameworks_Led_SimpleTextStore_h_*/
