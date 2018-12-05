# IsDebuggerPresentEx
This project was created to verify if a debugger is attached by executing code in x64 mode from a 32 bit process.

The idea behind the project is nothing new and in the source code you will find the references to the external projects. It is very simple, if it detects a _debugger_ the process exits with code _1_. If no _debugger_ are detected it exits with code _0_.

# Test case
In the following image you can see an example of execution of the program and it resulting exit code set to _0_.

<img src="https://raw.githubusercontent.com/enkomio/media/master/CheckDebuggerCrossArchitecture/NormalExitProcess.png">

If you run the program under a debugger (in this case x64dbg) and set a breakpoint on _ExitProcess_ you can see that the first argument (the exit code) is set to 1, this means that the debugger was detected:

<img src="https://raw.githubusercontent.com/enkomio/media/master/CheckDebuggerCrossArchitecture/DebuggerExitProcess.png">

Ok, nothing new for now, to achive this result is not necessary to transit to x64 mode. Now, let's try to set the _BeingDebugged_ flag from _PEB_ to _0_. This can be easily achived in x64dbg with the following command:

    byte:[peb()+2]=0
    
the result of the execution is visible in the following image:

<img src="https://raw.githubusercontent.com/enkomio/media/master/CheckDebuggerCrossArchitecture/ShowDebuggedPEB.png">

Now, in theory, the exit code should be _0_, but if you run the code until the breakpoint you will see that the exit code is still _1_ (surprise!).

# Building the code
The project is written in _Assembly_ with Visual Studio 2017, it compiles in both x64 and x86. To create the final program I first compiled it in x64, then I dumped the resulting binary code and copied it, as raw data, in the x86 code.

A pre-compiled binary is found <a href="https://github.com/enkomio/media/blob/master/CheckDebuggerCrossArchitecture/CheckDebuggerCrossArchitecture.exe">here</a>

# Conclusion
This toy project may cause some problems if you don't know the theory behind it. Anyway, even if you don't want to mess around with patching and stuff like that, x64dbg offer a very handy option to bypass the check (it is your duty to find it :P).

# References
[1] PEB - <a href="https://docs.microsoft.com/en-us/windows/desktop/api/winternl/ns-winternl-_peb">https://docs.microsoft.com/en-us/windows/desktop/api/winternl/ns-winternl-_peb</a>

[2] WoW64_call.cpp - <a href="https://gist.github.com/Cr4sh/76b66b612a5d1dc2c614">https://gist.github.com/Cr4sh/76b66b612a5d1dc2c614</a>

[3] Heaven's Gate: 64-bit code in 32-bit file - <a href="https://download.adamas.ai/dlbase/Stuff/VX%20Heavens%20Library/vrg16.html">https://download.adamas.ai/dlbase/Stuff/VX%20Heavens%20Library/vrg16.html</a>
