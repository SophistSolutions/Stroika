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
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a180">v2.0a180x</a><br/>2016-11-05</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a179...v2.0a180</li>
		<li>ThirdPartyComponents
			<ul>
				<li>SQLite 3.15.1</li>
				<li>use OpenSSL 1.1.0b</li>
				<li>LZMA SDK 16.04 instead of 16.02</li>
			</ul>
		</li>
		<li>draft of Bijection::PreImage/Image implementations</li>
		<li>Compilers
			<ul>
				<li>Support clang++-3.9 (big rev to BuildClang.sh shell script - use cmake)</li>
				<li>Support clang++-3.9 with libc++ (one bug workaround with regexp); requires apt-get install libc++abi1</li>
				<li>gcc 6.2</li>
			</ul>
		</li>
		<li>new Documentation/Thread-Safety.md: C++-Standard-Thread-Safety and Must-Externally-Synchronize-Letter-Thread-Safety</li>
		<li>in sqlite makefile - dont rebuild .a file just because .o file missing (third party products) - speeds builds - probabbly OK</li>
		<li>document and enforce (Debug::AssertExtenrallySynchonized) new thread policy on IO/FileSystem/DirectoryIterator and DirectoryIterable</li>
		<li>small fix to case of !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy</li>
		<li>possible memleak in posix DirectoryIterator fixed (very rare); but mostly lose deprecated readdir_r, and just use readdir</li>
		<li>renamed Sequence (GetLast/GetFirst to First); made them more clearly document/hide Iterbale versions; better document Iterbale verisons; and amke First/Last return optional (instead of assert); and added overload 'that' for First/Last to do handy search (first that meets criteria)</li>
		<li>migrated format-code to ScriptsLib/FormatCode.sh so that shellscript can be re-used by other projects more easily (and added .clang-format file and started experimeneting with clang-format)</li>
		<li>**********HistoricalPerformanceRegressionTestResults/PerformanceDump-2.0a180-{x86-vs2k15,linux-gcc-6.1.0-x64}.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a180-OUT.txt</li>
				<li>vc++2k15 Update 3.2</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>gcc 6.2</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>clang++3.9..0 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined, and debug/release builds (tried but not working threadsanitizer) on tests</li>
				<li>1 bug with regtest - https://stroika.atlassian.net/browse/STK-535 - some suppression/workaround (qIterationOnCopiedContainer_ThreadSafety_Buggy) - and had to manually kill one memcheck valgrind cuz too slow</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a179">v2.0a179</a><br/>2016-10-13</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a178...v2.0a179</li>
		<li>https://stroika.atlassian.net/browse/STK-535 - issue with regtest [37]	Foundation::Execution::ThreadSafetyBuiltinObject -Test10_MutlipleThreadsReadingOneUpdateUsingSynchonizedContainer_
			<ul>
				<li>TONS DONE - BUT STILL INEFFECTIVE; NOT a regression (apparently), just a newly noticed bug Test10_MutlipleThreadsReadingOneUpdateUsingSynchonizedContainer_ is a a new test</li>
				<li>STILL NOT FIXED - but worked on alot - and possibly slightly better - but possibly not (maybe just slower mmissing core problem)</li>
				<li>qIterationOnCopiedContainer_ThreadSafety_Buggy to trigger bug workaroudn (temporarily) and document hacks (which failed) to fix qIterationOnCopiedContainer_ThreadSafety_Buggy/https://stroika.atlassian.net/browse/STK-535</li>
				<li>modest assertexternallysync cleanups to Foundation/Containers/Private/IteratorImplHelper - but mostly docs</li>
				<li>PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::_ApplyToEachOwnedIterator () utility</li>
				<li>assignment operators on patchable iterstors =delete (cuz not implemetned in base class and apepars not needed)</li>
				<li>PatchableContainerHelper<> massive code cleanups and Containers/Private/IteratorImplHelper</li>
				<li>big cleanup to mutex controlling update of list of active iterators. I think I fixed a race (missing locks). It was so confused/messy before, I'm not really sure.</li>
				<li>lose LOCKER param to Patcher code (just use hardiwred mutex in Pather template) - big simplifciaotn</li>
				<li>Tried breaking Containers into two reps - one with assertextenrallysynchonzied an one without -  ContainerUpdateIteratoreSafety - but had to revert. Need to track list of iterators to done breakreferences and that requires a lock (or lockfree linked list NI- https://stroika.atlassian.net/browse/STK-535 - I need the list!!! (todo writeup why)</li>
				<li>fix Containers/Concrete/Sequence_stdvector call to FiundFirstThat - to const-cast so gets non-const iterator (avoiding more costly conversion of iterator from const-to-non-const</li>
				<li>lose (never implemtned) DataStructures/STLContainerWrapper::RemoveCurrent()</li>
				<li>re-enable some shared_lock<const AssertExternallySynchronizedLock> in array claees (even if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_) - and then can simplify subclasses like Concrete/Sequence_Array (cuz asserts in base)</li>
				<li>Added STLContainerWrapper<>::remove_constness helper</li>
				<li>Tons of refactoring -0 lose macro version of locking and use explicit shared_lock/lock_guard<> on appropriate debug mutexs (fake) on fData_.</li>
				<li>In MANY palces - lose CONTAINER_LOCK_HELPER_START and use
					std::shared_lock<Debug::AssertExternallySynchronizedLock> critSec { fData_ };
				</li>
				<li>renamed Foundation/Containers/ExternallySynchronizedDataStructures -> Foundation/Containers/DataStructures; and moved Private/DataStructures/ to Foundation/Containers/DataStructures as well</li>
			</ul>
		</li>
		<li>FileSystem
			<ul>
				<li>FileSystem::RemoveFileIf/RemoveDirectoryIf now return bool saying actually did something</li>
				<li>IO::FileSystem::FileSystem::CreateSymbolicLink ()</li>
				<li>doc/example using DirecotryIterable (using Where)</li>
			</ul>
		</li>
		<li>zlib reader: improved error reporting; and read gzip supportl</li>
		<li>Range
			<ul>
				<li>Range&lt;T, TRAIT&gt;::ContainedRange</li>
				<li> Added RangeTraits::ExplicitRangeTraitsWithoutMinMax&lt...&gt;::Difference () - and used in GetDistanceSpanned () - so now Range works properly with a
				 variaty of types like Date, Duration, enums etc and this method - and thefroe DateRange&lt&gt;.Enumeration () now also works/compiles
				 (very hadny with Linq type utilties like Selec&lt;String&gt; ...</li>
				<li>Range::Format -> ToString</li>
			</ul>
		</li>
		<li>Date/Time
			<ul>
				<li>operator++, and other oeprators improvements</li>
				<li>new type Date::SignedJulianRepType - and use that in AddDays, new operator+/operator- etc minor Date improvements</li>
				<li>Date::operator- (date) and Difference call now return SignedJulianRepType instead of Duration - so works better with Range (etc) - easier -and added regtest</li>
				<li>Redid Date / Time / DateTime constexpr kMin/kMax support to workaround qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy issue</li>
				<li>Make DateRange a DiscreteDateRange - so iterable. Added SimpleDateRange - so CAN be used as constexpr, but unlikely to use</li>
				<li>improve foramtting of Duration::PrettyPrint ... so for example 1.6e-6, outputs 1 us, 600 ns, instead of just 1 or 2 ns</li>
			</ul>
		</li>
		<li>Iterable
			<ul>
				<li>Added Iterable:: First/FirstValue/Last/LastValue()</li>
				<li>Added regtests with Select (and docs)</li>
				<li>https://stroika.atlassian.net/browse/STK-532 : fixed Iterable<T>::Take/Iterable<T>::Skip/Iterable<T>::Select etc.... several that returns iterable<> - commit 697b99a8456364f4e4196da6d91cb46fd73b8c77, and regtest commit c5bad743f973add82d1cee6e851ab2c369f5948d</li>
			</ul>
		</li>
		<li>Compiler bug defines
			<ul>
				<li>Cleanup qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy (not so widely broken - was anothe issue for nerwer compierls)</li>
				<li>qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy was the real issue - and still a problem (not clear compiler bug or lewis bug)</li>
			</ul>
		</li>
		<li>Web Server
			<ul>
				<li>webserver Request::GetContentLength () helper added</li>
				<li>fixed serious bug in Request::GetBody - dont call ReadAll (0-size) - but just directly return empty blob</li>
			</ul>
		</li>
		<li>Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE(X) and Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER(X) macros defined</li>
		<li>ProcessRunner::Exception class added wtih details of results and hopefully better messages; now throwin for a few cases like sub-process-failed</li>
		<li>tweak move ctors for Containers</li>
		<li>https://stroika.atlassian.net/browse/STK-541 - disable Mapping::CTOR(&amp;&amp;) move constructor - cuz crashes on unix - not sure why</li>
		<li>Draft (start at) linedlist lockfree  - new module LockFreeDataStrucutres</li>
		<li>https://stroika.atlassian.net/browse/STK-539 workaround for Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RetainAll ()</li>
		<li>Assert macro (and related) now use (a||b) - instead of if(a){b} - so Assert is now an expression, not a statement, and as such, can more easily be used in mem-initializers (or palces expecting an expression)</li>
		<li> https://stroika.atlassian.net/browse/STK-126 - after pretty careful consideraiton - lose optional SeekOffsetType* offset to read calls</li>
		<li>Lose obsolete qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_ feautre - https://stroika.atlassian.net/browse/STK-530 commit 9bca98b6cbdf035865db92d347bd811816194c90</li>
		<li>ElementType renamed value_type  commit 126d1bfbc385fe9c02b9184b7af5bc3a12f95444 - Foundation/Common/Compare etc...</li>
		<li>HistoricalPerformanceRegressionTestResults/PerformanceDump-2.0a179-{x86-vs2k15,linux-gcc-6.1.0-x64}.txt: NOTE - this release BASICALLY UNDOES all the performance gains with containers since 2.0a170. First focus on correctness, and then go back and tweak!</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a179-OUT.txt</li>
				<li>vc++2k15 Update 3.2</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined, and debug/release builds (tried but not working threadsanitizer) on tests</li>
				<li>1 bug with regtest - https://stroika.atlassian.net/browse/STK-535 - some suppression/workaround (qIterationOnCopiedContainer_ThreadSafety_Buggy) - and had to manually kill one memcheck valgrind cuz too slow</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a178">v2.0a178</a><br/>2016-09-20</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a177...v2.0a178</li>
		<li>new vs2k15 patch release - _MS_VS_2k15_Update3_02_FULLVER_ - assumed same bugs</li>
		<li>Interable thread safety and performance improvements
			<ul>
				<li>(still SOMEWHAT risky/experimental, but much more confidient this is fine)</li>
				<li>qStroika_Foundation_Containers_ExternallySynchronizedDataStructures_STLContainerWrapper_IncludeSlowDebugChecks_, qStroika_Foundation_Containers_ExternallySynchronizedDataStructures_Array_IncludeSlowDebugChecks_, etc</li>
				<li>https://stroika.atlassian.net/browse/STK-531 - first draft adding AssertExternallySynchronizedLock support to Containers/Private/DataStructures...stl/extenrallysynconized</li>
				<li>corrected misundersnatding - auto of an expression returning a reference doesnt produce a refernece value i nthe auto - so use auto&amp; - so now we dont copy iteratorreps anymore in various concrete impls of studff using iterators - (so no need to worry about corruption of thier linked list)</li>
				<li>Comments about https://stroika.atlassian.net/browse/STK-530 aka qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_ bug report (losing)</li>
				<li>added explicit  IteratorImplHelper_ (const IteratorImplHelper_&amp;) = default; and comemnt we must be careful about synchonization on this method; one place I know we call it - I added wrapper of CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (and others in a second will be eliminated)</li>
			</ul>
		</li>
		<li>OpenSSL
			<ul>
				<li>use #include openssl instead of tying to replacehide/hide definitions - in openssl subdir modules anyhow</li>
				<li>https://stroika.atlassian.net/browse/STK-488 - got building and passing regtests on windows (openssl 1.1.0)</li>
				<li>temporary workaroudn for https://stroika.atlassian.net/browse/STK-527 bug - diff  libcurl build workaround needed for newer openssl</li>
				<li>notes about +#https://stroika.atlassian.net/browse/STK-528 - using nasm for building openssl</li>
			</ul>
		</li>
		<li>lose lots of deprecated members - eg. ElementType; KeyType;/ValueType and change remaining code to use element_type and mapped_type and key_type</li>
		<li>libcurl use 7.50.3</li>
		<li>sqlite use 3.14.2 </li>
		<li>more data hiding with Frameworks/WebServer</li>
		<li>new Sequence<>::GetFirstIf/GetLastIf helpers</li>
		<li>speed tweak on AssertExternallySynchronizedLock::unlock_shared () - DEBUG code</li>
		<li>regression test harness PrintPassOrFail() now returns value to be passed out of main instead of calling exit - so fewer false leak reports on failure</li>
		<li>HistoricalPerformanceRegressionTestResults/PerformanceDump-2.0a178-{x86-vs2k15,linux-gcc-6.1.0-x64}.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a178-OUT.txt</li>
				<li>vc++2k15 Update 3.2</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined, and debug/release builds (tried but not working threadsanitizer) on tests</li>
				<li>1 bug with regtest - KILLED on Builds/gcc-release-sanitize/Test48 - OK cuz just ran out of memory for sanitizer code </li>
				<li>1 bug with regtest - FAILED: RegressionTestFailure; caughtExceptAt <= expectedEndAt + kMargingOfErrorHi_;;/home/lewis/Sandbox/Stroika-Reg-Tests-Dev/Tests/36/Test.cpp: 259 - just adjust timeout or ignore</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a177">v2.0a177</a><br/>2016-09-14/td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a176...v2.0a177</li>
		<li>***EXPERIMENTAL RELEASE TO EVALUATE https://stroika.atlassian.net/browse/STK-525 - qContainersPrivateSyncrhonizationPolicy_DebugExternalSyncMutex_ - RISKY but big performance improvement with containers</li>
		<li>Fix alignas usage in SmallStackBuffer - fBuffer</li>
		<li>a few helpful overloads of Thread methods (Wait*, Start)</li>


		<li>HistoricalPerformanceRegressionTestResults/PerformanceDump-2.0a177-{x86-vs2k15,linux-gcc-6.1.0-x64}.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a177-OUT.txt</li>
				<li>vc++2k15 Update 3.1</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a176">v2.0a176</a><br/>2016-09-12</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a175...v2.0a176</li>
		<li>hack to test https://stroika.atlassian.net/browse/STK-525 - experimental hack that may make container classes much faster (STILL NOT USED - MAYBE IT WILL HELP)</li>
		<li>Iterator/Iterable/Mapping changes to use more STL-like type names
			<ul>
				<li>added value_type typedef for Iterable and Iterator (REPLACING OBSOLETED/DEPRECATED ElementType)</li>
				<li>start adding std::map<> like type aliases for Mapping<> - key_type and mapped_type - DEPRECATING ValueType and KeyType</li>
			</ul>
		</li>
		<li>Containers::Adapters::Adder: support Collection<> type; use new name value_type instead of ElementType</li>
		<li>USE_NOISY_TRACE_IN_THIS_MODULE_ support in VariantValue code; and DefaultNames&lt;DataExchange::VariantValue::Type&gt;</li>
		<li>Support set<> with Containers/Adapters/Adder</li>
		<li>new BLOB::AsHex () const method</li>
		<li>use sqlite 3.14.1 (instead of 3.13)</li>
		<li>Characters::ToString
			<ul>
				<li>overload to handle arrays (and regtest)</li>
				<li>Added Version::ToString ()  support</li>
			</ul>
		</li>
		<li>various simplifications of ObjevctVariantMapper - using Adder code;</li>
		<li>LOSE DEFINES FOR OBSOLETE SYSTEMS/BUGS
			<ul>
				<li>lose gcc 4.9 support, so lost bug defines (and workarounds) for
					<ul>
						<li>qCompilerAndStdLib_make_unique_Buggy</li>
						<li>qCompilerAndStdLib_two_levels_nesting_Protected_Access_Buggy</li>
					</ul>
				</li>
				<li>lose compiler bug defines needed for clang 3.6 (had been desupported):        
					<ul>
						<li>qCompilerAndStdLib_string_conversions_Buggy</li>
						<li>qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy</li>
						<li>qCompilerAndStdLib_shared_ptr_atomic_load_missing_Buggy</li>
						<li>qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy</li>
						<li>qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy</li>
						<li>qCompilerAndStdLib_string_conversions_Buggy</li>
						<li>qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy</li>
					</ul>
				</li>
				<li>lose obsolete qCompilerAndStdLib_is_trivially_copyable_Buggy bug workaround</li>
				<li>Lose AIX (qPlatform_AIX) compatability - since I no longer have the need or ability to build for AIX so lose defines:
					<ul>
						<li>qPlatform_AIX</li>
						<li>qCompilerAndStdLib_AIX_GCC_TOC_Inline_Buggy</li>
						<li>qCompilerAndStdLib_fdopendir_Buggy</li>
						<li>qCompilerAndStdLib_Locale_Buggy (use !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy instead in many places)</li>
					</ul>
				</li>
				<li>Remove bug workaround/define for qCompilerAndStdLib_COutCErrStartupCrasher_Buggy thanks to Stephan T. Lavavej @ MSFT for hints</li>
			</ul>
		</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a176-OUT.txt</li>
				<li>vc++2k15 Update 3.1</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a175">v2.0a175</a><br/>2016-09-03</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a174...v2.0a175</li>
		<li>Synchonized&lt;&gt; RWSynchonized support - now load () method really uses shared_lock</li>
		<li>Frameworks/webserver
			<ul>
				<li>Interceptor support</li>
				<li>Router now is an interceptor so integrates better</li>
				<li>Use HandleFault code from Intercetor for some fault (e.g. 404/exception) handling</li>
				<li>refactored webserver code so new Message object that owns Request/Response and moved GetPeerAddress there</li>
				<li>Docs cleanups</li>
				<li>wrappers on data, and thread safety checks asserts with Debug::AssertExternallySynchonized</li>
				<li>ConnectionManager now owns its own interceptor to add in server header and CORs stuff</li>
				<li>ConnectionManager now delegates server loop to Connection::ReadAndProcessMessage () - so closer
    	to support (not there yet) for keep-alives.</li>
			</ul>
		</li>
		<li>HistoricalPerformanceRegressionTestResults/PerformanceDump-2.0a175-{x86-vs2k15,linux-gcc-6.1.0-x64}.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a173-OUT.txt</li>
				<li>vc++2k15 Update 3.1</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>


 



<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a174">v2.0a174</a><br/>2016-08-26</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a173...v2.0a174</li>
		<li>Thread::SuppressInterruptionInContext in ThreadPool DTOR - so it safely shuts down all owned threads</li>
		<li>in Frameworks/WebServer/ConnectionManager - reorder fListenr and fThreads (threadpool) object - so we default DTOR stops listeneer before killing threadpool</li>
		<li>SKIPPED REGRESSION TESTS</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a173">v2.0a173</a><br/>2016-08-25</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a172...v2.0a173</li>
		<li>{MAJOR}: Fixed Optional<> (requires much testing and reaction) to support multiple readers of const methods - using shared_lock of  AssertExternallySynchronizedLock on const methods; and fixed Optional<T, TRAITS>::AccumulateIf to do AssertExternallySynchronizedLock cuz that class is now recursive</li>
		<li>Foundation/IO/Network/Listener CTOR now takes backlog argument: NOTE - NOT FULLY backward compatible - because default is 1, and before we had hardwired 10</li>
		<li>Framework/WebServer
			<ul>
				<li>ToString support (request/response/response-state)</li>
				<li>fServerHeader_ must be synchonized in ConnectionMnaager because there is a  setter method</li>
				<li>Frameworks/WebServer/ConnectionManager takes new option arg - maxConnecitons; uses that for threadpool, and now uses threadpool instead of a single thread it blcoks on in onConnect"</li>
				<li>    Added Request::GetPeerAddress () - and pass it into the Request from the Conneciton</li>
			</ul>
		</li>
		<li>pthread_setschedprio assert calls fixed to check for if not succeed, errno = EPERM</li>
		<li>UNIX regression test cleanups - some name cleanups, lose gcc49, added no-third-party-components config</li>
		<li>Noticed I wasnt testing gcc49, and a bug (compiler crash) crept in. Not sure we need it  so begin process of de-supporting gcc49</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a173-OUT.txt</li>
				<li>vc++2k15 Update 3.1</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a172">v2.0a172</a><br/>2016-08-22</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a171...v2.0a172</li>
		<li>https://stroika.atlassian.net/browse/STK-519 - New Foundation/DataExchange/Compression/ module - with (limited but functional) zlib compression/decompression support (with regtests)</li>
		<li>refactor - DataExchange::{ArchiveReader,7z,Zip} -> DataExchange::Archive/{Reader,XML,7z} (NOT SRC COMPATIBLE)</li>
		<li>refactor- DataExchagne/{VariantReader,VariantWriter,someofxml,INI,JSON,CharacterDelimitedLines} -> DataExchange/Variant/ (NOT SRC COMPATIBLE)</li>
		<li>changed PERFORMANCE REGTEST qPrintOutIfFailsToMeetPerformanceExpectations includes qAllowBlockAllocation and !!qDefaultTracingOn so we dont have to keep weakening timing constraints</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a172-OUT.txt</li>
				<li>vc++2k15 Update 3.1</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a171">v2.0a171</a><br/>2016-08-21</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a170...v2.0a171</li>
		<li>Support _MS_VS_2k15_Update3_01_FULLVER_</li>
		<li>https://stroika.atlassian.net/browse/STK-518 - CompilerAndStdLib_AssumeBuggyIfNewerCheck_ () to
			try and improve the situation when people use Stroika with compilers newer than we've tested
		</li>
		<li>Added qCompilerAndStdLib_std_get_time_pctx_Buggy to capture reasons for workaround code
			in TimeOfDay::Parse(...locale) - I think the std::get_time way is better but doesnt work on windows
			cleanup some obsolete diffenrt stratgies in TimeOfDay::Parse()
		</li>
		<li>tweak kMargingOfErrorHi_ and VerifyTestResult () checks - to avoid/get better info on rare verifytestresults() test fauilure</li>
		<li>Tested (passed regtests)
			<ul>
				<li>OUTPUT FILES: Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-{Linux,Windows}-2.0a171-OUT.txt</li>
				<li>vc++2k15 Update 3.1</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








  
<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a170">v2.0a170</a><br/>2016-08-19</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a169...v2.0a170</li>
		<li>Raspberrypi regression tests now regularly run --sanitize address,undefined
			<ul>
				<li>required updating my raspberypi to 'testing' in /etc/apt/sources.list (for libasan.so.2)</li>
				<li>added to regression-test-configurations</li>
				<li>support new +REMOTE_RUN_PREFIX= hack on running remote regression tests - to work around issue with address sanitizer	
					REMOTE_RUN_PREFIX=LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.2</li>
			</ul>
		</li>
		<li>Alignas
			<ul>
				<li>Due to testing --sanitize undefined on raspberrypi...</li>
				<li>Fixed several places where we had WRONG or missing alignas</li>
				<li>workaround new qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy define</li>
				<li>Not sure what bad alignment could have done (speed or bad semantics) - but COULD have caused subtle ARM bugs?</li>
			</ul>
		</li>
		<li>https://stroika.atlassian.net/browse/STK-513 - had crash (originally due to time bug/issue) - which ONLY 
			affected ARM/raspberry pi target, and ONLY when address sanitizer running. Side effect, I changed the format of
			the filename (after lots of back and forth unimportant changes)
		</li>
		<li>Time bugs
			<ul>
				<li>longstanding bug with TimeOfDay::ClearSecondsField () - it must be a noop when called when 'empty' (not sure why only showed up with address sanitizer on arm)</li>
				<li>docs on Time::IsDaylightSavingsTime () - and added VERIFY call - that mktime didnt fail - https://stroika.atlassian.net/browse/STK-515</li>
				<li>Math::NearlyEquals (Time::DateTime l,... now uses As<time_t> isntead of ToTickCount()</li>
				<li>more assert calls in tm  DateTime::As () etc</li>
				<li>docs that TimeOfDay ensures result <  kMaxSecondsPerDay and added ensure where mktime retunrs -1 (wasnt checking</li>
				<li>https://stroika.atlassian.net/browse/STK-516 DateTime::AddSeconds (int64_t seconds) - if adding days only - dont convert 'emty' datetime to 0 datetime</li>
			</ul>
		</li>
		<li>Frameworks/WebServer/ConnectionManager
			<ul>
				<li>Minor debug cleanups</li>
				<li>improved error reporting in Frameworks/WebServer/ConnectionManager - defaults - for exceptions</li>
			</ul>
		</li>
		<li>openssl 1.1 - https://stroika.atlassian.net/browse/STK-488
			<ul>
				<li>rogress on getting openssl 1.1 (beta 6) working on windows. Now builds tpc on windows (did for a while on unix); 
				still not right - some flags wrong. But since I can switch between 1.0 and 1.1 for now - we can check this in</li>
				<li>fixed GetString2InsertIntoBatchFileToInit64BitCompiles ()</li>
			</ul>
		</li>
		<li>https://stroika.atlassian.net/browse/STK-166: support Duration * float, / float, and that fixes DateTimeRange::GetMidpoint()</li>
		<li>HistoricalPerformanceRegressionTestResults/PerformanceDump-v2.0a170-{x86-vs2k15,linux-gcc-6.1.0-x64}.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-2.0a170-OUT.txt</li>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Windows-2.0a170-OUT.txt</li>
				<li>vc++2k15 Update 3 </li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie-testing): --sanitize address,undefined</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>gcc with --sanitize address,undefined (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a169">v2.0a169</a><br/>2016-08-16</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a168...v2.0a169</li>
		<li>revised/improved check-prerequisite-tools support (renamed target and now recursive at start for third-party-components)</li>
		<li>Started support for openssl 1.1, but incomplete and mostly not checked in; but parts were and those required changes to zlib build</li>
		<li>SQLite: small memory leak fix, and makefile improvements</li>
		<li>Tested (passed regtests)
			<ul>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-2.0a169-OUT.txt</li>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Windows-2.0a169-OUT.txt</li>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie)</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a168">v2.0a168</a><br/>2016-08-09</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a167...v2.0a168</li>
		<li>Remove BOM from sqlite Makefile (breaks build on older Unix)</li>
		<li>Quick untested release</li>
	</ul>
</td>
</tr>




  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a167">v2.0a167</a><br/>2016-08-08</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a166...v2.0a167</li>
		<li>Support Memory::BLOB in VariantValue</li>
		<li>
			SQLite work
			<ul>
				<li>New SQLite regression test</li>
				<li>Fixed bugs with sqlite thirdparty makefiles</li>
				<li>Support BLOB</li>
				<li>SQLite APIs for running queries now take format strings - not queries per-se</li>
				<li>support (untested) In memoruy DB, and CTOR for URL or Filename</li>
			</ul>
		</li>
		<li>Documentation/CommonDefaultConfigurations.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-2.0a167-OUT.txt</li>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Windows-2.0a167-OUT.txt</li>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie)</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a166">v2.0a166</a><br/>2016-08-05</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a165...v2.0a166</li>
		<li>Several SQLite fixes</li>
		<li>Largely untested release</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a165">v2.0a165</a><br/>2016-08-05</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a164...v2.0a165</li>
		<li>New SQLite support - wrapper module, and automatically build sqlite3 in third-party-products</li>
		<li>cosmetic cleanups to ConnectionManager (http): use GetCORSModeSupport () instead of  GetIgnoreCORS</li>
		<li> Lose backward compat workarounds which were only needed for gcc 48.
			<ul>
				<li>qCompilerAndStdLib_LocaleTM_put_Buggy</li>
				<li>qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy</li>
				<li>qCompilerAndStdLib_Iterator_template_MakeSharedPtr_gcc_crasher_Buggy</li>
				<li>qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy</li>
				<li>qCompilerAndStdLib_ParameterPack_Pass_Through_Lambda_Buggy</li>
				<li>qCompilerAndStdLib_regex_Buggy</li>
				<li>qCompilerAndStdLib_TemplateDiffersOnReturnTypeOnly_Buggy</li>
				<li>qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy</li>
				<li>qCompilerAndStdLib_GCC_48_OptimizerBug</li>
				<li>qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy</li>
			</ul>
		</li>
		<li>ScriptsLib/RegressionTests-Unix.sh -> ScriptsLib/RegressionTests.sh and got working on windows (poorly but something).</li>
		<li>Revised exactly what builds get run and how in regression tests</li>
		<li>New ScriptsLib/RunPerformanceRegressionTests.sh script to facilitate running performance tests on both platforms</li>
		<li>Tested (passed regtests)
			<ul>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Windows-2.0a165-OUT.txt</li>
				<li>Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-2.0a165-OUT.txt</li>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7.1 (ubuntu)</li>
				<li>clang++3.8.1 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie)</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a164">v2.0a164</a><br/>2016-07-30</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a163...v2.0a164</li>
		<li>quick release - no changes, but runing full set of regtests this time</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile to raspberry-pi(3/jessie)</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a163">v2.0a163</a><br/>2016-07-29</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a162...v2.0a163</li>
		<li>Start losing support for gcc48 (still ifdefs but no longer test)</li>
		<li>Adder overload taking pair<> with Mapping</li>
		<li>SAXParser/ObjectReaderRegistry: many new regtests and fixed Fixed https://stroika.atlassian.net/browse/STK-504</li>
		<li>String class performance improvements relative to constructor char16_t, char32_t (done by Xerces/SAX parser) - but still needs work (open https://stroika.atlassian.net/browse/STK-506)</li>
		<li>new PerformanceDump-v2.0a163-linux-gcc-6.1.0-x64.txt and PerformanceDump-v2.0a163-x86-vs2k15-ReleaseU.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 6.1</li>
				<li>clang++3.8 (ubuntu &;amp; private compiler build 3.8.1)</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a162">v2.0a162</a><br/>2016-07-28</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a161...v2.0a162</li>
		<li> Characters::ToString() support for std::exception</li>
		<li>ObjectReaderRegistry: added mkReadDownToReader () with 2 names overload, and builtin support for duration</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu &;amp; private compiler build 3.8.1)</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a161">v2.0a161</a><br/>2016-07-25</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a160...v2.0a161</li>
		<li>New experimental Containers::Adapters::Adder, and used to simplify ObjectReaderRegistery</li>
		<li>tested - and BuildClang script was working - minor cleanups and fixes to config/build (pass version arg)</li>
		<li>configure script: fixed bug with --sanitizer multiple args and added  --append-extra-compiler-and-linker-args option, and --append-extra-compiler-args --append-extra-linker-args commandline args</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu &amp; private compiler build 3.8.1)</li>
				<li>cross-compile to raspberry-pi</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a160">v2.0a160</a><br/>2016-07-22</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a159...v2.0a160</li>
		<li>Quick (untested) release to provide needed SAXReader functionality for Block</li>
		<li>experimental ContainerAdder template helper and used in ListOfObjectReader, and related regression tests</li>
		<li>Comments, and remove some obsolete code.</li>
		<li>Minor fixes to PIDLoop</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a159">v2.0a159</a><br/>2016-07-21</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a158...v2.0a159</li>
		<li>Improved support for configure --sanitiize, and --no-sanitiize, and added --sanitize address,undefined --no-sanitize vptr to regression tests, and fixed a few small complaints detected by sanitizers</li>
		<li>Improved memory guard support (more control options and sep header/footer layout)</li>
		<li>Improved makefile handing of building witih CONFIGURATION=""</li>
		<li>Added output-sync=line to MAKEFLAGS in hopes of reducing broken makefile output due to parallel makes</li>
		<li>DataExchange
			<ul>
				<li>StructuredStreamEvents/ObjectReaderRegistry MakeClassReader is now static (addclass checks if fields present but make does not accordin gto docs)</li>
				<li>StructuredStreamEvents/ObjectReaderRegistry MakeCommonReader_NamedEnumerations and MakeCommonReader_EnumAsInt and regtests of enum version</li>
				<li>Fixed ObjectReaderRegistry so default for MakeCommonReader is to use MakeCommonReader_NamedEnumerations</li>
				<li>Regtest T7_SAXObjectReader_BLKQCL_ReadSensors_::DoTest; and docs</li>
				<li>Fix reading enum whose value == END - that counts as out of range! (chagne in behavior ofor DataExchange/ObjectVariantMapper</li>
				<li>refactored ObjectReaderRegistry::RepeatedElementReader so TRAITS allow external specify of type object used in subelements and alternate append strategy</li>
			</ul>
		</li>
		<li>Support Set Equals() with iterable arg - and operator== and operator!= to handle that automatically - when comparing with a set</li>
		<li>KeyValuePair members have default initiialization</li>
		<li>VS2k15 project files now require /bigobj option (because of new regtests but easier to do for all)</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a158">v2.0a158</a><br/>2016-07-15</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a157...v2.0a158</li>
		<li>Fix compile bug defines for new gcc 5.4 release</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 5.4</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile to raspberry-pi</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a157">v2.0a157</a><br/>2016-07-14</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a156...v2.0a157</li>
		<li>Irreproducible std::terminate/crash on Test35 - Foundation::Execution::Threads - std::terminate() - but no logs and when I ran over and over no reproduce - https://stroika.atlassian.net/browse/STK-497</li>
		<li>
			 Threads
			 <ul>
				<li>Slight race in Thread::WaitForDone:  https://stroika.atlassian.net/browse/STK-496 - minor changes to WaitForDone/WaitForDoneUnutl etc - in thread code. Test carefully. Very hard to reproduce</li>
				<li>Major thread bug - fThreadDone event was an autoreset event! Must be manual reset (once someone waits on done - were still DONE!</li>
				<li>slight cleanup of recent thread restructuring of waittildone code (needed manual reset event fix above to do this)</li>
				<li>cleanup - compare_exchange_strong instead of exchange - with status - slight unimportant race cleaned up</lI>
			 </ul>
		</li>
		<li>updated ScriptsLib/ExtractVersionInformation.sh to handle 3.0.1 format version</li>
		<li>malloc-guard (qStroika_Foundation_Debug_MallogGuard - ONLY for UNIX/GCC, configure --malloc-guard, to debug very hard to reproduce glibc memory corruption on BLKQCL device</li>
		<li>Small UNIX regression test cleanups</li>
		<li>fix makefile issue with xerces when its the only thirdpartylib</li>
		<li>big change to debug symbols for unix - now IncludeDebugSymbolsInLibraries config file entry to parallel IncludeDebugSymbolsInExecutables and thats checked directyly instead of COnfig.mk file flag INCLUDE_SYMBOLS (which was ambiguous and confusing); 
		instead - set +IncludeDebugSymbolsInExecutables by ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) IncludeDebugSymbolsInExecu... in makefile - and use that to add -g to LinkerPrefixArgs
		</li>
		<li>fixed Containers/ExternallySynchronizedDataStructures/Array to use delete[] - detected by AddressSanitizer</li>
		<li>Started adding more regtests - in preps for valgrind on arm, and addressanitizer/threadsantiizer tests</li>
		<li>added PerformanceDump-v2.0a157-linux-gcc-6.1.0-x64.txt PerformanceDump-v2.0a157-x86-vs2k15-ReleaseU.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile to raspberry-pi</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
				<li>AddressSanitizer (tried but not working threadsanitizer) on tests</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a156">v2.0a156</a><br/>2016-07-11</td>
<td>
	<ul>
		<li>https://github.com/SophistSolutions/Stroika/compare/v2.0a155...v2.0a156</li>
		<li>SpinLock
			<ul>
				<li>https://stroika.atlassian.net/browse/STK-494 - attempt at workaround for possible bug WITH SpinLock - optional BarrierFlag argument (defaults to acquire/release)
				option to SpinLock CTOR (may fix/address bug on ARM / BLKQCL device - malloc corruption)</li>
				<li>use qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex  - and set TRUE for msvc, and false for other compilers cuz gcc faster with mutex (limited testing) - change due to memory fence - slows down more on unix/gcc</li>
				<li>This fix (494) noticably slowed regtests (5-10%)</li>
				<li>Added qStroika_Foundation_Memory_BlockAllocator_UseLockFree_ code - for blockallocator - which undid much of the loss, but still probably about a percent slower than v2.0a155 (depending on what you measure)</li>
			</ul>
		</li>
		<li>Spelling error correction: renamed interupt -> interrupt</li>
		<li>thread code - fAccessSTDThreadMutex_ to protect access to fThread_ - handles a rare race detected by Helgrind on UNIX</li>
		<li>https://stroika.atlassian.net/browse/STK-477 - experimental rework of thread interuption TLS variable(s) - instead of two - down to 1 (two bools replaced with one enum) - must test more to see works, and Thread::Rep_::NotifyOfInteruptionFromAnyThread_ use of compare_exchange_strong to be simpler/faster/clearer</li>
		<li>docs and support FullVersionString opt for ScriptsLib/ExtractVersionInformation.sh, and whitespace fix</li>
		<li>{NOT BACKWARD COMPAT}:renamed BitSubstring_NEW to BitSubstring, and BitSubstring_NEW still around breifly deprecated</li>
		<li>Assertions/Tweaks/Cleanups in SystemPerformance/Instruments/Process</li>
		<li>lose obsolete qSupportValgrindQuirks - from a much earlier attempt at valgrind supprot - these hacks no longer needed</li>
		<li>Synchnonized
			<ul>
				<li>Lose deprecated Synchonized<>::get()</li>
				<li>Synchonized operator-> and cget() return non-const ReadableReference - so it can be moved. Safe to return non-const ebcause no non-const methods on ReadableReference anyhow (no way to modify data) - just allows moving the reference (handy in many cases)</li>
				<li>deprecated Synchonized<> :: GetReference() - use rwget/cget instead</li>
			</ul>
		</li>
		<li>added (test case) valgrind / helgrind on Samples_SystemPerformanceClient</li>
		<li>Fixed where we write PerformanceDump to - Build output directory</li>
		<li>renamed Thread::GetSignalUsedForThreadAbort -> Thread::GetSignalUsedForThreadInterrupt</li>
		<li>Added PerformanceDump-v2.0a156-x86-vs2k15-ReleaseU.txt and PerformanceDump-v2.0a156-linux-gcc-6.1.0-x64.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile to raspberry-pi</li>
				<li>valgrind Tests (memcheck and helgrind), helgrind some Samples</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a155">v2.0a155</a><br/>2016-06-29</td>
<td>
	<ul>
		<li>various code cleanups to Synchonized<>: use Configuration::ArgByValueType<T>, clarify/fix operator T(), docs cleanups, and fixed regression with oeprator-=, opeartor+=</li>
		<li>Xerces: updated to use 3.1.4, and updated list of mirrors</li>
		<li>Started transition of BitSubstring() API - by first creating BitSubstring_NEW, and deprecating BitSubstring (so we can transition through intermediate name as we chane meaning of 3rd parameter)</li>
		<li>Added PerformanceDump-v2.0a155-x86-vs2k15-ReleaseU.txt and PerformanceDump-v2.0a155-linux-gcc-6.1.0-x64.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a154">v2.0a154</a><br/>2016-06-28</td>
<td>
	<ul>
		<li>Synchonized&lt;&gt;- lose deprecated operator-> non-const - having it around still generates deprecation warnings cuz of how it matched.</li>
		<li>Quick release- untested</li>
	</ul>
</td>
</tr>



<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a153">v2.0a153</a><br/>2016-06-28</td>
<td>
	<ul>
		<li>Synchonized&lt;&gt;- fixed operator+=/operator-= to not use deprecated operator->; and changed choice - const operator-> () is OK - no longer deprecated</li>
		<li>Quick release- untested</li>
	</ul>
</td>
</tr>



  



<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a152">v2.0a152</a><br/>2016-06-27</td>
<td>
	<ul>
		<li>Switch to VisualStudio 2k15 Update 3</li>
		<li>Not BINARY backward compatible in binary format of Win32 4-byte version#s nor the binary format of Stroika verison#s</li>
		<li>renamed Samples and Tests solution to have 'Stroika-' prefix</li>
		<li>new PerformanceDump-v2.0a152-linux-gcc-6.1.0-x64.txt and PerformanceDump-v2.0a152-x86-vs2k15-ReleaseU.txt (on the surface, appears somewhat faster than update2)</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 3</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a151">v2.0a151</a><br/>2016-06-23</td>
<td>
	<ul>
		<li>https://stroika.atlassian.net/browse/STK-436 - Each Configuration file should have a comment at the head saying how to run (args) configure</li>
		<li>Discovered MSFT has qCompilerAndStdLib_cplusplus_macro_value_Buggy! so had been disabling shared_lock stuff for a while!!! - fixed many bug defines in headers accordingly! Sigh... Must retest carefully</li>
		<li>Cleaned up checked in valgrind supressions. Many no longer needed (due to compiler/os/upgrade most likely)</li>
		<li>ThirdPartyComponents updates
			<ul>
				<li>libcurl 7.49.1</li>
				<li>lzma SDK 1602</li>
				<li>openssl 1.0.2h, and progress towards supporting openssl 1.1 (beta) - but still too broken (windows build) to worry. Must do some restructure of my use code too NYI</li>
			</ul>
		</li>
		<li>fixed qCompilerAndStdLib_deprecated_attribute_Buggy for _MSC_FULL_VER? so no DEPRECATED works right on MSVC</li>
		<li>Synchronized&lt&gt;
			<ul>
				<li>docs and regtest for explicit lock on synchonized object</li>
				<li>https://stroika.atlassian.net/browse/STK-489 - DEPRECATED operator-> () in Synchonized - and force explicit use of cget or rwget() on synchonized to get reference, or load to get a copy.  For synchonized objects - these distrinctions just matter. Still experimental, but buetter documetned and I think clearer</li>
			</ul>
		</li>
		<li>TextReader() now takes overload CTOR with Iterable&lt;Character&gt;</li>
		<li>VariantReader now takes overlaod on Read method of Iterable&lt;Character&gt; (aka string) - and added regtest cases for this</li>
		<li>Math::Median always uses just nth_element (not sort) for speed</li>
		<li>Added docs for ModuleGetterSetter and regression test sample/test code, and code cleanups</li>
		<li>PerformanceDump-v2.0a151-linux-gcc-6.1.0-x64.txt and PerformanceDump-v2.0a151-x86-vs2k15-ReleaseU.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a150">v2.0a150</a><br/>2016-06-15</td>
<td>
	<ul>
		<li>Fixed bug with Math::Median (on even # of buckets), and enhanced template to support RETURN_TYPE, and other enhamcents</li>
		<li>SmallStackBuffer<> has type value_type, and CTOR taking PTR,PTR</li>
		<li>wrap valgrind include macor exclusion with cplusplus or stdc - so not included for msvc RC compiler etc</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a149">v2.0a149</a><br/>2016-06-13</td>
<td>
	<ul>
		<li>workaround qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy</li>
		<li>new macros Stroika_Foundation_Debug_ValgrindDisableHelgrind_START and Stroika_Foundation_Debug_ValgrindDisableHelgrind_END to temporarily disable heglrind warnings and test case for them</li>
		<li>replace use of (losing) _NoReturn_ with [[noreturn]]</li>
		<li>Improved make_unique_lock for rvalue-refs</li>
		<li>Ran performace tests (PerformanceDump-v2.0a149-x86-vs2k15-ReleaseU.txt and PerformanceDump-v2.0a149-linux-gcc-6.1.0-x64.txt) 
		and there were regressions, so worked to (mostlly) fix them.
		</li>
		<li>Various speedups/cleanups to string code - relating to https://stroika.atlassian.net/browse/STK-444 fix earlier and StringBuffer reserve code. Got back most of the lost speed</li>
		<li>renamed test HistoricalPerformanceRegressionTestResults</li>
		<li>ScriptsLib/RegressionTests-Unix.sh now writes to Tests/HistoricalRegressionTestResults</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
		<li>SKIPPED
			<ul>
				<li>cross-compile 2 raspberry-pi {skipped remote make run-tests}</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a148">v2.0a148</a><br/>2016-06-10</td>
<td>
	<ul>
		<li>https://stroika.atlassian.net/browse/STK-444 - preallocating nul byte on strings (for thread safety)</li>
		<li>ThreadPool::GetPoolSize () needs auto    critSec { make_unique_lock (fCriticalSection_) }; to access - event readonly - fThreads - beacuse it can be written by another thread at the same time</li>
		<li>Target compilers supported changes
			<ul>
				<li>Add gcc 6.1</li>
				<li>Add clang++3.7 (required new define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy for clang++ 3.7 and improved ifdefs for !qCompilerAndStdLib_Locale_Buggy)</li>
				<li>Add clang++3.8</li>
			</ul>
		</li>
		<li>configure --lto now requires arg - enable/disable; and configure --apply-release-flags now sets --lto enable by default if using gcc</li>
		<li>changed qCompilerAndStdLib_make_unique_lock_IsSlow from 0 to 1 - tested on gcc 5.3 and msvc2k15u2 and all have slight make_unique_lock penalty - so use fast way internally</li>
		<li>Changed qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr so only defaults to 1 on VS (windows) and else 0 (using shared_ptr)</li>
		<li>Valgrind Helgrind
			<ul>
				<li>silence helgrind warning on minor bug (still bug and added @todo but minor)</li>
				<li>Stroika_Foundation_Debug_ValgrindDisableHelgrind on 2 vars in Foundation/Execution/SharedStaticData</li>
				<li>Stroika_Foundation_Debug_ValgrindDisableHelgrind in SharedPtr</li>
				<li>use new Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic - just to better document when/where/why valgrind/helgrind suppressions</li>
			</ul>
			and SIMPLIFIED qCompilerAndStdLib_shared_mutex_module_Buggy
		</li>
		<li>Lose OpenSSL.supp (just test with PURIFY mode OpenSSL- no many suppressions to be useful)</li>
		<li>renamed EOFException::kEOFException -> EOFException::kThe, etc. Coding convention docs about kthe </li>
		<li>Silenced some (safe) compiler warnings</li>
		<li>fixed bug with Configuration::FindLocaleName - part1 and part3 cannot be static</li>
		<li>fixed qCompilerAndStdLib_make_unique_Buggy bug define gcc</li>
		<li>https://stroika.atlassian.net/browse/STK-188 - lose qStroika_Foundation_Memory_NeedPtrStoredInEnableSharedFromThis_</li>
		<li>Lose deprecated bug workarounds
			<ul>
				<li>qCompilerAndStdLib_uninitialized_copy_n_Buggy</li>
				<li>qCompilerAndStdLib_DefaultArgOfStaticTemplateMember_Buggy</li>
				<li>qCompilerAndStdLib_constexpr_arrays_Buggy</li>
				<li>qCompilerAndStdLib_static_initialization_threadsafety_Buggy</li>
				<li>qCompilerAndStdLib_DotTemplateDisambiguator_Buggy</li>
			</ul>
			and SIMPLIFIED qCompilerAndStdLib_shared_mutex_module_Buggy
		</li>
		<li>Small cleanups to BlockAllocation: assertions, handle delete with interupt execptions as well as abort execptions, and other minor changes</li>
		<li>Reggression Tests and new compilers supported
			<ul>
				<li>fix BuildGCC.sh script so uses GCC_VERSION as default (so can eb passed in as env var) and documetning tested 4.8, 4.9.3, 5.3.0, and 6.1.0</li>
				<li>fixed execute permission and reuqire bash not just sh - to run HasCompiler - so regtest conf builder only does ones for compilers we have installed.</li>
				<li>fixed compiler bug defines to support gcc 6.1, clang3-7. 3.8 and added tehse to regtested (and 48 now my private buold)</li>
			</ul>
		</li>
		<li>small inline cleanups (various modules)</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>gcc 6.1</li>
				<li>clang++3.7 (ubuntu)</li>
				<li>clang++3.8 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a147">v2.0a147</a><br/>2016-06-03</td>
<td>
	<ul>
		<li>SignalHandler
			<ul>
				<li>refactor SafeSignalsManager::Rep_ () code for its condition variable usage (small change)</li>
			</ul>
		</li>
		<li>Helgrind support - https://stroika.atlassian.net/browse/STK-471
			<ul>
				<li>qStroika_FeatureSupported_Valgrind</li>
				<li>Got all issues cleared up with helgrind - and now clean bill of health (with various caveats opened as bug reports in JIRA)</li>
				<li>Many small changes - mostly annotations and supression file entries - esp to SignalHandler, and WaitablEvent, and regression tests</li>
			</ul>
		</li>
		<li>lose deprecated DEFINE_CONSTEXPR_CONSTANT()</li>
		<li>Draft of new ModuleGetterSetter&lt;T,IMPL&gt;</li>
		<li>Small cleanups/enhamcents to ErrNoException module: Always define errno_t in that Execution namespace, 
		and lose qCanGetAutoDeclTypeStuffWorkingForTemplatedFunction because Handle_ErrNoResultInteruption fixed.
		</li>
		<li>fixed several modbus bugs (really mostly returning wrong payload size and returing uint16s for coils and discrete registers) - reported by Chad@blcok - and improved diagnostics (ifdefed)</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck and helgrind)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>









<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a146">v2.0a146</a><br/>2016-06-01</td>
<td>
	<ul>
		<li>Quick release - so no regression tests completed (and helgrind regtest still not clean).</li>
		<li>Valgrind - Helgrind work
			<ul>
				<li>qStroika_FeatureSupported_Valgrind and VALGRIND_HG_CLEAN_MEMORY support - https://stroika.atlassian.net/browse/STK-471</li>
				<li>Common-Helgrind.supp.</li>
				<li>Enabled running Helgrind in the normal release process Regression tests (still a few regression warnings, but all tests pass)</li>
			</ul>
		</li>
		<li>fixed slight data race in ThreadPool code - detected by helgrind, and other related minor ThreadPool cleanups</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a145">v2.0a145</a><br/>2016-05-27</td>
<td>
	<ul>
		<li>Optional
			<ul>
				<li>Gave up on Optional&lt;T&gt; (T*) - instead - Optional&lt;T&gt;::OptionalFromNullable</li>
				<li>More misc cleanups - nullopt_t, op=, etc.</li>
			</ul>
		</li>
		<li>Modbus TCP
			<ul>
				<li>added Modbus::SplitSrcAcrossOutputs</li>
				<li>relax restrictions on returned values from APIs so we can return things outside the range of request - and handle that on the server side</li>
			</ul>
		</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>valgrind (memcheck only; helgrind still broken)</li>
			</ul>
		</li>
		<li>SKIPPED
			<ul>
				<li>cross-compile 2 raspberry-pi {skipped remote make run-tests}</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a144">v2.0a144</a><br/>2016-05-26</td>
<td>
	<ul>
		<li>IO::Socket
			<ul>
				<li>SockAddr ToString support</li>
				<li>Socket::GetPeerAddress</li>
			</ul>
		</li>
		<li>Modbus TCP
			<ul>
				<li>New Modbus Framework</li>
				<li>Supports a handful of messages (needed for BlockEng) - and full serverside (but no client/proxy support)</li>
			</ul>
		</li>
		<li>Streams
			<ul>
				<li>Added Streams::EOFException</li>
				<li>Added OutputStream<ELEMENT_TYPE>::WritePOD () and InputStream<ELEMENT_TYPE>::ReadPOD ()</li>
			</ul>
		</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck only; helgrind still broken)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a143">v2.0a143</a><br/>2016-05-20</td>
<td>
	<ul>
		<li>Quick (ish) release with small fixes, and modest testing</li>
		<li>DEPRECATED DEFINE_CONSTEXPR_CONSTANT</li>
		<li>Configuration::SystemConfiguration
			<ul>
				<li>Fixed windows and Linux Configuration::GetSystemConfiguration_CPU () - to return right # logical and physical (socket) cores</li>
				<li>Linux (embedded old linux) fix so returns right number of logical cores</li>
				<li>ToString support, and added to regtest so does toString of result</li>
			</ul>
		</li>
		<li>Lose all support for VisualStudio.Net-2013
			<ul>
				<li>Lose all project files</li>
				<li>Lose version defines (_MS_VS_2k13_VER_, _MS_VS_2k13_FULLVER_, _MS_VS_2k13_Update1_FULLVER_ etc)</li>
				<li>Lose all bug defines (and their workarounds) that ONLY applied to VS2k13
					<ul>
						<li>qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy</li>
						<li>qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy</li>
						<li>qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy</li>
						<li>qCompilerAndStdLib_VarDateFromStrOnFirstTry_Buggy</li>
						<li>qCompilerAndStdLib_StdExitBuggy</li>
						<li>qCompilerAndStdLib_alignas_Buggy</li>
						<li>qCompilerAndStdLib_constexpr_Buggy</li>
						<li>qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy</li>
						<li>qCompilerAndStdLib_strtof_NAN_ETC_Buggy</li>
						<li>qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy</li>
						<li>qCompilerAndStdLib_noexcept_Buggy</li>
						<li>qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy</li>
						<li>qCompilerAndStdLib_StaticCastInvokesExplicitConversionOperator_Buggy</li>
						<li>qCompilerAndStdLib_hasEqualDoesntMatchStrongEnums_Buggy</li>
						<li>qCompilerAndStdLib_thread_local_keyword_Buggy</li>
						<li>qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug</li>
						<li>qCompilerAndStdLib_stdinitializer_of_double_in_ranged_for_Bug</li>
						<li>qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy</li>
						<li>qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy</li>
						<li>qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy</li>
						<li>qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy</li>
						<li>qCompilerAndStdLib_noexcept_Buggy</li>
						<li>qCompilerAndStdLib_constexpr_Buggy</li>
						<li>qCompilerAndStdLib_thread_local_keyword_Buggy</li>
					</ul>
				</li>
			</ul>
		</li>
		<li>updated building Stroika docs</li>
		<li>Lose deprecated Frameworks/SystemPerformance/Instruments/Process fPercentCPUTime</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile 2 raspberry-pi</li>
				<li>valgrind (memcheck only; helgrind still broken)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a142">v2.0a142</a><br/>2016-05-18</td>
<td>
	<ul>
		<li>small cleanups to samples - Characters::ToString(current_exception()), and added InternetAddress::ToString ()</li>
		<li>Improved error messages in ~AllThreadsDeadDetector_ for failure case</li>
		<li>Added helpers VariantWriter::WriteAsBLOB () and VariantWriter::WriteAsString ()</li>
		<li>minor mostly cosmetic and exception handling cleanusp to Frameworks::Service code</li>
		<li>Set&lt;T, TRAITS&gt;::IsSubsetOf ()</li>
		<li>VariantValue takes optional iterable of VariantValue</li>
		<li>added Time::operator- (const DateTime& lhs, const DateTime& rhs)</li>
		<li>
			SystemPerformance/Instruments/Process
			<ul>
				<li>deprecated PercentCPUTime and instead defined AverageCPUTimeUsed</li>
				<li>respect fRestrictToPIDs and fOmitPIDs</li>
				<li>support fCaptureTCPStatistics (default off) for Linux</li>
			</ul>
		</li>
		<li>NEW new PerformanceDump-v2.0a142-linux-gcc-5.3.0-x64.txt and PerformanceDump-v2.0a142-x86-vs2k15-ReleaseU.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>valgrind (memcheck only; helgrind still broken)</li>
			</ul>
		</li>
		<li>De-Supporting configurations
			<ul>
				<li>vc++2k13</li>
				<li>ppc-AIX-7.1/gcc 4.9 (release works fully, but Configuration=Debug crashes linker unless you disable Xerces)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh & use custom build of gcc 5.3.0)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>








<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a141">v2.0a141</a><br/>2016-04-22</td>
<td>
	<ul>
		<li>coding convention docs (_A name suffix), and renamed FormatThread to FormatThread_A</li>
		<li>draft of Thread GetDefaultConfiguration and passing in Configuration object, and GetConfigurationStatus  - https://stroika.atlassian.net/browse/STK-475 and https://stroika.atlassian.net/browse/STK-474;</li>
		<li>Transfer fucntion (curl/winhttp) support fOptions_.fMaxAutomaticRedirects and default to 0, and handle in curl/winhttp implementations; fixed io transer network connection regtest to use fMaxAutomaticRedirects = 1, since many of them use redirects</li>
		<li>fixed use of :WinHttpSetOption WINHTTP_DISABLE_COOKIES</li>
		<li>
			URL class
			<ul>
				<li>NOT backward compat change - URL::GetScheme() now returns optional - and eFlexiblyAsUI doesnt fill in scheme by default - but many places use new GetSchemeValue () - which populates with the default; and fixed regression tests</li>
				<li>added new utility URL::GetHostRelURLString () - badly named but needed;</li>
				<li>URL::ToString</li>
				<li>URL::GetHostRelativePathPlusQuery</li>
			</ul>
		</li>
		<li>fixed Network/Transfer/Client_WinHTTP to pass fURL_.GetHostRelativePathPlusQuery () instead of fURL_.GetHostRelativePath () - curl code was already fine</li>
		<li>Attempt at fixing CTRLC issue with services - while not running - install signal handler LATER - just around the actual run as service</li>
		<li>
			VS2k15
			<ul>
				<li>define qCompilerAndStdLib_COutCErrStartupCrasher_Buggy - last importnat bug (with workaround) for ms vs 2k15, and applied (cruddy but workable) workaround</li>
				<li>NEW PerformanceDump-v2.0a141-x86-vs2k15-ReleaseU.txt</li>
				<li>Switching all my development to using VS2k15 instead of VS2k13. Ran final release testing with VS2k13 also - for this release, but probably soon abanodon vs2k13</li>
			</ul>
		</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k15 Update 2</li>
				<li>vc++2k13 (probably last release to support/test this)</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>ppc-AIX-7.1/gcc 4.9 (release works fully, but Configuration=Debug crashes linker unless you disable Xerces)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh & use custom build of gcc 5.3.0)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>valgrind (memcheck only; helgrind still broken)</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a140">v2.0a140</a><br/>2016-04-04</td>
