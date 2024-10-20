/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Stroika/Foundation/Containers/Factory/Collection_Factory.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************************** Collection<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection<T>::Collection ()
        : inherited{Factory::Collection_Factory<value_type>::Default () ()}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
    inline Collection<T>::Collection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
        : Collection{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE2> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline Collection<T>::Collection (const shared_ptr<_IRep>& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline Collection<T>::Collection (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Collection<T>::Collection (const initializer_list<value_type>& src)
        : Collection{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Collection<T>>)
    inline Collection<T>::Collection (ITERABLE_OF_ADDABLE&& src)
        : Collection{}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    template <Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline bool Collection<T>::Contains (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer) const
    {
        return this->Find (item, forward<EQUALS_COMPARER> (equalsComparer)) != this->end ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
    void Collection<T>::AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE2> (end); ++i) {
            tmp._GetWriteableRep ().Add (*i, nullptr);
        }
    }
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline void Collection<T>::AddAll (ITERABLE_OF_ADDABLE&& items)
    {
        if constexpr (std::is_convertible_v<remove_cvref_t<ITERABLE_OF_ADDABLE>*, Collection<value_type>*>) {
            // very rare corner case
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[unlikely]] {
                // vector iterator CTOR does't support sentinel for second iterator arg...
                vector<value_type> copy{std::begin (items), Iterator<value_type>{std::end (items)}}; // because you can not iterate over a container while modifying it
                AddAll (std::begin (copy), std::end (copy));
                return;
            }
        }
        AddAll (std::begin (items), std::end (items));
    }
    template <typename T>
    inline void Collection<T>::Add (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, nullptr);
        Ensure (not this->empty ());
    }
    template <typename T>
    inline void Collection<T>::Add (ArgByValueType<value_type> item, Iterator<T>* addedAt)
    {
        RequireNotNull (addedAt);
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, addedAt);
        Ensure (not this->empty ());
        Ensure (not addedAt->Done ());
    }
    template <typename T>
    inline void Collection<T>::Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Update (patchedIterator, newValue, nextI);
    }
    template <typename T>
    inline void Collection<T>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> EQUALS_COMPARER>
    inline void Collection<T>::Remove (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer)
    {
        auto i = this->Find (item, forward<EQUALS_COMPARER> (equalsComparer));
        Require (i != this->end ()); // use remove-if if the item might not exist
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nullptr);
    }
    template <typename T>
    template <Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline bool Collection<T>::RemoveIf (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer)
    {
        if (auto i = this->Find (item, forward<EQUALS_COMPARER> (equalsComparer))) {
            auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
            writerRep->Remove (patchedIterator, nullptr);
            return true;
        }
        return false;
    }
    template <typename T>
    template <predicate<T> PREDICATE>
    bool Collection<T>::RemoveIf (PREDICATE&& p)
    {
        if (auto i = this->Find (forward<PREDICATE> (p))) {
            Remove (i);
            return true;
        }
        return false;
    }
    template <typename T>
    inline void Collection<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T>
    template <Common::IEqualsComparer<T> EQUALS_COMPARER>
    size_t Collection<T>::RemoveAll (const Iterator<value_type>& start, const Iterator<value_type>& end, EQUALS_COMPARER&& equalsComparer)
    {
        size_t cnt{};
        for (auto i = start; i != end;) {
            if (RemoveIf (*i, equalsComparer, &i)) {
                ++cnt;
            }
        }
        return cnt;
    }
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, typename EQUALS_COMPARER>
    inline size_t Collection<T>::RemoveAll (const ITERABLE_OF_ADDABLE& c, EQUALS_COMPARER&& equalsComparer)
    {
        if (static_cast<const void*> (this) == static_cast<const void*> (addressof (c))) {
            return RemoveAll (forward<EQUALS_COMPARER> (equalsComparer));
        }
        else {
            return RemoveAll (std::begin (c), std::end (c), forward<EQUALS_COMPARER> (equalsComparer));
        }
    }
    template <typename T>
    template <predicate<T> PREDICATE>
    size_t Collection<T>::RemoveAll (PREDICATE&& p)
    {
        size_t nRemoved{};
        for (Iterator<T> i = this->begin (); i != this->end ();) {
            if (p (*i)) {
                Remove (i, &i);
                ++nRemoved;
            }
            else {
                ++i;
            }
        }
        return nRemoved;
    }
    template <typename T>
    inline void Collection<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    template <Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline void Collection<T>::erase (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer)
    {
        Remove (item, forward<EQUALS_COMPARER> (equalsComparer));
    }
    template <typename T>
    inline auto Collection<T>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> nextI{nullptr};
        Remove (i, &nextI);
        return nextI;
    }
    template <typename T>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    inline RESULT_CONTAINER Collection<T>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, Collection>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> implementation then...
        }
    }
    template <typename T>
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
    inline RESULT_CONTAINER Collection<T>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        if constexpr (same_as<RESULT_CONTAINER, Collection>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default Iterable<> implementation then...
        }
    }
    template <typename T>
    inline auto Collection<T>::operator+= (ArgByValueType<value_type> item) -> Collection&
    {
        Add (item);
        return *this;
    }
    template <typename T>
    inline auto Collection<T>::operator+= (const Iterable<value_type>& items) -> Collection&
    {
        AddAll (items);
        return *this;
    }
    template <typename T>
    auto Collection<T>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type* writableRep = this->_fRep.rwget ([&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
            return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
        });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), move (patchedIterator));
    }
    template <typename T>
    inline void Collection<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }

    /*
     ********************************************************************************
     ********************************* operator+ ************************************
     ********************************************************************************
     */
    template <typename T>
    Collection<T> operator+ (const Iterable<T>& lhs, const Collection<T>& rhs)
    {
        Collection<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Collection<T> operator+ (const Collection<T>& lhs, const Iterable<T>& rhs)
    {
        Collection<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Collection<T> operator+ (const Collection<T>& lhs, const Collection<T>& rhs)
    {
        Collection<T> result{lhs};
        result += rhs;
        return result;
    }
}
