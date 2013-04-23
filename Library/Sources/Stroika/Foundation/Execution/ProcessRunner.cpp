/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "ProcessRunner.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;





/*
 ********************************************************************************
 ************************** Execution::ProcessRunner ****************************
 ********************************************************************************
 */
ProcessRunner::ProcessRunner (const TString& commandLine, Streams::BinaryInputStream in, Streams::BinaryOutputStream out, Streams::BinaryOutputStream error)
	: fCommandLine_ (commandLine)
	, fExecutable_ ()
	, fArgs_ ()
	, fWorkingDirectory_ ()	/// FIX TO GRAB CWD
	, fStdIn_ (in)
	, fStdOut_ (out)
	, fStdErr_ (error)
{
}

ProcessRunner::ProcessRunner (const TString& executable, const Containers::Sequence<TString>& args, Streams::BinaryInputStream in, Streams::BinaryOutputStream out, Streams::BinaryOutputStream error)
	: fCommandLine_ ()
	, fExecutable_ (executable)
	, fArgs_ (args)
	, fWorkingDirectory_ ()	/// FIX TO GRAB CWD
	, fStdIn_ (in)
	, fStdOut_ (out)
	, fStdErr_ (error)
{
}


#if 0
namespace	{
// still unsure if needed/useful - I now think the PeekNamedPipe stuff is NOT needed, but
// I can turn it on if needed -- LGP 2009-05-07
//#define	qUsePeekNamedPipe	1
#ifndef	qUsePeekNamedPipe
#define	qUsePeekNamedPipe	0
#endif
		/*
		 *	This code should all work with the smaller buffer sizes, but is more efficient with larger buffers.
		 *	Just set to use the smaller buffers to stress test and debug.
		 *
		 *	There is some subtle but serious bug with my pipe code - and that APPEARS to just be that
		 *	WaitForMultipleObjects doesn't work with PIPEs.
		 *
		 *	I COULD just rewrite alot of this code to NOT use PIPES - but actual files. That might solve the problem
		 *	because they never 'fill up'.
		 *
		 *	Alternatively - it might be that my switch to ASYNC mode (PIPE_NOWAIT) was a bad idea. Maybe if I got
		 *	rid of that - the WAIT code could be made to work? Not sure.
		 *
		 *	Anyhow - this appears to be adequate for now...
		 *
		 *			-- LGP 2006-10-17
		 */
		#if		1
			const size_t	kPipeBufSize	=	256*1024;
			const size_t	kReadBufSize	=	32*1024;
		#else
			const size_t	kPipeBufSize	=	10*1024;
			const size_t	kReadBufSize	=	100;
		#endif
}
namespace	{
	void	ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (HANDLE p, ostream& o)
		{
			char	buf[kReadBufSize];
			#if		qUsePeekNamedPipe
				DWORD	nBytesAvail	=	0;
			#endif
			DWORD	nBytesRead	=	0;
			// Read normally blocks, we don't want to because we may need to write more before it can output
			// and we may need to timeout
			while (
				#if		qUsePeekNamedPipe
					::PeekNamedPipe (p, nullptr, nullptr, nullptr, &nBytesAvail, nullptr) and
					nBytesAvail != 0 and
				#endif
					::ReadFile (p, buf, sizeof (buf), &nBytesRead, nullptr) 
					and nBytesRead > 0
				) {
				o.write (buf, nBytesRead);
			}
		}
	void	ReadAnyAvailableAndDumpMsgToTraceWithoutBlocking_ (HANDLE p)
		{
			char	buf[kReadBufSize];
			#if		qUsePeekNamedPipe
				DWORD	nBytesAvail	=	0;
			#endif
			DWORD	nBytesRead	=	0;
			// Read normally blocks, we don't want to because we may need to write more before it can output
			// and we may need to timeout
			while (
				#if		qUsePeekNamedPipe
					::PeekNamedPipe (p, nullptr, nullptr, nullptr, &nBytesAvail, nullptr) and
					nBytesAvail != 0 and
				#endif
					::ReadFile (p, buf, sizeof (buf), &nBytesRead, nullptr) 
					and nBytesRead > 0
				) {
				if (nBytesRead != 0) {
					buf[NEltsOf(buf)-1] = '\0';
					if (nBytesRead < NEltsOf (buf)) {
						buf[nBytesRead] = '\0';
					}
					DbgTrace ("STDERR(RunExternalProcess_): %s", LimitLength (buf, 50).c_str ());
				}
			}
		}

#if		qDefaultTracingOn
	size_t	sNCurRunExtProcessCalls	=	0;
	struct	ContextCounter {
		ContextCounter ()
			{
				sNCurRunExtProcessCalls++;
			}
		~ContextCounter ()
			{
				sNCurRunExtProcessCalls--;
			}
	};
#endif
}

