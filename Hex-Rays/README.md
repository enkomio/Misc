# Hide code in Hex-Rays decompiled pseudo-code

This project demonstrates that relying on a decompiler isn't always a good idea. As I'll show, the generated pseudo-code might miss very important information.

In this <a href="https://github.com/enkomio/Misc/tree/master/Hex-Rays/main.asm">example code</a>, the resulting Hex-Rays decompiled pseudo-code doesn't show the invocation of function <i>func_b</i>, but if the code is executed the function is invoked.

The cause of the "problem" seems to be the CMOVcc instruction. However, I don't think this can be considered as a bug, but the Hex-Rays developers can think about this case as a possibility for improvement.

Tested on Hex-Rays decompiler v7.4.0.191112 (which for the record is an awesome product ^^)

## Video

Click the image below for a demo.

[![Hiding code from Hex-Rays decompilation pseudo-code](http://i3.ytimg.com/vi/LzDaOTOJkVU/hqdefault.jpg)](https://www.youtube.com/watch?v=LzDaOTOJkVU)
