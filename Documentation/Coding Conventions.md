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

---

## Name Prefixes and Suffixes

### Prefixes

- the &#39;f&#39; prefix for data members (fields)
- &#39;k&#39; prefix for constants
- We use the &#39;e&#39; prefix for enumerators
- &#39;t&#39; prefix for thread_local variables
- &#39;s&#39; prefix for static varaibles.
- &#39;\_&#39; prefix for PROTECTED instance variables or functions

### Suffix

- &#39;\_&#39; suffix for PRIVATE instance variables or functions

### Case

- functions start with upper case (camelcase)
- local variables start with lower case (camelcase)
- properties start with lower case (camelcase) - because they are a hybrid between field and function, but syntactically you must treat them sometimes slightly differently than either, so this makes more clear what is going on. 'f' for plain field, and just starting with lower case to indicate a property.

---

## Begin/End versus start/length

STL is reasonably consistent, with most APIs using T\* start, T\* end, but some APIs use length instead of end. The Stroika convention is to always use T\* start, T\* end.

### Rationale

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

---

## User-defined literals

- Stroika makes extensive use of the builtin `operator"" sv`, which produces (more efficient) String objects (really the STL version produces string_view but Stroika's String class converts string_view to a String more efficiently - reusing the space for the characters)

  Stroika also provides `operator"" _k` which does about the same thing (producing String_Constant) but we use internally and encourage use of operator"" sv (since its a standard and does about the same thing). `operator"" _k` is only provided as an option because there are a few cases of ambiguity where its helpful.

- `operator"" \_RegEx () can be used as a shortcut for defining regular expressions

  ```c++
  RegularExpression re = L"foo.*"_RegEx;
  ```

---

## Uniform initialization {} vs. older style paren initialization ()

- Generally prefer Uniform initialization syntax {} because

  - makes construction appear distint from function calls
  - rule about no narrowing/implicit conversions (MAYBE? helpful - unclear)

- DONT use uniform initialzation syntax with call to perfect forwarding in templates. The natural behavior
  with perfect forwarding is that you get the same effect with the wrapper object as you would have had with initializing
  the underlying (wrapped) object. But initializing that object generally would have allowed implicit conversions (esp int to size_t).

- VERY UNCERTAIN about this policy - as of 2020-08-07 - just an experimental plan.

---

## quasi-namespace (quasi namespace)

This is the use of a struct as a namespace. Now that C++ has namespaces, you might ask - why would anybody do this? The answer is that struct allows defining part of the namespace is PRIVATE, and part public. This can be sometimes useful,
and that is why Stroika occasionally uses this pattern.

---

## New static methods and Factories

In Stroika, a New () is static method, which allocates an instance of some class, but returns some kind of shared_ptr/smart pointer to the type – not a bare C++ pointer.

Stroika doesn&#39;t make much use of the factory pattern, but occasionally – it is useful. If the type provided by the factory is exactly the type of a given class, then we generally use

struct T_Factory {

    static T New();

};

That technique is used to control the default kind of containers (backend algorithm) that is used.

Or for Stream classes, the &#39;stream quasi namespace&#39; contains a New method to construct the actual stream, and the definition of the Ptr type – smart pointer – used to access the stream.

---

## <a name="Comparisons"></a> Comparisons: spaceship operator, operator==, operator<=> and operator<, operator>, etc…

- Note this has materially changed in Stroika v2.1, due to the upcoming
  changes in C++20 to support the spaceship operator and automatic compare
  operation functions generated from it.

Stroika types generally support the c++20 operator== and operator<=> semantics and operators.

But sometimes you want to have a compare functor that takes parameters (e.g. string optionally case insensitive).

Stroika types follow the convention of providing a ThreeWayComparer function object and an EqualsComparer object for type T, as static members of that type T If-and-only-if the comparitors can be parameterized. These are **not** available (as nested members) if they cannot be parameterized.

But in either case (whether parameterizable or not) they are supported with operator <=> and operator== (assuming C++20 or later) or the equivilent in C++17 and earlier.

One subtlety that does occasionally need to be accomodated (especailly pre-C++20) is telling a less comparer from an equals comparer, etc in overloads. This is done with Common::ComparisonRelationType and Common::ComparisonRelationDeclaration.

### Definition of comparison

As much as possible, an attempt was made to make this the same across C++ versions, but C++20 is different enough, that this wasn't done perfectly.

These instructions cover support of both.

For a class T, where the compare functions are NOT paramterized:

#### ALWAYS define

```C++
#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * assumes strong_ordering, but use right ordering or auto if appropriate (mostly for templates)
         */
        constexpr strong_ordering operator<=> (const T& rhs) const;

    public:
        /**
         */
        constexpr bool operator== (const T& rhs) const;
#endif
```

(note - sometimes you can use instead)

```C++
#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        constexpr auto operator<=> (const T& rhs) const = default;
#endif
```

and then define

```C++
#if __cpp_impl_three_way_comparison < 201907
    constexpr bool operator< (const T& lhs, const T& rhs);
    constexpr bool operator<= (const T& lhs, const T& rhs);
    constexpr bool operator== (const T& lhs, const T& rhs);
    constexpr bool operator!= (const T& lhs, const T& rhs);
    constexpr bool operator>= (const T& lhs, const T& rhs);
    constexpr bool operator> (const T& lhs, const T& rhs);
#endif
```

    **NOTE** This is even simpler for code which is only required to support C++20 - just define the one or two compare funcitons and you are done.

#### If T compare function may be parameterized also

```C++
  struct T {
     ....
     struct EqualsComparer;
     struct ThreeWayComparer;
  };
  struct T::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr EqualsComparer (int extraArgs = 0);
        bool operator() (const T& lhs, const T& rhs) const;
  };
  struct T::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr ThreeWayComparer (int extraArgs = 0);
        strong_ordering operator() (const T& lhs, const T& rhs) const;
  };

