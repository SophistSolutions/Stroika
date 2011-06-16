	////PRIVATE IMPL DETAIL - DONT INCLUDE DIRECTLY


#ifndef	qTargetPlatformSDKUseswchar_t
	#if		(defined(_UNICODE) || defined(UNICODE))
		#define	qTargetPlatformSDKUseswchar_t	1
	#else
		#define	qTargetPlatformSDKUseswchar_t	0
	#endif
#endif

	#if		qTargetPlatformSDKUseswchar_t
		#if !defined(_UNICODE) || !defined(UNICODE)
#error "INCONSITENT VALS"
		#endif
	#else
		#if defined(_UNICODE) || defined(UNICODE)
#error "INCONSITENT VALS"
		#endif
	#endif