TypeNValue	ReportDefinition::RunExternalProcess_ (const TString& cmdLine, const TString& currentDir, const Memory::BLOB& stdinBLOB, const ContentType& resultFormat, float timeout)
{
	TraceContextBumper	traceCtx (_T ("ReportDefinition::RunExternalProcess_"));
	DbgTrace (_T ("cmdLine: %s"), LimitLength (cmdLine, 50, false).c_str ());
	DbgTrace (_T ("currentDir: %s"), LimitLength (currentDir, 50, false).c_str ());
	DbgTrace (_T ("timeout: %f"), timeout);
	#if		qDefaultTracingOn
		ContextCounter	ctxCounter;
		DbgTrace (_T ("sNCurRunExtProcessCalls: %d"), sNCurRunExtProcessCalls);
	#endif

	TypeNValue	result;
	result.first = resultFormat;

	/*
	 *	o	Build directory into which we can copy the JAR file plugin,
	 *	o	create STDIN/STDOUT file handles to send/grab results
	 *	o	Run the process, waiting for it to finish.
	 *	o	Grab results from STDOUT file.
	 *	o	Cleanup created directory.
	 */

	// use AutoHANDLE so these are automatically closed at the end of the procedure, whether it ends normally or via
	// exception.
	AutoHANDLE	jStdin[2];
	AutoHANDLE	jStdout[2];
	AutoHANDLE	jStderr[2];

	PROCESS_INFORMATION	processInfo;
	memset (&processInfo, 0, sizeof (processInfo));
	processInfo.hProcess = INVALID_HANDLE_VALUE;
	processInfo.hThread = INVALID_HANDLE_VALUE;

	try {
		{
			SECURITY_DESCRIPTOR	sd;
			memset (&sd, 0, sizeof (sd));
			Verify (::InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION));
			Verify (::SetSecurityDescriptorDacl (&sd, true, 0, false));
			SECURITY_ATTRIBUTES	sa	=	{	sizeof (SECURITY_ATTRIBUTES), &sd, true };
			Verify (::CreatePipe (&jStdin[1], &jStdin[0], &sa, kPipeBufSize));
			Verify (::CreatePipe (&jStdout[1], &jStdout[0], &sa, kPipeBufSize));
			Verify (::CreatePipe (&jStderr[1], &jStderr[0], &sa, kPipeBufSize));
			/*
			 *	Make sure the ends of the pipe WE hang onto are not inheritable, because otherwise the READ
			 *	wont return EOF (until the last one is closed).
			 */
			jStdin[0].ReplaceHandleAsNonInheritable ();
			jStdout[1].ReplaceHandleAsNonInheritable ();
			jStderr[1].ReplaceHandleAsNonInheritable ();
		}

		STARTUPINFO	startInfo;
		memset (&startInfo, 0, sizeof (startInfo));
		startInfo.cb = sizeof (startInfo);
		startInfo.hStdInput = jStdin[1];
		startInfo.hStdOutput = jStdout[0];
		startInfo.hStdError = jStderr[0];
		startInfo.dwFlags |= STARTF_USESTDHANDLES;
		DWORD	createProcFlags	=	0;
		createProcFlags |= CREATE_NO_WINDOW;
		createProcFlags |= NORMAL_PRIORITY_CLASS;
		createProcFlags |= DETACHED_PROCESS;
		{
			bool	bInheritHandles		=	true;
			TCHAR	cmdLineBuf[32768];			// crazy MSFT definition! - why this should need to be non-const!
			_tcscpy_s (cmdLineBuf, cmdLine.c_str ());
			Verify (::CreateProcess (nullptr, cmdLineBuf, nullptr, nullptr, bInheritHandles, createProcFlags, nullptr, currentDir.c_str (), &startInfo, &processInfo));
		}

		{
			/*
			 * Remove our copy of the stdin/stdout/stderr which belong to the child (so EOF will work properly).
			 */
			jStdin[1].Close ();
			jStdout[0].Close ();
			jStderr[0].Close ();
		}

		AutoHANDLE&	useSTDIN	=	jStdin[0];
		Assert (jStdin[1] == INVALID_HANDLE_VALUE);
		AutoHANDLE&	useSTDOUT	=	jStdout[1];
		Assert (jStdout[0] == INVALID_HANDLE_VALUE);
		AutoHANDLE&	useSTDERR	=	jStderr[1];
		Assert (jStderr[0] == INVALID_HANDLE_VALUE);

		stringstream	stdoutResultStream (ios_base::in | ios_base::out | ios_base::binary);
		if (processInfo.hProcess != INVALID_HANDLE_VALUE) {

			Time::DurationSecondsType	timeoutAt	=	Time::GetTickCount () + timeout;

			{
				{
					/*
					 * Set the pipe endpoints to non-blocking mode.
					 */
					{
						DWORD	stdinMode	=	0;
						Verify (::GetNamedPipeHandleState (useSTDIN, &stdinMode, nullptr, nullptr, nullptr, nullptr, 0));
						stdinMode |= PIPE_NOWAIT;
						Verify (::SetNamedPipeHandleState (useSTDIN, &stdinMode, nullptr, nullptr));
					}

					{
						DWORD	stdoutMode	=	0;
						Verify (::GetNamedPipeHandleState (useSTDOUT, &stdoutMode, nullptr, nullptr, nullptr, nullptr, 0));
						stdoutMode |= PIPE_NOWAIT;
						Verify (::SetNamedPipeHandleState (useSTDOUT, &stdoutMode, nullptr, nullptr));
					}

					{
						DWORD	stderrMode	=	0;
						Verify (::GetNamedPipeHandleState (useSTDERR, &stderrMode, nullptr, nullptr, nullptr, nullptr, 0));
						stderrMode |= PIPE_NOWAIT;
						Verify (::SetNamedPipeHandleState (useSTDERR, &stderrMode, nullptr, nullptr));
					}
				}

				/*
				 *	Fill child-process' stdin with the source document.
				 */
				if (not stdinBLOB.empty ()) {
					const Byte*	p	=	stdinBLOB.begin ();
					const Byte* e	=	p + stdinBLOB.GetSize ();
					while (p < e) {
						DWORD	written		=	0;
						if (::WriteFile (useSTDIN, p, e-p, &written, nullptr) != 0) {
							DWORD	err = ::GetLastError ();
							// sometimes we fail because the target process hasn't read enough and the pipe is full.
							// Unfortunately - MSFT doesn't seem to have a single clear error message nor any clear
							// documentation about what WriteFile () returns in this case... So there maybe other erorrs
							// that are innocuous that may cause is to prematurely terminate our 'RunExternalProcess'.
							//		-- LGP 2009-05-07
							if (err != ERROR_SUCCESS and 
								err != ERROR_NO_MORE_FILES and
								err != ERROR_PIPE_BUSY and
								err != ERROR_NO_DATA
								) {
								DbgTrace ("in RunExternalProcess_ - throwing %d while fill in stdin", err);
								Execution::Platform::Windows::Exception::DoThrow (err);
							}
						}
						Assert (written <= static_cast<size_t> (e-p));
						p += written;
						// in case we are failing to write to the stdIn because of blocked output on an outgoing pipe
						if (p < e) {
							ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (useSTDOUT, stdoutResultStream);
							ReadAnyAvailableAndDumpMsgToTraceWithoutBlocking_ (useSTDERR);
						}
						if (p < e and written == 0) {
							// if we have more to write, but that the target process hasn't consumed it yet - don't spin trying to
							// send it data - back off a little
							Execution::Sleep (0.1f);
						}
						if (Time::GetTickCount () > timeoutAt) {
							DbgTrace (_T ("process timed out (writing initial data) - so throwing up!"));
							// then we've timed out - kill the process and DONT return the partial result!
							(void)::TerminateProcess (processInfo.hProcess, -1);	// if it exceeded the timeout - kill it (could already be done by now - in which case - this will be ignored - fine...
							Execution::DoThrow (Execution::Platform::Windows::Exception (ERROR_TIMEOUT));
						}
					}
				}

				// in case invoked sub-process is reading, and waiting for EOF before processing...
				useSTDIN.Close ();
			}


			/*
			 *	Must keep reading while waiting - in case the child emits so much information that it
			 *	fills the OS PIPE buffer.
			 */
			int		timesWaited	=	0;
			while (Time::GetTickCount () < timeoutAt) {
				/*
				 *	It would be nice to be able to WAIT on the PIPEs - but that doesn't appear to work when they
				 *	are in ASYNCRONOUS mode.
				 *	
				 *	So - instead - just wait a very short period, and then retry polling the pipes for more data.
				 *			-- LGP 2006-10-17
				 */
				HANDLE	events[1] = { processInfo.hProcess };

				// We don't want to busy wait too much, but if its fast (with java, thats rare ;-)) don't want to wait
				// too long needlessly...
				//
				// Also - its not exactly a busy-wait. Its just a wait between reading stuff to avoid buffers filling. If the
				// process actually finishes, it will change state and the wait should return immediately.
				double	remainingTimeout		=	(timesWaited <= 5)? 0.1: 0.5;
				DWORD	waitResult	=	::WaitForMultipleObjects (NEltsOf (events), events, false, static_cast<int> (remainingTimeout * 1000));
				timesWaited++;

				ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (useSTDOUT, stdoutResultStream);
				ReadAnyAvailableAndDumpMsgToTraceWithoutBlocking_ (useSTDERR);
				switch (waitResult) {
					case	WAIT_OBJECT_0: {
						DbgTrace (_T ("process finished normally"));
						timeoutAt = -1.0f;	// force out of loop
					}
					break;
					case	WAIT_TIMEOUT: {
						DbgTrace ("still waiting for external process output (WAIT_TIMEOUT)");
					}
				}
			}
			if (timeoutAt > 0.0f) {
				DbgTrace (_T ("process timed out - so throwing up!"));
				// then we've timed out - kill the process and DONT return the partial result!
				(void)::TerminateProcess (processInfo.hProcess, -1);	// if it exceeded the timeout - kill it (could already be done by now - in which case - this will be ignored - fine...
				Execution::DoThrow (Execution::Platform::Windows::Exception (ERROR_TIMEOUT));
			}

			SAFE_HANDLE_CLOSER (&processInfo.hProcess);
			SAFE_HANDLE_CLOSER (&processInfo.hThread);

			{
				DWORD	stdoutMode	=	0;
				Verify (::GetNamedPipeHandleState (useSTDOUT, &stdoutMode, nullptr, nullptr, nullptr, nullptr, 0));
				stdoutMode &= ~PIPE_NOWAIT;
				Verify (::SetNamedPipeHandleState (useSTDOUT, &stdoutMode, nullptr, nullptr));

				/*
				 *	Read whatever is left...and blocking here is fine, since at this point - the subprocess should be closed/terminated.
				 */
				char	buf[kReadBufSize];
				DWORD	nBytesRead	=	0;
				while (::ReadFile (useSTDOUT, buf, sizeof (buf), &nBytesRead, nullptr)) {
					stdoutResultStream.write (buf, nBytesRead);
				}
			}
		}


		/*
		 * Grab and return the results from STDOUT (and someday maybe STDERR?)
		 */
		{
			if (stdoutResultStream.tellp () != iostream::pos_type (0)) {
				DbgTrace ("stdoutResultStream size=%d", stdoutResultStream.tellp ());
				if (resultFormat.IsTextFormat ()) {
					string	resultAsStr	=	stdoutResultStream.str ();
					result.second = MapUNICODETextWithMaybeBOMTowstring (Containers::Start (resultAsStr), Containers::End (resultAsStr));
				}
				else {
					result.second = StreamUtils::ReadStreamAsBLOB (stdoutResultStream);
				}
			}
		}
	}
	catch (...) {
		if (processInfo.hProcess != INVALID_HANDLE_VALUE) {
			(void)::TerminateProcess (processInfo.hProcess, -1);	// if it exceeded the timeout - kill it
			SAFE_HANDLE_CLOSER (&processInfo.hProcess);
			SAFE_HANDLE_CLOSER (&processInfo.hThread);
		}
		Execution::DoReThrow ();
	}
	return result;
}
#endif
