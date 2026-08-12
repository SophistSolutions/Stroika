/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include <algorithm>
#include <execution>

#include "Stroika/Foundation/Common/Empty.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
#include "Stroika/Foundation/Containers/Factory/Sequence_Factory.h"
#include "Stroika/Foundation/Containers/Private/IterableUtils.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** Sequence<T>::TemporaryElementReference_ *******************
     ********************************************************************************
     */
    /*
     * TemporaryElementReference_ is a private implementation detail, so we can do:
     *      Sequence<int> x;    // and initialize with several items, and then
     *      x[3] = 4;
     *
     *  We need two templated variants - one inheriting and one not - to handle the fact that some people want to call a method
     *  on T, as in:
     *
     *      Sequence<String> x;
     *      size_t a = x[3].length ();      // wont work if we use aggregating variant of TemporaryElementReference_
     *                                      // e.g: error: �struct Stroika::Foundation::Containers::Sequence<Stroika::Foundation::Characters::String>::TemporaryElementReference_� has no member named �Trim�
     */
    template <typename T>
    struct Sequence<T>::TemporaryElementReference_ : conditional_t<is_class_v<T> or is_union_v<T>, T, Common::Empty> {
    private:
        static constexpr bool                        kSubClass_ = is_class_v<T> or is_union_v<T>;
        Sequence<T>*                                 fV;
        size_t                                       fIndex_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<kSubClass_, Common::Empty, T> fValue_;

    public:
        TemporaryElementReference_ (const TemporaryElementReference_&) = default;
        TemporaryElementReference_ (TemporaryElementReference_&& src)
            : fV{move (src.fV)}
            , fIndex_{src.fIndex_}
            , fValue_{move (src.fValue_)}
        {
            src.fV = nullptr; // it no longer writes on its DTOR
        }
        TemporaryElementReference_ (Sequence<T>* s, size_t i)
            : fV{(RequireExpression (s != nullptr), s)}
            , fIndex_{i}
        {
            if constexpr (kSubClass_) {
                *static_cast<T*> (this) = s->GetAt (i);
            }
            else {
                fValue_ = s->GetAt (i);
            }
        }
        TemporaryElementReference_& operator= (const TemporaryElementReference_&) = delete;
        TemporaryElementReference_& operator= (TemporaryElementReference_&&)      = delete;
        TemporaryElementReference_& operator= (ArgByValueType<T> v)
        {
            RequireNotNull (fV);
            if constexpr (kSubClass_) {
                *static_cast<T*> (this) = v;
            }
            else {
                fValue_ = v;
            }
            return *this;
        }
        operator T () const
            requires (not(kSubClass_))
        {
            RequireNotNull (fV);
            if constexpr (kSubClass_) {
                return *static_cast<T*> (this);
            }
            else {
                return fValue_;
            }
        }
        operator T&()
            requires (not(kSubClass_))
        {
            RequireNotNull (fV);
            if constexpr (kSubClass_) {
                return *static_cast<T*> (this);
            }
            else {
                return fValue_;
            }
        }
        // Tried this for https://stroika.atlassian.net/browse/STK-1024 - but didn't help
        // and dont much like anyhow
        //auto ToString () const
        //{
        //    RequireNotNull (fV);
        //    if constexpr (kSubClass_) {
        //        return Characters::UnoverloadedToString (*static_cast<T*> (this));
        //    }
        //    else {
        //        return Characters::UnoverloadedToString (fValue_);
        //    }
        //}
        ~TemporaryElementReference_ ()
        {
            // now remaining problem with this strategy is that if we have
            // String a = sequence[i] = the temporary may get MOVE()d to 'a', and so *this is now invalid, and cannot be used in a set.
            // We don't need to set in that case, but we have no way to reliably tell that we got moved.

            // needed cuz modifications CAN come from from something like Sequence<String> x; x[1].clear ();
            if (fV != nullptr) {
                if constexpr (kSubClass_) {
                    IgnoreExceptionsForCall (fV->SetAt (fIndex_, *((T*)this)));
                }
                else {
                    IgnoreExceptionsForCall (fV->SetAt (fIndex_, fValue_));
                }
            }
        }
    };

    /*
     ********************************************************************************
     ******** Sequence<T>::_IRep::IndexBasedRandomAccessIteratorRep_ ***************
     ********************************************************************************
     */
    /*
     *  Generic RandomAccessIterator<T>::IRep, implemented purely in terms of _IRep::GetAt ()/size () -
     *  so it works for ANY Sequence<T> backend, but at the cost of a GetAt () call per step (which could
     *  be O(n) for a backend like Sequence_LinkedList).
     */
    template <typename T>
    class Sequence<T>::_IRep::IndexBasedRandomAccessIteratorRep_ : public RandomAccessIterator<T>::IRep {
    public:
        IndexBasedRandomAccessIteratorRep_ (const _IRep* rep, size_t idx)
            : fRep_{rep}
            , fIdx_{idx}
        {
            RequireNotNull (rep);
            Require (idx <= rep->size ());
        }
        IndexBasedRandomAccessIteratorRep_ (const IndexBasedRandomAccessIteratorRep_&) = default;

    public:
        virtual unique_ptr<typename Iterator<T>::IRep> Clone () const override
        {
            return make_unique<IndexBasedRandomAccessIteratorRep_> (*this);
        }
        virtual bool AtEnd () const override
        {
            return fIdx_ >= fRep_->size ();
        }
        virtual optional<T> Current () const override
        {
            if (fIdx_ >= fRep_->size ()) {
                return nullopt;
            }
            return fRep_->GetAt (fIdx_);
        }
        virtual optional<T> More () override
        {
            Require (not AtEnd ());
            ++fIdx_;
            return Current ();
        }
        virtual bool Equals (const typename Iterator<T>::IRep* rhs) const override
        {
            RequireNotNull (rhs);
            const auto* r = Debug::UncheckedDynamicCast<const IndexBasedRandomAccessIteratorRep_*> (rhs);
            return fIdx_ == r->fIdx_;
        }

    public:
        // BidirectionalIterator<T>::IRep
        virtual bool AtStart () const override
        {
            return fIdx_ == 0;
        }
        virtual T Back () override
        {
            Require (not AtStart ());
            --fIdx_;
            return fRep_->GetAt (fIdx_);
        }

    public:
        // RandomAccessIterator<T>::IRep
        virtual void Advance (ptrdiff_t i) override
        {
            Require (i >= 0 or static_cast<size_t> (-i) <= fIdx_);
            fIdx_ = static_cast<size_t> (static_cast<ptrdiff_t> (fIdx_) + i);
        }
        virtual ptrdiff_t Difference (const typename RandomAccessIterator<T>::IRep* rhs) const override
        {
            if (rhs == nullptr) {
                return static_cast<ptrdiff_t> (fIdx_) - static_cast<ptrdiff_t> (fRep_->size ());
            }
            const auto* r = Debug::UncheckedDynamicCast<const IndexBasedRandomAccessIteratorRep_*> (rhs);
            return static_cast<ptrdiff_t> (fIdx_) - static_cast<ptrdiff_t> (r->fIdx_);
        }
        virtual const T* PeekAtElement (ptrdiff_t i) const override
        {
            fPeekCache_ = fRep_->GetAt (static_cast<size_t> (static_cast<ptrdiff_t> (fIdx_) + i));
            return &*fPeekCache_;
        }

    private:
        const _IRep*        fRep_;
        size_t              fIdx_;
        mutable optional<T> fPeekCache_;
    };

    /*
     ********************************************************************************
     **************************** Sequence<T>::_IRep ********************************
     ********************************************************************************
     */
    template <typename T>
    BidirectionalIterator<T> Sequence<T>::_IRep::_MakeBidirectionalIterator_ViaGetAt () const
    {
        return BidirectionalIterator<T>{make_unique<IndexBasedRandomAccessIteratorRep_> (this, 0)};
    }
    template <typename T>
    RandomAccessIterator<T> Sequence<T>::_IRep::_MakeRandomAccessIterator_ViaGetAt () const
    {
        return RandomAccessIterator<T>{make_unique<IndexBasedRandomAccessIteratorRep_> (this, 0)};
    }

    /*
     ********************************************************************************
     ******************************** Sequence<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence<T>::Sequence ()
        : inherited{Factory::Sequence_Factory<T>::Default () ()}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Sequence<T>::Sequence (const initializer_list<value_type>& src)
        : Sequence{}
    {
        AppendAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence<T>>)
    inline Sequence<T>::Sequence (ITERABLE_OF_ADDABLE&& src)
        : Sequence{}
    {
        AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    inline Sequence<T>::Sequence (const shared_ptr<_IRep>& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), rep)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Sequence<T>::Sequence (shared_ptr<_IRep>&& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
    inline Sequence<T>::Sequence (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
        : Sequence{}
    {
        AppendAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE2> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline BidirectionalIterator<T> Sequence<T>::MakeBidirectionalIterator () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetBidirectionalIterator ();
    }
    template <typename T>
    inline RandomAccessIterator<T> Sequence<T>::MakeRandomAccessIterator () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetRandomAccessIterator ();
    }
    template <typename T>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    nonvirtual RESULT_CONTAINER Sequence<T>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, Sequence>) {
            // clone the rep so we retain the rep type
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited ::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper));
        }
    }
    template <typename T>
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
    inline RESULT_CONTAINER Sequence<T>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        if constexpr (same_as<RESULT_CONTAINER, Sequence>) {
            // clone the rep so we retain the rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue));
        }
    }
    template <typename T>
    template <IPotentiallyComparer<T> INORDER_COMPARER_TYPE>
    auto Sequence<T>::OrderBy (INORDER_COMPARER_TYPE&& inorderComparer, [[maybe_unused]] Execution::SequencePolicy seq) const -> Sequence
    {
        /*
         *  The copy into a vector<T> is unavoidable, not merely unoptimized. Sorting has to relocate elements,
         *  so it must be able to WRITE through the iterators: std::stable_sort requires them to be
         *  Cpp17ValueSwappable with a MoveAssignable value type (the equivalent for std::ranges::stable_sort
         *  is the std::sortable / std::permutable concept). Stroika's iterators are deliberately read-only -
         *  Iterator<T>::operator* and RandomAccessIterator<T>::operator[] both hand back a const T& - because
         *  exposing T& would break the copy-on-write sharing (see the note on Sequence<T>::operator[]).
         *
         *  So this is about the CONSTNESS of the iterators, not their category: giving Sequence better (eg
         *  random-access) iterators does not help, and an earlier @todo here claiming otherwise was wrong.
         *
         *  The copy is cheap-ish in the end, because Sequence_stdvector adopts the vector by move.
         */
        vector<T> tmp = this->As<vector<T>> ();
