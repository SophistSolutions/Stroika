/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_LineBasedPartition_h_
#define _Stroika_Frameworks_Led_LineBasedPartition_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/PartitioningTextImager.h"

/*
@MODULE:    LineBasedPartition
@DESCRIPTION:
        <p>A LineBasedPartition is a concrete @'Partition' subclass in which the character '\n' is used to break
    up the text into pieces (partition elements). This is the most basic, default, standard kind of Partition.</p>
 */

namespace Stroika::Frameworks::Led {

    /**
     *              <p>Partition which implements the partitioning based on NL characters.</p>
     *              <p>As of Led 3.0, it is the only concrete partition supported, and its the overwhelmingly
     *          most likely one you would want to use. But - in some applications (notably LogoVista Translator Assistant) -
     *          it can be quite handy being able to override how this partitioning takes place.</p>
     */
    class LineBasedPartition : public Partition {
    private:
        using inherited = Partition;

    public:
        LineBasedPartition (TextStore& textStore);

    protected:
        enum SpecialHackToDisableInit {
            eSpecialHackToDisableInit
        };
        LineBasedPartition (TextStore& textStore, SpecialHackToDisableInit hack);
        virtual void FinalConstruct () override;

    public:
        virtual void UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) noexcept override;

    protected:
        virtual void CheckForSplits (PartitionMarker* pm, const UpdateInfo& updateInfo, size_t i) noexcept;
        virtual bool NeedToCoalesce (PartitionMarker* pm) noexcept;

#if qStroika_Foundation_Debug_AssertionsChecked
    protected:
        virtual void Invariant_ () const override;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LineBasedPartition.inl"

#endif /*_Stroika_Frameworks_Led_LineBasedPartition_h_*/
