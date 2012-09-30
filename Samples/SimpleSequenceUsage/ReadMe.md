Not started.

Rough outline:

	Sequence<T>	GenData()
	{
		Bag<T> b;
		for (int i = 1; i < 100; ++i) {
			b.Add (i);
		}
		b.Shake();
		return Sequence (b);
	}
	
	Sequence	DoSort (Sequence s)
	{
		// Note that Sequences create iterators that CAN be used as STL random-access iterators,
		// and so work with builtin STL algorithms.
		qsort (s.begin (), s.end ());
		return s;
	}
	
	void	Test ()
	{
		Sequence<T>	origData	=	GenData();		// randomly ordered list
		
		{
			Sequence<T>	tmp	=	DoSort (origData);
			Assert (origData != tmp);		// count on fact that data will be randomly ordered
		}
		
		// You can create teh list as a doubl klink  list and it still acts as a seuqnce, being sortable
		// and direcltyy accessible - but SLOWLY
		Sequence<T>	dataAsLL = Sequence_DoubleLinkList<T> (origData);
		{
TIMESTAMP;
			Sequence<T>	tmp	=	DoSort (dataAsLL);
			Assert (origData != tmp);		// count on fact that data will be randomly ordered
TIMESTAMP;
		}
		
		Sequence<T>	dataAsLL = Sequence_Array<T> (origData);
		{
TIMESTAMP;
			Sequence<T>	tmp	=	DoSort (dataAsLL);
			Assert (origData != tmp);		// count on fact that data will be randomly ordered
TIMESTAMP;
		}
		
	}
	
