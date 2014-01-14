<style type='text/css'>
	table tr td {width: 1in;  vertical-align: top; }
	table tr td td {width: auto;  }
</style>

About
=====
These release notes are incomplete. They are merely a summary of
major user (developer)-impactful changes - especially those they need 
to be aware of when upgrading.

History
=======

<table>





<tr>
<td>v2.0a18x<br/>2014-???</td>
<td>
<ul>
<li>Started adding ...Iterators have a new owner (aka iterable/container) property and refined Equals() semantics</li>
<li>Lots of cleanups of threads and signal handling code. Needs more mileage/testing before ready for wider use/release. 
Improved supression of abort. Improved impl and docs on 'alertable' stuff for WinDoze. thread_local with std::atomic/volatile (testing).</li>
</ul>
</td>
</tr>





<tr>
<td>v2.0a17<br/>2014-01-10</td>
<td>
<ul>
<li>Fixed bug with Thread::SuppressAbortInContext</li>
</ul>
</td>
</tr>




<tr>
<td>v2.0a16<br/>2014-01-10</td>
<td>
<ul>
<li>Minor</li>
</ul>
</td>
</tr>





<tr>
<td>v2.0a15<br/>2014-01-10</td>
<td>
<ul>
<li>Progress on makefile re-vamping</li>	
<li>OpenSSL 1.0.1f</li>
<li>Important new Thread::SuppressAbortInContext - feature to avoid a class of thread cleanup bugs</li>
</ul>
</td>
</tr>






<tr>
<td>v2.0a14<br/>2014-01-08</td>
<td>
<ul>
<li>Minor</li>
</ul>
</td>
</tr>




<tr>
<td>v2.0a13<br/>2014-01-08</td>
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
