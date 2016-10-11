/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_ 1


/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */


#if     defined(__cplusplus)



/**
 *
 */
#define kStrokia_Foundation_Configuration_cplusplus_11  201103


/**
 *
 */
#define kStrokia_Foundation_Configuration_cplusplus_14  201402




/*
 *******************************************************************
 *******************************************************************
 ******** CHECK / WARN about supported compilers/versions **********
 *******************************************************************
 *******************************************************************
 */

#if     defined (__clang__)

// Must check CLANG first, since CLANG also defines GCC
// see
//      clang++-3.8 -dM -E - < /dev/null
#if     (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 7))
#pragma message ("Warning: Stroika does not support versions prior to clang++ 3.7")
#endif
#if     (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ > 8))
#pragma message ("Info: Stroika untested with this version of clang++ - USING PREVIOUS COMPILER VERSION BUG DEFINES")
#define   CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X)   1
#endif

#elif   defined (__GNUC__)

#if     __GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ < 0))
#pragma message ("Warning: Stroika does not support versions prior to GCC 5.0")
#endif
//#if     (__GNUC__ == 5 && (__GNUC_MINOR__ > 9))
//#pragma message ("Info: Stroika untested with this version of GCC")
//#endif
#if     __GNUC__ > 6 || (__GNUC__ == 6 && (__GNUC_MINOR__ > 1))
#pragma message ("Info: Stroika untested with this version of GCC - USING PREVIOUS COMPILER VERSION BUG DEFINES")
#define   CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X)   1
#endif

#elif     defined (_MSC_VER)

#define _MS_VS_2k15_VER_                1900
#define _MS_VS_2k15_RTM_FULLVER_        190023026
#define _MS_VS_2k15_Update1_FULLVER_    190023506
#define _MS_VS_2k15_Update2_FULLVER_    190023918
#define _MS_VS_2k15_Update3_FULLVER_    190024210
#define _MS_VS_2k15_Update3_01_FULLVER_ 190024213
#define _MS_VS_2k15_Update3_02_FULLVER_ 190024215

#if      _MSC_VER < _MS_VS_2k15_VER_
#pragma message ("Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2015")
#elif   _MSC_VER ==_MS_VS_2k15_VER_ && (_MSC_FULL_VER < _MS_VS_2k15_Update2_FULLVER_)
#pragma message ("Warning: Stroika requires update 2 or later if using Microsoft Visual Studio.net 2015")
#elif   _MSC_VER > _MS_VS_2k15_VER_
#pragma message ("Info: This version of Stroika is untested with this version of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES")
#define   CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X)   1
#elif   _MSC_FULL_VER > _MS_VS_2k15_Update3_02_FULLVER_
#pragma message ("Info: This version of Stroika is untested with this Update of of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES")
#define   CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X)   1
#endif

#else

#pragma message ("Warning: Stroika does recognize the compiler being used. It may work, but you may need to update some of the other defines for what features are supported by your compiler.")

#endif


/*
 *
 *  Stroika supports a wide variety of compilers with a wide variety of bugs and degrees of C++ standards conformance.
 *
 *  If you see the compile message "USING PREVIOUS COMPILER VERSION BUG DEFINES" - Stroika will use the set of bug defines
 *  from the most recent version of the compiler available. That may nor may not work.
 *
 *  To FIX Stroika - force this true, and then add // in front of #ifndef       CompilerAndStdLib_AssumeBuggyIfNewerCheck_ and its endif,
 *  and go through and test and adjust each bug define for the new version of the compiler.
 *
 */
#ifndef     CompilerAndStdLib_AssumeBuggyIfNewerCheck_
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X)   (X)
#endif









/*
 *******************************************************************
 *******************************************************************
 **************** Define Features (and Bug Workarounds) ************
 *******************************************************************
 *******************************************************************
 */













/*
@CONFIGVAR:     qCompilerAndStdLib_union_designators_Buggy
@DESCRIPTION:
                    union U4 {
                        short int si;
                        int        i;
                        long int  li;
                    } u4 = {.li=3};
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(79): error C2059: syntax error: '.'
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(79): error C2612: trailing '}' illegal in base/member initializer list
*/
#ifndef qCompilerAndStdLib_union_designators_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_union_designators_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_union_designators_Buggy      0
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\dataexchange\badformatexception.h(40): warning C4359: 'Stroika::Foundation::DataExchange::BadFormatException::kThe': Alignment specifier is less than actual alignment (8), and will be ignored.
...
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(373): error C2719: 'defaultValue': formal parameter with requested alignment of 8 won't be aligned
1>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterator.h(566): note: see reference to class template instantiation 'Stroika::Foundation::Memory::Optional<Stroika::Foundation::IO::Network::Interface,Stroika::Foundation::Memory::Optional_Traits_Inplace_Storage<T>>' being compiled
....1>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\interface.cpp(137): note: see reference to class template instantiation 'Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::IO::Network::Interface>' being compiled
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(440): error C2719: 'unnamed-parameter': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(544): error C2719: 'rhs': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(552): error C2719: 'rhs': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterable.h(691): error C2719: 'unnamed-parameter': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterable.h(767): error C2719: 'unnamed-parameter': formal parameter with requested alignment of 8 won't be aligned*/
#ifndef qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy      0
#endif

