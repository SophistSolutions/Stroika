The Stroika WebService Framework allows you to expose programatic APIs through web services (SOAP).

Status:
	[[[This is not even really started, just at the early desgin phase]]]



TODO/DESIGN NOTES:

	o	Stroika soap method take and reurning variant objects. At least params. Avoids issues with defining structure and handling optionals
	o	Layered – eventually maybe support typesafe stubs.
	o	But first part of Framework is Memory::Variant() – based, with optional function to validate according to WSDL (on in and out)

	class	SOAPProxy {
		public:
			SOAPProxy (WSDL, OPTIONAL URL);
			SOAPProxy (URL);

			// Sync call - with soap exceptions etc. Does validation if WSDL provided.
			Memory::Variant	Call (String methodName, Memory::Variant args);
	};


	// TODO: Need CFX-style interceptors mechanism (for security)... maybe not for v1.0?
	class	SOAPStub {
		public:
			class MethodCallHandler {
			public:
			virtual Memory::Variant	Call (String methodName, Memory::Variant args) = 0;
			};
			using		MethodCallHandlerPtr	=		shared_ptr<MethodCallHandler>;
			SOAPStub (WSDL, SocketStream, MethodCallHandlerPtr);
			SOAPProxy (SocketStream, MethodCallHandlerPtr);

			// blocks reading on input and invokes callback as needed
			void	ProcessNextCall();
	};

	o	Leverage existing ObjectMapper code
	o	Write tool to parse XSD to generate ObjectMapper definitions.
	o	Do in such a way that it can be edited/revfined/re-used:
		o	EITHER
			o	two passes - one from WSDL to generate bare bones config (or optionally fill details mapped config)
			o	Then you can edit the bare bones and copy in what you want (edited) from teh defailts filled config
			o	Then you can run a second phase to generate actual C++ mappings code - based on earlier config file.

			Idea is you quickly get basic mapping (like with gsoap). But you can reconfigure just the pieces you want
			to may to your own structures. And then even as the WSDL changes, those mapping choices remain.
		o OR
			o	something more like "Emily" for dailog editor. Use subclassing. Generate DEFAULT mapping. BUt have
				some way after the fact you can 'sub-in' your own replacemnet mapping. Not sure how that would
				work here.

	o	WSDL reader tool generates BOTH ObjectMapper code and (new TBD) WebServer router input- to appropriate execution
		functions.

	o	Also need builtin exception to SOAP/HTTP fault mapping
