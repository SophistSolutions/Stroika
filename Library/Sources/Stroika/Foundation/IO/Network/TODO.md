>>>TODO:

..code/library like:
    addrinfo hints;
    addrinfo* res = nullptr;
    memset ((void*)&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    string  tmp =   bindProperties.fHostName.AsUTF8<string> (); // BAD - SB tstring - or??? not sure what...
    try {
        Execution::Handle_ErrNoResultInteruption ([&tmp, &hints, &res] () -> int { return getaddrinfo (tmp.c_str (), nullptr, &hints, &res);});
    }
    catch (...) {
        // MUST FIX THIS - BROKEN - BUT LEAVE IGNORING ERRORS FOR NOW...
    }

	HostName module, or NameLookup, or DNS Lookup module?


+	WebSocket
	We probably should somehow support websockets - but not totally clear how. Maybe it's just a stream-based support on top
	of an http stream (client/server)? Maybe it can be defined totally in terms of streams? Then used trivially? I have to 
	read the spec to be sure...
