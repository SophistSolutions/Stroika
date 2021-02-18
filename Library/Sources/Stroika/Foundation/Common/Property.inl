/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Property_inl_
#define _Stroika_Foundation_Common_Property_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if !qStroika_Foundation_Common_Property_EmbedThisInProperties
#include "../Memory/ObjectFieldUtilities.h"
#endif

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** ReadOnlyProperty<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G>
    constexpr ReadOnlyProperty<T>::ReadOnlyProperty (G getter)
        : fGetter_ (getter) // no uniform initialization because this may involve conersions
    {
    }
#if !qCompilerAndStdLib_template_enable_if_const_nonconst_overload_Buggy
    template <typename T>
    template <typename CHECK, enable_if_t<not ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::Get () const
    {
        return fGetter_ (this);
    }
    template <typename T>
    template <typename CHECK, enable_if_t<ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::Get ()
    {
        return Get ();
    }
#endif
#if !qCompilerAndStdLib_template_enable_if_const_nonconst_overload_Buggy
    template <typename T>
    template <typename CHECK, enable_if_t<not ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline ReadOnlyProperty<T>::operator const T () const
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline ReadOnlyProperty<T>::operator T ()
    {
        return Get ();
    }
#endif
#if !qCompilerAndStdLib_template_enable_if_const_nonconst_overload_Buggy
    template <typename T>
    template <typename CHECK, enable_if_t<not ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline const T ReadOnlyProperty<T>::operator() () const
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::operator() ()
    {
        return Get ();
    }
#endif

    /*
     ********************************************************************************
     ****************************** WriteOnlyProperty<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename S>
    constexpr WriteOnlyProperty<T>::WriteOnlyProperty (S setter)
        : fSetter_ (setter) // no uniform initialization because this may involve conersions
    {
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::Set (Configuration::ArgByValueType<T> value)
    {
        fSetter_ (this, value);
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::operator() (Configuration::ArgByValueType<T> value)
    {
        Set (value);
    }
    template <typename T>
    inline auto WriteOnlyProperty<T>::operator= (Configuration::ArgByValueType<T> value) -> WriteOnlyProperty&
    {
        Set (value);
        return *this;
    }

    /*
     ********************************************************************************
     ******************************** Property<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G, typename S>
    inline Property<T>::Property (G getter, S setter)
        : ReadOnlyProperty<T>{getter}
        , WriteOnlyProperty<decayed_value_type>{setter}
    {
    }
    template <typename T>
    inline auto Property<T>::operator= (Configuration::ArgByValueType<T> value) -> Property&
    {
        WriteOnlyProperty<decayed_value_type>::operator= (value);
        return *this;
    }
    template <typename T>
    inline auto Property<T>::operator= (const Property& value) -> Property&
    {
        Set (value.Get ());
        return *this;
    }
    template <typename T>
    template <typename TT>
    inline bool Property<T>::operator== (const TT& rhs) const
    {
        return Get () == static_cast<T> (rhs);
    }

    /*
     ********************************************************************************
     ************************** ExtendableProperty<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G, typename S>
    ExtendableProperty<T>::ExtendableProperty (G getter, S setter)
        : Property<T>{
              [getter qStroika_Foundation_Common_Property_ExtraCaptureStuff1] ([[maybe_unused]] const auto* property) { return getter (property); },
              [getter, setter qStroika_Foundation_Common_Property_ExtraCaptureStuff1] (auto* property, const auto& newValue) {
                  // Subtle - but the 'property' here refers to 'this' (ExtendableProperty). The getter itself will want to extract the parent object, but
                  // unlike other getter/setters, here the auto property is already for this object.
                  ExtendableProperty* thisObj = static_cast<ExtendableProperty*> (property); // cannot use dynamic_cast without adding needless vtable to Property objects; static_cast needed (over reintepret_cast) to adjust sub-object pointer for multiple inheritance
                  AssertNotNull (thisObj);
                  if (not thisObj->fHandlers_.empty ()) {
                      T prevValue = getter (property);
                      for (const auto& handler : thisObj->fHandlers_) {
                          if (not handler (PropertyChangedEvent{prevValue, newValue})) {
                              return;
                          }
                      }
                  }
                  setter (property, newValue);
              }}
        , handlers{
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> const std::forward_list<EventHandler>& {
                  const ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::handlers);
                  return thisObj->fHandlers_;
              }}
        , rwHandlers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> std::forward_list<EventHandler>& {
                         ExtendableProperty* thisObj = const_cast<ExtendableProperty*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwHandlers));
                         return thisObj->fHandlers_;
                     },
                     [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, const auto& handlerList) {
                         ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwHandlers);
                         thisObj->fHandlers_         = handlerList;
                     }}
    {
    }
    template <typename T>
    inline auto ExtendableProperty<T>::operator= (Configuration::ArgByValueType<T> value) -> ExtendableProperty&
    {
        Property<T>::operator= (value);
        return *this;
    }
    template <typename T>
    inline auto ExtendableProperty<T>::operator= (const ExtendableProperty& value) -> ExtendableProperty&
    {
        Property<T>::operator= (value);
        return *this;
    }

}

#endif /*_Stroika_Foundation_Common_Property_inl_*/