```

### How to **call** the comparison functions

Calling comparison functions is simple.

#### Using default comparison

This works beautifully. Just writing code naturally as

```C++
    if (L"aa" < String (L"ss")) {
    }
```

All the various operators should just work with no effort, on both C++ 17, and with C++ 20 and later.

#### Using possibly explicit comparison functor

To support the C++20 compare_three_way function object (compatibly with C++17), it intoduces its own copy of this in the 'Common' namespace, along with:

```C++
  Common::strong_ordering   (alias for std::strong_ordering)
  Common::kLess             (alias for std::strong_ordering::less)
  Common::kEqual            (alias for std::strong_ordering::equal)
  Common::kGreater          (alias for std::strong_ordering::greater)
```

For code which assumes C++20 or later, just use the appropriate C++20 types/values/classes, with no need for the C++17-compatability helpers.

Then to make use of these explicit function compare objects (the most common case) where the comparison function is not parameterized:

```C++
auto compareFunc = equal_to<T>{};
if (compareFunc(t1, t2)) {
  ...
}
```

or

```C++
auto compareFunc = Common::compare_three_way<T,T>{};
if (compareFunc(t1, t2) == Common::kLess) {
  ...
}
```

For the more complicated case of passing in explicit parameters, you use the nested members inside T, for example:

```C++
  var cmp1 = String::EqualsComparer{};
  String s1 = "abc";
  String s2 = "ABC";
  if (s1 == s2) {
    // NOT REACHED
  }
  if (cmp1(s1, s2)) {
    // NOT REACHED
  }
  var cmp2 = String::EqualsComparer{String::CaseInsensivie};
  if (cmp2(s1, s2)) {
    // REACHED
  }

  var cmp3 = String::ThreeWayComparer{String::CaseInsensivie};
  if (cmp2(s1, s2) == Common::kEqual) { // can say == 0, if C++20 or later
    // REACHED
  }
```

### Comparisons Rationale

- Working with builtin types (e.g. int)
- Working with STL types, and 3rd-party libraries
- Seamlessly fit with user code
- Works with C++17, and the newer operator<=> () code interoperably
- when we abandon C++17 support, easy to deprecate 'Common:' wrappers on C++20 comparison code, and simplify.

Note that we choose to use non-member operator overloads for these comparison functions (in C++17) because putting them in the namespace where the class is defined provides the same convenience of use (name lookup) as member functions, but allows for cases like C \&lt; O where C is some time convertible to O, and O is the class we are adding operator\&lt; support for.

So for example:

```C++
    if (L"aa" < String (L"ss")) {
    }
