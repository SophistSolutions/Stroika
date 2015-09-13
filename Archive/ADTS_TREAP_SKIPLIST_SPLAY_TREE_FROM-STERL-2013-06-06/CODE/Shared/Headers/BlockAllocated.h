#pragma once


#include "Config.h"

template	<typename	T>		
class	BlockAllocated {
	public:
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

		#define	DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)\
				static	void*	operator new (size_t n)							{	return (BlockAllocated<THIS_CLASS>::operator new (n));	}\
				static	void*	operator new (size_t n,int,const char*,int)		{	return (BlockAllocated<THIS_CLASS>::operator new (n));	}\
				static	void	operator delete (void* p)						{	BlockAllocated<THIS_CLASS>::operator delete (p);		}\
				static	void	operator delete (void* p,int,const char*,int)	{	BlockAllocated<THIS_CLASS>::operator delete (p);		}

		#define	DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)\
				static	void*	operator new (size_t n)		{	return ::operator new (n);	}\
				static	void	operator delete (void* p)	{	::operator delete (p);		}

	private:
		static	void	GetMem_ ();	// Break out into separate function so we can
									// make op new inline for MOST important case
									// were alloc is cheap linked list operation...

		static	void*	sNextLink;
		static	void*	GetNextLink_ ();
		static	void	SetNextLink_ (void* nextLink);
};

#include "BlockAllocated.inl"
