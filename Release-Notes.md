These release notes are incomplete. They are merely a per-summary note of
major or user(developer)-impactful changes, they need to be aware of when upgrading.

v2.0a13		New 'safe signals' mechanism. This is not widely used, but is used
			in Services Framework, so users of the services framework (or anyone using safe signals)
			should add:
				Execution::SignalHandlerRegistry::SafeSignalsManager    safeSignals;
			to main(int argc, const char* argv[])

			Lose legacy nativive/pthread etc thread support (just C++-thread integration).

			Use zlib 1.2.8
