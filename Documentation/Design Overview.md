# Design Overview

## Assertions

Stroika makes extensive use of assertions to help assure correct code, and to document how to use the Stroika library.

Nearly every API has pre-requisite &#39;Require&#39; statements, and &#39;Ensure&#39; statements which make promises about the state of the object (in methods) or return values. This provides not only documentation, but executable documentation – which makes it much easier to develop correct Stroika applications.

### Debug Builds

In debug builds, all Stroika Assertions are checked – evaluated at runtime, and problems cause the program to abort, or drop into a debugger. These are _not_ recoverable (not subject to turning into exceptions or ignoring).

Debug builds are typically 2 or 3 times slower than release builds (this factor can vary a great deal depending on your program).

It is recommended programs be developed mostly with Debug builds, and transition to release builds more towards the end of a release cycle (but always use a mix of both).

### Release Builds

Release builds have zero overhead from assertions. There is no runtime or space cost.

This is very important to understand, because it the zero cost of assertion checking in the final delivered product helps encourage more use of assertions (by removing the excuse that the check would make the program seem slow for users). And it contributes to why Stroika is a very high performance framework.

---

## Copy by Value

Objects in Stroika are overwhelmingly copy-by-value in semantics, though often copy-by-reference internally, for performance reasons.

For example

String a = L&quot;a&quot;;

String b = a;

b += L&quot;a&quot;;

Assert (a == L&quot;a&quot;);

This principle – copy by value almost everywhere – except where clearly marked by names to the contrary – helps make Stroika semantics visually obvious.

### Ptr objects are something of an exception

There are a few kinds of objects in Stroika that violate this rule about copy-by-value. Some objects only make sense to share. For example, Sockets don&#39;t make sense to copy by value. They are intrinsically associated with network endpoints that cannot be duplicated, and so &#39;copies&#39; are copies of the **pointer** to that object. To emphasize this, Stroika uses the naming convention of Ptr at the end of the name, or as the name, of such objects.

These objects often have methods of the thing they point to – with a variety of convenient overloads etc, but copying those objects logically just copies a reference (pointer) to the underlying shared object.

Examples of such things that are intrinsically (and named) &#39;Ptr&#39; objects include:

- Thread (Thread::Ptr)
- Socket (Socket::Ptr, as well as ConnectionlessSocket::Ptr etc)
- InputStream::Ptr, OutputStream::Ptr, etc…

### Iterator objects – halfway between by value and by reference/Ptr

Stroika also makes extensive use of Iterator objects – and these are logically copied by value, but they are also logically pointers &#39;into&#39; some associated container.

### SharedByValue\<T>

A very helpful template class used internally in Stroika is SharedByValue\<T>. You don&#39;t need to know about this, but it may be helpful in efficiently implementing by-value semantics with nearly by-reference performance impact.

SharedByValue implements &#39;copy-on-write&#39;, fairly simply and transparently. So you store your actual data in a &#39;rep&#39; (letter part of letter-envelope pattern), and when const methods are accessed, you simply dereference the pointer. Such objects can be copied for the performance cost of copying a shared_ptr\<T> - fairly cheap. The only time you pay (a significant) cost, is when you mutate one of these objects (which is already shared) – then you do the copying of the data behind the object.

Stroika's container library, for example, uses SharedByValue, so copying a Stroika Sequence<T> is generally much cheaper than copying a vector<T>. Similarly for String objects (vs std::string). Note however, while this makes copying much cheaper, it makes accessing the insides of a String or Sequence correspondingly more expensive than their vector/string counterparts.

---

## Const and Logical Const

Generally Stroika uses the idea of logical const for its objects, and freely uses mutable for fields to enforce that notion.

But there is one case where this is slightly vague, and at first glance, may appear not fully adhered to: Ptr objects.

Ptr objects are really combinations of two kinds of things – smart pointers – and short-hand accessors for the underlying thing.

Because of the C++ thread safety rules (always safe to access const methods from multiple threads at once so long as no writers, and the need for synchronization on writes) – and because these rules only apply literally and directly to the &#39;envelope&#39; part – or the smart-pointer part of the object, we use the constness on Ptr objects to refer to the ptr itself, and not thing pointed to.

