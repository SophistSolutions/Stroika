Overview
--------
	Unsure about top-level modularity - but makes sense to move iterator/iterable out of container - since can and should
	be used for generators, like Range.



	GENERATOR:
		loop (yielder)
		{
			while (1) {
				yielder(val);
			}
		}


	// the question is - is there any way todo this without threads? I could run a look on a thread and yield would 'send a message' back to the
	// iterator? That maybe how it needs to happen.
		class generator : iterable<T> {
			public:
	
				generator (std:function<void(std::function<void(T)> yielder)>);
		};

		Plus - we need a better way to handle GetLength () - like 'not supported' - or maybe lose it as a common method of iterable?
		we could just iterate (make new useless iterator). That maybe OK. But not for things that genrate  infinite sequne.
