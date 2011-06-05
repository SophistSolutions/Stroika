/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__SmallStackBuffer_h__
#define	__SmallStackBuffer_h__	1

#include	"../StroikaPreComp.h"

#include	<cstddef>

#include	"../Support.h"



namespace	Stroika {	
	namespace	Foundation {


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
#endif	/*__SmallStackBuffer_h__*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SmallStackBuffer.inl"
