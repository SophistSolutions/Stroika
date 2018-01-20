/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_LedNative_h_
#define _Stroika_Frameworks_Led_StyledTextIO_LedNative_h_ 1

/*
@MODULE:    StyledTextIO
@DESCRIPTION:
        <p>A portable attempt at abstracting away the details of styled text file IO and all
    the different formats for styled text.</p>
        <p>This code defines APIs which should allow for reading any different styled text format.
    And dumping it to any different output (eg. text buffer).</p>
        <p>The only real LED-SPECIFIC parts of this are that I only provide concrete output (aka sinks)
    implementations to Led StandardStyledTextImagers. And that some of the Src/Sink APIs are oriented towards what would be
    helpful for a Led-based editor (in other words, features not supported by Led aren't communicated to/from the src/sinks).</p>
        <p>The <em>big picture</em> for this module is that there are two main basic subdivisions. There are
    @'StyledTextIOReader' subclasses, and @'StyledTextIOWriter' subclasses. The readers are for READING some file format,
    and converting it to a stream of method calls (on a sink to be described later). And writers are for writing those
    formatted data files, based on results of method calls on an abstract source class.</p>
        <p>Though @'StyledTextIOReader' and @'StyledTextIOWriter' share no common base class, they <em>do</em> follow
    a very similar design pattern. They both define abstract 'sources' and 'sinks' for their operation.</p>
        <p>For a @'StyledTextIOReader', it reads its data from a @'StyledTextIOReader::SrcStream' (typically maybe a file),
    and writes it to a @'StyledTextIOReader::SinkStream' (typically a Led-text-buffer/view).</p>
        <p>A @'StyledTextIOWriter', writes data extracted from a @'StyledTextIOWriter::SrcStream'
    (typically view/textstore, much like a @'StyledTextIOReader::SinkStream'),
    and writes it to a @'StyledTextIOWriter::SinkStream' (typically an output file).</p>
        <p>These abstract sources and sinks are defined to just the minimal pure virtual APIs needed to extract/write bare bytes,
    or formatted text in a format Led can understand. Several concrete instantiations of each are provided by Led (some here, and
    some in other modules, as appropriate).</p>
        <p>Subclasses of @'StyledTextIOReader' and @'StyledTextIOWriter' are where the knowledge of particular file formats resides.
    For example, the knowledge of how to read RTF is in @'StyledTextIOReader_RTF' and the knowledge of how to write HTML is in
    @'StyledTextIOWriter_HTML'.</p>
 */

#include "StyledTextIO.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {
            namespace StyledTextIO {

                /*
                @CLASS:         StyledTextIOReader_LedNativeFileFormat
                @BASES:         @'StyledTextIOReader'
                @DESCRIPTION:
                */
                class StyledTextIOReader_LedNativeFileFormat : public StyledTextIOReader {
                public:
                    StyledTextIOReader_LedNativeFileFormat (SrcStream* srcStream, SinkStream* sinkStream);

                public:
                    virtual void Read () override;
                    virtual bool QuickLookAppearsToBeRightFormat () override;

                protected:
                    nonvirtual void Read_Version4 (const char* cookie);
                    nonvirtual void Read_Version5 (const char* cookie); // Led 2.1 file format
                    nonvirtual void Read_Version6 (const char* cookie); // Introduced for Led 2.2a2

                    // handles default ones Led knows about. You must override to handle your own private types..
                protected:
                    virtual SimpleEmbeddedObjectStyleMarker* InternalizeEmbedding (Led_PrivateEmbeddingTag tag, size_t howManyBytes);
                };

                /*
                @CLASS:         StyledTextIOWriter_LedNativeFileFormat
                @BASES:         @'StyledTextIOWriter'
                @DESCRIPTION:
                */
                class StyledTextIOWriter_LedNativeFileFormat : public StyledTextIOWriter {
                public:
                    StyledTextIOWriter_LedNativeFileFormat (SrcStream* srcStream, SinkStream* sinkStream);

                public:
                    virtual void Write () override;

                protected:
#if !qWideCharacters
                    nonvirtual void Write_Version5 (); // Led 2.1 file format
#endif
                    nonvirtual void Write_Version6 (); // Introduced for Led 2.2a2

                    // handles default ones Led knows about. You must override to handle your own private types..
                protected:
                    virtual void ExternalizeEmbedding (SimpleEmbeddedObjectStyleMarker* embedding);
                };

                /*
                 ********************************************************************************
                 ***************************** Implementation Details ***************************
                 ********************************************************************************
                 */
            }
        }
    }
}

#endif /*_Stroika_Frameworks_Led_StyledTextIO_LedNative_h_*/
