/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SmallStackBuffer_h_
#define	_Stroika_Foundation_Memory_SmallStackBuffer_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"



namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {


		/*
		@DESCRIPTION:	<p>Often times in Led I must copy a bunch of characters to operate on them as an array
			(say when I'm computing word wrap info, for example). But the number of characters can vary,
			depending on the particular text you've entered. But it ALMOST ALWAYS would fit nicely in a
			little stack buffer. But you cannot GAURANTEE that is safe.</p>
				<p>In steps <code>SmallStackBuffer&ltT&gt</code>. Just declare one of these, and it
			automaticlly uses the stack for the buffer if it will fit, and silently allocates heap memory
			and returns it to the free store on stack based destruction if needed. Use it just like you
			would a stack-based array. You don't need to know about this to use any of Led's APIs, but its
			a helpful class you may want to use elsewhere in your code.</p>

			// typically sizeof(SmallStackBuffer<T,BUF_SIZE>) will come to roughly 4K, and always at least something.

			NB: this code is only safe for POD-types - not for types with constructors etc.
		*/
		template	<typename	T, size_t BUF_SIZE = ((4096/sizeof(T)) == 0? 1 : (4096/sizeof(T)))>
			class	SmallStackBuffer {
				public:
					SmallStackBuffer (size_t nElements);
					SmallStackBuffer (const SmallStackBuffer<T,BUF_SIZE>& from);
					~SmallStackBuffer ();

				public:
					SmallStackBuffer<T,BUF_SIZE>&	operator= (const SmallStackBuffer<T,BUF_SIZE>& rhs);

				public:
					#if		qCompilerBuggyOverloadingConstOperators
						operator T* () const;
					#else
						operator const T* () const;
						operator T* ();
					#endif

				public:
					typedef	T*			iterator;
					typedef	const T*	const_iterator;
				public:
					nonvirtual	iterator		begin ();
					nonvirtual	iterator		end ();
					nonvirtual	const_iterator	begin () const;
					nonvirtual	const_iterator	end () const;

				public:
					nonvirtual	size_t	GetSize () const;

				public:
					// Despite the name, it is OK if nElements shrinks the list. This really should be better called Resize ()
					nonvirtual	void	GrowToSize (size_t nElements);
				private:
					nonvirtual	void	GrowToSize_ (size_t nElements);

				public:
					nonvirtual	void	push_back (const T& e);

				private:
					size_t	fSize;
					T		fBuffer[BUF_SIZE];
					T*		fPointer;
			};


		}
	}
}
#endif	/*_Stroika_Foundation_Memory_SmallStackBuffer_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SmallStackBuffer.inl"
