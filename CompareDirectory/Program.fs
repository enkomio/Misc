open System
open System.IO
open System.Security.Cryptography
open System.Reflection

type File = {
    Path: String
    Hash: String
}

let toSHA1(buffer: Byte array) =
    use sha1 = new SHA1Managed()
    let hash = sha1.ComputeHash(buffer)
    BitConverter.ToString(hash).Replace("-", String.Empty)

let scanDirectory(directory: String) =
    Directory.GetFiles(directory, "*.*", SearchOption.AllDirectories)
    |> Array.map(fun file -> {
        Path = file.Replace(directory, String.Empty).Trim(Path.DirectorySeparatorChar)
        Hash = toSHA1(File.ReadAllBytes(file))
    })
    |> Array.sortBy(fun file -> file.Path)

let computeDifferences(directory1: File array, directory2: File array) =
    let set1 = Set.ofArray directory1
    let set2 = Set.ofArray directory2
    let set1MinusSet2 = Set.difference set1 set2
    let set2MinusSet1 = Set.difference set2 set1
    (set1MinusSet2, set2MinusSet1)

let printSetResult(set: File Set) =
    set |> Set.iter(fun file ->
        Console.WriteLine("File: {0} - SHA1: {1}", file.Path, file.Equals)
    )

let printResult (dir1: String) (dir1File: File array) (dir2: String) (dir2File: File array) (set1MinusSet2, set2MinusSet1) =
    Console.WriteLine("File present in {0} [#{1}] and not in {2} [#{3}]", dir1, dir1File.Length, dir2, dir2File.Length)
    printSetResult(set1MinusSet2)

    Console.WriteLine()
    Console.WriteLine("File present in {0} [#{1}] and not in {2} [#{3}]", dir2, dir2File.Length, dir1, dir1File.Length)
    printSetResult(set2MinusSet1)

[<EntryPoint>]
let main argv = 
    if argv.Length <> 2 then
        Console.WriteLine("Usage: {0} <directory1> <directory2>", Path.GetFileName(Assembly.GetEntryAssembly().Location))
    else
        let directory1 = scanDirectory(argv.[0])
        let directory2 = scanDirectory(argv.[1])
        computeDifferences(directory1, directory2) |> printResult argv.[0] directory1 argv.[1] directory2
    0
