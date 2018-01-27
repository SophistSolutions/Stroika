Overview
--------
  This folder contains all the Stroika Library Foundation::Streams source code..

  A Stream&lt;ELEMENT_TYPE&gt; is a sequence of data elements made available over time. These elements
  are typically 'Bytes' - or 'Characters' - but can be any copyable type.


Design Overview
--------
   *   A Stream&lt;ELEMENT_TYPE&gt; is a sequence of data elements made available over time.
       These elements are typically 'Bytes' - or 'Characters' - but can be 
       any copyable type.

   *   Streams are created, and then handled ONLY through smart pointers. Assigning Streams
       merely copies the 'smart pointer' to that same underlying stream. This means that offsets
       and underlying data, are all shared.
 
   *   Streams have two parallel hierarchies, which mirror one another, of smart pointers and related
       'virtual rep' objects which provide the API which implementers override.
 
   *   Seek Offsets are in elements of the kind of stream (e.g in Bytes for a Stream<Byte>, and
       in Characters for a Stream<Character>).
 
   *   Two important subclasses of Stream<> are InputStream<>::Ptr (for reading) and OutputStream<>::Ptr (for
       writing). So that each can maintain its own intrinsic current offset (separate seek offset
       for reading and writing) in mixed (input/output) streams, the actual offset APIs and
       logic are in those subclasses.
 
   *   Stream APIs are intrinsically blocking. This makes working with them simpler, since code
       using Streams doesn't need to be written to handle both blocking and non-blocking behavior.
	   You can always use the stack to maintain the context of your operation (clearer than closures)
	   and never be surprised when a 'mode' is set on a stream making it non-blocking.
 
   *   Relationship with std iostream
       
	   Stroika streams are in many ways similar to iostreams. They are interoperable with iostreams
       through the Streams::iostream::(InputStreamFromStdIStream, InputStreamToStdIStream,
       OutputStreamFromStdIStream, OutputStreamToStdIStream) classes.

       *   Stroika Streams are much easier to create (just a few intuitive virtual methods
       to override.

       *   Stroika streams are unrelated to formatting of text (@todo what other mechanism do we offer?)

       *   Stroika Streams are much easier to use and understand, with better internal error checking,
           (like thread safety), and simpler, more consistent naming for offets/seeking, and seekability.

       *   Stroika supports non-seekable streams (needed for things like sockets, and certain specail files, like
           Linux procfs files).

       *   Due to more orthoganal API, easier to provide intuitive simple adapters mapping one kind of stream
           to another (such as binary streams to streams of text, like the .net TextReader).

       *   (@todo - fill in more differences if there are any more?)


   *   Relationship with .Net Streams
       *   Vaguely similar.

       *   Reader/Writer classes not really needed, because the reading assistance APIs (like readlines) are baked
           into InputStreamPtr<>/OutputStream<>
		   
       *   Seekability handed similarly

       *   Stroika Streams have no async APIs, and (currently) no 'CanRead' etc operations

       *   more???

 
   *   Relationship with Java8 Streams
       *   These were inspirational. But Java Streams package is much more complex, with tons of tie
		   ins to threaded processing, map reduce etc.

       *   I currently have stuff like 'fiter' builtin to Iterable. But maybe here would be sensible, and possibly
	       better?

       *   As I have more time to consider this, I may move to more of an approach like Java8 Streams.