#endif










/*
@CONFIGVAR:     qCompilerAndStdLib_largeconstexprarray_Buggy
@DESCRIPTION:   causes internal compiler error
//// Build Test 28: Foundation::DataExchange::Reader/Writers(7z/CharacterDelimitedLines/INI/JSON/XML/Zip)
*/
#ifndef qCompilerAndStdLib_largeconstexprarray_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
#define qCompilerAndStdLib_largeconstexprarray_Buggy      (_MS_VS_2k15_Update1_FULLVER_ <= _MSC_FULL_VER and _MSC_FULL_VER <= _MS_VS_2k15_Update2_FULLVER_)
#else
#define qCompilerAndStdLib_largeconstexprarray_Buggy      0
#endif

#endif






/*
>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(117): error C3256: 'kUpperBound': variable use does not produce a constant expression
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(117): note: while compiling class template member function 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(void)'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(582): note: see reference to function template instantiation 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(void)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(447): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_Range_<Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>>(void)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(180): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>(const Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_> &)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.cpp(275): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer<Stroika::Foundation::Time::DateRange>(void)' being compiled
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(118): error C3256: 'kLowerBound': variable use does not produce a constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy      0
#endif

#endif








/*
 *  NOTE - when this fails - it compiles but crashes in MSFT implementaiton
*/
#ifndef qCompilerAndStdLib_std_get_time_pctx_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_std_get_time_pctx_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_std_get_time_pctx_Buggy      0
#endif

#endif








/*
>c:\sandbox\stroikadev\tests\36\test.cpp(203): error C2127: 'kOrigValueInit_': illegal initialization of 'constexpr' entity with a non-constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_stdinitializer_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
// still broken in _MS_VS_2k15_Update2_FULLVER_
// still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy      0
#endif

#endif








/*
1>..\..\Sources\Stroika\Foundation\Characters\Concrete\String_stdwstring.cpp(15): error C2143: syntax error: missing ';' before '<end Parse>'
1>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\BlockAllocator.inl(398): note: while compiling class template static data member 'std::atomic<void *> Stroika::Foundation::Memory::Private_::BlockAllocationPool_<8>::sHeadLink_' (compiling source file ..\..\Sources\Stroika\Foundation\Characters\Concrete\String_stdwstring.cpp)
1>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\BlockAllocator.inl(281): note: while compiling class template member function 'void Stroika::Foundation::Memory::Private_::BlockAllocationPool_<8>::Deallocate(void *) noexcept' (compiling source file ..\..\Sources\Stroika\Foundation\Characters\Concrete\String_stdwstring.cpp)
*/
#ifndef qCompilerAndStdLib_constexpr_atomic_ptr_null_initializer_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_constexpr_atomic_ptr_null_initializer_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_atomic_ptr_null_initializer_Buggy      0
#endif

#endif







/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(163): error C3256: 'kUpperBound': variable use does not produce a constant expression
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(163): note: while compiling class template member function 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(Stroika::Foundation::Traversal::Openness,Stroika::Foundation::Traversal::Openness)'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(124): note: see reference to function template instantiation 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(Stroika::Foundation::Traversal::Openness,Stroika::Foundation::Traversal::Openness)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(122): note: while compiling class template member function 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(void)'
*/
#ifndef qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy      0
#endif

#endif



/*
@DESCRIPTION:   http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not
*/
#ifndef qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 8)))
#else
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      0
#endif

#endif




/*
@DESCRIPTION:   http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not


http://open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3797.pdf
     A static data member of literal type can be declared in the
    class definition with the constexpr specifier; if so, its declaration shall specify a brace-or-equal-initializer
    in which every initializer-clause that is an assignment-expression is a constant expression. [ Note: In both
    these cases, the member may appear in constant expressions. — end note ] The member shall still be defined
    in a namespace scope if it is odr-used (3.2) in the program and the namespace scope definition shall not
    contain an initializer.


    VERY unsure about the right way todo this - and if its a compiler bug or a me bug but I spent hours trying every combo I could find

*/
#ifndef qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 8)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_(__GNUC__ < 6 || (__GNUC__ == 6 && (__GNUC_MINOR__ <= 1)))
#elif   defined (_MSC_VER)
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy      0
#endif

