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
	<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a21">v2.0a21x</a><br/>2014-02-??</td>
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
			<li>Revived SharedPtr<> template (work in progress; faster, and maybe add threadsafety option)</li>
			<li>String class changes
				<ul>
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
				</ul>
			</li>
			<li>(maybe) fixed long-standing bug with canceling a thread while its starting another. Minor issue but I think
			that was causing some failures of regtests - especailly test 34/threads/gcc/linux (clang++ issue isanother issue)</li>
			<li>New StringBuilder class</li>
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
