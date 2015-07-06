/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStream_h_
#define _Stroika_Foundation_Streams_TextInputStream_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Configuration/Common.h"

#include    "InputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   TextInputStream::Read () should have overload returning Optional (like its baseclass)
 *
 *      @todo   TextInputStream::ReadLines () NYI
 *
 *      @todo   Add Close() as with - binaryinputstream!
 *
 *      @todo   Consider making LineEnd format (LF,CR,CRLF, or Auto) an optional param to ReadLine().
 *              Then it would ONLY require Seekable() for CRLF or Auto.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;
            using   Characters::String;

#pragma message ("Warning: TextInputStream.h FILE DEPRECATED - use InputStream<Character>")


            using TextInputStream = InputStream<Character>;

#if 0
            /**
             *  @todo DOCS OBSOLETE
             *
             * Design Overview:
             *
             *      o   All read's on a TextInputStream are BLOCKING. If there is a desire to have a
             *          non-blocking read, then create a new mixin interface and through that interface
             *          you can do non-blocking reads, but this Read() method must always block.
             *
             *      o   EOF is handled by a return value of zero. Once EOF is returned - any subsequent
             *          calls to Read () will return EOF (unless some other mechanism is used to tweak
             *          the state of the object, like a mixed in Seekable class and calling SEEK).
             *
             *      o   Exceptions indicate something went wrong - like an IO error, or  formatting
             *          effort (e.g. if the source is encrypted, and the stream is decrypting, then
             *          it might detect a format error and throw).
             *
             *      o   TextInputStream and TextOutputStream CAN be naturally mixed togehter to make
             *          an input/output stream. Simlarly, they can both be mixed together with Seekable.
             *          But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             *
             *  ReadString/Seekable/PutBack Design Choices:
             *      o   Some common read methods with TextStreams (parsing) - involve some amount of lookahead.
             *          Lookahead COULD be implemented a number of ways:
             *          o   Seek backwards after fetching
             *          o   Special 'put back' variable/API - which allows you to put back either one,
             *              or a number of characters back into the input Q
             *          o   A special proxy object which stores the extra data, and maintains the context
             *              of the state of pre-reading.
             *
             *      Each of these approaches has some advantages and disadvantages. Just using Seek() is
             *      the simplest approach. IF all your streams support seeking, there is no reason for another
             *      mechanism. But we dont want to alwys require seeking.
             *
             *      PutBack () is like Seek, but then the question is - do we support just a PutBack of
             *      one character? So only lookahead of one character? That deals with many cases, but not all.
             *      And how does it interact with Seek - if the stream happens to be seekable? And is the
             *      PutBack buffer stored in the letter or envelope class? If in Letter, thats extra work
             *      in every rep (barrier to providing your own stream subtypes). If in the envelope, it doesn't
             *      work intuitively if two variables have separate smart pointers to the same underlying stream.
             *      Reads and writes affect each other EXCPET for the putback buffer!
             *
             *      A special Proxy object is a good choice - but requires the caller todo a lot of extra
             *      work. To some extent that can be automated by somewhat automatically maanging
             *      the proxy storage object in the smartpointer, but thats alot of work and concept
             *      for very little gain.
             *
             *      In the end - at least for now - I've decided on KISS - ReadLine() simply requires it
             *      is Seekable. And there are plenty of wrapper stream classes you can use with any stream
             *      to make them Seekable.
             */
            class   TextInputStream : public InputStream<Character> {
            private:
                using inherited = InputStream<Character>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit TextInputStream (const _SharedIRep& rep);

            public:
                /**
                 *  defaults to null (empty ())
                 */
                TextInputStream () = default;
                TextInputStream (nullptr_t);
                TextInputStream (const InputStream<Character>& from);


#if 0
            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. bufSize must
                 *  always be >= 1. Returns 0 iff EOF, and otherwise number of characters read BLOCKING until data
                 *  is available, but can return with fewer bytes than bufSize without prejudice about how much
                 *  more is available.
                 */
                nonvirtual  size_t  Read (wchar_t* intoStart, wchar_t* intoEnd) const;
                nonvirtual  size_t  Read (Character* intoStart, Character* intoEnd) const;

                /**
                 *  Blocking read of a single character. Returns a NUL-character on EOF ('\0')
                 */
                nonvirtual  Character   ReadCharacter () const;

            public:
                /**
                 * Readline looks for a trailing bare CR, or bare LF, or CRLF. It returns whatever line-terminator
                 * it encounters as part of the read line.
                 *
                 *  ReadLine() will return an empty string iff EOF.
                 */
                nonvirtual  String ReadLine () const;

            public:
                /**
                 *  Returns Iterable<String> object, so you can
                 *  write code:
                 *          for (String line : stream.ReadLines ()) {
                 *          }
                 *
                 *  Like ReadLine(), the returned lines include trailing newlines/etc.
                 */
                nonvirtual  Traversal::Iterable<String> ReadLines () const;

            public:
                /**
                 *  Read from the current seek position, until EOF, and accumulate all of it into a String.
                 *  Note - since the stream may not start at the beginning, this isn't necessarily ALL
                 *  that was in the stream -just all that remains.
                 */
                nonvirtual  String ReadAll () const;
#endif
            };
#endif


        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "TextInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_TextInputStream_h_*/
