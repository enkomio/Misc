# Hex-Rays decompiler test

This project aims to show that relying on decompiler isn't always a good idea, since very inportant information may be missing.

In the <a href="https://github.com/enkomio/Misc/tree/master/Hex-Rays/main.asm">example code</a> the decompiled code doesn't show the invocation of <i>func_b</i> but if the code is executed you will see that it is invoked.

The cause of the "problem" since to be the CMOVcc instruction.

Tested on Hex-Rays decompiler v7.4.0.191112