We arguably COULD get rid of PTR objects and just use shared_ptr\<T> or shared_ptr\<const T\> - but then we would lose the convenience of having simple interfaces for reps, and more complex, overloading etc interfaces for calling.

---

## Synchronization (thread safety)

One reason it&#39;s very important to understand what values are copy-by-value, and what are copy-by-reference, is because of understanding thread safety.

All of Stroika is built to be &#39;thread safe&#39;, but automatically synchronizing all operations would create a high and almost pointless performance penalty.

Instead, Stroika mostly follows the C++ STL (Standard Template Library) thread safety convention, of having const methods always safe for multiple readers, and non-const methods ONLY safe with a single caller at a time.

But that only goes one level deep – the outer object you are accessing. For the special case of these &#39;Ptr&#39; objects, the user must also worry about synchronizing the internal shared &#39;rep&#39; objects. The way this is done varies from class to class, and look at the particular &#39;Ptr&#39; classes you are using to see. For example, Thread::Ptr internal rep objects are always internally synchronized (meaning the caller only need worry about synchronizing the Ptr object). Stream internal rep objects are by default, _not_ externally synchronized, but you can easily construct an internally synchronized stream with InternallySynchronizedInputStream\<T>::New () – for example (which creates a new delegating object with locks around each call).

And to synchronize any c++ object, you can always use the utility template Synchronized\<T> - to wrap access to the object. You can also use lock_guard\<>; etc, but Synchronized\<> makes accessing shared data in a thread-safe way MUCH simpler and more transparent (but only synchronizes the &#39;envelope&#39; – not the &#39;shared rep&#39; of &#39;Ptr&#39; objects.

See &#39;Thread Safety.md&#39; for more details.

### Ptr vs. Rep (e.g Streams) thread safety

In the several families of classes, such as Threads, Streams (InputStream, OutputStream etc), Sockets, and others using the letter-envelope paradigm, users must separately consider the thread safety of the letter and the envelope.

The envelope typically follows C++-Standard-Thread-Safety, but the thread safety rules applying to the letter (shared rep object) – depend on how that object was created. So see its Object::New () method for documentation on this.

### Debug::AssertExternallySynchronizedMutex\<T>

To document, and to help ensure that Stroika classes are used in a thread safe manner, the helper class Debug::AssertExternallySynchronizedMutex\<T> is used fairly consistently throughout Stroika to &#39;wrap&#39; objects in a thread-safety-checking envelope. This has no performance cost (space or runtime) in release builds, but has a significant (roughly 2x slowdown) in debug builds.

But it means that if your code runs correctly (without assertion errors) in Debug builds, it&#39;s probably thread safe.

This doesn&#39;t completely replace tools like thread-sanitizer, and valgrind/helgrind, but it does help provide simpler, and clearer diagnostics directly when you are running your threaded applications.

---

## External Validation Tools

Tools like valgrind (helgrind and memcheck), and sanitizers (address, undefined behavior, and thread sanitizer) are all regularly run as part of the Stroika regression test suite, and are a sensible addition Stroika-based development process.

They are especially useful to help validate that any subtle bugs aren&#39;t present ONLY in release builds, but not in debug builds (extremely rare, but it can happen).

All Stroika's regression tests are regularly run with valgrind and sanitizers.

---

## Coding Conventions

### Formatting

make format-code

I&#39;m not even slightly happy about the way this looks but I&#39;ve found no better alternative. At least its automated and consistent. It can be configured to use astyle or clang-format, but I&#39;ve found clang-format slightly less buggy.

### STL naming versus &quot;Studly Caps&quot; or &quot;CamelCase&quot;

I personally prefer the style &quot;CamelCase&quot; – probably because I first did object oriented programing in Object Pascal/MacApp – a few years back. Maybe there is another reason. But now it&#39;s a quite convenient – providing a subtle but readable visual distinction.

All (or nearly all) Stroika classes, and methods use essentially the same &#39;Studly Caps&#39; naming styled from MacApp, with a few minor deviations:

However, STL / stdc++ - has its own naming convention (basically all lower case, and \_), plus its own words it uses by analogy / convention throughout (e.g. begin, end, empty).

Stroika methods will start with an upper case letter, EXCEPT in the case where they method mimics for follows an existing STL pattern. If you see lower case, assume the function follows STL semantics. If you see CamelCase, you can assume it follows Stroika semantics.

For example:

String:: SubString () follows Stroika semantics (asserting if values out of range).

String::substr() follows the semantics of STL&#39;s basic_string\&lt;\&gt;::substr().

Note – this &#39;convention&#39; doesn&#39;t replace documentation (the behavior of each method is documented). It just provides the user/reader a quick subtle convenient visual cue which semantics to expect without reading the docs.

Examples of common STL methods which appear in Stroika code (with STL semantics):

- clear()
- empty()
- push_back
- erase
- c_str()
- length()
- size()
- begin() // sort of – usually using Stroika iterators
- end() // ditto

### Name Prefixes and Suffixes

#### Prefixes

- the &#39;f&#39; prefix for data members (fields)
- &#39;k&#39; prefix for constants
- We use the &#39;e&#39; prefix for enumerators
- &#39;t&#39; prefix for thread_local variables
- &#39;s&#39; prefix for static varaibles.
- &#39;\_&#39; prefix for PROTECTED instance variables or functions
- 'I' prefix for concepts (since they essentially ACT as interfaces, but are used in a context where they cannot be confused with subclassable interfaces)

#### Suffix

- &#39;\_&#39; suffix for PRIVATE instance variables or functions

#### Case

- functions start with upper case (camelcase)
- local variables start with lower case (camelcase)
- properties start with lower case (camelcase) - because they are a hybrid between field and function, but syntactically you must treat them sometimes slightly differently than either, so this makes more clear what is going on. 'f' for plain field, and just starting with lower case to indicate a property.

####
Rationale

Note this slight variation of naming convention from the std c++ standard is helpful, in that it conveys information with practically no cost. The Stroika
functions will tend to have slightly different behavior and guarantees than std c++ (stl) counterparts, and the naming convention makes it a little easier to 
read code with the two libraries intermingled, knowing which style is in use. (for example, stl iterators have different requirements than Stroika ones, Stroika functions tend to use assertions (pre/post condition assertions), that STL/stdc++ libraries dont, etc).

### Begin/End versus start/length

STL is reasonably consistent, with most APIs using T\* start, T\* end, but some APIs use length instead of end. The Stroika convention is to always use T\* start, T\* end.

#### Rationale

One, this gives more consistent expectations. That&#39;s especially important for APIs that use offsets (like String) – so that it&#39;s obvious the meaning of integer parameters.

And it avoids problems with overflow. For example, if you had an API like:

```c++
basic_string substr(
   size_type _Off = 0,
   size_type _Count = npos
) const
```

To map this to an internal representation you have todo:

char\* s = m_bufPtr + \_Off;

char\* e = m_bufPtr + \_Off + \_Count;

but if count was numeric_limits\&lt;size_t\&gt;::max(), then the e pointer computation would overflow. There are ways around this, but mixing the two styles creates a number of problems - but for implementations – and for use.

### Use ++prefix instead of postfix++ preferentially (and --prefix)

#### Rationale

It often doesn't matter which you use, but sematically postfix requires returning the previous
value which involves an extra copy. This CAN in PRINCIPLE be costly. So in things like
for loops for the re-init case, and anyplace else we dont really count on the postfix++, just
use prefix preferentially. Similarly for --;

See https://stackoverflow.com/questions/24901/is-there-a-performance-difference-between-i-and-i-in-c

### User-defined literals

- Stroika makes extensive use of the builtin `operator"" sv`, which produces (more efficient) String objects (really the STL version produces string_view but Stroika's String class converts string_view to a String more efficiently - reusing the space for the characters)

  Stroika also provides `operator"" _k` which does about the same thing (producing String_Constant) but we use internally and encourage use of operator"" sv (since its a standard and does about the same thing). `operator"" _k` is only provided as an option because there are a few cases of ambiguity where its helpful.

- `operator"" \_RegEx () can be used as a shortcut for defining regular expressions

  ```c++
  RegularExpression re = L"foo.*"_RegEx;
  ```

### Uniform initialization {} vs. older style paren initialization ()

- Generally prefer Uniform initialization syntax {} because

  - makes construction appear distint from function calls
  - rule about no narrowing/implicit conversions (MAYBE? helpful - unclear)

- DONT use uniform initialzation syntax with call to perfect forwarding in templates. The natural behavior
  with perfect forwarding is that you get the same effect with the wrapper object as you would have had with initializing
  the underlying (wrapped) object. But initializing that object generally would have allowed implicit conversions (esp int to size_t).

- VERY UNCERTAIN about this policy - as of 2020-08-07 - just an experimental plan.

### 'Quasi-namespace'

Why are Ptr objects &#39;struct&#39; / &#39;class&#39; instead of actual namespaces?

This is the use of a struct as a namespace. Now that C++ has namespaces, you might ask - why would anybody do this? The answer is that struct allows defining part of the namespace is PRIVATE, and part public. This can be sometimes useful,
and that is why Stroika occasionally uses this pattern.

Things like &#39;Stream&#39; or &#39;Socket&#39; – are just logical groupings

These logical groupings could have implemented using actual namespaces or just struct&#39;s acting as &#39;quasi&#39; namespaces.

Advantages of using &#39;namespace&#39;:

- This is logically the best fit
- There are a number of namespace specific features – inlining and importing namespaces to leverage

Advantages of using struct/class

- Namespaces offer no mechanism for private or protected access control, which for many of our uses is sometimes helpful
- Namespaces cannot be templated (classes/structs can) – which makes clearer the grouping between classes (e.g. InputStream\<char>::Ptr and InputStream\<char>::\_IRep more clearly related than InputStream::Ptr\<char> and InputStream::\_IRep\<char>).
- Classes provide their own mechanism for automated related lookup (nested classes see members from parents) – which is helpful

In the end – no very strong arguments, but for now I&#39;ve gone with &#39;struct/class&#39; in several places.

### New static methods and Factories

In Stroika, a New () is static method, which allocates an instance of some class, but returns some kind of shared_ptr/smart pointer to the type – not a bare C++ pointer.

Stroika doesn&#39;t make much use of the factory pattern, but occasionally – it is useful. If the type provided by the factory is exactly the type of a given class, then we generally use

struct T_Factory {

    static T New();

};

That technique is used to control the default kind of containers (backend algorithm) that is used.

Or for Stream classes, the &#39;stream quasi namespace&#39; contains a New method to construct the actual stream, and the definition of the Ptr type – smart pointer – used to access the stream.


### static T::Parse () method versus T () constructor

Constructors return an object, not optional, of that object. You could just have the constructor throw an
exception when it fails to construct (due to bad arguments) - but sometimes its handy to just return optional<T> for that case.

One case where this is commonly true is with parsing arguments (like a date, or a URL). So for these cases, Stroika provides
a static Parse () function, which returns optional<T>, and acts kind of like a constructor, except that it returns
missing when unable to 'parse' its arguments.

Another reason why its sometimes helpful to use the static name Parse() instead of a constructor argument, is for clarity take
that you aren't converting (so maybe accidentally implicitly converting) a string into that object type, but explicitly noting
that the string is being parsed into that object type.


### <a name="Comparisons"></a> Comparisons: spaceship operator (operator<=>), operator== etc…

Stroika types generally support the c++20 operator== and operator<=> semantics and operators.

But for many classes, for example, 'set' containers, it matters if the function argument is an equality comparer, or ordering comparer, and the C++ comparison syntax does't make that distinction (less and equal are two functions objects
that have the same 'signature' but one 'works' in a std::set, but the other fails pretty badly).

Stroika uses a utility class template ComparisonRelationDeclaration<> and some related classes and functions and types, to annotate function objects
and some concepts to filter, so you can declare the type of comparison relation function.

NOTE - if you use 'three-way-comparers', there is no need for that, as their function signature is enough to automatically detect what they are.
<a href="https://stroika.atlassian.net/browse/STK-980">Stroika containers and concepts for equality comparosn etc should automatically convert/handle operator<=></a>

### Using T= versus typedef

C++11 now supports a new typedef syntax – using T=…. This is nearly the same as typedef in terms of semantics.

Stroika code will generally use the using T = syntax in preference to typedef for two reasons:

- The using = syntax is slightly more powerful, in that it supports defining derivative template typdefs.
- And more importantly, I believe it makes code more readable, because the type of INTEREST is the one being defined = which appears first. What it maps to is often more complicated (why we define the typedef) – and one can often ignore that detail (or skim it).

### Procedure name suffixes for string return type

The Windows SDK uses the convention of appending a W to the end of a function name that uses wide characters, and an A to the name that uses the current operating system locale for code page.

In C++ (and Stroika) – this convention is also generally unneeded, because of the availability of overloading.

Stroika generally avoids this issue by returning String classes nearly everywhere – which are Unicode based. But as the Stroika String classes uses the rest of the Stroika infrastructure – including thread interruption, it&#39;s sometimes inconvenient for some low level coding to use those String classes.

But you cannot overload on return types.

For this reason, a handful of Stroika APIs follow the convention of a suffix of:

- \_U for wstring return
- \_N for string return, being interpreted as the &#39;narrow SDK code page&#39;
- \_A for string returns which are guaranteed to be ASCII

### Magic Statics (Initialization)

Instead of using ModuleInit\<> - which Stroika used until 2.1b7, we now use
a combination of magic-inits, in most places, but occasionally call_once() with atexit().

This MAY not be a great idea. Its a little simpler, and will look more standardized to most eyes. But the ModuleInit<> mechaism trades off performace at startup for later performance not having to check if initialized. So not sure its a good switch.

<https://stackoverflow.com/questions/26759511/do-magic-statics-guarantee-that-right-side-is-executed-only-once>

Generally, when you have a shared bit of static content and are sure its only accessed after main, you can store it as:

```c++
namespace {
  mutex sMyMutex_;  // for example
}
```

But if this may be used before main,

```c++
namespace {
  mutex& sMyMutex_Get_ ()
  {
    static mutex sMyMutex_;
    return sMyMutex_;
  }
}
```

### class/method Final

- It does very little good (helps optimization sometimes but also can needlessly limit programmer options later)
- It CAN serve as a bit of DOCUMENTATION about what is intended to be extended or not (but unfortunately its not a good fit for this because you cannot say 'generally intended not to subclass' - just 'may not').
- So – Stroika use of final is haphazard – used when its clearly not going to be extended in CASE it might help performance, but rarely otherwise).

### Final Singleton objects (kThe, Get, Set)

Singleton objects are a common pattern. Stroika doesn't use these a ton, but some. One issue with singletons to be careful about is thread safety. Stroika leverages a couple of patterns to handle this.

#### kThe for constant (immutable) singletons

Where the object is intrinsically constant, we follow the pattern of

```c++
struct EOFException ... {
  static const EOFException kThe;
}
```

```c++
struct InterruptException ... {
  static const InterruptException kThe;
}
```

Here since the objects are constant, thread safety is obviously not an issue. Doing this where constexpr is not possible DOES present an issue with
accessing these objects before main(). Avoid this pattern (without constexpr) if the object maybe needed before main (use Get () below)

#### Get (and sometimes Set) for mutable singletons

Mutable singletons are accessed by the Get() static method. By definition of a mutable singleton, it will have some non-const methods.

Variants of this pattern are safe to use before main (because the Get() method can ensure the underlying singleton object is constructed before returning it).

##### Return mutable reference

Within this category of singletons, sometimes we have the Get method return a mutable reference to the global object.

For example:

```c++
struct Logger ... {
  static Logger& Get ()
}
```

In this case, all mutable methods (such as Logger::SetAppender(), SetSignalHandlers::SetSignalHanlders()) are internally syncrhonized,
and so safe to call from any thread.

This pattern is safe to use before main (because the Get() method can ensure the underlying singleton object is constructed before returning it).

##### Return shared_ptr<> or some other SharedValue() type object, and support singleton static Set method

Another pattern is to return a copyable managed object (either shared_ptr, or something that internally has a shared_ptr or like object).

For example,

```c++
struct InternetMediaTypeRegistry ... {
  static InternetMediaTypeRegistry Get ()
  static void Set (InternetMediaTypeRegistry newRegistry)
}
```

In this case, you Get a copy of the global object, update it, and then call Set() to reset the shared/default copy of the given object.

In this case, the mutable methods need NOT be internally synchronized, but the Get/Set static functions are guarnateed internally synchronized.

And something like

```c++
InternetMediaTypeRegistry::Get()->GetSomeValue()
```

is safe even if done in parallel with an update to the InternetMediaTypeRegistry (via Set) because the "Get" call maintains a temporary shared_ptr() reference to the old value when the new value is being updated.

**NOTE** Doing

```c++
SINGLETON::Get()->SomeMutableMethod()
```

MAY NOT be safe, depending on the particular type. See each singleton type to see how to call its mutable singleton methods.

### Concepts usage

Stroika templates make substantial use of concepts to help provide documentation about expectations and better error messages.

Since concepts are largely 'interfaces' and syntactically, cannot be confused with abstract class 'interfaces', Stroika uses the naming convention
of starting each concept with the prefix 'I'.

### Quietly

Most Stroika functions raise an exception when they fail. For example, Wait () methods etc, Parse() methods, etc.
But sometimes you just want the thing to return an optional result (for speed and simplicity - cuz the failure case is common, not
exceptional). To simplify these situations, many APIs have a 'Quietly' variant, that does what the main function does, but instead of raising an exception when it fails, it returns a nullopt.

### Deprecating old APIs

#### Use [[deprecated()]]

For many APIs (e.g. member functions) - we annotate the API with a deprecated attribute, and document the new API to be used instead.
We try to keep these around til the major version switch (e..g til we switch from 2.1b9 to 2.1rc, or 3.1a, to 3.1b, etc).

#### Use inline namespaces

For large sections of code which may change (e.g. the ORM or SQL APIs), we may version the entire API section with
inline namespaces

```c++
namespace Stroika::Foundation::Database::SQL:: inline v1 {
  // just add the inline v1 and a new section :v2 (to test backward compatabilty works)
  // Maybe provide #define to control if v1 or v2 is the default? Or maybe just quick test run
  // that all builds with one inline and then swtich to the other and require users to switch to explicitly add the 'v1'/v2 (and default to v2).
}
```

### Cosmetic Hint Macros

#### nonvirtual

```c++
struct b {
    virtual void f (){};
};
struct d : b {
    void f (){};
};
```

for this C++, f is really 'virtual'. It would be best if the compiler
warned that there was no 'override' directive, but at least some
compilers dont do that.

So when you see a function declaration WITHOUT a virtual or override, its not clear if that function is virtual or not.

Stroika uses the CONVENTION (sadly unenforced by the compilers)
of using the macro nonvirtual as a hint/reminder that the given function
is NOT virtual (not even implicitly because the base contains a virtual
with the same signature).


### Range-based for loop (auto)

I really have NO IDEA what is best here. I've searched alot and found no clear guidance. But consistency appears a virtue, so I've come up with a policy and documented. Its NOT ALWAYS right, just a good default

This form makes clear the 'c' in the loop is readonly and just examined in the loop.
~~~c++
for (const auto& c : container) {...}     // this is used most of the time OR
for (const String& c : container) {...}   // assuming String is the value_type of the container
~~~

Using the explicit name just makes it a little easier sometimes to see in the code where 'c' is used what its type is.

When the 'c' value will be modified in place, or if I KNOW size is small/basic type, I may use
~~~c++
for (auto c : container) {...}    // OR
for (String c : container) {...}  // assuming container is container of <String>
~~~

I cannot see the utility - except maybe in templated code where you may want to forward values, things like
~~~c++
for (auto&& c : container) {...}    // why except maybe

for (auto&& c : container) { someapi (forward<T> (c)); }
~~~

NB - use of auto& c won't work with Stroika Iterator<> classes (since operator* returns const reference only as we don't allow updating containers by fiddling with the iterator only.
~~~c++
for (auto& c : container) {...}    // WONT WORK WITH STK ITERABLE<>
~~~

