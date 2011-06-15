/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_LRUCache_h_
#define	_Stroika_Foundation_Containers_LRUCache_h_	1

#include	"../StroikaPreComp.h"

#include	<tchar.h>
#include	<map>

#include	"../Configuration/Basics.h"
#include	"../Debug/Assertions.h"
#include	"../Time/Realtime.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING


namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {

/*
@CONFIGVAR:		qKeepTimedCacheStats
@DESCRIPTION:	<p>Defines whether or not we capture statistics (for debugging purposes) in @'TimedCache<KEY,RESULT>'.
			This should be ON by default - iff @'qDebug' is true.</p>
 */
#ifndef	qKeepTimedCacheStats
#define	qKeepTimedCacheStats		defined (_DEBUG)
#endif


	// Keeps track of all items - indexed by Key - but throws away items which are any more
	// stale than given by the TIMEOUT
	//
	// The only constraint on KEY is that it must have an operator== and operator<, and the only constraint on
	// both KEY and RESULT is that they must be copyable objects.
	//
	// Note - this class doesn't employ a thread to throw away old items, so if you count on that
	// happening (e.g. because the RESULT object DTOR has a side-effect like closing a file), then
	// you may call DoBookkeeping () peridocially.
	template	<typename	KEY, typename RESULT>
		class	TimedCache {
			public:
				TimedCache (bool accessFreshensDate, Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds);

				nonvirtual	void	SetTimeout (Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds);

				nonvirtual	bool	AccessElement (const KEY& key, RESULT* result);
				nonvirtual	void	AddElement (const KEY& key, const RESULT& result);

				nonvirtual	void	DoBookkeeping ();	// optional - need not be called

			private:
				bool	fAccessFreshensDate;
				Stroika::Foundation::Time::DurationSecondsType	fTimeout;
				Stroika::Foundation::Time::DurationSecondsType	fNextAutoClearAt;
			
			private:
				nonvirtual	void	ClearIfNeeded_ ();
				nonvirtual	void	ClearOld_ ();

			private:
				struct	MyResult {
					MyResult (const RESULT& r):
						fResult (r),
						fLastAccessedAt (Time::GetTickCount ())
						{
						}
					RESULT	fResult;
					Stroika::Foundation::Time::DurationSecondsType	fLastAccessedAt;
				};
				map<KEY,MyResult>	fMap;

			#if		qKeepTimedCacheStats
			public:
				void	DbgTraceStats (const TCHAR* label) const;
			public:
				size_t		fCachedCollected_Hits;
				size_t		fCachedCollected_Misses;
			#endif
		};

		}
	}
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


	template	<typename	KEY, typename RESULT>
		TimedCache<KEY,RESULT>::TimedCache (bool accessFreshensDate, Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds):
			fMap (),
			fAccessFreshensDate (accessFreshensDate),
			fTimeout (timeoutInSeconds),
			fNextAutoClearAt (Time::GetTickCount () + timeoutInSeconds)
			#if		qKeepTimedCacheStats
				,fCachedCollected_Hits (0)
				,fCachedCollected_Misses (0)
			#endif
			{
				Require (fTimeout > 0.0f);
			}
	template	<typename	KEY, typename RESULT>
		void	TimedCache<KEY,RESULT>::SetTimeout (Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds)
			{
				Require (timeoutInSeconds > 0.0f);
				if (fTimeout != timeoutInSeconds) {
					ClearIfNeeded_ ();
					fTimeout = timeoutInSeconds;
					ClearIfNeeded_ ();
				}
			}
	template	<typename	KEY, typename RESULT>
		bool	TimedCache<KEY,RESULT>::AccessElement (const KEY& key, RESULT* result)
			{
				ClearIfNeeded_ ();
				map<KEY,MyResult>::iterator i = fMap.find (key);
				if (i == fMap.end ()) {
					#if		qKeepTimedCacheStats
						fCachedCollected_Misses++;
					#endif
					return false;
				}
				else {
					if (fAccessFreshensDate) {
						i->second.fLastAccessedAt = Time::GetTickCount ();
					}
					if (result != NULL) {
						*result = i->second.fResult;
					}
					#if		qKeepTimedCacheStats
						fCachedCollected_Hits++;
					#endif
					return true;
				}
			}
	template	<typename	KEY, typename RESULT>
		void	TimedCache<KEY,RESULT>::AddElement (const KEY& key, const RESULT& result)
			{
				ClearIfNeeded_ ();
				map<KEY,MyResult>::iterator i = fMap.find (key);
				if (i == fMap.end ()) {
					fMap.insert (map<KEY,MyResult>::value_type (key, MyResult (result)));
				}
				else {
					i->second = MyResult (result);	// overwrite if its already there
				}
			}
	template	<typename	KEY, typename RESULT>
		inline	void	TimedCache<KEY,RESULT>::DoBookkeeping ()
			{
				ClearOld_ ();
			}
	template	<typename	KEY, typename RESULT>
		inline	void	TimedCache<KEY,RESULT>::ClearIfNeeded_ ()
			{
				if (fNextAutoClearAt < Time::GetTickCount ()) {
					ClearOld_ ();
				}
			}
	template	<typename	KEY, typename RESULT>
		void	TimedCache<KEY,RESULT>::ClearOld_ ()
			{
				Stroika::Foundation::Time::DurationSecondsType	now	=	Time::GetTickCount ();
				fNextAutoClearAt = now + fTimeout/2.0f;	// somewhat arbitrary how far into the future we do this...
				Stroika::Foundation::Time::DurationSecondsType	lastAccessThreshold	=	now - fTimeout;
				for (map<KEY,MyResult>::iterator i = fMap.begin (); i != fMap.end (); ) {
					if (i->second.fLastAccessedAt < lastAccessThreshold) {
						i = fMap.erase (i);
					}
					else {
						++i;
					}
				}
			}
	#if		qKeepTimedCacheStats
	template	<typename	KEY, typename RESULT>
		void	TimedCache<KEY,RESULT>::DbgTraceStats (const TCHAR* label) const
			{
				size_t	total	=	fCachedCollected_Hits + fCachedCollected_Misses;
				if (total == 0) {
					total = 1;	// avoid divide by zero
				}
				DbgTrace (_T ("%s stats: hits=%d, misses=%d, hit%% %f."), label, fCachedCollected_Hits, fCachedCollected_Misses, float (fCachedCollected_Hits)/(float (total)));
			}
	#endif


		}
	}
}



#endif	/*__TimedCache_h__*/
