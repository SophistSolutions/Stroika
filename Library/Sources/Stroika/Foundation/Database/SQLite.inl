
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQLite_inl_
#define _Stroika_Foundation_Database_SQLite_inl_ 1

namespace Stroika::Foundation::Database::SQLite {

#if qHasFeature_sqlite
    /*
     ********************************************************************************
     ****************************** SQLite::CompiledOptions *************************
     ********************************************************************************
     */
    inline constexpr CompiledOptions CompiledOptions::kThe
    {
#if __cpp_designated_initializers
#ifdef SQLITE_ENABLE_NORMALIZE
        .ENABLE_NORMALIZE = true
#else
        .ENABLE_NORMALIZE = false
#endif
#ifdef SQLITE_THREADSAFE
            ,
        .THREADSAFE = SQLITE_THREADSAFE
#elif defined(THREADSAFE)
            ,
        .THREADSAFE = THREADSAFE
#else
        , .THREADSAFE = 1
#endif
#else
#ifdef SQLITE_ENABLE_NORMALIZE
        , SQLITE_ENABLE_NORMALIZE
#else
        false
#endif
#ifdef SQLITE_THREADSAFE
            ,
            SQLITE_THREADSAFE
#elif defined(THREADSAFE)
            ,
            THREADSAFE
#else
        , 1
#endif
#endif
    };

    /*
     ********************************************************************************
     ****************************** SQLite::Connection::Ptr *************************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const Ptr& src)
        : Ptr{src.fRep_}
    {
    }
    inline Connection::Ptr& Connection::Ptr::operator= (const Ptr& src)
    {
        if (this != &src) {
            fRep_ = src.fRep_;
#if qDebug
            if (fRep_) {
                _fSharedContext = fRep_->_fSharedContext;
            }
#endif
        }
        return *this;
    }
    inline Connection::Ptr& Connection::Ptr::operator= (Ptr&& src) noexcept
    {
        if (this != &src) {
            fRep_ = move (src.fRep_);
#if qDebug
            if (fRep_) {
                _fSharedContext = fRep_->_fSharedContext;
            }
#endif
        }
        return *this;
    }
    inline Connection::IRep* Connection::Ptr::operator-> () const noexcept
    {
        return fRep_.get ();
    }
    inline auto Connection::Ptr::operator== (const Ptr& rhs) const
    {
        return fRep_ == rhs.fRep_;
    }
    inline bool Connection::Ptr::operator== (nullptr_t) const noexcept
    {
        return fRep_.get () == nullptr;
    }
#if __cpp_impl_three_way_comparison < 201907
    inline bool Connection::Ptr::operator!= (const Ptr& rhs) const
    {
        return fRep_ == rhs.fRep_;
    }
    inline bool Connection::Ptr::operator!= (nullptr_t) const
    {
        return fRep_ != nullptr;
    }
#endif
    inline void Connection::Ptr::Exec (const String& sql) const
    {
        fRep_->Exec (sql);
    }
    inline ::sqlite3* Connection::Ptr::Peek () const
    {
        return fRep_->Peek ();
    }

    /*
     ********************************************************************************
     ****************************** SQLite::Statement *******************************
     ********************************************************************************
     */
    inline auto Statement::GetColumns () const -> Sequence<ColumnDescription>
    {
        return Sequence<ColumnDescription>{fColumns_};
    }
    inline auto Statement::GetParameters () const -> Sequence<ParameterDescription>
    {
        return fParameters_;
    }
#endif

}

#endif /*_Stroika_Foundation_Database_SQLite_inl_*/