#endif







/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
1>  SocketAddress.cpp
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
1>  Socket.cpp
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_union_variants_Buggy

#if     !defined (__clang__) && defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_union_variants_Buggy       CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__GNUC__ == 5 && (__GNUC_MINOR__ >= 4)) || (__GNUC__ == 6 && (__GNUC_MINOR__ <= 1)))
#elif   defined (_MSC_VER)
// still broken with _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_constexpr_union_variants_Buggy       CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_union_variants_Buggy       0
#endif

#endif



/*
  Range.cpp
  ****NOTE - WARNING NOT ERRR - BUT USELESS AND ANNOYING ****

2>c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory(169): warning C4996: 'std::uninitialized_copy_n::_Unchecked_iterators::_Deprecate': Call to 'std::uninitialized_copy_n' with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
2>  c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory(169): note: see declaration of 'std::uninitialized_copy_n::_Unchecked_iterators::_Deprecate'
2>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\externallysynchronizeddatastructures\array.inl(190): note: see reference to function template instantiation '_FwdIt std::uninitialized_copy_n<T*,std::size_t,T*>(_InIt,std::_Diff,_FwdIt)' being compiled
2>          with
2>          [
*/
#ifndef qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy   CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy   0
#endif

#endif




/*
 * Crazy man!  - https://connect.microsoft.com/VisualStudio/feedback/details/763051/a-value-of-predefined-macro-cplusplus-is-still-199711l
 *
 *       Stroika requires at least C++ ISO/IEC 14882:2011 supported by the compiler (informally known as C++ 11)
 */
#ifndef qCompilerAndStdLib_cplusplus_macro_value_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy      0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

    The issue is really
            warning: use of this statement in a constexpr function is a C++14
    for require lines at the start of constexp functions

*** FOR GCC 5.3 we get:
troika::Foundation::Configuration::EnumNames<Stroika::Foundation::Execution::Thread::Status>::BasicArrayInitializer{std::__cxx1998::__array_traits<std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>, 5ul>::_Type{std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)0u, ((const wchar_t*)"N\000\000\000u\000\000\000l\000\000\000l\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)1u, ((const wchar_t*)"N\000\000\000o\000\000\000t\000\000\000-\000\000\000Y\000\000\000e\000\000\000t\000\000\000-\000\000\000R\000\000\000u\000\000\000n\000\000\000n\000\000\000i\000\000\000n\000\000\000g\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)2u, ((const wchar_t*)"R\000\000\000u\000\000\000n\000\000\000n\000\000\000i\000\000\000n\000\000\000g\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)3u, ((const wchar_t*)"A\000\000\000b\000\000\000o\000\000\000r\000\000\000t\000\000\000i\000\000\000n\000\000\000g\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)4u, ((const wchar_t*)"C\000\000\000o\000\000\000m\000\000\000p\000\000\000l\000\000\000e\000\000\000t\000\000\000e\000\000\000d\000\000\000\000\000\000")}}})’
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:108:49: error: ‘constexpr void Stroika::Foundation::Configuration::EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_() const [with ENUM_TYPE = Stroika::Foundation::Execution::Thread::Status]’ called in a constant expression
RequireItemsOrderedByEnumValue_ ();
^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:207:41: note: ‘constexpr void Stroika::Foundation::Configuration::EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_() const [with ENUM_TYPE = Stroika::Foundation::Execution::Thread::Status]’ is not usable as a constexpr function because:
inline  constexpr   void    EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_ () const
^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:207:41: sorry, unimplemented: unexpected AST of kind loop_expr
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:207: confused by earlier errors, bailing out

*/
#ifndef qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

#if     !qCompilerAndStdLib_cplusplus_macro_value_Buggy && __cplusplus < kStrokia_Foundation_Configuration_cplusplus_14
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    1
#elif    !defined (__clang__) && defined (__GNUC__)
// this is still broken even if you say -std=+14 in gcc49
// this is still broken even if you say -std=+14 in gcc51
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    (__GNUC__ <= 5)
#elif   defined (_MSC_VER)
// still broken in _MS_VS_2k15_Update2_FULLVER_
// still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    0
#endif


#endif




