# Compare two directories and show the files that differ

## Download

You can download a pre-compiled binary from the <a href="https://github.com/enkomio/Misc/tree/master/CompareDirectory/Binary">Binary</a> directory.


## Usage

````shell
C:\Compare.exe b a
-=[ Comparison Tool ]=-
Copyright (c) 2019 Enkomio

Files that differ from 'b' (#64) and 'a' (#64): #6

[++] Summary
New Files: #0
Files with difference hash: #5
Files that were moved: #1

[++] Details

[+] Files with difference hash: #5
File: CompareDirectory\Program.fs (Len 1: 1921 - Len 2: 7006) - Match: N/A
File 1 SHA-1: B77187B22454E574756170F5BEFAD859040DFA5F
File 2 SHA-1: 01F5D3F26171FCEBC1E4C4D4F724C98912265CDC
File 1 SSDEEP: 24:AusvKyx078zBG2iYHoVC/+h9tOAPtoSfI25xEVTOUd8ORLpvxMOhNq7S/Ohr0Ywx:Abbk6iYIVC/MLOAPtoT21U9zGg1yL/Ib
File 2 SSDEEP: 96:yuN26CySHHy40RAOI12ZhpkitL6FD7bJ1LU2nkJ0v92gTp7FrkiAPPxa:xBS6eOMKGCGVTLU2mPY
+-----------------------------------------------+
File: CompareDirectory\README.md (Len 1: 444 - Len 2: 558) - Match: 91%
File 1 SHA-1: 58914409FEF099EE70DE9E24D3C6A90C738091BE
File 2 SHA-1: 15DCBC8BBD9C832EBC7B8F0A60F4172052D49788
File 1 SSDEEP: 12:uee/iOA1Rl9zlT+crH/rLiCYlgJ8Lxx0UFO7lgDbJLNuJb:u7iNrvx73eCZeMGhRIJb
File 2 SSDEEP: 12:uee/iOA1Rl9zlT+crH/rLiCYlgJ8Lxx0UFO7lgDbJLNuJL2ytzkxXv:u7iNrvx73eCZeMGhRIJLFtEv
+-----------------------------------------------+
File: CompareDirectory\CompareDirectory.fsproj (Len 1: 3530 - Len 2: 3852) - Match: 85%
File 1 SHA-1: E700A1401EAABB794916C4E1F44C2EC253FA2ECD
File 2 SHA-1: BD01080C05403CB15B72D020FEEBD01FF35DF463
File 1 SSDEEP: 96:zaPnpqBAX5SVSvS4OcGOcCFWERBx5+VVpgcl/:z/iXokDMCDQph/
File 2 SSDEEP: 96:zaPnpqBAX5SsSKS4OcGOcCF+ERBx5+Vp/q3Rcl/:z/iXo/+MC3mp/q3e/
+-----------------------------------------------+
File: Misc.sln (Len 1: 6914 - Len 2: 8252) - Match: 83%
File 1 SHA-1: ADC0B209EE3AE23B09CE53B17CA4ADF803EA9486
File 2 SHA-1: E40F569F76BCC1D75FBE16224E1652430D1A5317
File 1 SSDEEP: 48:qPxEjSfbb4ECEdjGfDpDFDFDdEW8AsvZRlzYXV7yqb4HhWHqmxf4hJqwVHhbHV1a:qe5ECEdKHEsqYBb/web5tHpLHxt7Bkkv
File 2 SSDEEP: 96:qe5ECEdKHEsqREUZqU+Bb/web5tHpLHxt7BYk+Y:/5ECEdKHEHEUZqU+Bb/wtY
+-----------------------------------------------+
File: CompareDirectory\Binary\Compare.exe (Len 1: 2688000 - Len 2: 2709504) - Match: 33%
File 1 SHA-1: CDB1497C828D5CAD8C5903279F3B284C7747FFBA
File 2 SHA-1: 141055734D275D7143A0FDEB400E40BBDA81F8FC
File 1 SSDEEP: 24576:mqgKhj0Cp9+7hyUOf08f8ogxsfwgwxcpfWEHC7oG24OY9e:mqgKhj0Cp9+76f05xoWX7lO
File 2 SSDEEP: 24576:Vc1dZuLJlC+y4UkCOf08f8ova3W8nMZiuuUp2CECC:VcoJVf0dWJiu+Cb
+-----------------------------------------------+

[+] Files that were moved: #1
File 1: CompareDirectory\packages.config
File 2: WordsGenerator\packages.config
+-----------------------------------------------+
````

## Package

To create a single file, once compiled go to the output directory and run:

```
ILRepack.exe /out:Compare.exe "CompareDirectory.exe" FSharp.Core.dll System.ValueTuple.dll SsdeepNET.dll
```

You must have <a href="https://github.com/gluck/il-repack">ILRepack.exe</a> in the same directory.
