/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ******************************** Common::Inc ***********************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr ENUM Inc (ENUM e)
    {
        return ToEnum<ENUM> (ToInt (e) + 1);
    }

    /*
     ********************************************************************************
     ****************************** Common::ToInt ***********************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr underlying_type_t<ENUM> ToInt (ENUM e)
    {
        // https://github.com/SophistSolutions/Stroika/issues/685 (STK-549)
        //static_assert (ENUM::eSTART <= e and e <= ENUM::eEND);
        return static_cast<typename underlying_type<ENUM>::type> (e);
    }

    /*
     ********************************************************************************
     ************************* Common::GetDistanceSpanned ***************************
     ********************************************************************************
     */
    template <typename ENUM>
    constexpr make_unsigned_t<underlying_type_t<ENUM>> GetDistanceSpanned ()
    {
        return static_cast<make_unsigned_t<underlying_type_t<ENUM>>> (ENUM::eCOUNT);
    }

    /*
     ********************************************************************************
     ******************************* Common::ToEnum *********************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr ENUM ToEnum (underlying_type_t<ENUM> e)
    {
        // https://github.com/SophistSolutions/Stroika/issues/685 (STK-549)
        //static_assert (ENUM::eSTART <= static_cast<ENUM> (e) and static_cast<ENUM> (e) <= ENUM::eEND);
        return static_cast<ENUM> (e);
    }

    /*
     ********************************************************************************
     *************************** Common::OffsetFromStart ****************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr make_unsigned_t<underlying_type_t<ENUM>> OffsetFromStart (ENUM e)
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        // https://github.com/SophistSolutions/Stroika/issues/685 (STK-549)
        //static_assert (ENUM::eSTART <= e and e <= ENUM::eEND);
        return static_cast<make_unsigned_t<underlying_type_t<ENUM>>> (ToInt (e) - ToInt (ENUM::eSTART));
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    }

    /*
     ********************************************************************************
     ****************************** Common::EnumNameText ****************************
     ********************************************************************************
     */
    namespace Private_ {
        // Not constexpr: reached in a constant expression (a table's names, a reflected name), the call fails the build - its name
        // says why. At run time, a Require failure.
        inline void EnumNameText_ACharNameMustBeASCII_WriteItU8_ ()
        {
            RequireNotReached ();
        }
        inline void EnumNameText_TooLongToHoldInPlace_WriteItU8SoItIsPointedAt_ ()
        {
            RequireNotReached ();
        }
        inline void EnumNameText_WideNameNotValidUTF16OrUTF32_ ()
        {
            RequireNotReached ();
        }
    }
    template <size_t N>
    constexpr EnumNameText::EnumNameText (const char (&s)[N])
        : EnumNameText{string_view{s, N - 1}}
    {
    }
    template <size_t N>
    constexpr EnumNameText::EnumNameText (const char8_t (&s)[N])
    {
        u8string_view v{s, N - 1};
        if (is_constant_evaluated () and v.size () > kMaxInlineBytes) {
            *this = FromStatic (v); // a literal lasts as long as the program (anything else is no constant expression)
        }
        else {
            SetCopyOf_ (v);
        }
    }
    template <size_t N>
    constexpr EnumNameText::EnumNameText (const wchar_t (&s)[N])
        : EnumNameText{wstring_view{s, N - 1}}
    {
    }
    constexpr EnumNameText::EnumNameText (string_view s)
    {
        for (char c : s) {
            if (static_cast<unsigned char> (c) > 0x7f) {
                Private_::EnumNameText_ACharNameMustBeASCII_WriteItU8_ ();
            }
        }
        *this = FromUTF8 (s);
    }
    constexpr EnumNameText::EnumNameText (u8string_view s)
    {
        SetCopyOf_ (s);
    }
    constexpr EnumNameText::EnumNameText (wstring_view s)
    {
        size_t  utf8Length = 0;
        char8_t buf[4]{};
        for (size_t i = 0; i < s.size ();) {
            optional<char32_t> c = NextCodePoint_ (s, i);
            if (not c) {
                Private_::EnumNameText_WideNameNotValidUTF16OrUTF32_ ();
                return;
            }
            utf8Length += EncodeUTF8_ (*c, buf);
        }
        if (utf8Length <= kMaxInlineBytes) {
            for (size_t i = 0; i < s.size ();) {
                fLength_ += EncodeUTF8_ (*NextCodePoint_ (s, i), fInline_.data () + fLength_);
            }
            return;
        }
#if qStroika_Foundation_Common_EnumNameText_DefineStaticString_
        if consteval {
            u8string t;
            for (size_t i = 0; i < s.size ();) {
                t.append (buf, EncodeUTF8_ (*NextCodePoint_ (s, i), buf));
            }
            fPointsTo_ = std::define_static_string (t);
            fLength_   = t.size ();
            return;
        }
#endif
        Private_::EnumNameText_TooLongToHoldInPlace_WriteItU8SoItIsPointedAt_ ();
    }
    constexpr EnumNameText::EnumNameText (const wchar_t* s)
        : EnumNameText{wstring_view{s}}
    {
    }
    constexpr EnumNameText EnumNameText::FromUTF8 (string_view utf8)
    {
        // a byte at a time: a constant expression cannot view chars as char8_ts
        EnumNameText r;
        if (utf8.size () <= kMaxInlineBytes) {
            for (size_t i = 0; i < utf8.size (); ++i) {
                r.fInline_[i] = static_cast<char8_t> (utf8[i]);
            }
            r.fLength_ = utf8.size ();
            return r;
        }
#if qStroika_Foundation_Common_EnumNameText_DefineStaticString_
        if consteval {
            u8string t;
            for (char c : utf8) {
                t += static_cast<char8_t> (c);
            }
            r.fPointsTo_ = std::define_static_string (t);
            r.fLength_   = t.size ();
            return r;
        }
#endif
        Private_::EnumNameText_TooLongToHoldInPlace_WriteItU8SoItIsPointedAt_ ();
        return r;
    }
    constexpr EnumNameText EnumNameText::FromStatic (u8string_view s)
    {
        EnumNameText r;
        if (s.size () <= kMaxInlineBytes) {
            r.SetCopyOf_ (s); // short names are always held in place
        }
        else {
            r.fPointsTo_ = s.data ();
            r.fLength_   = s.size ();
        }
        return r;
    }
    constexpr u8string_view EnumNameText::View () const
    {
#if qCompilerAndStdLib_constexpr_inline_variable_address_compared_to_null_Buggy
        return fLength_ <= kMaxInlineBytes ? u8string_view{fInline_.data (), fLength_} : u8string_view{fPointsTo_, fLength_};
#else
        return fPointsTo_ == nullptr ? u8string_view{fInline_.data (), fLength_} : u8string_view{fPointsTo_, fLength_};
#endif
    }
    constexpr EnumNameText::operator u8string_view () const
    {
        return View ();
    }
    constexpr bool EnumNameText::Equals (string_view s) const
    {
        u8string_view v = View ();
        if (s.size () != v.size ()) {
            return false;
        }
        for (size_t i = 0; i < v.size (); ++i) {
            if (static_cast<char8_t> (s[i]) != v[i]) {
                return false;
            }
        }
        return true;
    }
    constexpr bool EnumNameText::Equals (u8string_view s) const
    {
        return View () == s;
    }
    constexpr bool EnumNameText::Equals (wstring_view s) const
    {
        // s a code point at a time, as UTF-8, against this name's bytes
        u8string_view v = View ();
        size_t        m = 0;
        for (size_t i = 0; i < s.size ();) {
            optional<char32_t> c = NextCodePoint_ (s, i);
            if (not c) {
                return false;
            }
            char8_t buf[4]{};
            size_t  n = EncodeUTF8_ (*c, buf);
            if (m + n > v.size ()) {
                return false;
            }
            for (size_t j = 0; j < n; ++j) {
                if (v[m + j] != buf[j]) {
                    return false;
                }
            }
            m += n;
        }
        return m == v.size ();
    }
    constexpr bool EnumNameText::operator== (const EnumNameText& rhs) const
    {
        return View () == rhs.View ();
    }
    constexpr void EnumNameText::SetCopyOf_ (u8string_view s)
    {
        fPointsTo_ = nullptr;
        if (s.size () <= kMaxInlineBytes) {
            for (size_t i = 0; i < s.size (); ++i) {
                fInline_[i] = s[i];
            }
            fLength_ = s.size ();
            return;
        }
#if qStroika_Foundation_Common_EnumNameText_DefineStaticString_
        if consteval {
            fPointsTo_ = std::define_static_string (s);
            fLength_   = s.size ();
            return;
        }
#endif
        Private_::EnumNameText_TooLongToHoldInPlace_WriteItU8SoItIsPointedAt_ ();
    }
    constexpr optional<char32_t> EnumNameText::NextCodePoint_ (wstring_view s, size_t& i)
    {
        char32_t c = static_cast<char32_t> (s[i++]);
        if constexpr (sizeof (wchar_t) == 2) {
            if (c >= 0xD800 and c <= 0xDBFF) {
                if (i == s.size () or static_cast<char32_t> (s[i]) < 0xDC00 or static_cast<char32_t> (s[i]) > 0xDFFF) {
                    return nullopt; // a high surrogate not followed by a low one
                }
                return 0x10000 + ((c - 0xD800) << 10) + (static_cast<char32_t> (s[i++]) - 0xDC00);
            }
        }
        if (c > 0x10FFFF or (c >= 0xD800 and c <= 0xDFFF)) {
            return nullopt; // not a code point (or a surrogate on its own)
        }
        return c;
    }
    constexpr size_t EnumNameText::EncodeUTF8_ (char32_t c, char8_t* out)
    {
        if (c < 0x80) {
            out[0] = static_cast<char8_t> (c);
            return 1;
        }
        if (c < 0x800) {
            out[0] = static_cast<char8_t> (0xC0 | (c >> 6));
            out[1] = static_cast<char8_t> (0x80 | (c & 0x3F));
            return 2;
        }
        if (c < 0x10000) {
            out[0] = static_cast<char8_t> (0xE0 | (c >> 12));
            out[1] = static_cast<char8_t> (0x80 | ((c >> 6) & 0x3F));
            out[2] = static_cast<char8_t> (0x80 | (c & 0x3F));
            return 3;
        }
        out[0] = static_cast<char8_t> (0xF0 | (c >> 18));
        out[1] = static_cast<char8_t> (0x80 | ((c >> 12) & 0x3F));
        out[2] = static_cast<char8_t> (0x80 | ((c >> 6) & 0x3F));
        out[3] = static_cast<char8_t> (0x80 | (c & 0x3F));
        return 4;
    }

    /*
     ********************************************************************************
     *********************** Common::IdentifierNaming, StroikaNaming ****************
     ********************************************************************************
     */
    constexpr EnumNameText IdentifierNaming::Apply (const EnumNameText& identifier)
    {
        return identifier;
    }