/*
/home/lewis/Sandbox/Stroika-Reg-Tests-Dev/Library/Sources/Stroika/Foundation/Traversal/../Characters/../Containers/../Memory/BlockAllocator.inl:102:37: error: constexpr variable 'kLockedSentinal_' must be initialized by a constant expression
                constexpr   void*   kLockedSentinal_    =   (void*)1;   // any invalid pointer
*/
#ifndef qCompilerAndStdLib_constexpr_constant_pointer_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_constexpr_constant_pointer_Buggy     CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 8)))
#else
#define qCompilerAndStdLib_constexpr_constant_pointer_Buggy     0
#endif


#endif



/**
 */
#ifndef qCompilerAndStdLib_deprecated_attribute_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_deprecated_attribute_Buggy    0
#else
#define qCompilerAndStdLib_deprecated_attribute_Buggy    __cplusplus < kStrokia_Foundation_Configuration_cplusplus_14
#endif

#endif






/*
*/
#ifndef qCompilerAndStdLib_shared_mutex_module_Buggy

#if   defined (_MSC_VER)
#define qCompilerAndStdLib_shared_mutex_module_Buggy    0
#else
#define qCompilerAndStdLib_shared_mutex_module_Buggy    (__cplusplus < kStrokia_Foundation_Configuration_cplusplus_14)
#endif

#endif













/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
*/
#ifndef qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 8)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      (__GNUC__ == 5 && (__GNUC_MINOR__ <= 3))
#else
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_SFINAE_SharedPtr_Buggy

In file included from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.h:180:0,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Generator.inl:8,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Generator.h:91,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.inl:17,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.h:1004,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/DirectoryIterable.cpp:6:
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl: In instantiation of ‘Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&)::MyIterable_::Rep::Rep(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]’:
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/../Memory/../Memory/SharedPtr.inl:519:73:   required from ‘Stroika::Foundation::Memory::SharedPtr<T> Stroika::Foundation::Memory::MakeSharedPtr(ARG_TYPES&& ...) [with T = Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]::MyIterable_::Rep; ARG_TYPES = {const Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Characters::String, std::iterator<std::forward_iterator_tag, Stroika::Foundation::Characters::String, long int, Stroika::Foundation::Characters::String*, Stroika::Foundation::Characters::String&> >&}]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.h:143:89:   required from ‘static Stroika::Foundation::Memory::SharedPtr<T> Stroika::Foundation::Traversal::IterableBase::MakeSharedPtr(ARGS_TYPE&& ...) [with SHARED_T = Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]::MyIterable_::Rep; ARGS_TYPE = {const Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Characters::String, std::iterator<std::forward_iterator_tag, Stroika::Foundation::Characters::String, long int, Stroika::Foundation::Characters::String*, Stroika::Foundation::Characters::String&> >&}]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:150:136:   required from ‘Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&)::MyIterable_::MyIterable_(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:153:17:   required from ‘Stroika::Foundation::Traversal::Iterable<T> Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/DirectoryIterable.cpp:29:84:   required from here
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:129:25: internal compiler error: Segmentation fault
                         Rep (const Iterator<T>& originalIterator)
                         ^
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-4.8/README.Bugs> for instructions.

*/
#ifndef qCompilerAndStdLib_SFINAE_SharedPtr_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_SFINAE_SharedPtr_Buggy       CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ == 3) && (7 <= __clang_minor__ || __clang_minor__ <= 8))
#else
#define qCompilerAndStdLib_SFINAE_SharedPtr_Buggy       0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy

#if     qDebug && !qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                : fLock_ (ATOMIC_FLAG_INIT)
#endif

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\spinlock.inl(29): error C2280: 'std::atomic_flag::atomic_flag(const std::atomic_flag &)' : attempting to reference a deleted function
1>          c:\program files (x86)\microsoft visual studio 12.0\vc\include\atomic(191) : see declaration of 'std::atomic_flag::atomic_flag'

*/
#ifndef qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#elif   __clang__
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ == 3) && (7 <= __clang_minor__ || __clang_minor__ <= 8))
#else
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy

Compiling  $StroikaRoot/Tests/43/../TestHarness/TestHarness.cpp...
/home/lewis/Sandbox/StroikaDev/Tests/43/Test.cpp:66:41: warning: moving a temporary object prevents copy elision [-Wpessimizing-move]
Optional<NotCopyable>   n2 (std::move (NotCopyable ()));    // use r-value reference to move
*/
#ifndef qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy     CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ == 3) && (7 <= __clang_minor__ and __clang_minor__ <= 8))
#else
#define qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy     0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy


