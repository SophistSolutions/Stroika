About
=====
These release notes are incomplete. They are merely a summary of
major user (developer)-impactful changes - especially those they need 
to be aware of when upgrading.

History
=======

<table>

  <thead>
    <th style='width:1in; vertical-align: top'>Version<br/>Release Date</th>
    <th style='vertical-align: top'>Changes</th>
  </thead>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a114">v2.0a114</a><br/>2015-10-26</td>
<td>
	<ul>
		<li>Cleanup BLOB class, and probably slight performance improvements</li>
		<li>qStroika_Foundation_DataExchange_VariantValueUsesStroikaSharedPtr_ set to qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr</li>
		<li>fix for memory leak regression with SharedPtr (because of enabling the SharedPtr rvalue-ref code - old bug) - https://stroika.atlassian.net/browse/STK-215  Also, minor tweaks to SharedPtr (inlines).</li>
		<li>Added Tests/HistoricalReferenceOutput/{PerformanceDump-v2.0a114-linux-gcc-5.2.0-x64.txt,PerformanceDump-v2.0a114-x86-ReleaseU.txt} - showing no significant change, or
		slight improvement</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a113">v2.0a113</a><br/>2015-10-24</td>
<td>
	<ul>
		<li>Syntax Erorr (TimedCache)</li>
		<li>no testing - cuz little changed - emergency release</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a112">v2.0a112</a><br/>2015-10-24</td>
<td>
	<ul>
		<li>Fixed new ScriptsLibs UNIX shell script permissions</li>
		<li>no testing - cuz little changed - emergency release</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a111">v2.0a111</a><br/>2015-10-24</td>
<td>
	<ul>
		<li>Added ScriptsLib/MakeBuildRoot.sh and ScriptsLib/MakeDirectorySymbolicLink.sh</li>
		<li>LRUCache and TimedCache performance tweaks</li>
		<li>new ScriptsLib/WebGet.sh to help download dependent components and used to change Xerces source mirror (cuz was failing)</li>
		<li>no testing - cuz little changed - emergency release</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a110">v2.0a110</a><br/>2015-10-21</td>
<td>
	<ul>
		<li>Switched all Windows development from using Visual Studio.net Ultimate to using Community Edition</li>
		<li>ScriptsLib/RegressionTests-Unix.sh</li>
		<li>cleanup of how we generate default warning suppression messages (configure)</li>
		<li>More cleanups of IO/Network/Socket, including better handle windows ThrowIfErrors() for winsock</li>
		<li>New InputStream<ELEMENT_TYPE>::ReadAll/2</li>
		<li>Several fixes to SystemPerformance/Instruments/Process, partly due to Valgrind feedback, including
		most importantly, was treating an array of bytes as a null-terminated string without ever null-terminating</li>
		<li>lzma sdk version 1509; and libcurl use version 7.45</li>
		<li>minor cleanups to GetWinOSDisplayString_: mostly to supress the warning about ::GetVersionEx(), and
		fixed it to show right version of created than windows 8.1</li>
		<li>Silence more uneeded compiler warnings, and a few timeouts on regression tests triggered by excessively slow machines/valgrind</li>
		<li>Fixed qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_ issue, and lose define</li>
		<li>qCompilerAndStdLib_SFINAE_SharedPtr_Buggy define to workaround bugs</li>
		<li>qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_ DELETED </li>
		<li>Various Iterator<> fixes so we can define qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr 0 or 1</li>
		<li>New define qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr, and used to drive 
		qStroika_Foundation_Traveral_IteratorUsesStroikaSharedPtr qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr</li>
		<li>Added PerformanceDump-v2.0a110-x86-ReleaseU.txt, PerformanceDump-v2.0a110-linux-gcc-5.2.0-x64.txt</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a109">v2.0a109</a><br/>2015-10-16</td>
<td>
	<ul>
		<li>use {} init instead of memset(0) in several places</li>
		<li>cleanup socket error/throw code for quirky winsock - must call WSAGetLastError - not errno!</li>
		<li>Added regression tests for sterls bug report about JSON reader/writer and fixed bug with </li>
		<li>Use ArgByValueType<T> in several more container classes (colelciton/deque/multiset/queue/sequence/stack, etc... should improve performance).</li>
		<li>SystemPerformance/Instruments/CPU Windows RunLengthQ support</li>
		<li>Added PerformanceDump-v2.0a109-x86-ReleaseU</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a108">v2.0a108</a><br/>2015-10-14</td>
<td>
	<ul>
		<li>Significant change to build system: configure script now more central, and works better. make default-configuration now calls configure.  Now support args in --compiler-driver, and fixed AR/RANLIB autogen accordingly.</li>
		<li>in openssl build: make rehash to avoid occasional noise in output</li>
	</ul>
</td>
</tr>



 



<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a107">v2.0a107</a><br/>2015-10-13</td>
<td>
	<ul>
		<li>Streams/BufferedOutputStream FLUSH calls internal in implemeatnion causes re-entrant call to non-recursive AssertExternallySynchronizedLock mutex (reported by JohnB @ Block)</li>
		<li>Cleanup DataExchange/Zip/Reader</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a106">v2.0a106</a><br/>2015-10-13</td>