```

Works as expected, so long as either the left or right side is a String class, and the other side is convertible to a String.

**Design Note** - We considered specializing equal_to<> and three_way_compare<> templates to allow for refining the behavior (adding parameters) to compare functions. But the problem with this is I was unable to get ADDING NEW parameters to the base compare-templates (e.g. for Collection<>EqualsComparer we add in to the EqualsCompare template the BASE_COMPARER object, which cannot be added to the std::equal_to<> template - or I couldn't figure out how). So that left the approach of a nested type for the comparer objects.

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

## Magic Statics (Initialization)

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

---

## class/method Final

- It does very little good (helps optimization sometimes but also can needlessly limit programmer options later)
- It CAN serve as a bit of DOCUMENTATION about what is intended to be extended or not (but unfortunately its not a good fit for this because you cannot say 'generally intended not to subclass' - just 'may not').
- So – Stroika use of final is haphazard – used when its clearly not going to be extended in CASE it might help performance, but rarely otherwise).

## Final Singleton objects (kThe, Get, Set)

Singleton objects are a common pattern. Stroika doesn't use these a ton, but some. One issue with singletons to be careful about is thread safety. Stroika leverages a couple of patterns to handle this.

### kThe for constant (immutable) singletons

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

### Get (and sometimes Set) for mutable singletons

Mutable singletons are accessed by the Get() static method. By definition of a mutable singleton, it will have some non-const methods.

Variants of this pattern are safe to use before main (because the Get() method can ensure the underlying singleton object is constructed before returning it).

#### Return mutable reference

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

#### Return shared_ptr<> or some other SharedValue() type object, and support singleton static Set method

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

## enable_if<> usage

I’ve experiment with a number of different styles of enable_if usage, and finally standardized on an approach.

This may change – as I see more alternatives and gain more experience.

- First – just use enable_if_t.
- For values, use \_v type traits variants (brevity)

Tried and rejected approaches:

- typename ENABLE_IF= typename enable_if_t\<TEST\>
  - The problem with this approach is that you cannot repeat the enable_if in the implementation (.inl) file definition – just in the declaration. This is confusing.
- Enable_if_t\&lt;TEST,int\&gt; = 0
  - Value – not type; this works well, since the identical enable_if_t\&lt;\&gt; line can be included in the .inl file in the definition. But its needlessly confusing to define the &#39;int&#39; type on the enable_if_t.

USE:

enable_if_t\<is_function_v\<FUNCTION\>\>\* = nullptr

Reasons:

- No extra confusing types as args to enable_if_t.
- \* (to make it a type that can be initialized) is modest and any easy pattern to follow. Seems needed since else you cannot provide a default value
- Nullptr value maybe would be clearer with =0, which works, but everywhere else we initialize void\* ptr with nullptr.
- CAN be repeated (without default value) – in the .inl file (definition).

---

## Quietly

Most Stroika functions raise an exception when they fail. For example, Wait () methods etc, Parse() methods, etc.
But sometimes you just want the thing to return an optional result (for speed and simplicity - cuz the failure case is common, not
exceptional). To simplify these situations, many APIs have a 'Quietly' variant, that does what the main function does, but instead of raising an exception when it fails, it returns a nullopt.

---

## Deprecating old APIs

### Use [[deprecated()]]

For many APIs (e.g. member functions) - we annotate the API with a deprecated attribute, and document the new API to be used instead.
We try to keep these around til the major version switch (e..g til we switch from 2.1b9 to 2.1rc, or 3.1a, to 3.1b, etc).

### Use inline namespaces

For large sections of code which may change (e.g. the ORM or SQL APIs), we may version the entire API section with
inline namespaces

```c++
namespace Stroika::Foundation::Database::SQL:: inline v1 {
  // just add the inline v1 and a new section :v2 (to test backward compatabilty works)
  // Maybe provide #define to control if v1 or v2 is the default? Or maybe just quick test run
  // that all builds with one inline and then swtich to the other and require users to switch to explicitly add the 'v1'/v2 (and default to v2).
}
```

---

## Cosmetic Hint Macros

### nonvirtual

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