In file included from ../../../Tests/29/Test.cpp:9:0:
../../..//Library/Sources/Stroika/Foundation/Containers/Bijection.h:493:49: error: 'bool Stroika::Foundation::Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::Lookup(Stroika::Foundation::Configuration::ArgByValueType<T>, Stroika::Foundation::Memory::Optional<RANGE_TYPE>*) const [with DOMAIN_TYPE = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; RANGE_TYPE = Stroika::Foundation::Characters::String; TRAITS = Stroika::Foundation::Containers::Bijection_DefaultTraits<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred, Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred>, Stroika::Foundation::Common::ComparerWithEqualsOptionally<Stroika::Foundation::Characters::String> >; Stroika::Foundation::Configuration::ArgByValueType<T> = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred]', declared using local type 'Stroika::Foundation::Configuration::ArgByValueType<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred> {aka {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred}', is used but never defined [-fpermissive]
                 virtual  bool                   Lookup (ArgByValueType<DOMAIN_TYPE> key, Memory::Optional<RangeType>* item) const           =   0;
                                                 ^
../../..//Library/Sources/Stroika/Foundation/Containers/Bijection.h:495:49: error: 'void Stroika::Foundation::Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::Add(Stroika::Foundation::Configuration::ArgByValueType<T>, Stroika::Foundation::Configuration::ArgByValueType<RANGE_TYPE>) [with DOMAIN_TYPE = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; RANGE_TYPE = Stroika::Foundation::Characters::String; TRAITS = Stroika::Foundation::Containers::Bijection_DefaultTraits<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred, Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred>, Stroika::Foundation::Common::ComparerWithEqualsOptionally<Stroika::Foundation::Characters::String> >; Stroika::Foundation::Configuration::ArgByValueType<T> = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; Stroika::Foundation::Configuration::ArgByValueType<RANGE_TYPE> = const Stroika::Foundation::Characters::String&]', declared using local type 'Stroika::Foundation::Configuration::ArgByValueType<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred> {aka {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred}', is used but never defined [-fpermissive]
                 virtual  void                   Add (ArgByValueType<DOMAIN_TYPE> key, ArgByValueType<RANGE_TYPE> newElt)                    =   0;

../../..//Library/Sources/Stroika/Foundation/Containers/Bijection.h:493:49: error: 'bool Stroika::Foundation::Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::Lookup(Stroika::Foundation::Configuration::ArgByValueType<T>, Stroika::Foundation::Memory::Optional<RANGE_TYPE>*) const [with DOMAIN_TYPE = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; RANGE_TYPE = Stroika::Foundation::Characters::String; TRAITS = Stroika::Foundation::Containers::Bijection_DefaultTraits<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred, Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred>, Stroika::Foundation::Common::ComparerWithEqualsOptionally<Stroika::Foundation::Characters::String> >; Stroika::Foundation::Configuration::ArgByValueType<T> =


*/
#ifndef qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy

#if     !defined (__clang__) && defined (__GNUC__)
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy     (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2))
#else
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy     0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
@DESCRIPTION:   <p>Defined true if the compiler generates errors for static assertions in functions
            which should never be expanded. Such functions/static_assertions CAN be handy to make it
            more obvious of type mismatches with As<> etc templates.


    *** NOTE - this is so widely broken in clang/gcc - this maybe my misunderstanding. Review...
</p>
*/
#ifndef qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 8)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       CompilerAndStdLib_AssumeBuggyIfNewerCheck_(__GNUC__ < 6 || (__GNUC__ == 6 && (__GNUC_MINOR__ <= 1)))
#else
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       0
#endif

#endif








/*
Compiling regtests for Median/OrderBy...
>; _Compare = std::function<bool(const int&, const int&)>]’
../../..//Library/Sources/Stroika/Foundation/Characters/../Containers/../Traversal/Iterable.inl:538:22:   required from ‘RESULT_TYPE Stroika::Foundation::Traversal::Iterable<T>::Median(const std::function<bool(T, T)>&) const [with RESULT_TYPE = int; T = int]’
../../../Tests/46/Test.cpp:781:13:   required from here
/usr/include/c++/5/bits/predefined_ops.h:123:31: error: invalid type argument of unary ‘*’ (have ‘int’)
*/
#ifndef qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Bugg

#if     defined (__clang__)
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy       CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ == 3) && (7 <= __clang_minor__ and __clang_minor__ <= 8))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy       ((__GNUC__ == 5 && (__GNUC_MINOR__ == 1)) and __cplusplus == kStrokia_Foundation_Configuration_cplusplus_14)
#else
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy       0
#endif

