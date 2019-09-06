# Compare two directories and show the files that differ

## Download

You can download a pre-compiled binary from the <a href="https://github.com/enkomio/Misc/tree/master/CompareDirectory/Binary">Binary</a> directory.


## Usage

Compare.exe <dir 1> <dir 2>

## Pack

To create a single file, once compiled go to the output directory and run:

```
ILRepack.exe /out:Compare.exe "CompareDirectory.exe" FSharp.Core.dll System.ValueTuple.dll SsdeepNET.dll
```