<td>
	<ul>
		<li><em>Quick minor release (little tested)</em></li>
	</ul>
</td>
</tr>



<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a139">v2.0a139</a><br/>2016-04-04</td>
<td>
	<ul>
		<li><em>Quick minor release (little tested) to adjust timeouts on performance regtest for HealthFrame release</em></li>
		<li><em>warning - not backward compatible</em> Execution::FormatThreadID now returns string (ASCII); so use Characters::ToString (IDType) instead</li>
		<li>lose deprecated mkFinally</li>
	</ul>
</td>
</tr>



  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a138">v2.0a138</a><br/>2016-04-03</td>
<td>
	<ul>
		<li>Support GCC 5.3 in Defaults_CompilerAndStdLib</li>
		<li>curl version 7.48.0; and fixed download logic to only grab from mirrors and default download location - not github cuz that names folders differently</li>
		<li>Various docs cleanups (including Thread, Synchonized)</li>
		<li>Cleanup DbgTarace qStroika_Foundation_Exection_Thread_SupportThreadStatistics in Thread</li>
		<li>Synchonized usage in Frameworks/SystemPerformance/Capturer</li>
		<li>Slightly enlarged timeouts on performance regtest so hopefully wont fail anymore on AWS VMs</li>
		<li>Formatting of backtrace output on gcc/unix (now using demangle)</li>
		<li>New configure --runtime-stack-check {true|false} (hit by --apply-default-debug-flags) flag; but sadly doesnt work on AIX (platform where needed most)</li>
		<li>
			RegressionTets-Unix.sh and Makefile
			<ul>
				<li>gcc 5.3.0 support in regression test, and a few other additions/removals</li>
				<li>Conditionally added helgrind support (but disabeld cuz broken - https://stroika.atlassian.net/browse/STK-471) </li>
				<li>Attempt to support running valgrind memcheck or helgrind, and changed meaning of VALGRIND= param to tool to use, and changed regtests to run memcheck AND one helgrind</li>
			</ul>
		</li>
		<li>
			Frameworks::Service
			<ul>
				<li>Lose unused Main::sTHIS_; and example usage comments</li>
				<li>Use synchonized to protect instance variables on Main::BasicUNIXServiceImpl</li>
				<li>Instance/function object for safe signal handler to avoid use of global variable</li>
				<li>use fRunThread.load () instead of fRunThread_-> in Main::BasicUNIXServiceImpl::_Attach () to avoid deadlock</li>
				<li>Generally UNIX impl should be more solid/thread/signal safe</li>
			</ul>
		</li>
		<li>
			ProcessRunner
			<ul>
				<li>santiy check asserts on getrlimit results</li>
				<li>delete old obsolete code (ifdefed)</li>
				<li>ProcessRunner - use f_fork on AIX</li>
				<li>Incomplete draft of using spawn instead of fork/exec</li>
			</ul>
		</li>
		<li>Completed rewrite of https://analitiqa.atlassian.net/browse/AII-27 - Execution::Platform::AIX::GetEXEPathWithHintT - still sucks - but at least no popen</li>
		<li>Added PerformanceDump-v2.0a138-x86-ReleaseU.txt; PerformanceDump-v2.0a138-linux-gcc-4.9.2-x64.txt; PerformanceDump-v2.0a138-linux-gcc-5.3.0-x64.txt</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 Update 2 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc 4.8</li>
				<li>gcc 4.9</li>
				<li>gcc 5.3</li>
				<li>ppc-AIX-7.1/gcc 4.9 (release works fully, but Configuration=Debug crashes linker unless you disable Xerces)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh & use custom build of gcc 5.3.0)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a137">v2.0a137</a><br/>2016-03-31</td>