<td>
	<ul>
		<li>SystemPerformance Framework:
			<ul>
				<li>Instruments/CPU: support for run-q-length</li>
				<li>Instruments/Memory: windows OSReserved RAM, fixed minor page faults per second for AIX/Linux, refactored real/virtual/physical
						memory stats in SystemPerformance Memory - and added osreserved to phys memory stats (but not used), AIX report virt_active for fCommitedBytes,
						better handle missing 'MemAvailable' for older Linuxes - and estimate from SReclaimainble, Free and inactive RAM</li>
				<li>Instruments/Filesystem cleanups: div-by-zero, fQLength , in-use-pct, refactoring of  - to return stats on Disk and Volume (filesystem),
					ignore unmounted filesystems, and document todo perhaps having an option/separate list to return them
					(indexed by volume ID I guess, use q_sampled instead of ws_qsampled in AIX SystemPerformance Filesystem disk Q length monitor
					(still found no decent docs, but gives better answers and my guess is that q_sampled is for both read and write q, and wq_sampled is just for READ</li>
				<li>Instruments/Network AIX support for total # TCP segment and TCP retransmits, Added another name mapping kluge from description to WMINAME for Windows SystemPerformance Network API</li>
			</ul>
		</li>
		<li>Added FileSystem::GetCurrentDirectory and FileSystem::SetCurrentDirectory methods</li>
		<li>a little more support for ToString debugging utility (but still not working with Optional)</li>
		<li>fix compare VariantValue(NAN, Math::nan<double> ()) and with infinity- make equals coerece these special case strings</li>
		<li>JSON reader/writer now properly handles NAN/INF values - by converting them to/from strings so remains always valid JSON but can still roundtrip these values</li>
		<li>Major fixups to lzma makefile so pays attention to CC/etc from Stroika configuration, and fixes /MT windows issues etc</li>
		<li>Major fixups to zip makefile so pays attention to CC/etc from Stroika configuration, and fixes /MT windows issues etc</li>
		<li>Major fixups to curl makefile so pays attention to CC/etc from Stroika configuration</li>
		<li>Implemetned now fully functional cross platform LZMA ArchiveReader, and ZIP ArchiveReader (at least miniaml list/extract function),
			including new ArchiveUtility sample</li>
		<li>smarter SFINAE code for Optional and narrowing conversion (better warnings)</li>
		<li>LRUCache defaulttrraits now takes VALUE,a nd deifned OptionalValueType, and changed default to regular default for Optional but documented how to use Memory::Optional_Traits_Blockallocated_Indirect_Storage</li>
		<li>factored Platform_AIX_GetEXEPathWithHint into its own AIX specific module</li>
		<li>allow AF_LINK addresses on AIX for reported interface experimental); fix IO/Network/Interface regression</li>
		<li>Support for gcc5 bug defines</li>
		<li>Library/Sources/Stroika/Foundation/Execution/ProcessRunner tweaks; rewrote code to layout exec args in ProcessRunnner for POSIX to avoid issue depending on the layout of g++ vector (cuz broke with gcc5)</li>
		<li>placeholder KeyedCollection container file</li>
		<li>inor tweaks, asserts etc - based on feedback for msvc 2k15 analysis tool</li>
		<li>Tons of cosmetic cleanups and use Finally/cleanup code to simplify thread/process handle cleanup in windows CreatePRocess usage (warned by
			vs2k15 code analyze, but docs from msft said only needed to close process handle?)</li>
		<li>threadpool regtest improvements; Added RegressionTest15_ThreadPoolStarvationBug_ () - but didnt help and may not really be a problem</li>
		<li>use much more of ArgByValueType<T> (for performace) - and REPLACED (not backward compat) EachWtih in several containers to use IMPORVED 
			Where in Iterable (taking optional return container type) - still experimental</li>
		<li>include -lm in unix link args in case gcc used as driver instead of g++; and redid config code to allow config of gcc/g++ (C/C++ compierl name cuz needed to build lots of third party tools)
		and detect gcc version# in GeneragteDefaultCOnfig.pl file so we can do right defaykt flags per version.</li>
		<li>Repalce memset(0...) initialization with POD {} initialization</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc50, gcc51, pcc-AIX/gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>



  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a105">v2.0a105</a><br/>2015-09-22</td>
<td>
	<ul>
		<li>Started support for LZMA SDK (7z archive format). Got so SDK in third-party-products builds (but not yet fully intergrated into stroika).</li>
		<li>SystemPerformance Framework:
			<ul>
				<li>Tons of fixes for AIX - esp to process and memory instruments</li>
				<li>Several cross-platform changes to memory names, and kinds of memory stats we capture/report
					(fPrivateVirtualMemorySize, fTotalVirtualMemorySize, Free, Active, Inactive,Available RAM, better pagefaults and page outs, private working set, command line, EXE)</li>
				<li>caching staticinfo support for SystemPerformance/Instruments/Process - for AIX</li>
			</ul>
		</li>
		<li>More Foundation/Execution/Module String GetEXEPath (pid_t processID) performance
    improvements (FOR AIX)</li>
		<li>added operator +/-/*/// for two Optional&lt&gt; objects</li>
		<li>WebServer framework
			<ul>
				<li>Modest progress on WebServer framework: enhanced exmaple so that showed looking for differnt urls, exception handling, and POST (and input args handling)</li>
				<li>Added Cruddy but usable Request::GetBody () in Stroika/Frameworks/WebServer</li>
				<li>Kludged around bug in reading WebServer headers - so now allows reading web request body (but must rewrite that code)</li>
			</ul>
		</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), pcc-AIX/gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a104">v2.0a104</a><br/>2015-09-12</td>
<td>
	<ul>
		<li>cleanup aspects of Build structure - Project subdir Linux renamed to 'Unix', and re-purposed --platform generate config arg, and lose Platform value from config file</li>
		<li>AIX support
			<ul>
				<li>Much improved reliability and performance of Execution::GetEXEPathT</li>
				<li>Linker/Makefile cleanups</li>
				<li>qCompilerAndStdLib_Locale_Buggy for buggy AIX</li>
				<li>endianness fixes to IO/Network/InternetAddress (AIX), and other endianness fixes/improvements, and hash code</li>
				<li>tweak GenerateConfiguration.pl default C++ warnings (better for AIX)</li>
				<li>Hopefully workaround AIX crasher in Network::GetInterfaces ()</li>
				<li>Workaround AIX bug with gcc snprintf() - Time::Duration - e90bf07ee6f6bed174e5aee49d77fdb3a48a1fa3</li>
				<li>DateTime::AddSeconds() takes int64_t instead of time_t - because AIX only supports 4-byte time_t and that causes overflow with duration of only 100 years; and Added int/long/longlong return overloads from Duration</li>
				<li>just DBGTRACE _SC_V6_LP64_OFF64 if macro not defined (POSIX) - for now</li>
				<li>use sigaction instead of signal API - since fixes bug where signals stop working on 2nd created thread for AIX</li>
				<li>Make Time::Duration code a little more resilient (and more assert checking) for floating point rounding errors</li>
				<li>Tons of work on SystemPerformance framework for AIX. Works decently now, but still a bit more todo. TONS of changes required</li>
				<li>Some changes to Configuraiton::GetSystemConfiguration for AIX</li>
				<li>qCompilerAndStdLib_fdopendir_Buggy</li>
				<li>if defined AIX, then add -lperfstat to StroikaFrameworksSupportLibs in Projects/Unix/SharedMakeVariables-Default.mk</li>
				<li>replace use of defined (_AIX) with qPlatform_AIX</li>
			</ul>
		</li>
		<li>GetEXEPath (pid_t)</li>
		<li>fixed bug with ProcessRunner - if we get error calling GetWD() - we throw. On POSIX we nver even used it (fixed now). But also changed API so WD is optional, and if not specified, not used (default OS behavior)</li>
		<li>Cleanup IO/FileSystem/DirectoryIterator (some for AIX) - re-entrant readdir_r, etc</li>
		<li>Added support for vector<T> to ObjectVariantMapper, along with regresstion test to verify</li>
		<li>rename subdir of Projects from Linux to Unix (as we now support 2 non-linux unix  variants)</li>
		<li>cleanup of Generate/Apply config scripts - start (testing) getting rid of Platform variable</li>
		<li>Get rid of (most of?) remaining uname usage in makefiles and perl scripts. Use configuraiton define PlatformSubDirectory instead</li>
		<li>Lose gcc 4.7 support, and lose obsolete bug defines: 
		qCompilerAndStdLib_thread_local_initializers_Buggy,
    	qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy,
		qCompilerAndStdLib_UnreasonableCaptureThisRequirement_Buggy,
    	qCompilerAndStdLib_threadYield_Buggy</li>
		<li>Cleanup logging/debugging code for signals, and simplify some legacy thread/signal code</li>
		<li>added test case for building 32-bit Stroika code on 64-bit linux - and got working, and added to set of every-release-tests</li>
		<li>--static-link-gccruntime option to make default-configuraiton</li>
		<li>Misc docs and regtest improvements (e.g. IO::Network::Transfer::CreateConnection)</li>
		<li>qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy seems broken even with gcc49</li>
		<li>new bug define qCompilerAndStdLib_strtof_NAN_ETC_Buggy; Refactored String2Float and Float2String into FloatConversions; 
		Cleanup Float2StringOptions; new - little tested - semantics for handling NAN and INF values</li>
		<li>lose qCompilerAndStdLib_constexpr_static_member_functions_default_args_Buggy because we redid NearlyEquals() using overloading - much simpler. 
		And now facteored out epsilon calc to private routine, where we can try being smarter; and use Math::NearlyEquals () and added overload for DateTime;
		Redid Math::NearlyEquals() templates, so works with differnt argument types/li>
		<li>redefined GetTickCount() to zero at app start to minimize problems with overflow</li>
		<li>allow Optional<> to assign common assignable types</li>
		<li>Fixed a number of things so we CAN set DurationSecondsType to long double, but dont leave it that way until we can performance test</li>
		<li>fixed Duration::PrettyPrint () to return empty string on empty duration but zero duration returns '0 seconds' and added regtests for this and other format calls</li>
		<li>Generate new performance regression test results</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), pcc-AIX/gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind; </li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a103">v2.0a103</a><br/>2015-08-23</td>
<td>
	<ul>
		<li>Started attempts at support for AIX
			<ul>
				<li>Mostly makefile changes - losing some old perl scripts, and repalcing use of stuff like $^O with ConffigurationReader.pl and PlatformSubdir... variable</li>
				<li>GetOSInfo/version</li>
				<li>thread/std::thread conflicts</li>
				<li>::sysconf (_SC_V6_LP64_OFF64)</li>
				<li>Large number of qPlatform_Linux versus qPlatform_POSIX changes</li>
				<li>lose unneeded sys/types.h includes</li>
				<li>Added Xerces patch for AIX (configure)</li>
				<li>sysinfo for reading uptime is Linux specifc</li>
				<li>for AIX - set bigtoc option by default in GenerateConfiguration.pl</li>
				<li>>makedev on AIX is a macro, so cannot use ::</li>
			</li>
		</li>
		<li>curl 7.44</li>
		<li>better exception message on locale not found (and other tweaks)</li>
		<li>DateTime now supports timeval (in addition to timespec)</li>
		<li>Support _MS_VS_2k13_Update5_FULLVER_</li>
		<li>lose define qNBytesPerWCharT and Defaults_Characters_Character_.h - anachonisitic</li>
		<li>factor GetStartOfEpoch_ () out of Emitter::DoEmitMessage_ () so we get one start-of-epoch</li>
		<li>draft configure script (top level) - so for indirects. May leave it that way?</lI>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind; 3/4 regtests pass on AIX 7.1/gcc49</li>
	</ul>
</td>
</tr>




  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a102">v2.0a102</a><br/>2015-08-17</td>
<td>
	<ul>
		<li>added new Debug/CompileTimeFlagChecker and used for qDebug qDefaultTracingOn ... etc</li>
		<li>cleanup docs: doxygen - Example Usage and code declaration around bodies</li>
		<li>Lose some deprecated options from make default-configuration</li>
		<li>implemented Logger::FileAppender - and Logger::StreamAppender</li>
		<li>Added FileOutputStream optional AppendFlag support</li>
		<li>More varied build tweaks (project files) for MSVC2k15</li>
		<li>use define qFeatureFlag_ATLMFC instead of qStroika_Framework_Led_SupportATL</li>
		<li>use new Characters::Platform::Windows::SmartBSTR () instead of CComBStr - cuz VS Express has no ATL</li>
		<li>Add optional parameter to InputStreamFromStdIStream<> for seekability (e.g. for cin)</li>
		<li>Replace SLN file per sample with one big SLN file for all of them</li>
		<li>Fixed small but serious bug with Mapping<>::RetainAll ()</li>
		<li>JSON Writer - handling (document todo and added assert) for NAN values</li>
		<li>Tweak openssl builds</li>
		<li>Big/Subtle change in thread safety rules for container reps. They now use a recursive mutex, instead
			of the former non-recursive mutex.

			This then lifted the restriction on access the underlying iterable from inside an Apply() method.
			(ContainerRepLockDataSupport_)
		</li>
		<li>Generate new performance regression test results (no interesting changes)</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro&&Community} (except some crashers in 64 bit code due to MSFT lib bug), gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind</li>
	</ul>
</td>
</tr>





  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a101">v2.0a101</a><br/>2015-08-03</td>
<td>
	<ul>
		<li>Got nearly fully working with VisualStudio.net 2k15 (one serious 64 lib bug exists, and I must report 
			found bugs to MightSlop).</li>
		<li>Use libcurl 7_43_0</li>
		<li>OpenSSL - upgrade to 1.0.2d, and several cleanups to build process etc, including using .pdb file
		 instead of patch todo /Z7</li>
		<li>Started cleanup of Synchonized&lt;&gt; code now that we have shared_lock stuff in MSVC. Got a decent
		start at supporting read/write locks in this code, but still needs a bit more to be testable</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug, and VS2k15 Express not working), gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind</li>
	</ul>
</td>
</tr>




  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a100">v2.0a100</a><br/>2015-07-21</td>
<td>
	<ul>
		<li>Rework Containers to use enable_if so constructor taking container is more restricive. This allowed greatly simplfying other uses of constructors, possibly improving perofrmance, but defintely making much simple and more robust.</li>
		<li>New ArgByValueType&lt;T&gt; template to improve performance (for now just used in container reps)</li>
		<li>SystemPerofrmance framework: filesystem imporvements (e.g. fAverageQLength), and Process (fCaptureCommandLine regexp)</li>
		<li>Mapping<>::RetainAll()</li>
		<li>RegularExpression cleanups</li>
		<li>fixed serious/subtle bug with DataExchange/ObjectVariantMapper read of 'empty' variant value mapped to 'null' (missing) data. That means null in the JSON (or a missing field) was treated the same as an  empty string (or nan float); Dangerous fix, but significant improvement</li>
		<li>Generate new performance regression test results</li>
		<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind, and next rev should work with VC++2k15</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a99">v2.0a99</a><br/>2015-07-12</td>
<td>
	<ul>
		<li>A few minor Streams cleanups</li>
		<li>Very little testing</li>
	</ul>
</td>
</tr>





     
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a98">v2.0a98</a><br/>2015-07-11</td>
<td>
	<ul>
		<li>Major cleanup of Streams code. Mostly just polishing, and completing the work started in the last release.</li>
		<li>Fixed bugs with Synchonized<>/WritableReference - copying and const indirection</li>
		<li>POSIX SystemPerformance/Instruments/Filesystem aveQLen code</li>
		<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind, and built and most tests OK on VC++2k15</li>
	</ul>
</td>
</tr>






   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a97">v2.0a97</a><br/>2015-07-07</td>
<td>
	<ul>
		<li>Major Design and Name Changes for Streams library
			<ul>
				<li>New template Stream<ELEMENT_TYPE> replaces BinaryStream and TextStream</li>
				<li>New templates InputStream<ELEMENT_TYPE> and OutputStream<ELEMENT_TYPE> replace BinaryInputStream, BinaryOutputStream, TextInputStream, TextOutputStream</li>
				<li>Lose Seekable base class, and instead have that functionality built into InputStream and OutputStream, so that the virtual methods for offset and seek can be different, allowing virtual mixins that permit separate read/write seek pointers.</li>
				<li>Combined BasicBinaryInputOutputStream, BasicBinaryInputStream, and BasicBinaryOutputStream, and the three text stream variations, into
				MemoryStream (in such a way as to generalize them ALL and allow of Memory streams of structured objects)</li>
				<li>Lose explicit promise that all streams are synchonized, and instead added 'Synchnonized' method to InputStream<ELEMENT_TYPE> and OutputStream<ELEMENT_TYPE>
				which creates synchonized streams, and other ones merely use Debug::AssertExternallySynchonized</li>
				<li>Lose BinaryTiedStream, and instead, generalized BinaryInputOutputStream to the case where writes to one end COULD map to reads from the other, or just
				to some (e.g. remote) endpoint (so case of Memory based input/output stream same base class as socket streams).</li>
			</ul>
		</li>
		<li>Minor FileSystem fixes: FileSystem::RemoveFile () etc - use wunlink() if qTargetPlatformSDKUseswchar_t/windows</li>
		<li>use Range<DurationSecondsType> instead DateTimeRange in SystemPerformance/MeasurementSet to get higher resolution</li>
		<li>Use openssl 1.0.1o</li>
		<li>Improved STLContainerWrapper support for vectors patching; and use that to fix patching bugs in Sequence_stdvector</li>
		<li>Added and refactored ReserveSpeedTweekAddNCapacity() to Containers common code</li>
		<li>Generate new performance regression test results</li>
		<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, arm-linux-gnueabihf-g++-4.9(regtest44 failed), and valgrind, and built and most tests OK on VC++2k15</li>
	</ul>
</td>
</tr>



   
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a96">v2.0a96</a><br/>2015-06-22</td>
<td>
	<ul>
		<li>avoid LRUCache lock_guard<AssertExternallySynchronizedLock> reentrancy issue</li>
		<li>Time::GetTimezoneInfo(); disable +%Z tz mapping - was unwise and not helpful in, fixed setting fBiasInMinutesFromUTC, and wrong dst flag to Time::GetLocaltimeToGMTOffset</li>
		<li>DurationRange should have diff type of DurationSecondsType - not int</li>
		<li>Made Duration::Format() alias for PrettyPrint () - so it works well with DurationRange::Format()</li>
		<li>SystemPerformance/Instruments/Filesystem: POSIX: fixed devName to use canonicalize, and use .st_rdev so we get right device number to lookup IO stats, and use dev_t instead of devicename to cross-index between device data files</li>
		<li>new Range<>::Pin() function</li>
		<li>use make_shared and MakeSharedPtr<> in a few places instead of new Rep_</li>
		<li>Fixed bugs with qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr off, and experiemnted, but left on for now as still faster</li>
		<li>use Get/SetCapacity on Sequence_Array and Sequence_stdvector insert, as slight speed tweak</li>
		<li>Added FileSystem::RemoveFile/RemoveFileIf</li>
		<li>Improved messages in started/stopped etc in Stroika/Frameworks/Service/Main</li>
		<li>improved OptionsFile message</li>
		<li>Frameworks/SystemPerformance/Instruments/Memory: quite a few (mostly unix) virtual memrory accounting/stats changes. Mostly settled on CommitLimit and CommittedBytes (like windows)</li>
		<li>Many RISKY changes to undo threadsafety support in Iterator/Iterable. Must me tested much more, but I think this takes us a good ways to using new 
			Synchonized API, and better performacne.
			<ul>
			   <li>turn qStroika_Foundation_Memory_SharedByValue_DefaultToThreadsafeEnvelope_</li>
			   <li>turn qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_ OFF</li>
			   <li>Turned off qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_</li>
			</ul>
		</li>
		<li>Generate new performance regression test results</li>
		<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind, and built and most tests OK on VC++2k15</li>
	</ul>
</td>
</tr>




 

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a95">v2.0a95</a><br/>2015-06-09</td>
<td>
	<ul>
		<li>SystemPerformance Process Insturment support for SetPrivilegeInContext to get a few extra procesess captured</li>
		<li>performance tweak Windows process System Perofmrance WMI code</li>
		<li>Added DateTime::FromTickCount ()/ToTickCount</li>
		<li>fixed bug iwth Foundation/Cache/LRUCache clear(KEY) method</li>
		<li>make SystemPerformance/Instruments/Filesystem IOStats element optional, and added fAvailableSizeInBytes to VolumeInfo and related fixes to posix FS code</li>
		<li> use Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER in IO/FileSystem/BinaryFileOutputStream.</li>
		<li>Timezone improvements for RedHat 5</li>
		<li>renamed Memory::TakeNBitsFrom -> BitSubstring ()</li>
		<li>define new MakeSharedPtr () static function and started using in container code as slight performance improvement</li>
		<li>Other small container speed tweaks</li>
		<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind, and built and most tests OK on VC++2k15</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a94">v2.0a94</a><br/>2015-05-25</td>
<td>
	<ul>
		<li>
			Platforms/Compilers added (mostly):
			<ul>
				<li>RedHat5 (various bugfixes mentioned below)</li>
				<li>Visual Studio.Net 2k15 (various compiler bug defines needed)</li>
			</ul>
		</li>
		<li>
			SystemsPerformance Framework:
			<ul>
				<li>Added -c option to Samples/SystemPerformanceClient - to make it easier to test</li>
				<li>Performance instrument ICapturer refacotoring - so we can more easily provide additional methods</li>
				<li>Improved (windows) time to first capture for many instruments - by putting the sleepuntil into the public API but using the private API taht doesnt wait todo initial capture</li>
				<li>cleanup Instruments/ProcessDetails POSIX procfs code to read commandline -mapping nuls to spaces</li>
				<li>overload Instrument::CaptureOneMeasurement to take optional return var argument measurementTimeOut</li>
				<li>Template specialization of CaptureOneMesaurement - required alot of restructuring, but helped performance significantly</li>
				<li>DriveType support to MountedFilesystemUsage, Frameworks/SystemPerformance/Instruments/MountedFilesystemUsage: renamed DriveType to MountedDeviceType</li>
				<li>Most instruemnts now support optional loading of some data - esp filesystem and process</li>
				<li>tons more here</li>
				<li>various code cleanups to SystemPerformance/Instruments/MountedFilesystemUsage (renamed fIOStats -> fCombinedIOStas, use sequnce update instead of copy new one - perfornamce)</li>
				<li>Small WMI robsustness fixes, but that code is still very rough</li>
				<li>renamed SystemPerformance/Instruments/NetworkInterfaces data member from fInterfaceStatistics to fInterfaces</li>
				<li>include Interface status (up/down/running) and type (wifi etc) to SystemPerformance/Instruments/NetworkInterfaces</li>
				<li>SystemPerformance/Instruments/MountedFilesystemUsage: better handle duplicate mounts in /proc/mounts - later ones win</li>
				<li>fixed SystemPerformance/Instruments/MountedFilesystemUsage to ahndle symbolic links to device names in /proc/mounts table</li>
				<li>SystemPerformance/Instruments/NetworkInterfaces now aggregates Foundation::IO::Network::Interface</li>
				<li>sepearate dout in SystemPerformance/Instruments/ProcessDetails totalprocessor usage from totalProcessCPUUsage (diff is irqs etc) - and capture more/better data on unix</li>
				<li>renamed SystemPerofrmcne ProcesDetails fTotalCPUTimeUsed to fTotalCPUTimeEverUsed</li>
				<li>SystemPerofmrance Process: CachePolicy::eOmitUnchangedValues</li>
				<li>Support OPTION - mostly to debug/test - to use 'ps' for process capture in SystemPerformacne module.
				<li>fixed order isuse in posix/ps SystemPerformance/Instruments/ProcessDetails</li>
				<li>Added new kernelprocess flag to ProcessDetails output, and used it as optimizaiton of some lookups</li>
				<li>renamed SystemPerformance/Instruments: MountedFilesystemUsage to Filesystem, NetworkInterfaces->Network, ProcessDetails to Process</li>
			</ul>
		</li>
		<li>ethtool_cmd_speed hack for old (centos5) linux</li>
		<li>support old linux /gcclib before pthread_setname_np</li>
		<li>Revised API for Common::CompareNormalizer</li>
		<li>no longer need workaround for qCompilerAndStdLib_Template_Baseclass_WierdIterableBaseBug - found underlying issue and corrected it</li>
		<li>lose specification of ForceConformanceInForLoopScope in windows project file</li>
		<li>turn off qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_ - cuz broken (for now). MAJOR PROBLEM!
			More tweaks to regtest timing so the tests dont fail - and had to adjust cuz this is noticably slower!</li>
		<li>
			Tons of build system scripts (configuration) improvements
			<ul>
				<li>lose obsolete 'target' code</li>
				<li>tweak display of run builds etc - indenting (../ScriptsLib/PrintLevelLeader.sh)</li>
				<li>Allow configuraiotn of AR and RANLIB from make defuault-configuraiton</li>
				<li>lots of refactoring/simplficaiton of options passed to generate congiguraiton - and can now pass in arbitray extra g++/linker options, so I can do stuff like -lto, and -pg more easily</li>
				<li> deprecated --default-for-platform and --online-if-unconfiratured...</li>
			</ul>
		</li>
	<li>enahnce Configuration::GetSystemConfiguration_OperatingSystem () to look for older redhat/centos systems without /etc/os-release</li>
	<li>attempt at better handling missing /etc/timezone file on posix systems</li>
	<li>Assure DNS module calls IO::Network::Platform::Windows::WinSock::AssureStarted (); on windoze; and fix placement of assert after throw-if</li>
	<li> document and fix IO::FileSystem::FileSystem::ResolveShortcut () to handle case where not pointing at a shortcut and return original name; and Rough draft of FileSystem::CanonicalizeName and FileSystem::GetPathComponents (): NONE IN GOOD SHAPE - DRAFT</li>
	<li>baudrate SB integer not floating point in Foundation/IO/Network/Interface; set friendly name</li>
	<li>lose explicit defines for WINVER, _WIN32_WINNT, _WIN32_WINDOWS in stroika - use externally defined values (like from make defualkt-configariotn or whatever windows msvc defines</li>
	<li>define new qCompilerAndStdLib_inet_ntop_const_Buggy bug</li>
	<li>Time::TimeOfDay::CTOR overload taking hours/minutes/seconds, and new DateTime::ToTickCount () helper, and CTOR bugfixes</li>
	<li>factored part of MakeVersionFile.sh into a more re-usable ExtractVersionInformation.sh</li>
	<li>cleanup names and DbgTrace in ProcessRunner for error in DBGTRACE execing child (POSIX ONLY)</li>
	<li>Network::GetPrimaryInternetAddress() on POSIX more robust</li>
	<li>Network::GetInterfaceById ()</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, and valgrind, and built and most tests OK on VC++2k15</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a93">v2.0a93</a><br/>2015-05-02</td>
<td>
<ul>
	<li>Cleaned up Stroika-Config code - so now that builds ONLY do the IntermediateFiles direactory and gets included</li>
	<li>Refactoring of SystemPerformacne code and lots of new system performance functionality - esp using WMI, using new wildcard
		support and reading back process ids from WMI, ProcessDetails,
		bind context, SleepUntil style, and much more</li>
	<li>undo mistake of adding Debug-A-32 and Release-A-32 to normal windows projhects.</li>
	<li>Added FileSystem::WellKnownLocations::GetRuntimeVariableData () and used that to store PID files</li>
	<li>new Rebin BasicDataDescriptor<X_TYPE, VALUE_TYPE>::GetValues ()</li>
	<li>Added Mapping<>::Values() method</li>
	<li> Redefined Optional<>::operator+/*- = methods to use AccumulateIf()</li>
	<li>Imprved regression test code to use median so more stable, and then could weak down range of regtest checks and still pass</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, cand valgrind </li>
</ul>
</td>
</tr>





	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a92">v2.0a92</a><br/>2015-04-26</td>
<td>
<ul>
	<li>MAJOR changes</li>
	<li>Added Debug-A-32, Release-A-32 builds (windows) and major restructing of build process
	so ThirdPartyProducts built to to level Builds directory (incomplete but mostly done). Major changes
	to apply configuraitons etc - all related to making the configuraiton stuff work more closely to
	new deisgn. Includes fixing OpenSSL build for 64 bits</li>
	<li>Scripts and makefile etc support to compute version strings/makefile stuff from single file, and #include from that common place - STROIKA_VERSION - ScriptsLib/MakeVersionFile.sh</li>
	<li>Fixed threading bug that caused valgrind leak (was mostly bug in regtest on thread code) but discovered
	and documemented a bug with thread code (deferred in todo {Thread d;} leak)</li>
	<li>new experiemntal Optional helpers (like Accumulate)</li>
	<li>BufferedBinaryOutputStream::Flush () not needed - inherited; and refined/loosed support for aborted in BufferedBinaryOutputStream</li>
	<li>Major improvements/cleanups/progress to SystemPerformance code - including WMI support</li>
	<li>Improved make project-files (and related)</li>
	<li>Major progress (though incomplete) on OpenSSL support - including new DerviedKey and much more</li>
	<li>Debug::AssertExternallySynchronizedLock copyable</li>
	<li>Improved SystemConfiguraiton support - especially for windows</li>
	<li>Improved Windows timezone support - olsen DB</li>
	<li>Optional AutoStartFlag arg to Thread CTOR</li>
	<li>WindowsEventLogAppender::WindowsEventLogAppender () needs appName param for name of source of events</li>
	<li>fix (little tested) for ObjectVariantMapper::MakeCommonSerializer_ to support Optional with non-standard traits</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, clang++3.6, cand valgrind </li>
</ul>
</td>
</tr>






	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a91">v2.0a91</a><br/>2015-04-05</td>
<td>
<ul>
	<li>Another minor patch...</li>
</ul>
</td>
</tr>





	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a90">v2.0a90</a><br/>2015-04-05</td>
<td>
<ul>
	<li>Minor patch release</li>
	<li>fixed missing namespace in definition of function (was link error)</li>
	<li>Minor cleanups to OpenSSLCryptoStream - didnt solve anything but a hint, and documented remaining angles to attack</li>
	<li>fixed CPPSTD_VERSION_FLAG to default to --std=c++11 (since thats all that gcc49 supports)</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever, AND new test26 openssl warnings to investigate)</li>
</ul>
</td>
</tr>








	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a89">v2.0a89</a><br/>2015-04-05</td>
<td>
<ul>
	<li>Re-implemented make 'default configuration' stuff to use --Xerces use, --ZLib no, etc.</li>
	<li>Fixed Windows 64 bit builds (was building 32-bit version)</li>
	<li>Fixed OpenSSL wrappers - they were buggy/untested. Not working fully compatibly, but decent/close, 
	and added regtests - most of which pass</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever, AND new test26 openssl warnings to investigate)</li>
</ul>
</td>
</tr>





	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a88">v2.0a88</a><br/>2015-03-30</td>
<td>
<ul>
	<li>Cleanup Debug::AssertExternallySynchronizedLock and its use (empty base class optimization)</li>
	<li>Draft of MSFT LINQ style methods in Iterable<T> - enough to evaluate / test /consider</li>
	<li>Fix some reporting of filenames with Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER</li>
	<li>Minor ThreadPool cleanups</li>
	<li>big revisions to SystemConfiguration::CPU code - to accomodate tracking per cor config info, model name, and reading /proc/cpuinfo on POSIX</li>
	<li>renamed Iterable<>::ExactEquals to Iterable<>::SequenceEquals()</li>
	<li>Draft ToString() function</li>
	<li>new STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS to help define concepts - and simplify code</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a87">v2.0a87</a><br/>2015-03-21</td>
<td>
<ul>
	<li>Use Xerces 3.1.2</li>
	<li>use openssl 1.0.1m</li>
</ul>
</td>
</tr>





	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a86">v2.0a86</a><br/>2015-03-17</td>
<td>
<ul>
	<li>Refactored most of Stroika to use non-member function style for equalable, and comparable, and new Common::CompareNormalizer helper</li>
	<li>fixed rebin Math::ReBin() CheckRebinDataDescriptorInvariant() code- to check less aggressively</li>
	<li>Fixed a few minor issues with WaitForIOReady for POSIX so more ready to test</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>





	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a85">v2.0a85</a><br/>2015-03-05</td>
<td>
<ul>
	<li>Lots of Rebin code cleanups and assertion checking and internal validation, GetIntersectingBuckets, kNullValue, etc (not all bacwkard compatible changes)</li>
	<li>Math::NearlyEquals() overload for the case of non-integral/non-floating point values, and changed integer overload default epsilon to zero</li>
	<li>cleanup display/formatting of threadids</li>
	<li>Revised design document about how to handle operator overloads (namespace function not member function), and started implementing in a few places</li>
	<li>Range&lt;&gt;Offset</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a84">v2.0a84</a><br/>2015-03-02</td>
<td>
<ul>
	<li>Fixed small bug with TraceContext code on UNIX</li>
	<li>Essentially no new testing</li>
</ul>
</td>
</tr>







	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a83">v2.0a83</a><br/>2015-03-01</td>
<td>
<ul>
	<li>Thread code cleanups (relating to races and new inturrupt support, and hopefully siginterupt fix, docs)</li>
	<li>experiments with new concepts (added e.g. has_eq, EqualityComparable, LessThanComparable, and used in ComparerWithWellOrder etc)</li>
	<li>Containers/ExternallySynchronizedDataStructures Array/LinkedList/DoublyLinkedList use ComparerWithEqualsOptionally instead of ComparerWithEquals: because we dont always need the method. Just fail when we access teh method (like call contains)</li>
	<li>Lose map&lt;&gt; code in TraceContextBumper, and use thread_local int instead (much simpler)</li>
	<li>Documentation</li>
	<li>Support Release-Logging-32/64 in Tests VS2k13 project</li>
	<li>TraceContextBumper CTOR wchar_t* / char* instead of ONLY requiring const SDKChar*</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>







	
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a82">v2.0a82</a><br/>2015-02-23</td>
<td>
<ul>
	<li>New Execution/WaitForIOReady class, and new IO/Network/IOWaitDispatcher</li>
	<li>Began major restructuring of Thread Abort() support, in preparation for new Interupt() support. Very incomplete
	but many name changes, to faciliate this structural change. ThreadAbortExcetpion is now Thread::AbortException, and
	many APIs use the word Interrupt instead of Abort(). This SHOULD be so far - semaantics free, but doing a release here
	to test that.
	</li>
	<li>new draft Execution::TimedLockGuard</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a81">v2.0a81</a><br/>2015-02-18</td>
<td>
<ul>
	<li>Added optional FlushFlag support to BinaryFileOutputStream</li>
	<li>got draft of Association working - but wrong API - really so far nearly identical to Mapping</li>
	<li>LRUCache fixes: supprot Mapping with traits properly passing new compare funciton, cleanups, performance improvements</li>
	<li>fixed bug with Mapping::Accumulate, and Mapping CTOR overload</li>
	<li>Very little testing</li>
</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a80">v2.0a80</a><br/>2015-02-12</td>
<td>
<ul>
	<li>possible fix bug with SignalHandlerRegistry::FirstPassSignalHandler_() - handling exceptions on thread which is being aborted</li>
	<li>made BlockAllocator&lt;T&gt;::Deallocate (void* p): NOEXCEPT; and used new DoDeleteHandlingLocksExceptionsEtc_ to supress threadabort exceptions</li>
	<li>Added StringBuilder::empty ()</li>
	<li>Very little testing</li>
</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a79">v2.0a79</a><br/>2015-02-11</td>
<td>
<ul>
	<li>Added new WaitableEvent::PeekIsSet () and better documented some details in WaitableEvent</li>
	<li>cleanups to LRUCache to remove remnamts of old API/classes</li>
	<li>Very little testing</li>
</ul>
</td>
</tr>


  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a78">v2.0a78</a><br/>2015-02-11</td>
<td>
<ul>
	<li>Typo</li>
</ul>
</td>
</tr>




  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a77">v2.0a77</a><br/>2015-02-11</td>
<td>
<ul>
	<li>Bugfixes with Traversal/DisjointDiscreteRange</li>
	<li>use Synchonized<> with recursive mutex  instead of nonrecursive mutext to avoid a flush deadlock in Execution::Logger</li>
	<li>Very little testing</li>
</ul>
</td>
</tr>



  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a76">v2.0a76</a><br/>2015-02-07</td>
<td>
<ul>
	<li>Draft of DisjointDiscreteRange<>::FindFirstThat/FindLastThat</li>
	<li>Fixed small bug(s) with LRUCache due to recent change to using Optional internally; 
	KeyType in its DefaultTraits, and ifdef out LRUCache_LEGACY_SOON2GO_</li>
	<li>LRUCache clear() method overloads</li>
	<li>Improved 'validateguards' support in SmallStackBuffer (fixed bug that not setup gaurds on X(X&) CTOR</li>
	<li>Reimplemnted MeasureTextCache::CacheElt using new LRUCache code (cleanup)</li>
	<li>Very little testing</li>
</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a75">v2.0a75</a><br/>2015-02-02</td>
<td>
<ul>
	<li>Small fixes to TimedCache code.</li>
</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a74">v2.0a74</a><br/>2015-02-02</td>
<td>
<ul>
	<li>Major refactoring of Optional&lt;&gt; code: adding back TRAITs, and using that to support plugable storage strategies (inline versus blockallocated), and documented better.</li>
	<li>allow mutable Optional<T, TRAITS>::peek () (not sure good idea)</li>
	<li>String REGEXP improvements (really experiments). Still VERY INCHOATE. But added docs/example use cases and added those to regtests (as well as inline docs)</li>
	<li>DisjointRange&lt;T, RANGE_TYPE&gt;::Format()</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a73">v2.0a73</a><br/>2015-01-29</td>
<td>
<ul>
	<li>Minor CallerStalenessCache&lt;&gt; fixes, and docs, and similar for Mapping&lt;&gt;</li>
	<li>SAFER fix for Array<> code - not using realloc(): eventually rewrite so sometimes uses this,
	 or expand or some such. But for now - at least its SAFE!!! (thjough more costly) than what we had before
	</li>
	<li>Turn back on qUseDirectlyEmbeddedDataInOptionalBackEndImpl_: safe now that we fixed above Array bug</li>
	<li>started adding more SFINAE support from Stroustrup C++11 book, and used to tweak ComparerWithWellOrder<T>::Compare() slightly</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>









<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a72">v2.0a72</a><br/>2015-01-28</td>
<td>
<ul>
	<li>fixed re-entrancy bug(deadlock) in optional mutex debug checking (debug only issue)</li>
</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a71">v2.0a71</a><br/>2015-01-27</td>
<td>
<ul>
	<li>Tests\HistoricalReferenceOutput\PerformanceDumps</li>
	<li>Lose Iteartor<>::operator-&ht; NON_CONST overload returning non-const pointer</li>
	<li>Various Optional&lt;&gt; changes:
		<br/>As checked in - now appears to work without crash in BLKQCL-Controller (still dont know exact proiblem)
		<br/>Turn qUseDirectlyEmbeddedDataInOptionalBackEndImpl_ off (which was key to working in BLKQCL-Controller)
		<br/>Use Holder_ to allow threadsafety checking
		<br/>Lose lots of non-const overloads and make API threadafe except for actual threadafety part)
		<br/>Lose TRAITS support
		<br/>Added ifdef(qDebug) - for now - fDebugMutex_ support
	</li>
	<li>redo DiscreteRange() to not inherit from Iterable<> but have convert op, and experimental begin/end methods so can be used in iteration expressions</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a70">v2.0a70</a><br/>2015-01-26</td>
<td>
<ul>
	<li>Attempted (mostly failed) to cleanup Debug::AssertExternallySynchronizedLock so we can save space used in release
		builds.</li>
	<li>Many LRUCache<> improvements: new Elements() method, and improved/revised use of Debug::AssertExternallySynchronizedLock</li>
	<li>Huge performance improvement (qUseDirectlyEmbeddedDataInOptionalBackEndImpl_)to Optional<> and 
		performance regression tests for Optional.</li>
	<li>Lose several deprecated functions/classes, like IRunnable, Lockable etc</li>
	<li>DiscreteRange<> cleanups (private Iterable)</li>
	<li>Large improvement to Range<>::Format</li>
	<li>new Configuration::ScopedUseLocale</li>
	<li>Minor misc progress on performance regtests (e.g. bufzize of Streams/TextInputStream::ReadAll)</li>
	<li>Tested (passed regtests) on vc++2k13, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)[gcc47 didnt work, didnt investigate]</li>
</ul>
</td>
</tr>





  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a69">v2.0a69</a><br/>2015-01-19</td>
<td>
<ul>
	<li>Added experimental Accumulate () API to Mapping</li>
	<li>Major cleanups to ObjectVariantMapper, as well as performance improvements:
			DataExchange/ObjectVariantMapper added ToVariantMapperType/FromVariantMapperType typedefs, 
			fixed MakeCommonSerializer_ support for C++ arrays, 
			and big performance improvements (cache / re-use looked up converters)
	</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a68">v2.0a68</a><br/>2015-01-15</td>
<td>
<ul>
	<li>Added new (draft) JSON readwrite and object-variant-mapper performacne test (and made 2.5x faster with below changes)</li>
	<li>Started a variety of performance tweaks based on the realization that constructing codecvt_utf8<wchar_t> was extremely slow (so do statically) and can re-use it from 
	differnt threads cuz with STL and const methods, this is guaranteed thread safe
	(see http://open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3797.pdf 17.6.5.9 Data race avoidance: A C++ standard library function shall not directly or indirectly modify objects (1.10) accessible by threads
other than the current thread unless the objects are accessed directly or indirectly via the function’s non-const
arguments, including this)
	</li>
	<li>added begin/end to StringBuilder</li>
	<li>Use StringBuilderand other tweaks in DataExchange/JSON/Writer for performance sake</li>
	<li>Simplify Mapping<>/old legacy map<> usage in VariantValue</li>
</ul>
</td>
</tr>

  




  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a67">v2.0a67</a><br/>2015-01-12</td>
<td>
<ul>
	<li>operator+= and operator-= support for Synchronized&lt;T, TRAITS&gt;</li>
	<li>LRUCache: tiny improvements to StatsType, and mutex now defaults to Debug::AssertExternallySynchronizedLock</li>
	<li>DoReThrow() overload with exception_ptr, and used in a few places</li>
	<li>use new Thread::ThrowIfDoneWithException() in ProcessRunner::Run () so we propagate exception to caller</li>
	<li>Used exception_ptr/current_exception in Thread class: added new Thread::ThrowIfDoneWithException() to support it;</li>
	<li>Instruments/MountedFilesystemUsage now has (POSIX) capture_Process_Run_DF_ overloads to work with df when it returns an error (imperfect but improved)</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a66">v2.0a66</a><br/>2015-01-11</td>
<td>
<ul>
	<li>Added WaitableEvent::WaitQueitly and WaitableEvent::WaitUntilQuietly</li>
	<li>use Function<> in Thread class, and fixed enable_if CTOR for Thread</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>
  



  



  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a65">v2.0a65</a><br/>2015-01-11</td>
<td>
<ul>
	<li>Minor fix to IRunnable backward compat Thread CTOR</li>
</ul>
</td>
</tr>







  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a64">v2.0a64</a><br/>2015-01-11</td>
<td>
<ul>
	<li>LRUCache: fixed hash table size, and further name changes / refactoring.</li>
	<li>Refactored operator== etc stuff outside of class members for Execution::Function, and suported nullptr</li>
	<li>Added performacne test comparing  IRunnable versus std::function, and based on that deprecated IRunnable, and
	switched to using std::function directly.</li>
	<li>Refactored ThreadPool due to change in using std::function</li>
	<li>Started adding qSUPPORT_LEGACY_SYNCHO to get rid of legacy syncho code</li>
	<li>Added ObjectVariantMapper::MakeCommonSerializer_ (const Execution::Synchronized<T, TRAITS>&) overload</li>
	<li>Some SFINAE improvements to Optional<> operator== etc stuff - but still inadequate</li>
	<li>Synchonized improvements, CTORs, Synchonized<>::load/store, etc</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>




  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a63">v2.0a63</a><br/>2015-01-02</td>
<td>
<ul>
	<li>Remove deprecated Tokenize</li>
	<li>Several improvements to DisjointRange/DisjointDiscreteRange (SFINAE, and bug fixes and new CTOR overloads)</li>
	<li>Fixed Range::GetDistanceSpanned, and some related typename templating magic</li>
	<li>DisjointDiscreteRange<T, RANGE_TYPE>::Elements () - redid using Generator code - so should perform better</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>


  

  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a62">v2.0a62</a><br/>2014-12-29</td>
<td>
<ul>
	<li>Fixup (name progress) on LRUCache and Synchonized (minor)</li>
	<li>New DisjointRange, and DisjointDiscreteRange classes</li>
	<li>New DiscreteRange::Elements() method, in preps for losing base class iterable</li>
	<li>new draft BidirectionalIterator and RandomAccessIterator, with revised templating for Iterator to support this</li>
	<li>restructuring operator==/!= etc for many classes, like Range, Iterator, etc, to support better LHS/RHS support</li>
	<li>get rid of CodePageConverter::MapToUNICODE assertion</li>
	<li>RangeTraits::ExplicitRangeTraitsWithoutMinMax<>::GetPrevious/GetNext()</li>
	<li>Added overload of Iterable<T>::FindFirstThat (const Iterator<T>& startAt...</li>
	<li>Added Sequence&lt;T&gt;::Insert (const Iterator<T>& i, T item) overload</li>
	<li>added TimingTrace helper (Debug module)</li>
	<li>Synchonized<> uses recursive_mutex by default</li>
</ul>
</td>
</tr>




  



  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a61">v2.0a61</a><br/>2014-12-20</td>
<td>
<ul>
	<li>Optional<T, TRAITS> changes, including new CheckedValue, new strategy for operator== etc overloads, 
	new use of Common::Equals....comarer, optimizations</li>
	<li>use CheckForThreadAborting () in SpinLock::Yield_ ()</li>
	<li>LRUCache name cleanups - migrating names to LEGACY_LRUCache ... etc as part of changeover</li>
	<li>Renamed Synchonized to LEGACY_Synchonized to help conversion process</li>
	<li>Improved EnumNames&lt;&gt; code, and fixed OffsetFromStart()</li>
	<li>Completed operator XXXX revisions for String class</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>






  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a60">v2.0a60</a><br/>2014-12-17</td>
<td>
<ul>
	<li>Patch release with fix for Queue&lt;ENUM_TYPE&gt;::length()</li>
</ul>
</td>
</tr>





  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a59">v2.0a59</a><br/>2014-12-17</td>
<td>
<ul>
	<li>Patch release with fix for EnumNames&lt;ENUM_TYPE&gt;::PeekValue()</li>
</ul>
</td>
</tr>



  

  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a58">v2.0a58</a><br/>2014-12-14</td>
<td>
<ul>
	<li>EnumNames&lt;&gt; constexpr support, as well as support for GetValue() and assertions to enforce CTOR arg order.</li>
	<li>Experimental String operator== / etc support</li>
	<li>MAJOR new code appraoch - experimental nu_Synchonized</li>
	<li>operator bool Optional&lt;&gt;</li>
	<li>qCompilerAndStdLib_constexpr_StaticDataMember_Buggy</li>
	<li>constexpr Version support</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>



  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a57">v2.0a57</a><br/>2014-12-06</td>
<td>
<ul>
	<li>Fixed Bits assertion checks</li>
	<li>Lose deprecated functions</li>
	<li>New IO::Network::Platoform::Windows::WinSock module to share code, and make easier to override startup</li>
	<li>variadic template pass extra params to make_unique_lock</li>
	<li>Ran most regtests (including valgrind)</li>
</ul>
</td>
</tr>






  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a56">v2.0a56</a><br/>2014-12-04</td>
<td>
<ul>
	<li>Configuration::EnumNames constexpr improevements</li>
	<li>DateRange difftype is duration</li>
	<li>Quickie mini-release with limited testing, due to workaround for FileAccessMode Configuration::EnumNames code</li>
</ul>
</td>
</tr>





  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a55">v2.0a55</a><br/>2014-12-01</td>
<td>
<ul>
	<li>Minor URL class cleanups (name changes, docs etc)</li>
	<li>New Configuration::Endian support</li>
	<li>New IO::Neworking::DNS module</li>
	<li>Placeholder for new Set_SparseArray, and Sequence_ChunkedArray</li>
	<li>Socket code cleanups (and fixes)</li>
	<li>Added experimental const T* CTOR (explicit) for Optional</li>
	<li>Lose some deprecated functions</li>
	<li>ThirdPartyProducts makefile cleanups, including LOSE doxygen - just use system installed version</li>
	<li>Incomplete attempts at getting constexpr working for EnumNames<> template. But at least got 
	FileAccessMode table created before main, to workaround bug / issue with HealthFrameWorksServer, which 
	generates calls to this dumptrace code before main()</li>
	<li>Quickie mini-release with limited testing, due to workaround for FileAccessMode EnumNames code</li>
</ul>
</td>
</tr>


  



  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a54">v2.0a54</a><br/>2014-11-23</td>
<td>
<ul>
	<li>Updated to support Visual Studio.net 2k13 Update 4. Ran those regtests, but thats all that changed so just release.</li>
</ul>
</td>
</tr>


  

  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a53">v2.0a53</a><br/>2014-11-22</td>
<td>
<ul>
	<li>Lose LeakChecker module, and DEBUG_NEW. It caused troubles, and was on balance not worth the trouble.
	On unix Valgrind seems to work well, and for windows, I can try Dr Memory (http://drmemory.org/)</li>
	<li>renamed Iterator2Address -> Iterator2Pointer (and fixed)</li>
	<li>Stricter CTOR for  ExternallyOwnedMemoryBinaryInputStream</li>
	<li>new and use more Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_... to improve error message reporting</li>
	<li>Configuration::EnumNames cleanups (and more use)</li>
	<li>default type args for String2Int template</li>
	<li>Lose DEBUG_NEW and LeackChecker module</li>
	<li>nu_LRUCache - started LARGE revisions</li>
	<li>new TimeZone support</li>
	<li>Improved SystemConfigruation stats gathering code (linux)</li>
	<li>tweaks to Duration::PrintAge code</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>








  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a52">v2.0a52</a><br/>2014-11-15</td>
<td>
<ul>
	<li>Added Set\<T, TRAITS\>::RemoveIf</li>
	<li>SystemPerformance/Instruments/ProcessDetails minor improvements (like Options argument) - prelim</li>
	<li>Renamed DataExchange::CharacterDelimitedLines::Reader::ReadAs2DArray to DataExchange::CharacterDelimitedLines::Reader::ReadMatrix</li>
	<li>URL cleanups - relating to parsing. Migrated OLD_Cracker to regtests; parse cleanups to URL, incliuding new
	URL::eStroikaPre20a50BackCompatMode</li>
	<li>Instrument now aggregates  ObjectVariantMapper code and used that to provide nice CaptureOnce() method(s)</li>
	<li>Very modest/incomplete testing</li>
</ul>
</td>
</tr>




  

  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a51">v2.0a51</a><br/>2014-11-10</td>
<td>
<ul>
	<li>Added Optional<>::AssignIf(), and fixed Optional::Compare() code - including != etc - to work with synchonized 
	Optional's by using an extra layer of member templates.</li>
	<li>Refactored SystemPerformance::Instruments::ProcessDetails code</li>
	<li>Fixes to SystemPerformance::Instruments::MountedFileSystem code</li>
	<li>Implemented new SystemPerformance::Instruments::Memory code</li>
	<li>New String::Tokenize() API and deprecated older Characters::Tokenize() API</li>
	<li>New DataExcahnge::CharacterDelimitedLines reader</li>
	<li>Docs</li>
	<li>new BinaryFileInputStream::mk helper, to simplify buffering. And added support for not-seekable</li>
	<li>Major rework of InternetAddress code: constexpr support, more constructors, fixed/documented net/host byte order issues,
	and support for IPv6, and better Compare behavior, and accessors for IPv4 octets.</li>
	<li>Small fixes to Range::CTOR templating</li>
	<li>Regression tests and better docs for Math::Rebin code</li>
	<li>Fixes to Network/Interfaces code for Linux, and GetInterfaces() impl for Windows: many new fields - much improved</li>
	<li>Added SocketAddress overload taking WINDOWS SOCKET_ADDRESS</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>


  

  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a50">v2.0a50</a><br/>2014-10-30</td>
<td>
<ul>
	<li>Makefile cleanups, so make -j N works pretty well, $(MAKE), and much less noisy builds of most third-party-libs, losing a few more perl scripts. Also new GetDefaultConfiguration.pl and PrintConfigurationVariable.pl script in makefiles (work towards support of multiple configs)</li>
	<li>Suport gcc-4.9, and clang++-3.5</li>
	<li>use 1.0.1j openssl</li>
	<li>Execution::ProcessRunner imporvements, including (incompatible) String args (instead of SDKString). Also, POSIX use waitpid() - still not 100% right but much closer, and throws if child returns status nonzero.</li>
	<li>DataExchange::INI::Reader bug fix (last named section was lost)</li>
	<li>Started  Framework_SystemPerformance_Instrumemnts_Memory</li>
	<li>SystemPerformance/Instruments/MountedFilesystemUsage supports Collect…ion{VolumeInfo}</li>
	<li>GetTimezone/IsDaylightSavings time fixes</li>
	<li>more explicit DataExchange::CheckedConverter{Characters::String, </li>
	<li>cleanups to CheckedConverter_ValueInRange</li>
	<li> forward inlines for Synchonized{String}...</li>
	<li>chrono::duration better integration with Stroika...Time::Duration</li>
	<li>Tested (passed regtests) on vc++2k13, gcc47, gcc48, gcc49, clang++3.4, clang++3.5, and valgrind (one small thread warning I've had forever)</li>
</ul>
</td>
</tr>




  

  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a49">v2.0a49</a><br/>2014-10-20</td>
<td>
<ul>
	<li>Fixed dangerous deadlock bug with ProcessRunner(). (POSIX ONLY).</li>
</ul>
</td>
</tr>


  

  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a48">v2.0a48</a><br/>2014-10-19</td>
<td>
<ul>
	<li>Refactored Configuration::Platform code into new Configuration::SystemConfiguration::OperatingSystem, and added tons
		of functionaly to the reset of Configuration::SystemConfiguration</li>
	<li>Several VariantValue improvements, including move constructors, and improved documentation and behavior
	on the various As converters</li>
	<li>Improvements to the POSIX ProcessRunner code - so closes unused (above 3 file) descriptors, and fixed
	fd leak, and other cleanups</li>
	<li>Set::AddIf()</li>
	<li>Better exception logging for DataExchange::BadFormatException</li>
	<li>Experiemental ObjectVariantMapper class support for ArrayElementHandling</li>
	<li>std::hash cleanups/docs my hash code</li>
	<li>Doc file scope / static Stroika Strings and regtest</li>
	<li>New String support for String::FromNarrowString () and String::AsNarrowString ()</li>
	<li>removed deprecated ExceptionPropagate code - intead use make_excpetion_ptr/current_exception</li>
	<li>minor cleanups to FileSystem::WellKnownLocations code (docs) and addition of FileSystem::WellKnownLocations::GetSpoolDirectory ()</li>
	<li>StringException now inherites from std::exception</li>
	<li>Map windows ERROR_ACCESS_DENIED to throw IO::FileAccessException ()</li>
	<li>Quite a few libcurl makefile cleanups</li>
	<li>Fixed serious bug with FileSystem::Access() check function (bacwards perm check)</li>
	<li>SystemPerformance/Instruments/ProcessDetails revisions to reduce dbgtrace noise about access errors (e.g. running as lewis and opening process data for root)</li>
	<li>Tested MSVC.net 2k13Update 3, gcc47, gcc48, clang++ 3.4, and valgrind (just one longstanding issue with leak/thread/rare)</li>
</ul>
</td>
</tr>






  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a47">v2.0a47</a><br/>2014-10-08</td>
<td>
<ul>
	<li>Spotty here and there progress on new Synchonized<> deisgn - a few more methods forwarded (Set/?)</li>
	<li>Major improvements to IO/Network/Transfer/Client code: especially libcurl code. Supported PUT and POST,
	and much improved the regression tests for these. Used BLOBs and nicely integrated Streams (and added
	some notes on future stream enhancements).</li>
	<li>enhanced use of constexpr in a number of modules</li>
	<li>Traversal::Iterator2Address</li>
	<li>Small Stream improvements, like TextInputStream::ReadAll() using StringBuilder</li>
	<li>Minor improvements to SmallStackBuffer and StringBuilder</li>
	<li>Tested with clang++, gcc48, gcc47, msvc2k13-update3, and clean report card (one minor issue) from VALGRIND</li>
</ul>
</td>
</tr>




  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a46">v2.0a46</a><br/>2014-09-28</td>
<td>
<ul>
	<li>Improved constexpr support, mostly with Time::Date/DateTime/etc..., as well as Math::Angle</li>
	<li>Big change to Execution::Synchronized: instead of subclass use aggregation and operator T () conversion operator.</li>
	<li>Small cleanups to Range/DiscreteRange</li>
	<li>proto / 1/2 done build support improvements, tried out with libcurl - so can say --libcurl=no, or buildonly or build or usesystem</li>
</ul>
</td>
</tr>




  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a45">v2.0a45</a><br/>2014-09-08</td>
<td>
<ul>
	<li>Fixed typo</li>
</ul>
</td>
</tr>




  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a44">v2.0a44</a><br/>2014-09-08</td>
<td>
<ul>
	<li>Tweak Range<> template</li>
	<li>Fixed small bug with Logger / syslog / error priority</li>
</ul>
</td>
</tr>


  
   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a43">v2.0a43</a><br/>2014-09-07</td>
<td>
<ul>
	<li>Improved use of constexpr functions - especially to Range and DiscreteRange classes.</li>
	<li>Improved Math::ReBin() code.</li>
	<li>DiscreteRange versions of some functions from Range() - except return DiscreteRange.</li>
	<li>STDCPP_VERSION_FLAG / ECHO_BUILD_LINES makefile</li>
	<li>Modest progress (and testing) on Synchonized templates</li>
	<li>Guards (debug) for SmallStackBuffer</li>
	<li>Passed regtests on clang++ 3.4, g++-4.7, g++-4.8, msvc.net 2k13 update 3, and re-ran performance tests (and saved as reference)</li>
</ul>
</td>
</tr>



  


   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a42">v2.0a42</a><br/>2014-09-03</td>
<td>
<ul>
	<li>Small fixes for portability of last release (#includes)</li>
	<li>SpinLock::try_lock() support, and use SpinLock in several more places (since faster)</li>
	<li>Added CallerStalenessCache<>::Lookup() overload</li>
	<li>constexpr functions in Configuration::Enumeration</li>
	<li>Still no significant testing (excepted tested no perf regressions from spinlock/etc changes)</li>
</ul>
</td>
</tr>





   
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a41">v2.0a41</a><br/>2014-09-02</td>
<td>
<ul>
	<li>Partial prototype of major new Synchonized<> template design
		<ul>
			<li>Containers::Optional deprecated</li>
			<li>Decent Sychonized<Memory::Optional<>> impl to replace it</li>
			<li>Placeholder so I can experiement with usage for Synchonized<Sequence<>>, Synchonized<String>, etc... most other containers</li>
		</ul>
	</li>
	<li>make_unique_lock, and qCompilerAndStdLib_make_unique_lock_IsSlow MACRO_LOCK_GUARD_CONTEXT</li>
	<li>Duration::PrettyPrintAge</li>
	<li>constexpr usage in Memory::Bits code</li>
	<li>Many improvements to POSIX procfs ProcessDetails SystemMonitor module. Now does a usable job of capturing process statistics (for analytiqa).</li>
</ul>
</td>
</tr>




  

  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a40">v2.0a40</a><br/>2014-08-08</td>
<td>
<ul>
	<li>Support for MSVC 2k13 Update 3, and other cleanups to Compiler bug defines.</li>
	<li>Practically no testing for this release</li>
</ul>
</td>
</tr>




  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a39">v2.0a39</a><br/>2014-08-06</td>
<td>
<ul>
	<li>Simple (POSIX ONLY for now) implemenation of Network Interfaces, and enumerator to list interfaces (and address bindings and status)</li>
</ul>
</td>
</tr>

  


  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a38">v2.0a38</a><br/>2014-08-06</td>
<td>
<ul>
	<li>More cleanups (BLOB overloads) of the Cryptography code</li>
	<li>Practically no testing for this release</li>
</ul>
</td>
</tr>


  


  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a37">v2.0a37</a><br/>2014-08-05</td>
<td>
<ul>
	<li>More cleanups and simplifications of the Cryptography Digest code</li>
	<li>Practically no testing for this release</li>
</ul>
</td>
</tr>

  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a36">v2.0a36</a><br/>2014-08-05</td>
<td>
<ul>
	<li>Major cleanups (not all backwards compatible) with the SystemPerformance framework. This includes a new (incomplete but usable on linux) ProcessDetails instrument (using procfs).</li>
	<li>Linux implementation of ResolveShortcut()</li>
	<li>BinaryStream and String minor additions, and cleanup of OptionsFile</li>
	<li>Cleanups (optional members and explicit type register and better merge) for ObjectVariantMapper</li>
	<li>Major cleanup/refactoring of Cryptography (esp Digest/Hash code)</li>
	<li>Lots of work adding stuff to github issues database</li>
	<li>Practically no testing for this release</li>
</ul>
</td>
</tr>









  
  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a35">v2.0a35</a><br/>2014-07-10</td>
<td>
<ul>
	<li>Many improvements to OptionsFile code, including version/upgrade support, better message handling, and much more.</li>
	<li>Fixed bug with POSIX DirectoryIterator/DirectoryIterable</li>
	<li>Essentially no testing of this release</li>
</ul>
</td>
</tr>







  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a33">v2.0a33</a><br/>2014-07-07</td>
<td>
<ul>
	<li>Fixed POSIX bug with DirectoryIterator/DirectoryIterable</li>
	<li>OptionsFile improvements (logerror/logwarning, file suffixes)</li>
	<li>Makefile / build tweaks, including progress on mkae skel, qHasFeature_zlib, and ENABLE_GLIBCXX_DEBUG configurability not strictly implied by enable-assertions, and small improvement to make clobber (still wrong)</li>
	<li>Minimal testing</li>
</ul>
</td>
</tr>




  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a32">v2.0a32</a><br/>2014-07-03</td>
<td>
<ul>
	<li>Makefile tweaks (lose runastyle.pl - do directly in makefile)</li>
	<li>Fixed Execution::ParseCommandLine() implementation to support quotes</li>
	<li>Minor improveemnts to String::CircularSubString, and helpers to StringBuilder</li>
	<li>Minor Iterator<> performance tweeks (cget instead of get to avoid clone)</li>
	<li>New DirectoryIterator/DirectoryIterable File support classes - DirectoryContentsIterator ALMOST deprecated</li>
	<li>Passed gcc48-x64/valgrind regtests, and clang++</li>
</ul>
</td>
</tr>





    

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a31">v2.0a31</a><br/>2014-06-29</td>
<td>
<ul>
	<li>A variety of fixes to Range<> code - most notably supporting data-parameterized openness on endpoints.</li>
	<li>Improved Configuration::EnumNames - so can call GetName()</li>
	<li>Did only modest testing (clang++, gcc47,gcc48 and msvc2k13 passed regtests)</li>
	<li>Passed gcc48-x64/valgrind regtests</li>
</ul>
</td>
</tr>











  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a30">v2.0a30</a><br/>2014-06-24</td>
<td>
<ul>
	<li>Fixed deadlock in Containers::Optional<>::operator*</li>
</ul>
</td>
</tr>








  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a29">v2.0a29</a><br/>2014-06-23</td>
<td>
<ul>
	<li>Quite a few improvements to UPnP SSDP code (especailly SearchListener) - fixing issues with case sensatity on compares, advertising devicetypes, and fixing ST/NT usage.</li>
	<li>New Configuration::Platform module - essentially reporting OS version (primitive but portable)</li>
	<li>Fixed bug with varargs and Logger::Log() code</li>
	<li>A variety of (mostly linux) makefile improvements - so you can now rebuild without getting messed up
	.a files - and no longer need to make clean/clobber</li>
	<li>ObjectVariantMapper improvements - Container::Optional, Collection, SortedCollection, SortedSet, and a few others. Also - refactoring to make it simpler</li>
	<li>socket_storage fix to SocketAddress class</li>
	<li>Fixed bug with missing Containers::Optional<>::operator*</li>
	<li>Added compare operators for InternetAddress (so can be stored in maps etc)</li>
	<li>Tested with clang++, gcc48, msvc2k13, and clean report card (one minor issue) from VALGRIND</li>
</ul>
</td>
</tr>






  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a28">v2.0a28</a><br/>2014-06-11</td>
<td>
<ul>
	<li>Logger::WouldLog</li>
	<li>Mapping no longer requires operator== for VALUE_TYPE () - but at a slight cost for Equals() comparisons on entire Mapping</li>
	<li>ObjectVariantMapper now supports optional preflight phase on each object reader for a CLASS (user defined type) - to implement backward comatability/schema changing for a type</li>
	<li>Very little tested</li>
</ul>
</td>
</tr>




  
  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a27">v2.0a27</a><br/>2014-06-04</td>
<td>
<ul>
	<li>Two bugfixes to ProcessRunner () - Windows. And when writing emty string to process dont stick BOM on front end</li>
	<li>String::LimitLength() overload that takes ellipsis string arg</li>
	<li>Little tested</li>
</ul>
</td>
</tr>


  




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a26">v2.0a26</a><br/>2014-05-31</td>
<td>
<ul>
	<li>Quite a few Logger class improvements, including better numbering of priorities,
	buffering of output, automatic duplication suppression, and respect of min/max logger
	priority (automatic filtering).</li>
	<li>Experiemental new EnumNames</li>
	<li>Fixed thread priority support for POSIX threads</li>
	<li>New experimental threadsafe Containers::Optional, and minor fixes to Memory::Optional</li>
	<li>Minor tweaks to BlockingQueue code</li>
	<li>Little tested</li>
</ul>
</td>
</tr>





  

<tr>
<td>
<a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a25">v2.0a25</a><br/>2014-05-29</td>
<td>
<ul>
	<li>Minor fix to CallerStalenessCache</li>
</ul>
</td>
</tr>




  

<tr>
<td>
<a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a24">v2.0a24</a><br/>2014-05-29</td>
<td>
<ul>
	<li>Minor tweaks to Time::Duration (new CTOR); and DataExchange::CheckedConverter<>.</li>
	<li>New CallerStalenessCache.</li>
	<li>OpenSSL 1.0.1g (post heartbleed)</li>
	<li>Lose some earlier deprecated functions</li>
	<li>Very little change, and largely untested.</li>
</ul>
</td>
</tr>








<tr>
<td>
<a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a23">v2.0a23</a><br/>2014-05-26</td>
<td>
<ul>
	<li>_MS_VS_2k13_Update2_FULLVER_ Visual Studio.net 2013 Update 2 compiler defines and re-ran tests.</li>
	<li>A few other minor tweaks to ObjectVariantMapper</li>
</ul>
</td>
</tr>
















<tr>
<td>
<a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a22">v2.0a22</a><br/>2014-04-25</td>
<td>
<ul>
	<li>
		Major Rework of thread-safety appraoch. This extends throughout the memory and 
		Execution and Containers and Characters/Strings classes.

		Overall, this was neutral to performance, but intrinsicially slowed things down about 3x to 10x. However,
		I paired the changes with other performance optimizations to make up.

		See the tests/Performance regtest (now #46) for details.
	</li>
	<li>
		Tons of further string performance/threading cleanups
		<ul>
			<li>Renamed String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly to String_ExternalMemoryOwnership_ApplicationLifetime and 
			String_ExternalMemoryOwnership_StackLifetime_ReadOnly to String_ExternalMemoryOwnership_StackLifetime</li>
			<li>String::InsertAt() deprecated, and new (temporary) String::InsertAt_nu which returns String object.</li>
		</ul>
	</li>
	<li>Tons of Iterable/Container thread safety fixes. These currently have a good deal of
		overhead (performance), and we need to fix that. But better they be correct, and then fast.
		Still alot more work todo just DOCUMENTING the thread-safety issues.
		And I did a ton to ameliorate/counter-act the performance overhead of the memory barriers.
		Containers and strings are Slightly slower than with v2.0a21, but only slightly. 
		And I've docuemented quite a few more things which I hope will further improve performance,
		while retaining thread safety.
	</li>
	<li>
		<li>Refactor Containers/Private/DataStructures to Containers/ExternallySynchonizedDataStructures (experimental)</li>
		<li>New Containers/UpdatableIterbale - part of thread safety fixes)</li>
	</li>
	<li>Deprecated WaitTimedOutException in place of new improved TimeOutException, and ThrowTimeoutExceptionAfter is now templated</li>
	<li>SpinLock does yield</li>
	<li>qStroika_Foundation_Traveral_IterableUsesSharedFromThis_ , and use Memory::SharedPtr (and enable_shared_from_this) in several
	ther classes like BLOB - as a performance tweak</li>
	<li>qStroika_Foundation_Memory_SharedByValue_DefaultToThreadsafeEnvelope_ - even though now is big performance sink, probably best in the
	long run
	</li>
	<li>Renamed Execution::ExternallySynchronizedLock to Debug::AssertExternallySynchronizedLock</li>
	<li>
		Lose TRAITS support from Stack<>, Queue<>, and Deque<>, since simpler, and only needed for equals(). Instead, use Equals
		tempalte param on Equals<> method
	</li>
	<li>New DataStructures/OptionsFile</li>
</ul>
</td>
</tr>








<tr>
	<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a21">v2.0a21</a><br/>2014-03-12</td>
	<td>
		<ul>
			<li>Updated license to MIT license (at least tentatively - Sterl thinks this is best)</li>
			<li>New Execution::SpinLock.
				<ul>
					<li>Used in BlockAllocated<> - made a big speedup of regtests (apx 20-100%)</li>
				</ul>
			</li>
			<li>New DataExchage::INI file support</li>
			<li>BlockAllocation-related cleanups - renamed BlockAllocated<> to AutomaticallyBlockAllocated<> and added
			ManuallyBlockAllocated<>. Other related cleanups.</li>
			<li>Revived SharedPtr<> template (faster, and maybe add future threadsafety option)</li>
			<li>String class changes
				<ul>
					<li>Moving to IMMUTABLE backend design - like Java/C#</li>
					<li>Deprecated String::Remove, and String::RemoveAt - not because really going away, but because changing API to return a new string and
					this will make it easier to avoid code subtly depending on old behavior. DO switch next release.
					</li>
					<li>SubString() used to have default second arg: instead overload of 2 versions. NOT BACKWARD COMPAT if someone
						passed a value > end of string to second arg - now assert error!
					</li>
					<li>tons string classlib improvemnts - maybe fix threadsafty issue</li>
					<li>Lots of performance improvements on string classlib</li>
					<li>Deprecated several concrete subclasses (because going to immutable string design), and use
					StringBuilder instead</li>
					<li>New StringBuilder class</li>
				</ul>
			</li>
			<li>Fixed long-standing bug with canceling a thread while its starting another. Minor issue but I think
			that was causing some failures of regtests - especailly test 34/threads/gcc/linux (clang++ issue is another issue)</li>
			<li>New Execution::ExternallySynchronizedLock class</li>
			<li>Stroika performance measurements tool and regression test. Little done to improve performance, but 
			at least now we have baseline measurements of some key benchmarks. Motivated by neeed to evaluate costs of
			String threadsafety.</li>
			<li>Math::ReBin() draft</li>
			<li>Duration/Time/etc Range () code now fixed for 'startup before main()' races, and todo comments on how to fix more performantly in
			the future</li>
			<li>SetStandardCrashHandlerSignals() incompatable second argument changes, and improved/factoring</li>
			<li>Duration::PrettyPrint and Lingustics/Word Pluralize etc all use new String class (instead of std::wstring/string). This isn't backward comptable, but close. (so we can use String_Constant in more places).</li>
			<li>Implemented Mapping<>::Keys () and added regtest for it</li>
		</ul>
	</td>
</tr>






<tr>
<td>
  <a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a20">v2.0a20</a><br/>2014-02-09</td>
<td>
<ul>
	<li>Move String_Constant to its own file in Characters/String_Constant.h. Use of Characters::Concrete::String_Constant is deprecated.
		Generally this is fixed by just changing #include of Concrete/String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly.h to
		String_Constant.h
	</li>
	<li>WaitableEvent changes:
		<ul>
			<li>WaitableEvent now requires a parameter indicating auto-reset or manual reset.</li>
			<li>New experimental WaitableEvent::WaitForAny*</li>
		</ul>
	</li>
	<li>new Function<> object - like std::function - but can be used for callbacks</li>
	<li>String class - progress on - but mostly docs - on envelope thread safety</li>
	<li>Sequence<T> no longer has TRAITS support - I decided it was simpler to just pass in equals comparer on
		methods that needed it (template param) - rationale documented in class docs header.
	</li>
	<li>SystemPerformance framework
		<ul>
			<li>Variety of framework clenaups so a more respectable first draft</li>
			<li>SystemPerformance Client now works well</li>
			<li>New Capturer module - used in SystemPerformance Client to demonstrate. Incomplete, but usable.</li>
		</ul>
	</li>
	<li>Factored String2Int<>, HexString2Int(), ... to String2Int.h; String2Float<> to String2Float.h, and Float2String<>(and its related options class) to Float2String.h</li>
	<li>Fixed POSIX GetTickCount() to use monotonic clock</li>
	<li>Characters::StripTrailingCharIfAny deprecated and new String methods to support replacement</li>
</ul>
</td>
</tr>






  <tr>
	<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a19">v2.0a19</a><br/>2014-02-02</td>
	<td>
		<ul>
			<li>Started SystemPerformance framework (mostly placeholders)</li>
			<li>Started Containers Association, SortedAssociation, and Bag (just placeholders)</li>
			<li>New DataExchange::Atom</li>
			<li>Renamed Execution::Event -> Execution::WaitableEvent (old name around but deprecated)</li>
			<li>Draft of AbortableMutex (and list things todo); 
			Lose lose qEVENT_GCCTHREADS_LINUX_WAITBUG, and fixed Event (WaitableEvent) class.
			Net effect is making Thread::AbortAndWaitTilDone() much faster. Lots of related docs/things todo
			</li>
			<li>ThreadPool clenaups</li>
			<li>make run-tests VALGRIND=1
				<ul>
					<li>default suppression file - not perfect results, but good</li>
					<li>Alot of work here to cleanup valgrind results</li>
					<li>Cleanup issues with Trace code (module init startup)</li>
					<li>Now just two problem areas</li>
				</ul>
			 </li>
			<li>make run-tests (makefile cleanups +) REMOTE= (so can run via ssh on remote host)</li>
			<li>Renamed Tally<> to MultiSet<></li>
			<li>Iterator/Iterable cleanups (much docs) - but also deprecated ApplyStatic (use Apply), and deprecated
			ApplyUntilTrue, and ApplyUntilTrueStatic (use FindFirstThat).</li>
			<li>Major speed tweeks (Iterable::Apply/FindFirstThat, and things that use it, 
			and fix to String memory optimization qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings).</li>
			<li>CopyableMutex is deprecated</li>
		</ul>
	</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a18">v2.0a18</a><br/>2014-01-24</td>
<td>
<ul>
	<li>renamed Stroika/Foundation/IO/Network/NetworkInterfaces.h to  Stroika/Foundation/IO/Network/LinkMonitor.h, and added new feature to monitor
		network connections, and used this to improve the stability / robustness of SSDP server support.
	</li>

	<li>Leveraging new IteratorOwner code, major backend-refactoring of how we manage patching of iterators
		associated with a container to fix a long standing bug (that iterators must be associated with originating owner
		to preseve proper semantics). That allowed fixing / cleaning up code in Tally<> - that had workarounds for this.
		(note - there was nothing special about Tally<> excpet that it was the first place I implemented sub-iterators,
		where you have an Elements and UniqueElements() iterable that were trivial wrappers on the original base iterator).
	</li>

	<li>Many refinements to SharedByValue code, including variadic template use to forward extra paraemters to Copy, and more sophisticated use of r-value references and noexcept.</li>

	<li>Started adding ...Iterators have a new owner (aka iterable/container) property and refined Equals() semantics</li>

	<li>Lots of cleanups of threads and signal handling code. Improved supression of abort. 
	Improved impl and docs on 'alertable' stuff for WinDoze. 
	thread_local with std::atomic/volatile.</li>
	
	<li>Significant cleanup of templates/containers - using 'using =' etc</li>
</ul>
</td>
</tr>





<tr>
<td>
  <a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a17">v2.0a17</a><br/>2014-01-10</td>
<td>
<ul>
<li>Fixed bug with Thread::SuppressAbortInContext</li>
</ul>
</td>
</tr>




<tr>
<td>
<a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a16">v2.0a16</a><br/>2014-01-10</td>
<td>
<ul>
<li>Minor</li>
</ul>
</td>
</tr>





<tr>
<td>
  <a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a15">v2.0a15</a><br/>2014-01-10</td>
<td>
<ul>
<li>Progress on makefile re-vamping</li>	
<li>OpenSSL 1.0.1f</li>
<li>Important new Thread::SuppressAbortInContext - feature to avoid a class of thread cleanup bugs</li>
</ul>
</td>
</tr>






<tr>
<td>
  <a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a14">v2.0a14</a><br/>2014-01-08</td>
<td>
<ul>
<li>Minor</li>
</ul>
</td>
</tr>




<tr>
<td>
  <a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a13">v2.0a13</a><br/>2014-01-08</td>
<td>
<ul>
<li>	
New 'safe signals' mechanism. This is not widely used, but is used
in Services Framework, so users of the services framework (or anyone using safe signals)
should add:
<pre>
	Execution::SignalHandlerRegistry::SafeSignalsManager    safeSignals;
</pre>

to main(int argc, const char* argv[])
</li>

<li>Lose legacy nativive/pthread etc thread support (just C++-thread integration).</li>

<li>Use zlib 1.2.8</li>

<li>	
Experimental new ContainsWith() and EachWith()
</li>
</ul>
</td>
</tr>



</table>
