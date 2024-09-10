/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#if !qStroika_Foundation_Common_Property_EmbedThisInProperties
#include "Stroika/Foundation/Memory/ObjectFieldUtilities.h"
#endif

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** ReadOnlyProperty<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <qCompilerAndStdLib_UseREQ1_BWA (invocable<const ReadOnlyProperty<T>*>) G>
    constexpr ReadOnlyProperty<T>::ReadOnlyProperty (G getter)
        qCompilerAndStdLib_UseREQ2_BWA (requires (convertible_to<invoke_result_t<G, const ReadOnlyProperty<T>*>, T>))
        : fGetter_ (getter) // no uniform initialization because this may involve conversions
    {
    }
    template <typename T>
    inline T ReadOnlyProperty<T>::Get () const
        requires (not IPropertyMutatable<T>)
    {
        return fGetter_ (this);
    }
    template <typename T>
    inline T ReadOnlyProperty<T>::Get ()
        requires (IPropertyMutatable<T>)
    {
        return fGetter_ (this);
    }
    template <typename T>
    inline ReadOnlyProperty<T>::operator const T () const
        requires (not IPropertyMutatable<T>)
    {
        return Get ();
    }
    template <typename T>
    inline ReadOnlyProperty<T>::operator T ()
        requires (IPropertyMutatable<T>)
    {
        return Get ();
    }
    template <typename T>
    inline const T ReadOnlyProperty<T>::operator() () const
        requires (not IPropertyMutatable<T>)
    {
        return Get ();
    }
    template <typename T>
    inline T ReadOnlyProperty<T>::operator() ()
        requires (IPropertyMutatable<T>)
    {
        return Get ();
    }

    /*
     ********************************************************************************
     ****************************** WriteOnlyProperty<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    template <qCompilerAndStdLib_UseREQ1_BWA (invocable<WriteOnlyProperty<T>*, T>) S>
    constexpr WriteOnlyProperty<T>::WriteOnlyProperty (S setter)
        : fSetter_ (setter) // no uniform initialization because this may involve conversions
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
    template <invocable<const ReadOnlyProperty<T>*> G, invocable<WriteOnlyProperty<remove_cvref_t<T>>*, remove_cvref_t<T>> S>
    inline Property<T>::Property (G getter, S setter)
        requires (convertible_to<invoke_result_t<G, const ReadOnlyProperty<T>*>, T>)
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
     ****************************** ConstantProperty<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <invocable F>
    constexpr ConstantProperty<T>::ConstantProperty (F oneTimeGetter)
        requires (convertible_to<invoke_result_t<F>, T>)
        : fOneTimeGetter_{oneTimeGetter}
    {
    }
    template <typename T>
    inline ConstantProperty<T>::operator const T () const
    {
        return Getter_ ();
    }
    template <typename T>
    inline const T ConstantProperty<T>::operator() () const
    {
        return Getter_ ();
    }
    template <typename T>
    inline const T* ConstantProperty<T>::operator->() const
    {
        return &(Getter_ ());
    }
    template <typename T>
    const inline T& ConstantProperty<T>::Getter_ () const
    {
        call_once (fValueInitialized_, [&] () { fCachedValue_ = fOneTimeGetter_ (); });
        Ensure (fCachedValue_.has_value ());
        return *fCachedValue_;
    }

    /*
     ********************************************************************************
     ************************** ExtendableProperty<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    template <qCompilerAndStdLib_UseREQ1_BWA (invocable<const ExtendableProperty<T>*>) G,
              qCompilerAndStdLib_UseREQ1_BWA (invocable<ExtendableProperty<T>*, remove_cvref_t<T>>) S>
    ExtendableProperty<T>::ExtendableProperty (G getter, S setter)
        qCompilerAndStdLib_UseREQ2_BWA (requires (convertible_to<invoke_result_t<G, const ExtendableProperty<T>*>, T>))
        : Property<T>{[getter] ([[maybe_unused]] const auto* property) -> typename Property<T>::base_value_type {
                          // Subtle - but the 'property' here refers to 'this' (ExtendableProperty). The getter itself will want to extract the parent object, but
                          // unlike other getter/setters, here the auto property is already for this object.
                          const ExtendableProperty* thisObj = static_cast<const ExtendableProperty*> (property); // cannot use dynamic_cast without adding needless vtable to Property objects; static_cast needed (over reintepret_cast) to adjust sub-object pointer for multiple inheritance
                          AssertNotNull (thisObj);
                          if constexpr (is_reference_v<base_value_type>) { // see docs on PropertyReadEventHandlerArgAndReturnValue_
                              PropertyReadEventHandlerArgAndReturnValue_ value = &getter (property);
                              for (const auto& handler : thisObj->fPropertyReadHandlers_) {
                                  value = handler (value);
                              }
                              return *value;
                          }
                          else {
                              base_value_type value = getter (property);
                              for (const auto& handler : thisObj->fPropertyReadHandlers_) {
                                  value = handler (value);
                              }
                              return value;
                          }
                      },
                      [getter, setter] (auto* property, const auto& newValue) {
                          // Subtle - but the 'property' here refers to 'this' (ExtendableProperty). The getter itself will want to extract the parent object, but
                          // unlike other getter/setters, here the auto property is already for this object.
                          ExtendableProperty* thisObj = static_cast<ExtendableProperty*> (property); // cannot use dynamic_cast without adding needless vtable to Property objects; static_cast needed (over reintepret_cast) to adjust sub-object pointer for multiple inheritance
                          AssertNotNull (thisObj);
                          if (not thisObj->fPropertyChangedHandlers_.empty ()) {
                              T prevValue = getter (property);
                              for (const auto& handler : thisObj->fPropertyChangedHandlers_) {
                                  if (handler (PropertyChangedEvent{prevValue, newValue}) ==
                                      PropertyCommon::PropertyChangedEventResultType::eSilentlyCutOffProcessing) {
                                      return;
                                  }
                              }
                          }
                          setter (property, newValue);
                      }}
        , propertyReadHandlers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> const std::forward_list<PropertyReadEventHandler>& {
            const ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::propertyReadHandlers);
            return thisObj->fPropertyReadHandlers_;
        }}
        , rwPropertyReadHandlers{
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> std::forward_list<PropertyReadEventHandler>& {
                  ExtendableProperty* thisObj = const_cast<ExtendableProperty*> (
                      qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyReadHandlers));
                  return thisObj->fPropertyReadHandlers_;
              },
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, const auto& handlerList) {
                  ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyReadHandlers);
                  thisObj->fPropertyReadHandlers_ = handlerList;
              }}
        , propertyChangedHandlers{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] (
                                      const auto* property) -> const std::forward_list<PropertyChangedEventHandler>& {
            const ExtendableProperty* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::propertyChangedHandlers);
            return thisObj->fPropertyChangedHandlers_;
        }}
        , rwPropertyChangedHandlers{
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (const auto* property) -> std::forward_list<PropertyChangedEventHandler>& {
                  ExtendableProperty* thisObj = const_cast<ExtendableProperty*> (
                      qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyChangedHandlers));
                  return thisObj->fPropertyChangedHandlers_;
              },
              [qStroika_Foundation_Common_Property_ExtraCaptureStuff] (auto* property, const auto& handlerList) {
                  ExtendableProperty* thisObj =
                      qStroika_Foundation_Common_Property_OuterObjPtr (property, &ExtendableProperty::rwPropertyChangedHandlers);
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
