# Hex-Rays decompiler test

This project aims to show that relying on decompiler isn't always a good idea, since very important information may be missing.

In the <a href="https://github.com/enkomio/Misc/tree/master/Hex-Rays/main.asm">example code</a> the decompiled pseudo-code doesn't show the invocation of <i>func_b</i> but if the code is executed you will see that it is invoked.

The cause of the "problem" since to be the CMOVcc instruction.

Tested on Hex-Rays decompiler v7.4.0.191112 (which for the record is an awesome product ^^)

## Video

Click the image below to see a demo video.

[![Hiding code from Hex-Rays decompilation pseudo-code](http://i3.ytimg.com/vi/LzDaOTOJkVU/hqdefault.jpg)](https://www.youtube.com/watch?v=LzDaOTOJkVU)
