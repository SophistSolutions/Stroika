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
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a60">v2.0a60x</a><br/>2014-12-??</td>
<td>
<ul>
	<li>TODO</li>
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
	<li>Refactored Configuration::Platform code into new Configuraiton::SystemConfiguration::OperatingSystem, and added tons
		of functionaly to the reset of Configuraiton::SystemConfiguration</li>
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
		I paired the changes with other performance optimizaitons to make up.

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