#if __cpp_lib_execution >= 201603L
        if (seq == Execution::SequencePolicy::eSeq) {
            stable_sort (tmp.begin (), tmp.end (), inorderComparer);
        }
        else {
            stable_sort (std::execution::par, tmp.begin (), tmp.end (), inorderComparer);
        }
#else
        stable_sort (tmp.begin (), tmp.end (), inorderComparer);
#endif
        return Concrete::Sequence_stdvector<T>{move (tmp)};
    }
    template <typename T>
    inline void Sequence<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> accessor{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not accessor._ConstGetRep ().empty ()) {
            this->_fRep = accessor._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T>
    template <predicate<T> PREDICATE>
    size_t Sequence<T>::RemoveAll (PREDICATE&& p)
    {
        // @todo Consider migrating this method to _IRep? Doing so would allow for different (e.g. vector) implementations
        // to be more efficient (for example, bubbling last to first); but at a small code-bloat cost, so not likely
        // worthwhile tradeoff; if this is a performance issue, convert to Sequence_LinkedList{s}.RemoveAll(p) and then convert
        // back to whatever backend-implementation sequence you wish... --LGP 2022-12-14
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
    inline auto Sequence<T>::GetAt (size_t i) const -> value_type
    {
        _SafeReadRepAccessor<_IRep> accessor{this};
        Require (i < accessor._ConstGetRep ().size ());
        return accessor._ConstGetRep ().GetAt (i);
    }
    template <typename T>
    inline void Sequence<T>::SetAt (size_t i, ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep> accessor{this};
        Require (i < accessor._ConstGetRep ().size ());
        accessor._GetWriteableRep ().SetAt (i, item);
    }
    template <typename T>
    inline auto Sequence<T>::operator[] (size_t i) const -> const value_type
    {
        _SafeReadRepAccessor<_IRep> accessor{this};
        Require (i < accessor._ConstGetRep ().size ());
        return accessor._ConstGetRep ().GetAt (i);
    }
    template <typename T>
    inline auto Sequence<T>::operator() (size_t i) -> TemporaryElementReference_
    {
        return TemporaryElementReference_{this, i};
    }
    template <typename T>
    template <Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline optional<size_t> Sequence<T>::IndexOf (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer) const
    {
        /*
         *  FAST PATH - an index is exactly what a contiguous buffer yields for free (pointer difference),
         *  where the general path below asks Find () to carry a counting side effect in its predicate
         *  (see Private::IndexOf_ ()) - so it pays type erasure into std::function, per-element virtual
         *  iteration, and an Iterator<T> construction, all to produce a number.
         *
         *  Deliberately duplicated rather than shared with Iterable<T>::Contains (): a common helper
         *  would have to distinguish "backend has no contiguous storage" from "searched and not found",
         *  ie return a nested optional, which reads far worse than these few lines do twice.
         *
         *  The index is the FIRST match in iteration order, as this method requires - overriders of
         *  PeekContiguousStorage () must hand back storage in iteration order, which is the same
         *  precondition As<> () and SequentialEquals () rely on.
         */
        {
            _SafeReadRepAccessor<_IRep> accessor{this};
            if (auto s = accessor._ConstGetRep ().PeekContiguousStorage ()) {
                auto i = [&] () {
                    if constexpr (same_as<remove_cvref_t<EQUALS_COMPARER>, equal_to<T>> or same_as<remove_cvref_t<EQUALS_COMPARER>, equal_to<>>) {
                        return std::find (s->begin (), s->end (), item);
                    }
                    else {
                        return std::find_if (s->begin (), s->end (), [&] (const T& e) { return equalsComparer (e, item); });
                    }
                }();
                if (i == s->end ()) {
                    return optional<size_t>{};
                }
                return static_cast<size_t> (i - s->begin ());
            }
        }
        return Private::IndexOf_<T, EQUALS_COMPARER> (*this, item, forward<EQUALS_COMPARER> (equalsComparer));
    }
    template <typename T>
    template <Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline optional<size_t> Sequence<T>::IndexOf (const Sequence& s, EQUALS_COMPARER&& equalsComparer) const
    {
        return Private::IndexOf_<T, EQUALS_COMPARER> (*this, s, forward<EQUALS_COMPARER> (equalsComparer));
    }
    template <typename T>
    template <typename IGNORED>
    inline size_t Sequence<T>::IndexOf (const Iterator<value_type>& i) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().IndexOf (i);
    }
    template <typename T>
    inline void Sequence<T>::Insert (size_t i, ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep> accessor{this};
        Require (i <= accessor._ConstGetRep ().size ());
        return accessor._GetWriteableRep ().Insert (i, span{&item, 1u});
    }
    template <typename T>
    inline void Sequence<T>::Insert (const Iterator<value_type>& i, ArgByValueType<value_type> item)
    {
        // an AtEnd () iterator means append. Note this must be checked BEFORE calling _IRep::IndexOf (), because the
        // end () sentinel (Iterator{default_sentinel}) has a nullptr rep, and IndexOf () downcasts i.ConstGetRep ()
        if (i.AtEnd ()) {
            Append (item);
            return;
        }
        _SafeReadWriteRepAccessor<_IRep> accessor{this};
        size_t                           idx = accessor._ConstGetRep ().IndexOf (i);
        Require (idx <= accessor._ConstGetRep ().size ());
        return accessor._GetWriteableRep ().Insert (idx, span{&item, 1u});
    }
    template <typename T>
    inline void Sequence<T>::insert (const Iterator<value_type>& i, ArgByValueType<value_type> item)
    {
        Insert (i, item);
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<ITERATOR_OF_ADDABLE> ITERATOR_OF_ADDABLE2>
    void Sequence<T>::InsertAll (size_t i, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
    {
        Require (i <= this->size ());
        size_t insertAt = i;
        for (auto ii = forward<ITERATOR_OF_ADDABLE> (start); ii != forward<ITERATOR_OF_ADDABLE2> (end); ++ii) {
            Insert (insertAt++, *ii);
        }
    }
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
    inline void Sequence<T>::InsertAll (size_t i, ITERABLE_OF_ADDABLE&& s)
    {
        Require (i <= this->size ());
        InsertAll (i, s.begin (), s.end ());
    }
    template <typename T>
    inline void Sequence<T>::Prepend (ArgByValueType<value_type> item)
    {
        Insert (0, item);
    }
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
    inline void Sequence<T>::PrependAll (ITERABLE_OF_ADDABLE&& s)
    {
        InsertAll (0, forward<ITERABLE_OF_ADDABLE> (s));
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline void Sequence<T>::PrependAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        InsertAll (0, forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
    }
    template <typename T>
    inline void Sequence<T>::Append (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Insert (_IRep::_kSentinelLastItemIndex, span{&item, 1});
    }
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
    inline void Sequence<T>::AppendAll (ITERABLE_OF_ADDABLE&& s)
    {
        AppendAll (s.begin (), s.end ());
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
    inline void Sequence<T>::AppendAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
    {
        _SafeReadWriteRepAccessor<_IRep> accessor = {this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE2> (end); ++i) {
            const T& tmp = *i;
            accessor._GetWriteableRep ().Insert (_IRep::_kSentinelLastItemIndex, span{&tmp, 1u});
        }
    }
    template <typename T>
    inline void Sequence<T>::Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI)
    {
        Require (not i.AtEnd ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Update (patchedIterator, newValue, nextI);
    }
    template <typename T>
    inline void Sequence<T>::Remove (size_t i)
    {
        Require (i < this->size ());
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i, i + 1);
    }
    template <typename T>
    inline void Sequence<T>::Remove (size_t start, size_t end)
    {
        Require (start <= end and end <= this->size ());
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (start, end);
    }
    template <typename T>
    inline void Sequence<T>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.AtEnd ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline void Sequence<T>::As (CONTAINER_OF_ADDABLE* into) const
    {
        RequireNotNull (into);
        *into = this->template As<CONTAINER_OF_ADDABLE> (); // ie Iterable<T>::As, so this gets its fast paths too
    }
    template <typename T>
    inline auto Sequence<T>::First () const -> optional<value_type>
    {
        return this->empty () ? optional<T>{} : GetAt (0);
    }
    template <typename T>
    inline auto Sequence<T>::First (const function<bool (ArgByValueType<value_type>)>& that) const -> optional<value_type>
    {
        return inherited::First (that);
    }
    template <typename T>
    inline auto Sequence<T>::FirstValue (ArgByValueType<value_type> defaultValue) const -> value_type
    {
        return this->empty () ? defaultValue : GetAt (0);
    }
    template <typename T>
    inline auto Sequence<T>::Last () const -> optional<value_type>
    {
        // IRep::GetAt() defined to allow special _IRep::_kSentinelLastItemIndex
        return this->empty () ? optional<T>{} : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (_IRep::_kSentinelLastItemIndex);
    }
    template <typename T>
    inline auto Sequence<T>::Last (const function<bool (ArgByValueType<value_type>)>& that) const -> optional<value_type>
    {
        // @todo when we have reverse iterators - we could implement this more efficiently by walking the sequence backwards
        return inherited::Last (that);
    }
    template <typename T>
    inline auto Sequence<T>::LastValue (ArgByValueType<value_type> defaultValue) const -> value_type
    {
        // IRep::GetAt() defined to allow special _IRep::_kSentinelLastItemIndex
        return this->empty () ? defaultValue : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (_IRep::_kSentinelLastItemIndex);
    }
    template <typename T>
    inline void Sequence<T>::push_back (ArgByValueType<value_type> item)
    {
        Append (item);
    }
    template <typename T>
    inline auto Sequence<T>::back () const -> value_type
    {
        return *Last ();
    }
    template <typename T>
    inline auto Sequence<T>::front () const -> value_type
    {
        return *First ();
    }
    template <typename T>
    inline void Sequence<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    inline void Sequence<T>::erase (size_t i)
    {
        this->Remove (i);
    }
    template <typename T>
    inline auto Sequence<T>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<T> nextI{nullptr};
        this->Remove (i, &nextI);
        return nextI;
    }
    template <typename T>
    inline auto Sequence<T>::operator+= (ArgByValueType<value_type> item) -> Sequence&
    {
        Append (item);
        return *this;
    }
    template <typename T>
    inline auto Sequence<T>::operator+= (const Sequence& items) -> Sequence&
    {
        AppendAll (items);
        return *this;
    }
    template <typename T>
    auto Sequence<T>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.AtEnd ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type* writableRep = this->_fRep.rwget ([&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
            return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
        });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), move (patchedIterator));
    }
    template <typename T>
    inline void Sequence<T>::_AssertRepValidType () const
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T>
    inline bool Sequence<T>::operator== (const Sequence& rhs) const
        requires (equality_comparable<T>)
    {
        return EqualsComparer<>{}(*this, rhs);
    }
    template <typename T>
    inline auto Sequence<T>::operator<=> (const Sequence& rhs) const
        requires (three_way_comparable<T>)
    {
        return ThreeWayComparer<>{}(*this, rhs);
    }

    /*
     ********************************************************************************
     ********************************* operator+ ************************************
     ********************************************************************************
     */
    template <typename T>
    Sequence<T> operator+ (const Iterable<T>& lhs, const Sequence<T>& rhs)
    {
        Sequence<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Sequence<T> operator+ (const Sequence<T>& lhs, const Iterable<T>& rhs)
    {
        Sequence<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Sequence<T> operator+ (const Sequence<T>& lhs, const Sequence<T>& rhs)
    {
        Sequence<T> result{lhs};
        result += rhs;
        return result;
    }

}
