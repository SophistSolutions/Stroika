# Coding Conventions

---

## Formatting

make format-code

I&#39;m not even slightly happy about the way this looks but I&#39;ve found no better alternative. At least its automated and consistent. It can be configured to use astyle or clang-format, but I&#39;ve found clang-format slightly less buggy.

---

## STL naming versus &quot;Studly Caps&quot; or &quot;CamelCase&quot;

I personally prefer the style &quot;CamelCase&quot; – probably because I first did object oriented programing in Object Pascal/MacApp – a few years back. Maybe there is another reason. But now it&#39;s a quite convenient – providing a subtle but readable visual distinction.

All (or nearly all) Stroika classes, and methods use essentially the same &#39;Studly Caps&#39; naming styled from MacApp, with a few minor versions:

However, STL / stdc++ - has its own naming convention (basically all lower case, and \_), plus its own words it uses by analogy / convention throughout (e.g. begin, end, empty).

Stroika methods will start with an upper case letter, EXCEPT in the case where they method mimics for follows an existing STL pattern. If you see lower case, assume the function follows STL semantics. If you see CamelCase, you can assume it follows Stroika semantics.

For example:

 String:: SubString () follows Stroika semantics (asserting if values out of range).

 String::substr() follows the semantics of STL&#39;s basic\_string\&lt;\&gt;::substr().

Note – this &#39;convention&#39; doesn&#39;t replace documentation (the behavior of each method is documented). It just provides the user/reader a quick subtle convenient visual cue which semantics to expect without reading the docs.

Examples of common STL methods which appear in Stroika code (with STL semantics):

- clear()
- empty()
- push\_back
- erase
- c\_str()
- length()
- size()
- compare()
- begin()     // sort of – usually using Stroika iterators
- end()           // ditto

---

## Name Prefixes and Suffixes

### Prefixes

- the &#39;f&#39; prefix for data members
- &#39;k&#39; prefix for constants
- We use the &#39;e&#39; prefix for enumerators
- &#39;t&#39; prefix for thread\_local variables
- &#39;s&#39; prefix for static varaibles.
- &#39;\_&#39; prefix for PROTECTED instance variables or functions

### Suffix

- &#39;\_&#39; suffix for PRIVATE instance variables or functions

---

## Begin/End versus start/length

STL is reasonably consistent, with most APIs using T\* start, T\* end, but some APIs use length instead of end. The Stroika convention is to always use T\* start, T\* end.

### Rationale

One, this gives more consistent expectations. That&#39;s especially important for APIs that use offsets (like String) – so that it&#39;s obvious the meaning of integer parameters.

And it avoids problems with overflow. For example, if you had an API like:

~~~c++
basic_string substr(
   size_type _Off = 0,
   size_type _Count = npos
) const
~~~

To map this to an internal representation you have todo:

 char\* s = m\_bufPtr + \_Off;

 char\* e = m\_bufPtr + \_Off + \_Count;

but if count was numeric\_limits\&lt;size\_t\&gt;::max(), then the e pointer computation would overflow. There are ways around this, but mixing the two styles creates a number of problems - but for implementations – and for use.

---

## User-defined literals