#endif

















/*
@CONFIGVAR:     qCompilerAndStdLib_inet_ntop_const_Buggy
    MSFT API bug - second arg to inet_ntop SB const 0 http://pubs.opengroup.org/onlinepubs/009695399/functions/inet_ntop.html
*/
#ifndef qCompilerAndStdLib_inet_ntop_const_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k13_Update4_FULLVER_
// Still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_inet_ntop_const_Buggy          CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_inet_ntop_const_Buggy          0
#endif

#endif









/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): error C2244: 'Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone': unable to match function definition to an existing declaration
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(29): note: see declaration of 'Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: definition
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: 'Iterator<T,std::iterator<std::forward_iterator_tag,T,ptrdiff_t,_Ty*,_Ty&>>::SharedIRepPtr Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone(void) const'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: existing declarations
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: 'std::shared_ptr<SHARED_T> Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone(void) const'
*/
#ifndef qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy

#if   defined (_MSC_VER)
// Still broken in _MS_VS_2k13_Update4_FULLVER_
// Still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy          CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy          0
#endif

#endif







/**
 *  This works, and as near as I can tell, its just a compiler 'misfeature' with that make_unique_lock is slightly slower.
 *
 *  Tell that this is slow by running/comparing performance regtests with and without.
 *      Test:
 *          time Builds/Release-U-32/Test47/Test47 -x 1
 *
 *          Run test several time and average realtime results.
 *
 *          Run same test on UNIX and Windows.
 *
 *      \note   Tiny improvement found 2016-06-09 - v2.0a148x - with setting '1' on gcc 5.3-release, and even more tiny improvement
 *              for vs2k15 (Release-U-32). But for now - take the improvement
 */
#ifndef qCompilerAndStdLib_make_unique_lock_IsSlow

#define qCompilerAndStdLib_make_unique_lock_IsSlow      1

#endif /*qCompilerAndStdLib_make_unique_lock_IsSlow*/













/*
 *
 * clang uses the g++ stdlib, and since the bug is in both places, I suspect the issue is with the library, and
 * not the compiler, but its hard to unravel, and I need to get back to other things, so just disable. Now it only
 * affects Bijection<> and when you use it with types that have no operator< and operator== (type needs both to workaround
 * this bug) DECLARED - but  not DEFINED (somtimes need to define, but can be assert(false) definition).
 *
 EXAMPLE OUTPUT:
 In file included from /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Tests/11/Test.cpp:7:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/iostream:39:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/ostream:38:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/ios:40:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/char_traits.h:39:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/stl_algobase.h:64:
/usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/stl_pair.h:215:24: error: invalid operands to binary expression ('const Stroika::SimpleClassWithoutComparisonOperators' and
      'const Stroika::SimpleClassWithoutComparisonOperators')
    { return __x.first == __y.first && __x.second == __y.second; }
             ~~~~~~~~~ ^  ~~~~~~~~~
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:64: note: in instantiation of function template specialization
      'std::operator==<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
                                                               ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:82:55: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            static auto check(const X& x) -> decltype(XTEST);\
                                                      ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:13: note: while substituting deduced template arguments into function template 'check'
      [with X = std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>]
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:84:35: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            using type = decltype(check(declval<T>()));\
                                  ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:13: note: in instantiation of template class
      'Stroika::Foundation::Configuration::Private_::eq_result_impl<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:88:5: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
    using   NAME##_result = typename Private_::NAME##_result_impl<T>::type;\
    ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:13: note: in instantiation of template type alias 'eq_result' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:90:63: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
    struct  has_##NAME : integral_constant <bool, not is_same<NAME##_result<T>, Stroika::Foundation::Configuration::substitution_failure>::value> {};
                                                              ^
<scratch space>:8:1: note: expanded from here
eq_result
^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/Compare.h:196:45: note: in instantiation of template class
      'Stroika::Foundation::Configuration::has_eq<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
                             Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value,
                                            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Traversal/Iterable.h:414:65: note: in instantiation of default argument for
      'DefaultEqualsComparer<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' required here
                template    <typename EQUALS_COMPARER = Common::DefaultEqualsComparer<T>>
                                                                ^~~~~~~~~~~~~~~~~~~~~~~~~
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/Bijection.h:120:40: note: in instantiation of template class
      'Stroika::Foundation::Traversal::Iterable<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
            class   Bijection : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>> {
                                       ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Tests/11/Test.cpp:32:44: note: in instantiation of template class 'Stroika::Foundation::Containers::Bijection<Stroika::SimpleClassWithoutComparisonOperators,
      Stroika::SimpleClassWithoutComparisonOperators, Stroika::Foundation::Containers::Bijection_DefaultTraits<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators,
      MySimpleClassWithoutComparisonOperators_ComparerWithEquals_, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_> >' requested here
        auto testFunc = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & s) {


 * OR WITH CLANG error messages look like:
/usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/stl_pair.h:215:24: error: invalid operands to binary expression ('const Stroika::SimpleClassWithoutComparisonOperators' and
      'const Stroika::SimpleClassWithoutComparisonOperators')
    { return __x.first == __y.first && __x.second == __y.second; }
             ~~~~~~~~~ ^  ~~~~~~~~~
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:103:64: note: in instantiation of function template specialization
      'std::operator==<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
                                                               ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:82:55: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            static auto check(const X& x) -> decltype(XTEST);\
                                                      ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:103:13: note: while substituting deduced template arguments into function template 'check'
      [with X = std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>]
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:84:35: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            using type = decltype(check(declval<T>()));\
                                  ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:103:13: note: in instantiation of template class
      'Stroika::Foundation::Configuration::Private_::eq_result_impl<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
 *
 *
 */