<td>
	<ul>
		<li>
			Exeuction::Finally
			<ul>
				<li>lose obsolete DEPRECATED Exeuction::Finally</li>
				<li>Decprecated mkFinally()</li>
				<li>Added new Finally () - essentially same as mkFinally - so net net a rename</li>
				<li>Re-jiggered and documented future and current noexcept support for Finally (now easier and safer), and related future todo (optional suppress abort)</li>
			</ul>
		</li>
		<li>Thread::SuppressInterruptionInContext in one spot of Thread::ThreadMain_, and in a couple finally's in WaitableEvent</li>
		<li>print BackTrace in Assert/Fatal and neaten report in signalhandler</li>
		<li>added ToString (const thread::id& t); and ifdefed use of that to dump tostring of runnign threads</li>
		<li>support for Visual Studio.net 2015 Update 2 - but it STILL fails on 64lib lib bug - looks like I may need to debug this</li>
		<li>minor tweak (static const string_constnat) for ObjectVariantMapper::MakeCommonSerializer_Range_ ()</li>
		<li>lose a few depecated functions /classes (GetStandardTerminationSignals/ Has_Operator_LessThan)</li>
		<li>adjusted regression test times (test 47 performance) - for compat with vs2k15 (in vm)</li>
		<li>Lose qCompilerAndStdLib_deprecatedFeatureMissing and related deprecation macros (just _Deprecated_ now)</li>
		<li>fixed typos in configure code - when appending to EXTRA_LINKER_ARGS include space in case args already there</li>
		<li>Tested (passed regtests)
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 Update 2 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49 (release works fully, but DEBUG crashes linker unless you disable Xerces)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a136">v2.0a136</a><br/>2016-03-29</td>
<td>
	<ul>
		<li>
			Finally/mkFinally
			<ul>
				<li>Multistage rewrite - but for now switch all uses to using Exeuction::mkFinally</li>
				<li>Gist of change is to allow use with no mallocs/locks</li>
			</ul>
		</li>
		<li>
			SignalHandler code
			<ul>
				<li>Rewrote much SignalHanlder code so safer interlock if signals while updating signal handlers</li>
				<li>use Platform::POSIX::ScopedBlockCurrentThreadSignal before attempting signal lock in SignalHandlerRegistry::SetSignalHandlers () - to avoid deadlock</li>
				<li>no arg overload of Platform::POSIX::ScopedBlockCurrentThreadSignal CTOR (all signals) and used that in Execution/SignalHandlers - so no signals on the thread adding/removing cached signal handler callbacks</li>
				<li>Deprecated GetStandardTerminationSignals</li>
				<li>SetStandardCrashHandlerSignals (eDirect)</li>
				<li>SignalHandler::ToString ()</li>
			</ul>
		</li>
		<li>improved DBGTrace code for InputStream::ReadAll logic; Tightened some assertions; 
		and fixed important bug (still not perfrect)  on InputStream<Byte>::ReadAll() seekable case;
		simplified InputStream<Byte>::ReadAll ()</li>
		<li>Cleanup BlockingQueue test and added example docs</li>
		<li>Thread::Status now uses DefaultNames<> and supports Thread::ToString(), and used more thoroughly inside DbgTraces inside Thread code</li>
		<li>Tweak tolerances on performance regtests to hopefully pass on AWS VMs</li>
		<li>more DbgTrace() calls - cleanups to track down AIX bug; more int-size sensative in printfs/DbgTrace Streams/InputStream</li>
		<li>minor cleanup to ordering in logger threads to avoid possible deadlock</li>
		<li>fixed missing StringBuilder operator +/&lt;&lt; overloads</li>
		<li>add ; separators in Debug/BackTrace</li>
		<li>make ToString for Iterables - use [ instead of {</li>
		<li>fixed case where SharedStaticData&lt;T&gt;::~SharedStaticData () could throw (abort exception)</li>
		<li>re-disable the CFLAGS attempted crap for xerces build (for aix) -didnt work there and broke other builds. Must improve Xerces autoconf!</li>
		<li>added PerformanceDump-v2.0a136-x86-ReleaseU.txt and PerformanceDump-v2.0a136-linux-gcc-5.2.0-x64.txt</li>
		<li>Tested (passed regtests) on 
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49 (release works fully, but DEBUG crashes linker unless you disable Xerces)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a135">v2.0a135</a><br/>2016-03-22</td>
<td>
	<ul>
		<li>Signal Handlers:
			<ul>
				<li>Misc cleanups</li>
				<li>Simplified SignalHandler CTOR code - so just static sThe</li>
				<li>new - hopefully safe now - SignalHanlder notification scheme for 'safe signal handlers'; 
				has one defect which is delivered oout of order; 
				and by default uses qConditionVariableSetSafeFromSignalHandler_ which now testing is safe with signals, but not super clearly documetned as such
				</li>
			</ul>
		</li>
		<li>Cleanup BlockingQueue test and added it to example docs</li>
		<li>Enlarge tolerances on performance regtests to hopefully pass on AWS VMs</li>
		<li>Debug:
			<ul>
				<li>configure apply-default-debug-flags for gcc, then set -rdynamic as default linker option - so stack traces come out better in debug traces</li>
			</ul>
		</li>
		<li>Threads
			<ul>
				<li>define and use Thread::AbortException::kThe and Thread::InterruptException::kThe to avoid problems constructing them (copyies strings) during abort, so abort in abort</li>
				<li>document that Execution::FormatThreadID () is not a cancelation point and force with Thread::SuppressInterruptionInContext</li>
				<li>document that Trace::EmitTraceMEssage is not a cancelation point; include Thread::SuppressInterruptionInContext</li>
			</ul>
		</li>
		<li>Before/After Main:
			<ul>
				<li>docs and assertion testing to document/ensure we dont have Thread objects outside the lifetime of main (at least not running)</li>
				<li>in test harness, call to EXIT is _ version - so no call to after main</li>
				<li>Added code to detect problems with threads running before or after main. Not perfect (cuz I lack a good way to detect)</li>
			</ul>
		</li>
		<li>
			AIX
			<ul>
				<li>experiment with hack to make extra compiler args (like AIX minimal TOC stuff) work with xerces compile (didnt work)</li>
				<li>dont use -rdynamic on AIX (doesnt work - no idea why - but just changing defaults so SB OK)</li>
			</ul>
		</li>
		<li>Added PerformanceDump-v2.0a135-x86-ReleaseU.txt, PerformanceDump-v2.0a135-linux-gcc-5.2.0-x64.txt</li>
		<li>Tested (passed regtests) on 
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49 (release works fully, but DEBUG crashes linker unless you disable Xerces)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a134">v2.0a134</a><br/>2016-03-19</td>
<td>
	<ul>
		<li>Cleanup docs (e.g. markdown readmes, and inline in source code)</li>
		<li>Signal handling, and thread safety and deadlocks
			<ul>
				<li>safer signal hanlding - no memory allocation during handling - INCOMPELTE</li>
				<li>Disable thread abort during destroy of blocking q un Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::Rep_::~Rep_</li>
			</ul>
		</li>
		<li>Cleanup signal handling and static usage (still more todo) on Frameworks::Service</li>
		<li>minor comments/celanups/. Containers::ReserveSpeedTweekAddNCapacity in InputStream<Byte>::ReadAll</li>
		<li>New Debug::Backtrace() call to generate a string with stack backtrace, suitable
			for logging. Now automatically called at the point where exceptions
			are thrown in tracelog.
			DefaultFatalErrorHandler_ prints current backtace.
		</li>
		<li>Thread/Execution
			<ul>
				<li>Cleanup code starting/naming threads (pass in threadname in CTOR so clearer each thread is named)</li>
				<li>Fixed non-POSIX calls to Thread::GetID() for null thread.</li>
				<li>moved lock_guard<mutex>   critSec  { sChangeInterruptingMutex_ } inside scope in Execution::CheckForThreadInterruption since Throw now does mcuh more than it used to (backtrace)</lI>
				<li>FIX bug with thread id stuff in ThreadMain ... issue is we dont assign to fThread yet in other thread sometimes - RACE - but fixed using GetCurrentThread and better documetned and asserts we eventually get the right value</loi>
				<li>use qStroika_Foundation_Exection_Thread_SupportThreadStatistics to add testings to regtests all threads cleaned up</li>
				<li> Thread InterruptSuppressCountType_ need not be atomic (corrcted comment too) - cuz only accessed within a single thread. But added assert it never goes negative (wraps)</li>
				<li>make Thread::InterruptException inherit from StringException - so its caught in Characters::ToString() conversion - and displayed reasonable</li>
				<li>optional name parameter to Thread objects, new Thread::IsDone()</li>
			</ul>
		</li>
		<li>Threadpool cleanups, and added primitive ToString() support</li>
		<li>threadpool::abort also uses Thread::SuppressInterruptionInContext</li>
		<li>Restrucutre Logger code
			<ul>
				<li>define qCompilerAndStdLib_StdExitBuggy; renamed Logger::...FlushBuffer to Flush(); added Logger::ShutdownSingleton (); and cleaned up samples usage to be more correct and use the above</li>
				<li>Logger::Log () now non-static method</li>
				<li>Much improved threadsafety - https://stroika.atlassian.net/browse/STK-463</li>
				<li>Lose all(?) static fields, and make them fields of Rep_</li>
				<li>Enhanced samples with buffering for logging, and Logger::ShutdownSingleton</li>
			</ul>
		</li>
		<li>Tracing
			<ul>
				<li>document DbgTrace not cancelation point</li>
				<li>TraceContextBumper noexcept</li>
				<li>draft new support for qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace</li>
				<li>Attempted ToString() support for exception_ptr objects</li>
			</ul>
		</li>
		<li>Build System
			<ul>
				<li>make (clobber/indent) cleanups</li>
				<li>try -mminimal-toc gcc arg instead of -bbigtoc for AIX TOC size workaround issue; causes fewer linker crashes (though docs say make slower code) - see https://stroika.atlassian.net/browse/STK-464</li>
				<li>draft support in configure script for --debug-symbols (workaround crazy AIX debug build failure) (ADD BUGRE PROT TO MAKE MORE THAN TRUEFASLE</li>
			</ul>
		</li>
		<li>More use of USE_NOISY_TRACE_IN_THIS_MODULE_ code (disabled but easy to turn on noisy per module)</li>
		<li>change IO::Networking::Transfer::Options  fOptions_.fFailConnectionIfSSLCertificateInvalid to be optional - and default true in WinHTTP, and false in curl (cuz we may not have ssl cert files - as on AIX - fixing https://stroika.atlassian.net/browse/STK-451)</li>
		<li>experimental FinallyT<> and mkFinally to try and avoid memory allocaiton (used in  in SignalHandlerRegistry::FirstPassSignalHandler_)</li>
		<li>Fixed Frameworks/SystemPerformance/INstruments/Process windwos genration of percent values,a nd bug on aix side first time through (mislabled commit 46c83f715bed3700988b58f6bdcaf2aac08e3cea)</li>
		<li>new String::FilteredString</li>
		<li>Tested (passed regtests) on 
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a133">v2.0a133</a><br/>2016-03-13</td>
<td>
	<ul>
		<li>Check-tools code for Stroika makefile - so prints clearer error message</li>
		<li>PIDLoop support</li>
		<li>Improved Characters::ToStrings () support: Duration, Characters::ToString module loses include of KeyValuePair, but adds support for serializing pair and CountedValue</li>
		<li>Synchronized:- readable/writable reference code</li>
		<li>Fixed default for ObjectVariantMapper::MakeCOmmonType<> for enum to use DefaultNames<> an dallow MakeCommonSerializer_EnumAsInt () for cases where people want to explicit write as integer</li>
		<li>Added MultiSet&lt;T, TRAITS&gt;::SetCount (), added MultiSet&lt;T, TRAITS&gt;::clear ()</li>
		<li>ObjectVariantMapper: simplified (fewer overloads - Iterable isntead of zillions of choices) for AddClass and added AddSubClass </li>
		<li>Lose deprecated CheckFileAccess</li>
		<li>Cleanup code and docs for VariantValue::Equals() - now more cases it compares as equals it used to treat as unequal (like taking a date and convert to string with json converter and then readback and compare - now compares equal</li>
		<li>Big changes to SystemPerformance/Instruments/Process for windows. It no longer (requires/uses by default) WMI.</li>
		<li>Patch for curl/openssl issue AIX bug (SSHv2 not recognized properly by curl autoconf)</li>
		<li>Emit EOL after shortened / elipsis added dbg log message</li>
		<li>Added PerformanceDump-v2.0a133-x86-ReleaseU.txt, PerformanceDump-v2.0a133-linux-gcc-5.2.0-x64.txt</li>
		<li>Tested (passed regtests) on 
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49 (except https://stroika.atlassian.net/browse/STK-451)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a132">v2.0a132</a><br/>2016-03-06</td>
<td>
	<ul>
		<li>fixed typo with Foundation/Cache/TimedCache, and new draft Cache/SynchronizedTimedCache</li>
		<li>FileSystem::IsDirectoryName ()</li>
		<li>Frameworks/WebServer: Request exception cleanups, new ClientErrorException</li>
		<li>fixed serious bug with Request::GetBody (): was not handling partial reads (where read returns less than requested), 
			optional upTo arg to InputStream::ReadAll()</li>
		<li>fixed has KeyvaluePair definition - for ToString, and sampe for Range<>::ToString</li>
		<li>Small cleanups to Iterator/Iterable/MakeSharedPtr - and use mew MakeSharedPtr/make_shared_ptr isntead of shared_ptr<> (new x)</li>
		<li>updated version of astyle</li>
		<li>Tested (passed regtests) on 
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49 (except https://stroika.atlassian.net/browse/STK-451)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a131">v2.0a131</a><br/>2016-02-20</td>
<td>
	<ul>
		<li>OMG another bugfix for Debug::EmitTrace... code... minor but...</li>
		<li>quickie release - no testing</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a130">v2.0a130</a><br/>2016-02-20</td>
<td>
	<ul>
		<li>OMG another bugfix for Debug::EmitTrace... code... minor but...</li>
		<li>quickie release - no testing</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a129">v2.0a129</a><br/>2016-02-20</td>
<td>
	<ul>
		<li>OMG another bugfix for Debug::EmitTrace... code... minor but...</li>
		<li>quickie release - no testing</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a128">v2.0a128</a><br/>2016-02-20</td>
<td>
	<ul>
		<li>fixed xerces makefiles to also have archive.apache fetch url; and switched to 3.1.3</li> 
		<li>fix Debug::EmitTrace... code ...thread interupt exception in low level DebugTrace code</li>
		<li>quickie release - no testing</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a127">v2.0a127</a><br/>2016-02-20</td>
<td>
	<ul>
		<li>Small assertion cleanups to FileSystem::ExtractDirAndBaseName; and in FileSystem::GetFileBaseName</li>
		<li>dont assert out and pass through - thread interupt exception in low level DebugTrace code</li>
		<li>quickie release - no testing</li>
	</ul>
</td>
</tr>




<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a126">v2.0a126</a><br/>2016-02-15</td>
<td>
	<ul>
		<li>
			OpenSSL crypto
			<ul>
				<li>progress on https://stroika.atlassian.net/browse/STK-123 Finish getting wrapper on openssl working</li>
				<li>RESOLVED https://stroika.atlassian.net/browse/STK-190</li>
				<li>RESOLVED https://stroika.atlassian.net/browse/STK-191</li>
				<li>PROGRESS but not fixed - https://stroika.atlassian.net/browse/STK-192</li>
				<li>RESOLVED https://stroika.atlassian.net/browse/STK-193 - issue with short encrpytions failing with block ciphers - was bug in pull code in streams</li>
				<li>Lowered priority - defaults good - https://stroika.atlassian.net/browse/STK-194</li>
				<li>tons .. fixed mostly working now</li>
				<li>EVP_BytesToKey  - nRounds defaults to 1 (and document why)</li>
				<li>Cryptography::OpenSSL::WinCryptDeriveKey (didnt produce same results as windows in one case I tested - not sure why)</li>
				<li>slightly restrucutre AES wrapper API - so usable, and now regrssion test passes (was missing IV); API A HORRIBLE MESS. see https://stroika.atlassian.net/browse/STK-458</li>
				<li>attempt at fixing openssl makefile for crosscompiling (https://stroika.atlassian.net/browse/STK-427)</li>
				<li>DecodeBase64() takes String overload</li>
			</ul>
		</li>
		<li>
			<ul>
				<li>openssl 1.0.2f</li>
				<li>-no-dso and -no-engines on building openssl - to wrokaround crasher bug using ssl from curl on centos6 (II-ESX-Agent); I should be able to get it working, but it doesn't appear needed so disabling SB OK - at least for now</li>
				<li>hopefully bug workaround for https://stroika.atlassian.net/browse/STK-452 - AIX openssl build issue</li>
				<li>I think I solved problem with using pkg-config with openssl/curl - issue seems to be its configure script misses the dependency on teh private ldl, so we force that in, and the link tests work again</li>
			</ul>
		</li>
		<li>
			<ul>
				<li>attempted to add nullopt_t support to Optional<> but not quite fully working (added https://stroika.atlassian.net/browse/STK-456)</li>
				<li>attempt losing two Optional operator= overloads (not backward compat) - but designed to more closely mimic http://en.cppreference.com/w/cpp/experimental/optional/operator%3D and avoid some overload ambiguities I was running into. Just testing...</li>
			</ul>
		</li>
		<li>
			<ul>
				<li>use curl 7.47, and use --without-zsh-functions-dir to workaround  https://stroika.atlassian.net/browse/STK-419; PATCH to workaround ZSH curl bug</li>
				<li>fixed curl makefile to use PKG_CONFIG_PATH - not PKG_CONFIG</li>
			</ul>
		</li>
		<li>
			<ul>
				<li>workaround trnascoder bug/issue on RedHat 6 - so did for xerces config - --enable-transcoder-gnuiconv --disable-transcoder-icu;
				use enable-transcoder-iconv disable-transcoder-gnuiconv for xerces - cuz gnuiconv depends on the LANG environment variable/locale stuff and that doesnt work on centos6
				</li>
			</ul>
		</li>
		<li>fixed zlib mirror</li>
		<li>VariantValue::As<float> returns NAN if null-type (more consistent with other docs on behavior of empty - but NOTE - NOT BACKWARD COMPATIBLE)</li>
		<li>Added optional Socket BindFlags to ConnecitonManager and Listener</li>
		<li>default values (not on cygwin) for AR/RANLIB (used in new openssl makefile and we didnset set them for AIX)</li>
		<li>Added new Filesystem::ExtractDirAndBaseName and used that in place of FileSystem::GetFileBaseName in processrunner (cuz abotu to change GetFileBaseName a bit). AND refined definition of Filesystem::GetFileBaseName and added regression tests</li>
		<li>
			BLOB
			<ul>
				<li>BLOB::Hex () utility function; BLOB::Raw()</li>
			</ul>
		</li>
		<li>
			Frameworks/WebServer/ConnectionManager and WebServer Sample
			<ul>
				<li>in Frameworks/WebServer/ConnectionManager - since we dont yet support connection keepalives, set Connection: close (required)</li>
				<li>WebServer Response - better support String, and fixed missing printf()</li>
				<li>WebServer Resposne::Write () overload for BLOB</li>
				<li>wrap new Route/ConnectionMgr stuff with !qCompilerAndStdLib_regex_Buggy; and  support new Route RequestMatch</li>
				<li>WebServer request uses Mapping (for now so i can use ToString); And fixed bug (assert error) fetchbgin BODY with zero sized content-length</li>
				<li>Added draft WebServer Router class; more cleanups of webserver code - including sample - to use (proto) router;
				got usable WebServer ConnectionManager, and greatly simplfied the WebServer sample app demo
				</li>
			</ul>
		</li>
		<li>Fix re-order regtests and fixed fixed accidentlaly remapped regression tests - #20 - Set</li>
		<li>ToString now supports KeyValuePair</li>
		<li>Framewowrks::Service - if we have an exception runing the thread service - propagate it</li>
		<li>Added Socket SO_LINGER support</li>
		<li>Added Set<T, TRAITS>::operator^=</li>
		<li>
			ProcessRunner (Big - not fully backward compatible - changes)
			<ul>
				<li>fixed a serious bug with ProcessRunner::CreateRunnable - capturing stack variable and returning it in function object before exec!</li>
				<li>First - on windows - with default settings - we throw on failed process exec (subprocess returns nonzero) - like on UNIX.</li>
    			<li>There are now overloads of Run() - taking a reference to a
    				Memory::Optional<ProcessResultType> - which will be filled in iff we
    				have status return results. And if filled in, the throw behaivor (on windows
    				and unix) is disabled.</li>
    			<li>and on UNIX fixed EINTR bug - where waidpid was not handling EINTR.</li>
				<li>Cleanup Execution::DetachedProcessRunner - mode DebugStrs - and execvp on UNIX - so searches path; and documentation</li>
			</ul>
		</li>
		<li>FOR AIX, we just add -latomic to StroikaFoundationSupportLibs to avoid ld: 0711-317 ERROR: Undefined symbol: .__atomic_load_8</li>
		<li>DataExchange/OptionsFile - docs, and Write and WriteRaw() does noting (no file change) if no actual changes; OptionsFile::WriteRaw () optimizaiton to not write doesnt apply when read/write paths are differnt! - like in upgrade!</li>
		<li>Added exceedingly preliminary/primitive SOAP support (just parse some kinds of partial) SOAPFault objects</li>
		<li>
			Logger
			<ul>
				<li>Logger::LogIfNew ()</li>
				<li> Imporved DbgTrace() handling of Logger::Log - always logging immediately to tracelog - not when output to syslog - and with proirity, and note about supressions</li>
			</ul>
		</li>
		<li>
			<ul>
				<li>SLIGHLY incompatible change to DirectoryIterator/DirectoryIterable - so they NEVER return . or ..: looking through my existing usage, it was NEVER helpful and I had tons of specail case code filtering these out (nearly every use)</li>
				<li>IO::FileSystem::FileSystem::FindExecutableInPath ()</li>
				<li>POSIX support for IO::FileSystem::FileSystem::GetFileLastAccessDate/IO::FileSystem::FileSystem::GetFileSize and IO::FileSystem::FileSystem::GetFileLastModificationDate</li>
				<li> some cleanups o fFileSystem::WellKnownLocations:: - making POSIX GetTemporary respoect enviroment variables, and CACHE (so faster), and other related cleanups</li>
			</ul>
		</li>
		<li>fixed Execution::IsProcessRunning() for AIX</li>
		<li>
			<ul>
				<li>lose String::ConstGetRep_ - and use _SafeReadRepAccessor instead; and added strika bug defines for a few importatnt string threading bugs (one very serious);
				stop using obsolete Interable::_ConstGetRep and instead use _SafeReadRepAccessor<> and iuts _ConstGetRep;
				lose obsolete Iterable<T>::_ConstGetRep - use _SafeReadRepAccessor<>::_ConstGetRep () instead</li>
			</ul>
		</li>
		<li>new system configuration fPreferedInstallerTechnology</li>
		<li>Added load/store names for readableReference/WritableReference in Synchonized</li>
		<li>
			IO::Network::Transfer
			<ul>
				<li>IO::Network::Transfer - cleanup and force consistently ALL POST, Send () etc calls will throw on failure, and to get the status/details catch and find the response object in exception</li>
				<li>slight improvement on https://stroika.atlassian.net/browse/STK-442 - for winhttp - set flag earlier so we ignore ssl errors if we dont need to know about them (leave bug open cuz even when we do need ssl info, we should be able to tell if it would have failed)</li>
				<li>exception memleak bulletproof in Transfer/Client_WinHTTP</li>
				<li>make WinHTTP support .fFailConnectionIfSSLCertificateInvalid and added regresison test teo verify this</li>
			</ul>
		</li>
		<li>small fix to ObjectReaderRegistry::RepeatedElementReader, and docs/examples</li>
		<li>overload IsPartition so takes optional comparer for range elements, and use in Math::ReBin()</li>
		<li>fixed _Deprecated_ macro</li>
		<li>DEPRECATED -Stroika_Enum_Names - use Configuration::DefaultNames&lt;ENUMNAME&gt; instead, and used that in in ToString(), and EnumNames<ENUM_TYPE>::PeekName (ENUM_TYPE e) const constexpr</li>
		<li>Many fixes to RegressionTests-Unix.sh</li>
		<li>Debug/Trace code: fixed dbgstr code for dumping very large lines to windows debugger, and noexcept usage</li>
		<li>ToString () improvements (fix unsigned)</li>
		<li>better factor EQUALS_COMPARER for SortedSet_DefaultTraits, SortedMapping_DefaultTraits, Set_stdset_DefaultTraits, Mapping_stdmap_DefaultTraits - depending on their base 'archtype' traits</li>
		<li>Common::DefaultEqualsComparer and DefaultEqualsComparerOptionally</li>
		<li>Various cleanups to containers - mostly relating to use of DefaultEqualsComparer</li>
		<li>qCompilerAndStdLib_hasEqualDoesntMatchStrongEnums_Buggy and workarounds</li>
		<li>AtLeast/AtMost</li>
		<li>deprecated Common::Has_Operator_LessThan in favor of Configuration::has_lt</li>
		<li>Containers factory fixups to handle TRAITS differ from standard</li>
		<li>New CountedValue<> and used that in MultiSet<></li>
		<li>New Containers submodule for DefaultTraits and Factory</li>
		<li>use improved strategy (...) for Default_SFINAE_ in many cases </li>
		<li>https://stroika.atlassian.net/browse/STK-431 - moved STL less<> to Common</li>
		<li>renamed Execution::DoThrow -> Execution::Throw (and a few similar renames)</li>
		<li>hopefully improved dependencies on rules in top level makefile (so less redundant building when called with no configuraiton argument - SB no diff otherwise)</li>
		<li>docs cleanups and deprecated FileSystem::CheckFileAccess (use CheckAccess)</li>
		<li>use noexcept on a few functions: GetTickCount, and Debug:: trace code</li>
		<li>PerformanceDump-v2.0a126-x86-ReleaseU.txt, PerformanceDump-v2.0a126-linux-gcc-5.2.0-x64.txt, PerformanceDump-v2.0a126-linux-gcc-4.9-x64.txt added</li>
		<li>Tested (passed regtests) on 
			<ul>
				<li>vc++2k13</li>
				<li>vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug)</li>
				<li>gcc48</li>
				<li>gcc49</li>
				<li>gcc52</li>
				<li>ppc-AIX/gcc49 (except https://stroika.atlassian.net/browse/STK-451)</li>
				<li>Centos 5 (scl enable devtoolset-2 sh gcc 4.8.2)</li>
				<li>clang++3.5 (ubuntu)</li>
				<li>clang++3.6 (ubuntu)</li>
				<li>cross-compile-raspberry-pi</li>
				<li>valgrind</li>
			</ul>
		</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a125">v2.0a125</a><br/>2016-01-13</td>
<td>
	<ul>
		<li>Substantial fixes to Debug/AssertExternallySynchronized - needed because of recent shared_lock support (multiset)</li>
		<li>renmaed Execution::DoReThrow -> ReThrow() - and deprecated old name</li>
		<li>several cleanups of TimedCache code - including deprecations, and docs, regression tests, TRAITS COMPARE and DO_DIRTY_ON_READ flag support.</li>
		<li>cleanup MakeSharedPtr / new code for a few more places - Iterable</li>
		<li>fixed Xerces makefile for AIX, and use explicit std:: before thread to avoid issues on AIX</li>
		<li>Further ToString support (bool/datetime etc)</li>
		<li>fix redirection in ScriptsLib/RegressionTests-Unix.sh and other output cleaning</li>
		<li>Define define qCompilerAndStdLib_static_initialization_threadsafety_Buggy</li>
		<li>qTemplateAccessCheckConfusionProtectedNeststingBug</li>
		<li>PerformanceDump-v2.0a125-linux-gcc-5.2.0-x64.txt and PerformanceDump-v2.0a125-x86-ReleaseU.txt added</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc52, ppc-AIX/gcc49, clang++3.5, clang++3.6, cross-compile-raspberry-pi, and valgrind;</li>
	</ul>
</td>
</tr>







<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a124">v2.0a124</a><br/>2016-01-04</td>
<td>
	<ul>
		<li>Consolidate and test minor tweaks.</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, ppc-AIX/gcc49, clang++3.5, clang++3.6, cross-compile-raspberry-pi, and valgrind;</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a123">v2.0a123</a><br/>2016-01-03</td>
<td>
	<ul>
		<li>Just updated copyrights for 2016.</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a122">v2.0a122</a><br/>2015-12-30</td>
<td>
	<ul>
		<li>Minor tweaks to Cryptography / Digest code.</li>
		<li>Untested release.</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a121">v2.0a121</a><br/>2015-12-29</td>
<td>
	<ul>
		<li>Minor docs changes</li>
		<li>Fixed scripting issue that was causing problems (ln -s cygwin) on JohnB's computer</li>
		<li>Quick emergency release - little tested - cuz little changed</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a120">v2.0a120</a><br/>2015-12-28</td>
<td>
	<ul>
		<li>Fixed make check</li>
		<li>Fixed Configuration::IterableOfT</li>
		<li>Fixed https://stroika.atlassian.net/browse/STK-422 - Bijection CTOR</li>
		<li>Fixed small CTOR issue with levels of conversion in Mapping CTOR - added regression test to capture</li>
		<li>Make tests speed improvements (less use of perl so -j works more)</li>
		<li>configure --only-if-has-compiler support</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.5, clang++3.6, cross-compile-raspberry-pi, and valgrind;</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a119">v2.0a119</a><br/>2015-12-27</td>
<td>
	<ul>
		<li>Supported Rasberry PI cross-compile builds (added to regression test)</li>
		<li>NOTE - dangerous not backward compat - change on Date::Format (String format,LCID OPT PARAM) -> Date::Format (LCID,String) - so
			ew Date::Format(String) we effectively re-interpet as meaning STDC++ API for format strings instead of Windows LCID (in both case curent localeish)
		</li>
		<li>quick hack partial shared_lock code - if qCompilerAndStdLib_shared_mutex_module_Buggy</li>
		<li>Various Containers cleanups - especially surrounding CTOR
			<ul>
				<li>lose _SortedSetSharedPtrIRep, _StackSharedPtrIRep, _QueueSharedPtrIRep etc using declarations - just use _SharedPtrIRep</li>
				<li>lose many contianer :_ConstGetRep () methdos (cuz can uyse _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep () - and use that to cleanup ccontainer ::_AssertRepValidType () calls</li>
				<li>https://stroika.atlassian.net/browse/STK-420 explicit CTOR(CONTINAER) for some (all) our containers to non-explicit</li>
				<li>missing move CTORs</li>
				<li>new Concept utility: Configuration::IsIterableOfT&lt;&gt; used on above CTOR changes</li>
				<li>Making Iterable inherit from private Debug::AssertExternallySynchronizedLock</li>
				<li>Iterable<T>::_SafeReadRepAccessor docs revised / semantics revised, and now uses Debug::AssertExternallySynchronizedLock</li>
				<li>migrating _SafeReadWriteRepAccessor from UpdateIterable to Iterable and make it inherit from Debug::AssertExternallySynchronizedLock, and lose UpdateIterable class</li>
			</ul>
		</li>
		<li>make regression-test-configuraitons</li>
		<li>Get rid of Iterable<T>::_ReadOnlyIterableIRepReference/Iterable&lt;T&gt;::_GetReadOnlyIterableIRepReference and SharedByValue&lt;TRAITS&gt;::ReadOnlyReference: though this was well done, we never used it, and its onbsoleted by new Synchonized approach</li>
		<li> https://stroika.atlassian.net/browse/STK-182 make Debug/AssertExternallySynchronizedLock recursive and draft support for shared_lock</li>
		<li>Network::GetDefaultPortForScheme etc - URL optional use for ports</li>
		<li>add parameterpack forwarding support to ConvertReaderToFactroy - args passed to new readers; and define qCompilerAndStdLib_ParameterPack_Pass_Through_Lambda_Buggy to deal with gcc 48 bugs</li>
		<li>Various makefile / build improvements (clobber, etc)</li>
		<li>Changed meaning of CPU::fRunQLength - now normalized to total cores, document better, and on windows, correct for discrpenecy  with how it provides runQLength (take into account CPU usage)</li>
		<li>Library/Sources/Stroika/Foundation/Execution/Logger.cpp - logger bookkeeping thread - use RemoveHEadIfPossible instead of RemoveHeadn and ignore expcetion</li>
		<li>fixed BlockingQueue<T>::RemoveHeadIfPossible () and clarifed docs. Now waits timeout period</li>
		<li>Documentation (esp build process/configuration) improvements</li>
		<li>fixed make run-tests script so if CrossCompiling (and note REMOTE= specified) we dont run the tests directly</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.5, clang++3.6, cross-compile-raspberry-pi, and valgrind;</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a118">v2.0a118</a><br/>2015-12-08</td>
<td>
	<ul>
		<li>docs and fix SERIOUS bug with FileOutputStream: it was not properly handling APPEND/TRUNC flag - essentially adding random flag isntead of correct one</li>
		<li>Use lzma SDK 15.12</li>
		<li>Lose some deprecated APIs (e.g. TakeNBitsFrom, DataExchange/Writer, DataExchange/Reader)</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>





<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a117">v2.0a117</a><br/>2015-12-06</td>
<td>
	<ul>
		<li>Got Xerces working on AIX</li>
		<li>Serveral fixes to Debug::TimingTrace, including allow default warnIfLongerThan</li>
		<li>updated _MS_VS_2k15_Update1_FULLVER_ for actual update 1 release</li>
		<li>Improved WGet, and ftp mirrors list for openssl, and change to 1.0.2e</li>
		<li>
			<ul>
				<li>Tons more make system cleanups - mostly cosmetic</li>
				<li>renamed default-configuration target to default-confiogurations</li>
				<li>apply conifgurations - so can be done per config</li>
				<li>top level makefile cleanups for applying one config at a time</li>
				<li>Many fixes to support ECHO env variable, so make looks much better under AIX</li>
				<li>build stroika docs to Builds folder and only once</li>
				<li>fixed serious regression in Test projectfile templates - always doing test 1 instead of NNN</li>
				<li>new ../ScriptsLib/GetDefaultShellVariable.sh helper</li>
				<li>https://stroika.atlassian.net/browse/STK-417 - ScriptsLib/MakeDirectorySymbolicLink.sh on windows broken</li>
				<li>renamed buildall / checkall scripts in samples to _vs - since they are visual studio specific and got rid of BuildUtils.pl</li>
			</ul>
		</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>


  

<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a116">v2.0a116</a><br/>2015-11-29</td>
<td>
	<ul>
		<li>Huge changes to build system - lots of progress on build system refactoring:
			<ul>
				<li>KEY is that you can create multiple configurations and build each independently. Default now at top level is to iterate over them and build each</li>
				<li>ScriptsLib/GetConfigurations.sh</li>
				<li>Lose ScriptsLib/GetDefaultConfiguration.pl</li>
				<li>fixed ScriptsLib/PrintEnvVarFromCommonBuildVars.pl to take CONFIGURATION arg and no longer depend on BuildUtils</li>
				<li>fixed occasional hang in lzma build - did 7za - redirect sdtout to /dev/null but could be CURRENT exists so promts for overwrite</li>
				<li>rewrote Make MAKE_INDENT_LEVEL support and use PrintLevelLeader.sh</li>
				<li> new configure --apply-default-debug-flags --apply-default-release-flags options; used by default in makefile; and on unix - default to Debug and Release configs</li>
			</ul>
		</li>
		<li>Renamed ThirdPartyLibs to ThirdPartyComponents</li>
		<li>
			DataExchange:
			<ul>
				<li>pass attribute namespace uri through Xerces SAX parser</li>
				<li>new ObjectReaderRegistry::RepeatedElementReader</li>
				<li>StructuredStreamEvents/ObjectReaderRegistry: lose Context& param from Deactivate etc. Make sure
					we always call Activate() in the right cases (classreader/listreader). And save context as needed
					for later use.
					API CHANGE - NOT BACKWARD COMPAT.
					</li>
				<li>ObjectReaderRegistry: print better dbgtrace saying failure to lookup particular type; and add MakeCommonReader_ overloads for several more (most?) builtin POD types.
				AND AddClass<> check for ObjectReaderRegistery and ObjectVariantMapper 
				</li>
			</ul>
		</li>
		<li>new utility Execution::WhenTimeExceeded and TimingTrace supported</li>
		<li>fixed signed/unsinged comparison bug with AIX Frameworks/SystemPerformance/Instruments/Memory code to fixup inconsistnet memory stats</li>
		<li>fixed FileSystem::RemoveFile/FileSystem::RemoveFileIf to set eWrite acces mode on exception; and implemnted draft of FileSytem::RemoveDirectory/RemoveDirectoryIf (incliing removepolicy to optioanlly delete subdris)</li>
		<li>https://stroika.atlassian.net/browse/STK-96 use Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER to map exceptions to use filename</li>
		<li>fixes for MakeBuildRoot and MakeDirectorySymbolicLink code - for sh/bash issue, and UAC on winDoze
			use stkRoot intead of relativepath, in ApplyConfiguraitons.pl - since we want 
			Lots of related makefile cleanups
		</li>
		<li>ServiceMain cleanup: 
			Simplifed UNIX - only know pid# if alive - check if alive correctly (was wrong)
			threw in vaguely reasonable default timeout for stop/restart - so doesnt hang (30 seconds) Stroika/Frameworks/Service/Main
		</li>
		<li>Libcurl - only init GlobalSSL and also set CURL_GLOBAL_ACK_EINTR set CURLOPT_NOSIGNAL</li>
		<li>use ::setsid in Execution::DetachedProcessRunner () to avoid sighup/etc - detach from terminal group</li>
		<li>Added optional hardware address to Network Interface info returned from GetInterfaces/GetInterfaceByID</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>






<tr>
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a115">v2.0a115</a><br/>2015-11-11</td>
<td>
	<ul>
		<li>If using MSVC2k13 - require at least Update5.
    Because of this, we could lose:
    	qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy
    	qCompilerAndStdLib_UsingInheritedConstructor_Buggy
    	qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug
    
    	FILE_SCOPE_TOP_OF_TRANSLATION_UNIT_MSVC_FLOATING_POINT_BWA
    
    and related detritus in code to workaround.</li>
		<li>minor tweaks to accomodate the possability to the total system memory changing while we run (can happen with vmware/linux)</li>
		<li>ScriptsLib/RegressionTests-Unix.sh major improvements
			<ul>
				<li>Attempt to allow automatic skip in regresison tests of missing compilers</li>
			</ul>
		</li>
		<li>ThirdPartyLibs makefile improvements
			<ul>
				<li>got libcurl to respect qFeatureFlag_OpenSSL, and tons of fixes to building openssl, curl, and other thirdpartylibs, using pkg_config</li>
				<li>use ScriptsLib/WebGet.sh to fetch thirdparty packages insgtead of direct wget call so it supports mirrors</li>
			</ul>
		</li>
		<li>IO/Network/Transfer/Client:
			<ul>
				<li>allow configure of option for fSupportSessionCookies;
    			Supprot for WinHTTP (which defaaulted on) and curl (whcih had defaulted off).
    			(NEITHER implemeation good, and curl worst, but usable)</li>
				<li>IO/Network/Transfer/Client option to specify useragent; cleanup other options</li>
				<li>set user agent for Client_libcurl (for now just Stroika/2.0)</li>
				<li>libcurl support for new fFailConnectionIfSSLCertificateInvalid transfer option</li>
				<li>Fixed tiny bug with Windows Transfer/Client_WinHTTP which caused it to never re-use HTTP connections
					(a big performance problem and a bigger semantics problem for talking ESXi proxy wire protocol)
				</li>
				<li>dangerous bug good update - Network/Transfer/Client_WinHTTP no longer throws internally on bad result - returns Reponse with status=500. We should return what is needed in the response, and only throw if we cannot get a valid response. That way the caller can inspect the resposne (oftne it has a soap fault or something). TODO - verify we do likewise with curl and document!)</li>
			</ul>
		</li>
		<li> AnyVariantValue - comments; move CTOR, use make_shared, and lose Equals and related operators (and document why), Added AnyVariantValue::IfAs () template</li>
		<li>lose last two params for MakeVersionFile.sh: can do a good job of infering from filenames</li>
		<li>OpenSSL.supp and -DPURIFY support so that we cna pass all regression tests with openssl/libcurl and valgrind (configure --openssl-extraargs PURIFY, and --block-allocation)</li>
		<li>Drop support for Clang-3.4, and so got rid of bug defines: 
			qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy
			qCompilerAndStdLib_CompareStronglyTypedEnums_Buggy
			qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy because these were only needed for clang-3.4
		</li>
		<li>COMPATABILITIY WARNING: Refactoring of SAX like streaming reader code. Almost total rewrite.
		Some non-backward compatible changes include:
		   replace all use of SAXCallbackInterface with StructuredStreamEvents::IConsumer, and
			lose the qname argumetns to StartElement() and EndElement()
			as well as HandleChildStart () in subclasses of SAXObjectReader
			<ul>
					<li>COMPATABILITIY WARNING: HandleChildStart and StartElement and EndElement (uri/name combined into new Name type) amd new StructuredStreamEvents::IConsumer instead of SAXCallbackInterface</li>
					<li>COMPATABILITIY WARNING: SAXObjecReader renanmed -> StructuredStreamEvent/ObjectReader</li>
					<li>Now very much patterend after ObjectVariantMapper</li>
    				<li>	ObjectVariantMapper now uses DataExchange::StructFieldInfo with
    					its macro for creation</li>
    				<li>	ObjectVariantMapper::StructureFieldInfo deprecated and renamed to
    					ObjectVariantMapper::StructFieldInfo</li>
    				<li>	ObjectVariantMapper_StructureFieldInfo_Construction_Helper macro deprecated</li>
    				<li>	Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey macro deprecated
    					ObjectVariantMapper_StructureFieldInfo_Construction_Helper(A,B,C) -> ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (A,B), C }
    					(identical to Stroika_Foundation_DataExchange_StructFieldMetaInfo)</li>
					<li>Attempt at getting rid of many Winvalid-offsetof  DISABLE_COMPILER_CLANG_WARNING_START/DISABLE_COMPILER_GCC_WARNING_START by migrating them to Stroika_Foundation_DataExchange_StructFieldMetaInfo - but only really fixed for clang</li>
			</ul>
			</li>
		<li>InputStream<Character>::ReadLine () performance tweak</li>
		<li>Tested (passed regtests) on vc++2k13, vc++2k15 {Pro} (except some crashers in 64 bit code due to MSFT lib bug), gcc48, gcc49, gcc49(32-bit x compile - but limited - no tpp libs), gcc52, pcc-AIX/gcc49, clang++3.5, clang++3.6, and valgrind;</li>
	</ul>
</td>
</tr>






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
		<li>Syntax Error (TimedCache)</li>
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
