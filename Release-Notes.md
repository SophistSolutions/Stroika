About
=====
These release notes are incomplete. They are merely a summary of
major user (developer)-impactful changes - especially those they need 
to be aware of when upgrading.

<style type='text/css'>
	table tr td {width: 1in;  vertical-align: top; }
	table tr td td {width: auto;  }
</style>

History
=======

<table>





<tr>
<td>v2.0a15x<br/>2014-???</td>
<td>
<ul>
<li>Progress on makefile re-vamping</li>	


</ul>
</td>
</tr>






<tr>
<td>v2.0a14<br/>2014-01-08</td>
<td></td>
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