#ifndef qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy     CompilerAndStdLib_AssumeBuggyIfNewerCheck_(__clang_major__ == 3 && (__clang_minor__ <= 8))
#else
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy     0
#endif

#endif






/**
 *  qCompilerAndStdLib_Supports_errno_t controls whether or not the compiler provides an implementation
 *  of errno_t (gcc for example just assumes its int).
 *
 *  NB: POSIX and C99 just say to assume its an int and doesn't define errno_t.
 */
#if     !defined (qCompilerAndStdLib_Supports_errno_t)
#if     defined (__clang__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_errno_t 1
#else
#define qCompilerAndStdLib_Supports_errno_t 1
#endif
#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug
In file included from ../../..//Library/Sources/Stroika/Foundation/Characters/String.h:12:0,
                 from ../../../Tests/18/Test.cpp:13:
../../..//Library/Sources/Stroika/Foundation/Characters/../Containers/Sequence.h:548:45: error: 'void Stroika::Foundation::Containers::Sequence<T>::_IRep::Insert(size_t, const T*, const T*) [with T = {anonymous}::SimpleSequenceTest_14_Sequence_stdinitializer_complexType_()::StructureFieldInfo_; size_t = long unsigned int]', declared using local type 'const {anonymous}::SimpleSequenceTest_14_Sequence_stdinitializer_complexType_()::StructureFieldInfo_', is used but never defined [-fpermissive]
                 virtual void                Insert (size_t at, const T* from, const T* to)                          =   0;
                                             ^
*/
#if     !defined (qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug)

#if     !defined (__clang__) && defined (__GNUC__)
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug      ((__GNUC__ < 5) || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug      0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
*
*   Looking at returned string object from locale - its got a bogus length. And hten the DTOR for that string causes crash. Just dont
*   use this til debugged.
*/
#ifndef qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy

#if     defined (__clang__)
#define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy   CompilerAndStdLib_AssumeBuggyIfNewerCheck_((__clang_major__ < 3) || ((__clang_major__ == 3) && (7 <= __clang_minor__ and __clang_minor__ <= 8)))
#else
#define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy   0
#endif

#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_Support__PRETTY_FUNCTION__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__PRETTY_FUNCTION__

#if     defined (__clang__) || defined (__GNUC__)
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__   1
#else
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__   0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Support__func__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__func__

#if     defined (__clang__) || defined (__GNUC__)
#define qCompilerAndStdLib_Support__func__   1
#else
#define qCompilerAndStdLib_Support__func__   0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Support__FUNCTION__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__FUNCTION__

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Support__FUNCTION__   1
#else
#define qCompilerAndStdLib_Support__FUNCTION__   0
#endif

#endif









/*
@CONFIGVAR:     qDecoratedNameLengthExceeded_Buggy
@DESCRIPTION:
        C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\xrefwrap(173): warning C4503: 'Stroik...
        ...: decorated name length exceeded, name was truncated []
*/
#ifndef qDecoratedNameLengthExceeded_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still borken in _MS_VS_2k15_Update1_FULLVER_
// Still broken in _MS_VS_2k15_Update2_FULLVER_
// Still broken in _MS_VS_2k15_Update3_FULLVER_
// Still broken in _MS_VS_2k15_Update3_01_FULLVER_
// untested - _MS_VS_2k15_Update3_02_FULLVER_
#define qDecoratedNameLengthExceeded_Buggy      CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k15_Update3_02_FULLVER_)
#else
#define qDecoratedNameLengthExceeded_Buggy      0
#endif

#endif










/*
 *******************************************************************
 *******************************************************************
 ********************* REACT TO SOME DEFINES ***********************
 *******************************************************************
 *******************************************************************
 */



// When MSFT fixes qCompilerAndStdLib_cplusplus_macro_value_Buggy move back to the top of the file
#if     !qCompilerAndStdLib_cplusplus_macro_value_Buggy && __cplusplus < kStrokia_Foundation_Configuration_cplusplus_11

#pragma message ("Stroika requires at least C++ ISO/IEC 14882:2011 supported by the compiler (informally known as C++ 11)")

#endif






#if     qSilenceAnnoyingCompilerWarnings && defined(__GNUC__) && !defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE)\
    _Pragma ( "GCC diagnostic push") \
    _Pragma ( WARNING_TO_DISABLE )
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE)\
    _Pragma ( "GCC diagnostic pop" )
