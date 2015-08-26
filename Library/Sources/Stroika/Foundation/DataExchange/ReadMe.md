This Folder contains the Foundation::DataExchange source code.

	This module is about data structures and formats - and NOT about the IO/networking interactions of
	an exchagne.

	This module contains code relating to serialization and deserializaiton of structured data.
	
	Most of these modules will use VariantValue as a linga-franca to express structured data
	going in/out of a module.

	(e.g. JSON or XML) reader / writer <--> VariantValue <-- (via ObjectVariantMapper)--> C++ struct