- Stroika makes extensive use of the builtin `operator"" sv`, which produces (more efficient) String objects (really the STL version produces string_view but Stroika's String class converts string_view to a String more efficiently - reusing the space for the characters)

  Stroika also provides `operator"" _k` which does about the same thing (producing String_Constant) but we use internally and encourage use of operator"" sv (since its a standard and does about the same thing). `operator"" _k` is only provided as an option because there are a few cases of ambiguity where its helpful.

- `operator"" _RegEx () can be used as a shortcut for defining regular expressions

  ~~~c++
  RegularExpression re = L"foo.*"_RegEx;
  ~~~

---

## New static methods and Factories

In Stroika, a New () is static method, which allocates an instance of some class, but returns some kind of shared\_ptr/smart pointer to the type – not a bare C++ pointer.

Stroika doesn&#39;t make much use of the factory pattern, but occasionally – it is useful. If the type provided by the factory is exactly the type of a given class, then we generally use

struct T\_Factory {

    static T New();

};

That technique is used to control the default kind of containers (backend algorithm) that is used.

Or for Stream classes, the &#39;stream quasi namespace&#39; contains a New method to construct the actual stream, and the definition of the Ptr type – smart pointer – used to access the stream.

---

## Compare () and operator\&lt;, operator\&gt;, etc…

- Note this has materially changed in Stroika v2.1, due to the upcoming
  changes in C++20 to support the spaceship operator and automatic compare
  operation functions from it.

For types Stroika defines, it generally uses the convention of providing a ThreeWayComparer function object (generally fully constexpr).

e.g.
~~~C++
    struct TimeOfDay::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr int operator() (const TimeOfDay& lhs, const TimeOfDay& rhs) const;
    };
~~~

and provides non-member (#if __cpp_lib_three_way_comparison < 201711)
 bool operator\&lt;, operator\&lt;=, operator\&gt;, operator\&gt;=, operator==, operator!= which inline trivially maps to this.

Stroika code which COUNTS on comparison doesn&#39;t directly call Compare(), but instead uses &#39;a \&lt; b&#39; or Common::ThreeWayCompare, or Common:ThreeWayComparer.

- Working with builtin types (e.g. in)
- Working with STL types, and 3rd-party libraries
- Probably more likely to seamlessly fit with user code

Note that we choose to use non-member operator overloads for these comparison functions because putting them in the namespace where the class is defined provides the same convenience of use (name lookup) as member functions, but allows for cases like C \&lt; O where C is some time convertible to O, and O is the class we are adding operator\&lt; support for.

So for example:

 if (L&quot;aa&quot; \&lt; String (L&quot;ss&quot;)) {

 }

Works as expected, so long as either the left or right side is a String class, and the other side is convertible to a String.

---

## Using T= versus typedef

C++11 now supports a new typedef syntax – using T=…. This is nearly the same as typedef in terms of semantics.

Stroika code will generally use the using T = syntax in preference to typedef for two reasons:

- The using = syntax is slightly more powerful, in that it supports defining derivative template typdefs.
- And more importantly, I believe it makes code more readable, because the type of INTEREST is the one being defined = which appears first. What it maps to is often more complicated (why we define the typedef) – and one can often ignore that detail (or skim it).

---

## Procedure name suffixes for string return type

The Windows SDK uses the convention of appending a W to the end of a function name that uses wide characters, and an A to the name that uses the current operating system locale for code page.

In C++ (and Stroika) – this convention is also generally unneeded, because of the availability of overloading.

Stroika generally avoids this issue by returning String classes nearly everywhere – which are Unicode based. But as the Stroika String classes uses the rest of the Stroika infrastructure – including thread interruption, it&#39;s sometimes inconvenient for some low level coding to use those String classes.

But you cannot overload on return types.

For this reason, a handful of Stroika APIs follow the convention of a suffix of:

- \_U for wstring return
- \_N for string return, being interpreted as the &#39;narrow SDK code page&#39;
- \_A for string returns which are guaranteed to be ASCII

---
## kThe for some final singleton objects

Some objects which are only usable after the start of main (and until end of main), may be slightly more convenient and performant to use pre-existing ones. For example, EOFException::kThe, InterruptException::kThe, etc.

---
## enable_if<> usage

I’ve experiment with a number of different styles of enable_if usage, and finally standardized on an approach.

This may change – as I see more alternatives and gain more experience.

- First – just use enable\_if\_t.
- For values, use \_v type traits variants (brevity)

Tried and rejected approaches:

- typename ENABLE\_IF= typename enable\_if\_t\&lt;TEST\&gt;
  - The problem with this approach is that you cannot repeat the enable\_if in the implementation (.inl) file definition – just in the declaration. This is confusing.
- Enable\_if\_t\&lt;TEST,int\&gt; = 0
  - Value – not type; this works well, since the identical enable\_if\_t\&lt;\&gt; line can be included in the .inl file in the definition. But its needlessly confusing to define the &#39;int&#39; type on the enable\_if\_t.

USE:

enable\_if\_t\&lt;is\_function\_v\&lt;FUNCTION\&gt;\&gt;\* = nullptr

Reasons:

- No extra confusing types as args to enable\_if\_t.
- \* (to make it a type that can be initialized) is modest and any easy pattern to follow. Seems needed since else you cannot provide a default value
- Nullptr value maybe would be clearer with =0, which works, but everywhere else we initialize void\* ptr with nullptr.
- CAN be repeated (without default value) – in the .inl file (definition).