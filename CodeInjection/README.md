# Process Injection via Thread Pol Wait

This code recreates the process injection technique used by the *RoningLoader* malware, as described in the following article:
[RONINGLOADER: DragonBreath’s New Path to PPL Abuse](https://www.elastic.co/security-labs/roningloader).

Malware sample: [da2c58308e860e57df4c46465fd1cfc68d41e8699b4871e9a9be3c434283d50b](https://www.virustotal.com/gui/file/da2c58308e860e57df4c46465fd1cfc68d41e8699b4871e9a9be3c434283d50b)

This technique appears to be a variant of the **Attacking Thread Pools** technique described in the SafeBreach article:
[The Pool Party You Will Never Forget: New Process Injection Techniques Using Windows Thread Pools](https://www.safebreach.com/blog/process-injection-using-windows-thread-pools/).

However, I identified several variations in the malware’s implementation that I found particularly interesting, so I replicated them in this project to make the technique easier to analyze and detect.

# Update
This technique is part of [SafeBreach github.com PoolParty repository](https://github.com/SafeBreach-Labs/PoolParty/blob/main/PoolParty/PoolParty.cpp#L153) nothing new :)