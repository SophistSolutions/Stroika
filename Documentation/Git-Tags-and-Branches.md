***Git Tags and Branches***


<h3>Branches</h3>
	
<ul>
	<li>V2-Dev
		<div>
			The development branch may not be stable, and represents work in progress.
		</div>
	</li>
	<li>V2-Release
		<div>
			Roughly once per week (to once per-month) - a release is merged from the V2-Dev branch. When we do a release
			we run and record all the regression tests, including performance tests. Nearly every time we do a release, all regression tests
			pass fully.
		</div>
	</li>
	<li>V2-Stable
		<div>
			This is based on V2-Release, but lags behind by typically weeks or months, and represents a snapshot thats been used in other products
			and appears to be working well.
		</div>
	</li>
</ul>

---

<h3>Tags</h3>

<div>
	Each v2.0 release is taged vITSVERSION, for example, v2.0a205
</div>
