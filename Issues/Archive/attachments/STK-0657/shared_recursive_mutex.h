#if 0
            namespace PrivateFiddling_ {
                // See https://stackoverflow.com/questions/36619715/a-shared-recursive-mutex-in-standard-c
                class shared_recursive_mutex {
                private:
                    shared_mutex fM_;

                public:
                    /**
                     *  Like a regular shared mutex, except that we allow:
                     *      o   lock () when we already own the lock exclusively (we do NOT support upgrading the lock from shared to exclusive - for now undetected error)
                     */
                    void lock ()
                    {
                        std::thread::id this_id = std::this_thread::get_id ();
                        if (fExclusivelyLockedOwner_ == this_id) {
                            // recursive locking
                            Assert (fExclusivelyLockedByOwningThreadCount_ > 0);
                            fExclusivelyLockedByOwningThreadCount_++;
                        }
                        else {
                            // @todo - handle case where there is existing shared_lock by this thread (already handled other case where owned by other thread - normal)

                            // normal locking
                            fM_.lock ();
                            fExclusivelyLockedOwner_               = this_id;
                            fExclusivelyLockedByOwningThreadCount_ = 1;
                        }
                    }
                    void unlock ()
                    {
                        if (fExclusivelyLockedByOwningThreadCount_ > 1) {
                            Require (this_thread::get_id () == fExclusivelyLockedOwner_);
                            // recursive unlocking - OK to access these variables freely because we still hold the lock
                            fExclusivelyLockedByOwningThreadCount_--;
                            Assert (fExclusivelyLockedByOwningThreadCount_ > 0);
                        }
                        else {
                            // normal unlocking
                            fExclusivelyLockedOwner_               = std::thread::id{};
                            fExclusivelyLockedByOwningThreadCount_ = 0;
                            if (fSharedLockCount_ > 0) {
                                // todo
                            }
                            fM_.unlock ();
                        }
                    }

                    void lock_shared ()
                    {
                        thread::id this_id = this_thread::get_id ();
                        if (fExclusivelyLockedOwner_ == this_id) {
                            // allow lock_shared() while we hold exclusive lock, because nobody else can mess with us then
                            fSharedLockedOwner_ = this_id;
                            fSharedLockCount_++;
                        }
                        else if (fSharedLockedOwner_ == this_id) {
                            // If we already have a shared_lock
                            fSharedLockedOwner_ = this_id;
                            fSharedLockCount_++;
                        }
                        else {
                            fM_.lock_shared ();
                        }

                        unique_lock<mutex> critSec{fSharedLockCountsMutex_};
                        if (fSharedLockCounts_.count (this_id)) {
                            ++(fSharedLockCounts_[this_id]);
                        }
                        else {
                            critSec.unlock ();	// unlock/relock to avoid deadlock
                            fM_.lock_shared ();	// @todo - not legal to lock()/lock_shared at same time so check/avoid here
                            critSec.lock ();
                            fSharedLockCounts_[this_id] = 1;
                        }
                    }
                    void unlock_shared ()
                    {
                        thread::id this_id = this_thread::get_id ();
                        #if 0
                        if (fSharedLockCount_ > 0 and this_id == fSharedLockedOwner_) {
                            fSharedLockCount_--;
                            if (fSharedLockCount_ == 0) {
                                fSharedLockedOwner_ = thread::id{};
                            }
                        }
                        else {
                            fM_.unlock_shared ();
                        }
                        #endif
                        unique_lock<mutex> critSec{fSharedLockCountsMutex_};
                        if (fSharedLockCounts_.count (this_id) == 1) {
                            fSharedLockCounts_.erase (this_id);
                            fM_.unlock ();
                        }
                        else {
                            --(fSharedLockCounts_[this_id]);
                        }

                    }

                private:
                    /*
                     *  Use atomic so it can be read / compared without first acquiring (some other) lock
                     */
                    atomic<thread::id> fExclusivelyLockedOwner_;
                    /*
                     *  ONLY safe to read/write this value if this_thread::get_id () == fExclusivelyLockedOwner_. Use atomic<> so values flushed from
                     *  thread to thread.
                     */
                    atomic<int>             fExclusivelyLockedByOwningThreadCount_{};
                    atomic<thread::id>      fSharedLockedOwner_;
                    atomic<int>             fSharedLockCount_{};
                    mutex                   fSharedLockCountsMutex_;
                    map<thread::id, size_t> fSharedLockCounts_;
                };
            }
#endif


struct track_state<MUTEX> {
private:
 MUTEX m;
 atomic<int > exclusiveLockCount{};	// right within thread, but not across threads
 atomic<int > sharedLockCount{};

public:
 void lock ()
  {
     m.lock ();
   	++ exclusiveLockCount;
  }  
  void unlock ()
  {
   	-- exclusiveLockCount;	// update while lock held
     m.unlock ();
  } 
  
  void shared_lock ()
  {
     m.shared_lock ();
   	++ sharedLockCount;
  }  
  void shared_unlock ()
  {
   	-- sharedLockCount;	// update while lock held
     m.shared_unlock ();
  } 
};

struct recursive_lock {
};