#else
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE)
#endif





#if     qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER)
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE)\
    __pragma ( warning (push) ) \
    __pragma ( warning (disable : WARNING_TO_DISABLE) )
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE)\
    __pragma ( warning (pop) )
#else
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE)
#endif



#if     qSilenceAnnoyingCompilerWarnings && defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE)\
    _Pragma ( "clang diagnostic push") \
    _Pragma ( WARNING_TO_DISABLE )
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE)\
    _Pragma ( "clang diagnostic pop" )
#else
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE)
#endif






// MSFT has a hack that prevents workarounds to other features they don't support, but luckily, they may that hack
// easy enough to disable ;-)
#if     defined (_MSC_VER) && (_MSC_VER == _MS_VS_2k15_VER_)
#define _ALLOW_KEYWORD_MACROS
#endif







// doesnt seem any portable way todo this, and not defined in C++ language
// Note - this doesn't appear in http://en.cppreference.com/w/cpp/language/attributes - as of 2016-06-22
#if     defined (__clang__) || defined (__GNUC__)
#define dont_inline    __attribute__((noinline))
#else
#define dont_inline    __declspec(noinline)
#endif









/**
 *   The Standard C++ mechanism of commenting out unused parameters isn't good enuf
 *   in the case where the parameters might be used conditionally. This hack is
 *   to shutup compiler warnings in those cases.
 *
 *      @todo eventually (when compilers support it) switch to using [[maybe_unused]] - http://en.cppreference.com/w/cpp/language/attributes
 */
#if     !defined (Arg_Unused)
#define Arg_Unused(x)   ((void) &x)
#endif






/*
 *   Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
 * trace macros).
 */
#if     !defined (_NoOp_)
#if defined(_MSC_VER)
#define _NoOp_  __noop
#else
#define _NoOp_(...)
#endif
#endif







/*
 *  This doesnt always work, but works for c++14 or later, and VS
 *  EXAMPLE:
 *
 *      struct _Deprecated_("FRED NOW DEPRECATED - USE BARNY") Fred { int a; };
 *
 *      _Deprecated_ ("USE CheckAccess")
 *      void     CheckFileAccess(int);
 */
#if     !defined (_Deprecated_)
#if     qCompilerAndStdLib_deprecated_attribute_Buggy
#define _Deprecated_(MESSAGE)
#else
#define _Deprecated_(MESSAGE) [[deprecated(MESSAGE)]]
#endif
#endif




/*
 *  Wrap this macro around entire declaration, as in:
 *       _DeprecatedFile_ ("DEPRECATED in v2.0a32 - use IO::FileSystem::DirectoryIterator");
 */
#define STRINGIFY(a) #a
#if     !defined (_DeprecatedFile_)
#define _DeprecatedFile_(MESSAGE) \
    _Pragma (STRINGIFY (message ##  DEPRECATED  ## MESSAGE ))
#endif







#if     qCompilerAndStdLib_shared_mutex_module_Buggy
namespace std {
    template    <typename MUTEX>
    class   shared_lock {
    public:
        using mutex_type = MUTEX;
    private:
        mutex_type& fMutex_;
    public:
        explicit shared_lock (mutex_type& m)
            : fMutex_ (m)
        {
            fMutex_.lock_shared ();
        }
        ~shared_lock ()
        {
            fMutex_.unlock_shared ();
        }
    };
}
#endif






#if     qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER) && qDecoratedNameLengthExceeded_Buggy
__pragma ( warning (disable : 4503) )
#endif



#endif  /*defined(__cplusplus)*/



#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/
