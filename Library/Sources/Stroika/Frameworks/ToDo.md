﻿TODO
	>	Add todo/placeholder for new stroika framework for capturing system statistics 
		Eg. GetCPUUsage, disk space usage etc. Like what MSFT gives you – so review their performance monitoring stuff… Do portably – but with looks for os-specific values. 

		Biggest is load average, or pct-cpu-usage, or amount of free disk space?

		Call the framework “SystemPerformanceMonitor” or “SystemPerformanceStatistics”?

		o	Add (SOON) performance-measurement framework. 
			o	
			};
			o	Nice to use atoms for registry below
			o	Define registry of measurerers – stuff like  ‘network usage’, uname, (os name/version), pretty name (/etc/*release), and I’msure many more.
			o	Be clear how some platform-specific (ifdef their defintiions)
			o	For backend – use (among other thigns) – processrunner to fork process and gather output
			o	CaptureMgr – where you can assign a frequency (maybe per item) and give it a list of items to measure, and then it runs in a thread. Maybe assign to it ‘handlers’ for results of capture (callback function maybe default/best).
			o	Internally – it runs thread todo captures
			o	Maybe add to list of ‘catureres’ something that does pings and traceroutes to given argument addresses).