#if qStroika_Foundation_Common_EnumNames_Annotations_
    /*
     ********************************************************************************
     ********************************* Common::Named ********************************
     ********************************************************************************
     */
    template <size_t N>
    consteval Named::Named (const char (&s)[N])
        : fText{nullptr}
        , fLength{N - 1}
    {
        u8string t;
        for (size_t i = 0; i + 1 < N; ++i) {
            if (static_cast<unsigned char> (s[i]) >= 0x80) {
                Private_::EnumNameText_ACharNameMustBeASCII_WriteItU8_ ();
            }
            t += static_cast<char8_t> (s[i]);
        }
        fText = std::define_static_string (u8string_view{t});
    }
    template <size_t N>
    consteval Named::Named (const char8_t (&s)[N])
        : fText{std::define_static_string (u8string_view{s, N - 1})}
        , fLength{N - 1}
    {
    }
    constexpr u8string_view Named::View () const
    {
        return u8string_view{fText, fLength};
    }
#endif

    constexpr EnumNameText StroikaNaming::Apply (const EnumNameText& identifier)
    {
        u8string_view v = identifier.View ();
        if (v.size () >= 2 and v[0] == u8'e' and ((v[1] >= u8'A' and v[1] <= u8'Z') or (v[1] >= u8'0' and v[1] <= u8'9'))) {
            v.remove_prefix (1);
            // a name too long to hold in place points at text lasting the program - and so does its tail
            return v.size () > EnumNameText::kMaxInlineBytes ? EnumNameText::FromStatic (v) : EnumNameText{v};
        }
        return identifier;
    }

    /*
     ********************************************************************************
     ************************ enumerators' identifiers, reflected *******************
     ********************************************************************************
     */
    namespace Private_ {
        // The compiler's text for this function's signature names V - as the enumerator with that value, if any. The technique
        // magic_enum and enchantum use; C++26 reflection (P2996) can replace it where a compiler has it.
        template <auto V>
        constexpr string_view EnumSignature_ ()
        {
#if defined(_MSC_VER) and not defined(__clang__)
            return __FUNCSIG__;
#else
            return __PRETTY_FUNCTION__;
#endif
        }
        // how the compiler writes V: "Ns::Color::eRed" for an enumerator; "(Ns::Color)3" or the like for a value with none
        template <auto V>
        constexpr string_view EnumValueText_ ()
        {
            string_view s = EnumSignature_<V> ();
#if defined(_MSC_VER) and not defined(__clang__)
            // ... __cdecl Stroika::Foundation::Common::Private_::EnumSignature_<Ns::Color::eRed>(void)
            constexpr string_view kStart_ = "EnumSignature_<";
            size_t                b       = s.find (kStart_) + kStart_.size ();
            size_t                e       = s.rfind (">(void)");
#else
            // gcc: ... [with auto V = Ns::Color::eRed; std::string_view = ...]       clang: ... [V = Ns::Color::eRed]
            constexpr string_view kStart_ = "V = ";
            size_t                b       = s.find (kStart_) + kStart_.size ();
            size_t                e       = s.find_first_of (";]", b);
#endif
            return s.substr (b, e - b);
        }
        // V's enumerator's identifier, or empty if no enumerator has the value V. After the last ::, an identifier - not
        // "Color)3" (from "(Ns::Color)3"), nor "3"; before it may be "(anonymous namespace)::" (clang) or "{anonymous}::" (gcc).
        template <auto V>
        constexpr string_view EnumIdentifier_ ()
        {
            string_view t = EnumValueText_<V> ();
            size_t      c = t.rfind ("::");
            string_view n = c == string_view::npos ? t : t.substr (c + 2);
            if (n.empty () or not((n[0] >= 'a' and n[0] <= 'z') or (n[0] >= 'A' and n[0] <= 'Z') or n[0] == '_') or n.find (')') != string_view::npos) {
                return {};
            }
            return n;
        }
        template <typename ENUM_TYPE, size_t... I>
        constexpr array<string_view, sizeof...(I)> EnumIdentifiers_ (index_sequence<I...>)
        {
            return {EnumIdentifier_<static_cast<ENUM_TYPE> (static_cast<long long> (ToInt (ENUM_TYPE::eSTART)) + static_cast<long long> (I))> ()...};
        }

        // V's identifier, as UTF-8 and exactly sized, in storage that lasts the program - only for one too long to hold in place
        template <auto V>
        inline constexpr auto kLongEnumIdentifier_ = [] () {
            constexpr string_view      id = EnumIdentifier_<V> ();
            array<char8_t, id.size ()> r{};
            for (size_t i = 0; i < id.size (); ++i) {
                r[i] = static_cast<char8_t> (id[i]);
            }
            return r;
        }();
        // V's identifier: held in place, or pointing at kLongEnumIdentifier_<V> - never at the signature text it is cut from
        template <auto V>
        constexpr EnumNameText EnumIdentifierText_ ()
        {
            constexpr string_view id = EnumIdentifier_<V> ();
            if constexpr (id.size () <= EnumNameText::kMaxInlineBytes) {
                return EnumNameText::FromUTF8 (id);
            }
            else {
                return EnumNameText::FromStatic (u8string_view{kLongEnumIdentifier_<V>.data (), kLongEnumIdentifier_<V>.size ()});
            }
        }

        // an EnumNamingRule declaration beside the enum picks its rule - only the declaration's return type is used
        template <typename ENUM_TYPE>
        concept IHasNamingRule_ = requires (ENUM_TYPE e) { EnumNamingRule (e); };
        template <typename ENUM_TYPE>
        struct NamingRule_ {
            using type = IdentifierNaming;
        };
        template <IHasNamingRule_ ENUM_TYPE>
        struct NamingRule_<ENUM_TYPE> {
            using type = decltype (EnumNamingRule (ENUM_TYPE{}));
            static_assert (IEnumNamingRule<type>,
                           "EnumNamingRule's return type must be an IEnumNamingRule - as IdentifierNaming, StroikaNaming");
        };
        // an EnumeratorName beside the enum renames enumerators, after the rule
        template <typename ENUM_TYPE>
        concept IHasEnumeratorName_ = requires (ENUM_TYPE e, EnumNameText n) {
            { EnumeratorName (e, n) } -> convertible_to<EnumNameText>;
        };

#if qStroika_Foundation_Common_EnumNames_Annotations_
        // clang-format off
        template <typename T>
        constexpr bool kIsEnumNamingRule_ = IEnumNamingRule<T>;
        // the naming rule an annotation on the enum gives - enum class [[= Common::StroikaNaming{}]] E - or void
        template <typename ENUM_TYPE>
        consteval std::meta::info AnnotatedNamingRule_ ()
        {
#if qCompilerAndStdLib_reflection_debug_mode_vector_of_info_Buggy
            vector<std::meta::info> annotations = std::meta::annotations_of (^^ENUM_TYPE);
            for (size_t i = 0; i < annotations.size (); ++i) {
                std::meta::info a = annotations.data ()[i];
#else
            for (std::meta::info a : std::meta::annotations_of (^^ENUM_TYPE)) {
#endif
                std::meta::info t = std::meta::remove_cvref (std::meta::type_of (a));
                if (std::meta::extract<bool> (std::meta::substitute (^^kIsEnumNamingRule_, {t}))) {
                    return t;
                }
            }
            return ^^void;
        }
        template <typename ENUM_TYPE>
        concept IHasAnnotatedNamingRule_ = (AnnotatedNamingRule_<ENUM_TYPE> () != ^^void);
        template <IHasAnnotatedNamingRule_ ENUM_TYPE>
            requires (not IHasNamingRule_<ENUM_TYPE>)
        struct NamingRule_<ENUM_TYPE> {
            using type = typename [:AnnotatedNamingRule_<ENUM_TYPE> ():];
        };
        // the Named annotation on V's enumerator (the first declared with the value V - not eSTART, ...), if any
        template <auto V>
        consteval optional<u8string_view> AnnotatedName_ ()
        {
#if qCompilerAndStdLib_reflection_debug_mode_vector_of_info_Buggy
            vector<std::meta::info> enumerators = std::meta::enumerators_of (^^decltype (V));
            for (size_t i = 0; i < enumerators.size (); ++i) {
                std::meta::info e = enumerators.data ()[i];
#else
            for (std::meta::info e : std::meta::enumerators_of (^^decltype (V))) {
#endif
                if (std::meta::extract<decltype (V)> (std::meta::constant_of (e)) == V) {
                    vector<std::meta::info> names = std::meta::annotations_of_with_type (e, ^^Named);
                    if (names.empty ()) {
                        return nullopt;
                    }
#if qCompilerAndStdLib_reflection_debug_mode_vector_of_info_Buggy
                    return std::meta::extract<Named> (*names.data ()).View ();
#else
                    return std::meta::extract<Named> (names.front ()).View ();
#endif
                }
            }
            return nullopt;
        }
        // clang-format on
#endif

        // V's default name: its identifier, through its enum's naming rule, then its Named annotation and its EnumeratorName -
        // or empty, for a value no enumerator has (never passed to either)
        template <auto V>
        constexpr EnumNameText DefaultEnumeratorName_ ()
        {
            using ENUM_TYPE = decltype (V);
            if constexpr (EnumIdentifier_<V> ().empty ()) {
                return EnumNameText{};
            }
            else {
                EnumNameText n = NamingRule_<ENUM_TYPE>::type::Apply (EnumIdentifierText_<V> ());
#if qStroika_Foundation_Common_EnumNames_Annotations_
                static_assert (not(IHasNamingRule_<ENUM_TYPE> and IHasAnnotatedNamingRule_<ENUM_TYPE>),
                               "an enum picks its naming rule one way: by annotation, or by EnumNamingRule");
                if constexpr (constexpr optional<u8string_view> a = AnnotatedName_<V> (); a.has_value ()) {
                    n = EnumNameText::FromStatic (*a);
                }
#endif
                if constexpr (IHasEnumeratorName_<ENUM_TYPE>) {
                    n = EnumeratorName (V, n);
                }
                return n;
            }
        }
        template <typename ENUM_TYPE, size_t... I>
        constexpr array<EnumNameText, sizeof...(I)> DefaultEnumeratorNames_ (index_sequence<I...>)
        {
            return {DefaultEnumeratorName_<static_cast<ENUM_TYPE> (static_cast<long long> (ToInt (ENUM_TYPE::eSTART)) + static_cast<long long> (I))> ()...};
        }
    }

    /*
     ********************************************************************************
     ********************************* Common::EnumNames ****************************
     ********************************************************************************
     */
    template <typename ENUM_TYPE>
    constexpr EnumNames<ENUM_TYPE>::EnumNames (const BasicArrayInitializer& init)
    {
        for (const auto& [e, n] : init) {
            Require (n != nullptr); // a name for each value from eSTART to eLAST
            fNames_[fSize_++] = Entry{e, EnumNameText{n}};
        }
    }
    template <typename ENUM_TYPE>
    constexpr EnumNames<ENUM_TYPE>::EnumNames (initializer_list<Entry> names)
    {
        Require (names.size () <= kCapacity);
        for (const Entry& n : names) {
            fNames_[fSize_++] = n;
        }
    }
    template <typename ENUM_TYPE>
    constexpr EnumNames<ENUM_TYPE> EnumNames<ENUM_TYPE>::Reflected ()
    {
        constexpr array<string_view, kCapacity>  kIdentifiers = Private_::EnumIdentifiers_<ENUM_TYPE> (make_index_sequence<kCapacity>{});
        constexpr array<EnumNameText, kCapacity> kNames = Private_::DefaultEnumeratorNames_<ENUM_TYPE> (make_index_sequence<kCapacity>{});
        EnumNames                                r;
        for (size_t i = 0; i < kCapacity; ++i) {
            if (not kIdentifiers[i].empty ()) { // else no enumerator has this value
                r.fNames_[r.fSize_++] =
                    Entry{static_cast<ENUM_TYPE> (static_cast<long long> (ToInt (ENUM_TYPE::eSTART)) + static_cast<long long> (i)), kNames[i]};
            }
        }
        return r;
    }
    template <typename ENUM_TYPE>
    constexpr auto EnumNames<ENUM_TYPE>::begin () const -> const_iterator
    {
        return const_iterator{fNames_.data ()};
    }
    template <typename ENUM_TYPE>
    constexpr auto EnumNames<ENUM_TYPE>::end () const -> const_iterator
    {
        return const_iterator{fNames_.data () + fSize_};
    }
    template <typename ENUM_TYPE>
    constexpr size_t EnumNames<ENUM_TYPE>::size () const
    {
        return fSize_;
    }
    template <typename ENUM_TYPE>
    constexpr optional<u8string_view> EnumNames<ENUM_TYPE>::PeekName (ENUM_TYPE e) const
    {
        for (size_t i = 0; i < fSize_; ++i) {
            if (fNames_[i].first == e) {
                return fNames_[i].second.View ();
            }
        }
        return nullopt;
    }
    template <typename ENUM_TYPE>
    constexpr u8string_view EnumNames<ENUM_TYPE>::GetName (ENUM_TYPE e) const
    {
        optional<u8string_view> r = PeekName (e);
        Require (r.has_value ());
        return *r;
    }
    template <typename ENUM_TYPE>
    constexpr optional<ENUM_TYPE> EnumNames<ENUM_TYPE>::PeekValue (string_view name) const
    {
        for (size_t i = 0; i < fSize_; ++i) {
            if (fNames_[i].second.Equals (name)) {
                return fNames_[i].first;
            }
        }
        return nullopt;
    }
    template <typename ENUM_TYPE>
    constexpr optional<ENUM_TYPE> EnumNames<ENUM_TYPE>::PeekValue (u8string_view name) const
    {
        for (size_t i = 0; i < fSize_; ++i) {
            if (fNames_[i].second.Equals (name)) {
                return fNames_[i].first;
            }
        }
        return nullopt;
    }
    template <typename ENUM_TYPE>
    constexpr optional<ENUM_TYPE> EnumNames<ENUM_TYPE>::PeekValue (wstring_view name) const
    {
        for (size_t i = 0; i < fSize_; ++i) {
            if (fNames_[i].second.Equals (name)) {
                return fNames_[i].first;
            }
        }
        return nullopt;
    }
    template <typename ENUM_TYPE>
    constexpr ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (string_view name) const
    {
        optional<ENUM_TYPE> r = PeekValue (name);
        Require (r.has_value ());
        return *r;
    }
    template <typename ENUM_TYPE>
    constexpr ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (u8string_view name) const
    {
        optional<ENUM_TYPE> r = PeekValue (name);
        Require (r.has_value ());
        return *r;
    }
    template <typename ENUM_TYPE>
    constexpr ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (wstring_view name) const
    {
        optional<ENUM_TYPE> r = PeekValue (name);
        Require (r.has_value ());
        return *r;
    }
    template <typename ENUM_TYPE>
    template <typename NOT_FOUND_EXCEPTION>
    inline ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (string_view name, const NOT_FOUND_EXCEPTION& notFoundException) const
    {
        if (optional<ENUM_TYPE> r = PeekValue (name)) [[likely]] {
            return *r;
        }
        throw notFoundException;
    }
    template <typename ENUM_TYPE>
    template <typename NOT_FOUND_EXCEPTION>
    inline ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (u8string_view name, const NOT_FOUND_EXCEPTION& notFoundException) const
    {
        if (optional<ENUM_TYPE> r = PeekValue (name)) [[likely]] {
            return *r;
        }
        throw notFoundException;
    }
    template <typename ENUM_TYPE>
    template <typename NOT_FOUND_EXCEPTION>
    inline ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (wstring_view name, const NOT_FOUND_EXCEPTION& notFoundException) const
    {
        if (optional<ENUM_TYPE> r = PeekValue (name)) [[likely]] {
            return *r;
        }
        throw notFoundException;
    }
    template <typename ENUM_TYPE>
    constexpr EnumNames<ENUM_TYPE> EnumNames<ENUM_TYPE>::With (ENUM_TYPE e, const EnumNameText& name) const
    {
        Require (PeekName (e).has_value ());
        EnumNames r = *this;
        for (size_t i = 0; i < r.fSize_; ++i) {
            if (r.fNames_[i].first == e) {
                r.fNames_[i].second = name;
            }
        }
        return r;
    }

    /*
     ********************************************************************************
     ********************** Common::EnumNames::const_iterator ***********************
     ********************************************************************************
     */
    template <typename ENUM_TYPE>
    constexpr EnumNames<ENUM_TYPE>::const_iterator::const_iterator (const Entry* e)
        : fEntry_{e}
    {
    }
    template <typename ENUM_TYPE>
    constexpr auto EnumNames<ENUM_TYPE>::const_iterator::operator* () const -> value_type
    {
        return value_type{fEntry_->first, fEntry_->second.View ()};
    }
    template <typename ENUM_TYPE>
    constexpr auto EnumNames<ENUM_TYPE>::const_iterator::operator++ () -> const_iterator&
    {
        ++fEntry_;
        return *this;
    }
    template <typename ENUM_TYPE>
    constexpr auto EnumNames<ENUM_TYPE>::const_iterator::operator++ (int) -> const_iterator
    {
        const_iterator r = *this;
        ++fEntry_;
        return r;
    }

    /*
     ********************************************************************************
     ****************************** Common::DefaultNames ****************************
     ********************************************************************************
     */
    template <typename ENUM_TYPE>
    constexpr DefaultNames<ENUM_TYPE>::operator const EnumNames<ENUM_TYPE>&() const
    {
        return kDefaultNames<ENUM_TYPE>;
    }
    template <typename ENUM_TYPE>
    constexpr auto DefaultNames<ENUM_TYPE>::begin () const -> typename EnumNames<ENUM_TYPE>::const_iterator
    {
        return kDefaultNames<ENUM_TYPE>.begin ();
    }
    template <typename ENUM_TYPE>
    constexpr auto DefaultNames<ENUM_TYPE>::end () const -> typename EnumNames<ENUM_TYPE>::const_iterator
    {
        return kDefaultNames<ENUM_TYPE>.end ();
    }
    template <typename ENUM_TYPE>
    constexpr size_t DefaultNames<ENUM_TYPE>::size () const
    {
        return kDefaultNames<ENUM_TYPE>.size ();
    }
    template <typename ENUM_TYPE>
    constexpr optional<u8string_view> DefaultNames<ENUM_TYPE>::PeekName (ENUM_TYPE e) const
    {
        return kDefaultNames<ENUM_TYPE>.PeekName (e);
    }
    template <typename ENUM_TYPE>
    constexpr u8string_view DefaultNames<ENUM_TYPE>::GetName (ENUM_TYPE e) const
    {
        return kDefaultNames<ENUM_TYPE>.GetName (e);
    }
    template <typename ENUM_TYPE>
    template <typename NAME>
    constexpr optional<ENUM_TYPE> DefaultNames<ENUM_TYPE>::PeekValue (NAME&& name) const
    {
        return kDefaultNames<ENUM_TYPE>.PeekValue (forward<NAME> (name));
    }
    template <typename ENUM_TYPE>
    template <typename... ARGS>
    constexpr ENUM_TYPE DefaultNames<ENUM_TYPE>::GetValue (ARGS&&... args) const
    {
        return kDefaultNames<ENUM_TYPE>.GetValue (forward<ARGS> (args)...);
    }

    /*
     ********************************************************************************
     ****************************** Common::kDefaultNames ***************************
     ********************************************************************************
     */
    namespace Private_ {
        // a table written for Stroika v3.0d25 and before: its DefaultNames<ENUM_TYPE>::k (DEPRECATED - asked about here, used as
        // it is) is a constant expression; an unspecialized k, declared only, is not (clang warns, asked about it - that is the point)
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wundefined-var-template\"");
        template <typename ENUM_TYPE>
        concept IHasOldTable_ = requires { typename integral_constant<size_t, DefaultNames<ENUM_TYPE>::k.size ()>; };
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wundefined-var-template\"");

        template <IBoundedEnum ENUM_TYPE>
        constexpr EnumNames<ENUM_TYPE> ComputeDefaultNames_ ()
        {
            if constexpr (IHasOldTable_<ENUM_TYPE>) {
                return DefaultNames<ENUM_TYPE>::k;
            }
            else {
                return EnumNames<ENUM_TYPE>::Reflected ();
            }
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    }

}
