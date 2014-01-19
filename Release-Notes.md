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
<td><a href="https://github.com/SophistSolutions/Stroika/commits/v2.0a18">v2.0a18x</a><br/>2014-???</td>
<td>
<ul>
  <li>Assorted (but still incomplete) code cleanups to Container DataStructure (esp patching) shared code.</li>
  <li>Many refinements to SharedByValue code, including variadic template use to forward extra paraemters to Copy, and more sophisticated use of r-value references and noexcept.</li>
<li>Started adding ...Iterators have a new owner (aka iterable/container) property and refined Equals() semantics</li>
<li>Lots of cleanups of threads and signal handling code. Needs more mileage/testing before ready for wider use/release. 
Improved supression of abort. Improved impl and docs on 'alertable' stuff for WinDoze. thread_local with std::atomic/volatile (testing).</li>
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
