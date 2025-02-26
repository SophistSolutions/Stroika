/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_LazyInitialized_h_
#define _Stroika_Foundation_Execution_LazyInitialized_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <concepts>
#include <functional>
#include <mutex>

#include "Stroika/Foundation/Common/Common.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \brief value-object, where the value construction is delayed until first needed (can be handy to avoid c++ include/initializer deadly embrace)
     * 
     *  Also can be used to 'lazy initialize' facilities that might be costly to setup, but might never be used.
     * 
     *  Can be used to initialize a static constant object - declared at file scope - dependent on another file-scope data object,
     *  without incurring the pain of static initialization problems (before main). Often this is not needed, if you just
     *  make the dependent objects constexpr. But sometimes you cannot do that.
     * 
     *  LazyInitialized<T> acts mostly like a T (as much as I could figure out how to).
     * 
     *  This object (at least the magic init part) - is fully internally synchronized (though other operations of T itself are in general not).
     * 
     *  This object CAN be constructed before main, and accessed before main (after constructed) - but its up to caller to assure
     *  the 'oneTimeGetter' is safe to call when called.
     * 
     *  @aliases for ConstantProperty, 'virtual constant', VirtualConstant
     * 
     *  \par Example Usage
     *      \code
     *          // say not legal to call EVP_md5 til you've initialized openssl, and maybe you never will - but still want to declare but
     *          // not use these constants - or at least declare the constants in a file/module (file scope so constructed before main) and
     *          // initialize openssl from after main starts?
     *          const LazyInitialized<DigestAlgorithm> DigestAlgorithms::kMD5{[] () { return ::EVP_md5 (); }};
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          const LazyInitialized<Sequence<filesystem::path>> Execution::kPath{[] () -> Sequence<filesystem::path> {
     *              if (const char* env_p = std::getenv ("PATH")) {
     *                  String pathVar = String::FromNarrowSDKString (env_p);
     *                  return pathVar.Tokenize ({':'}).Map<Sequence<filesystem::path>> ([] (auto i) { return i.template As<filesystem::path> (); });
     *              }
     *              return {};
     *          }};
     *      \endcode
     *
     *  \par Example ALTERNATIVE (use lambda as with LazyInitialize but directly invoke) - but this invoked before main()
     *      \code
     *          const Sequence<filesystem::path> Execution::kPath{[] () -> Sequence<filesystem::path> {
     *              if (const char* env_p = std::getenv ("PATH")) {
     *                  String pathVar = String::FromNarrowSDKString (env_p);
     *                  return pathVar.Tokenize ({':'}).Map<Sequence<filesystem::path>> ([] (auto i) { return i.template As<filesystem::path> (); });
     *              }
     *              return {};
     *          } ()};
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          inline String                 kXGetter_ () { return "X"; }
     *          const LazyInitialized<String> kX {kXGetter_};
     *          ...
     *          const String a = kX;
     *      \endcode
     *
     *  \note   it would be HIGHLY DESIRABLE if C++ allowed operator'.' overloading, as accessing one of these
     *          values without assigning to a temporary first - means that you cannot directly call its methods.
     *          That's a bit awkward.
     *
     *          So if you have a type T, with method m(), and variable of type T t.
     *          Your starter code might be:
     *              T   t;
     *              t.m ();
     *          When you replace 'T t' with
     *              ConstantProperty<T> t;
     *              you must call t().m();
     *          OR
     *              you must call t->m();
     * 
     *  \note   C++ also only allows one level of automatic operator conversions, so things like comparing
     *          optional<T> {} == LazyInitialized<T,...> {} won't work. To workaround, simply
     *          apply () after the LazyInitialized<> instance.
     */
    template <typename T>
    class LazyInitialized {
    public:
        /**
         *  oneTimeGetter is a function (can be a lambda()) which computes the given value. It is called 
         *  just once, and LAZILY, the first time the given VirtualConstant value is required.
         * 
         *      LazyInitialized (ONE TIME GETTER) - is the normal way to use LazyInitialized
         *      LazyInitialized (T) - somewhat pointless, but you can do it....
         *      copy-constructible
         */
        LazyInitialized () = delete;
        template <invocable F>
        constexpr LazyInitialized (F&& oneTimeGetter)
            requires (convertible_to<invoke_result_t<F>, T>);
        constexpr LazyInitialized (const T& v);
        constexpr LazyInitialized (const LazyInitialized&) = delete;

    public:
        /**
         */
        LazyInitialized& operator= (const LazyInitialized&) = delete;

    public:
        /**
         */
        constexpr ~LazyInitialized ();

    public:
        /**
         *  A LazyInitialized can be automatically assigned to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions). But when it works (80% of the time) - its helpful.
         */
        nonvirtual constexpr operator const T () const;

    public:
        /**
         *  Just use the function syntax, and you get back the initialized value.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::LazyInitialized<InternetMediaType> kPNG...
         *
         *          bool checkIsImage1 = PredefinedInternetMediaType::kPNG().IsA (InternetMediaTypes::Wildcards::kImage);
         *      \endcode
         */
        nonvirtual const T operator() () const;

    public:
        /**
         *  Just use the operator-> syntax, and you get back the wrapper objects value (initializing if needed).
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::LazyInitialized<InternetMediaType> kPNG = ...
         *
         *          bool checkIsImage2 = PredefinedInternetMediaType::kPNG->IsA (InternetMediaTypes::Wildcards::kImage);
         *      \endcode
         */
        nonvirtual T*       operator->();
        nonvirtual const T* operator->() const;

    private:
        mutable once_flag fOnceFlag_; // cannot go in union cuz this 'discriminates' the union
        // small space savings - don't need both getter and value at same time
        #if qCompilerAndStdLib_lazyunion_Buggy
        mutable optional<T>                  fValue_;
        mutable function<T (void)> fOneTimeGetter_;
    #else
    union {
        mutable T                  fValue_;
   
        mutable function<T (void)> fOneTimeGetter_;
   };
    #endif

    private:
        T&       Getter_ ();
        const T& Getter_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LazyInitialized.inl"

#endif /*_Stroika_Foundation_Execution_LazyInitialized_h_*/
