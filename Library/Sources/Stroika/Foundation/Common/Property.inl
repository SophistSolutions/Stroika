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
    template <typename T>
    template <typename CHECK, enable_if_t<not PropertyCommon::kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::Get () const
    {
        return fGetter_ (this);
    }
    template <typename T>
    template <typename CHECK, enable_if_t<PropertyCommon::kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::Get ()
    {
        return fGetter_ (this);
    }
    template <typename T>
    template <typename CHECK, enable_if_t<not PropertyCommon::kIsMutatableType<CHECK>>*>
    inline ReadOnlyProperty<T>::operator const T () const
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<PropertyCommon::kIsMutatableType<CHECK>>*>
    inline ReadOnlyProperty<T>::operator T ()
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<not PropertyCommon::kIsMutatableType<CHECK>>*>
    inline const T ReadOnlyProperty<T>::operator() () const
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<PropertyCommon::kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::operator() ()
    {
        return Get ();
    }

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
    inline auto Property<T>::operator= (Configuration::ArgByValueType<decayed_value_type> value) -> Property&
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
              [getter] ([[maybe_unused]] const auto* property) -> typename Property<T>::base_value_type {
                  // Subtle - but the 'property' here refers to 'this' (ExtendableProperty). The getter itself will want to extract the parent object, but
                  // unlike other getter/setters, here the auto property is already for this object.
                  const ExtendableProperty* thisObj = static_cast<const ExtendableProperty*> (property); // cannot use dynamic_cast without adding needless vtable to Property objects; static_cast needed (over reintepret_cast) to adjust sub-object pointer for multiple inheritance
                  AssertNotNull (thisObj);
                  for (const auto& handler : thisObj->fPropertyReadHandlers_) {
                      if (auto o = handler ()) {
                          if constexpr (is_reference_v<base_value_type>) {
                              // we force return value of property read handler to be optional<ptr> because optional<refrence> is illegal in c++ (as of 2021-02-19)
                              return **o;
                          }
                          else {
                              return *o;
                          }
                      }
                  }
                  return getter (property);
              },
              [getter, setter] (auto* property, const auto& newValue) {
                  // Subtle - but the 'property' here refers to 'this' (ExtendableProperty). The getter itself will want to extract the parent object, but
                  // unlike other getter/setters, here the auto property is already for this object.
                  ExtendableProperty* thisObj = static_cast<ExtendableProperty*> (property); // cannot use dynamic_cast without adding needless vtable to Property objects; static_cast needed (over reintepret_cast) to adjust sub-object pointer for multiple inheritance
                  AssertNotNull (thisObj);
                  if (not thisObj->fPropertyChangedHandlers_.empty ()) {
                      T prevValue = getter (property);
                      for (const auto& handler : thisObj->fPropertyChangedHandlers_) {
                          if (handler (PropertyChangedEvent{prevValue, newValue}) == PropertyCommon::PropertyChangedEventResultType::eSilentlyCutOffProcessing) {
                              return;
                          }
                      }
                  }
                  setter (property, newValue);
              }}
        , propertyReadHandlers{
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> const std::forward_list<PropertyReadEventHandler>& {
                  const ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::propertyReadHandlers);
                  return thisObj->fPropertyReadHandlers_;
              }}
        , rwPropertyReadHandlers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> std::forward_list<PropertyReadEventHandler>& {
                ExtendableProperty* thisObj = const_cast<ExtendableProperty*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyReadHandlers));
                return thisObj->fPropertyReadHandlers_;
            },
            [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, const auto& handlerList) {
                ExtendableProperty* thisObj     = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyReadHandlers);
                thisObj->fPropertyReadHandlers_ = handlerList;
            }}
        , propertyChangedHandlers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> const std::forward_list<PropertyChangedEventHandler>& {
            const ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::propertyChangedHandlers);
            return thisObj->fPropertyChangedHandlers_;
        }}
        , rwPropertyChangedHandlers{
            [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> std::forward_list<PropertyChangedEventHandler>& {
                ExtendableProperty* thisObj = const_cast<ExtendableProperty*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyChangedHandlers));
                return thisObj->fPropertyChangedHandlers_;
            },
            [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, const auto& handlerList) {
                ExtendableProperty* thisObj        = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyChangedHandlers);
                thisObj->fPropertyChangedHandlers_ = handlerList;
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
