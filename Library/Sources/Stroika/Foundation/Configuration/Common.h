/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Common_h_
#define	_Stroika_Foundation_Configuration_Common_h_	1

#include	<cstddef>
#include	<cstdint>
#include	<ciso646>


namespace	Stroika {
	namespace	Foundation {

		using	namespace	std;

		namespace	Configuration {

			typedef	unsigned char	Byte;

			#define	nonvirtual

			#define	NEltsOf(X)		(sizeof((X))/sizeof((X)[0]))
			#define	StartOfArray(X)	(&(X)[0])
			#define	EndOfArray(X)	(&(X)[NEltsOf(X)])

			template	<typename	ENUM>
				ENUM	Inc (ENUM e);


			/*
			 * NB: we can lose these macros once all our compilers support the new C++ syntax.
			 */
			#if		qCompilerAndStdLib_Supports_CTORDELETE
				#define	NO_DEFAULT_CONSTRUCTOR(NAME)\
					NAME () = delete;
				#define	NO_COPY_CONSTRUCTOR(NAME)\
					NAME (const NAME&) = delete;
				#define	NO_COPY_CONSTRUCTOR2(NAME,NAMEWITHT)\
					NAME (const NAMEWITHT&) = delete;
				#define	NO_ASSIGNMENT_OPERATOR(NAME)\
					const NAME& operator= (const NAME&) = delete;
			#else
				#define	NO_DEFAULT_CONSTRUCTOR(NAME)\
					private:	NAME ();
				#define	NO_COPY_CONSTRUCTOR(NAME)\
					private:	NAME (const NAME&);
				#define	NO_COPY_CONSTRUCTOR2(NAME,NAMEWITHT)\
					private:	NAME (const NAMEWITHT&);
				#define	NO_ASSIGNMENT_OPERATOR(NAME)\
					private:	const NAME& operator= (const NAME&);
			#endif



		}

		using	Configuration::Byte;
	}
}


#endif	/*_Stroika_Foundation_Configuration_Common_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Common.inl"


