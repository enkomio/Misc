open System

open System
open System.Text.RegularExpressions
open System.IO
open System.Security.Cryptography
open System.Reflection
open SsdeepNET

type File = {
    FullPath: String
    Name: String
    Hash: String   
    Ssdeep: String
    Length: Int32
} 

type DifferenceReason =
    | New
    | DifferentName
    | DifferentHash

type Difference = {
    File1: File
    File2: File option
    Comparison: Int32 option
    Reason: DifferenceReason
}

let toSHA1(buffer: Byte array) =
    use sha1 = new SHA1Managed()
    let hash = sha1.ComputeHash(buffer)
    BitConverter.ToString(hash).Replace("-", String.Empty)

let scanDirectory(directory: String) =
    if File.GetAttributes(directory).HasFlag(FileAttributes.Directory) then
        Directory.GetFiles(directory, "*.*", SearchOption.AllDirectories)
        |> Array.map(fun file -> (file, File.ReadAllBytes(file)))
        |> Array.map(fun (file, content) -> {
            FullPath = file
            Name = Regex.Replace(file, "$" + Regex.Escape(directory), String.Empty).Trim('.').Trim(Path.DirectorySeparatorChar)
            Hash = toSHA1(content)
            Ssdeep = Hasher.HashBuffer(content, content.Length)
            Length = content.Length
        })
        |> Array.sortBy(fun file -> file.Name)
    else
        let content = File.ReadAllBytes(directory)
        [|{
            FullPath = directory
            Name = Path.GetFileName(directory).Trim('.').Trim(Path.DirectorySeparatorChar)
            Hash = toSHA1(content)
            Ssdeep = Hasher.HashBuffer(content, content.Length)
            Length = content.Length
        }|]

let computeDifferences(directory1: File array, directory2: File array) =
    let fileByName = directory2 |> Array.map(fun file -> (file.Name, file)) |> dict
    let fileByHash = directory2 |> Array.map(fun file -> (file.Hash, file)) |> dict

    directory1
    |> Array.map(fun file ->
        let sameName = fileByName.ContainsKey(file.Name)
        let sameHash = fileByHash.ContainsKey(file.Hash)
        (file, sameHash, sameName)
    )
    |> Array.filter(fun (_, sameHash, sameName) -> not (sameName && sameHash))
    |> Array.map(fun (file, sameHash, sameName) ->
        if sameName then 
            (file, Some <| fileByName.[file.Name], DifferenceReason.DifferentHash)
        elif sameHash then
            (file, Some <| fileByHash.[file.Hash], DifferenceReason.DifferentName)
        else 
            (file, None, DifferenceReason.New)
    )
    |> Array.map(fun (file1, file2, reason) ->
        {
            File1 = file1
            File2 = file2
            Comparison = 
                match file2 with
                | Some file2 -> 
                    // this is necessary to bypass an error in compare
                    Comparer.Compare(file1.Ssdeep, file2.Ssdeep) |> Some
                | None -> None
            Reason = reason
        }
    )

let printResult (dir1: String) (dir1File: File array) (dir2: String) (dir2File: File array) (difference: Difference array) =
    Console.WriteLine("Files present in '{0}' (#{1}) and not in '{2}' (#{3}): #{4}", dir1, dir1File.Length, dir2, dir2File.Length, difference.Length)
    Console.WriteLine()

    difference
    |> Array.groupBy(fun difference -> difference.Reason)
    |> Array.sortBy(fun (reason, _) ->
        match reason with
        | DifferenceReason.New -> 0
        | DifferenceReason.DifferentHash -> 1
        | DifferenceReason.DifferentName -> 2
    )
    |> Array.iter(fun (reason, differences) ->
        Console.WriteLine()
        match reason with
        | DifferenceReason.New -> Console.WriteLine("[+] New Files: #{0}", differences.Length)
        | DifferenceReason.DifferentHash -> Console.WriteLine("[+] Files with difference hash: #{0}", differences.Length)
        | DifferenceReason.DifferentName -> Console.WriteLine("[+] Files that were moved: #{0}", differences.Length)

        differences 
        |> Array.sortByDescending(fun file ->
            match file.Comparison with
            | Some value -> value
            | None -> 101
        )
        |> Array.iter(fun difference ->
            match difference.File2 with
            | Some file2 ->
                Console.WriteLine("File: {0} - Match: {1}%", difference.File1.Name, difference.Comparison.Value)
                Console.WriteLine("File 1 SHA-1: {0}", difference.File1.Hash, file2.Hash)
                Console.WriteLine("File 2 SHA-1: {0}", file2.Hash)
                Console.WriteLine("File 1 SSDEEP: {0}", difference.File1.Ssdeep)
                Console.WriteLine("File 2 SSDEEP: {0}", file2.Ssdeep)
            | None ->
                Console.WriteLine("File '{0}' sha-1: {1}", difference.File1.Name, difference.File1.Hash)
            Console.WriteLine("+-----------------------------------------------+")
        )
    )    

let printBanner() =             
    let banner = "-=[ Comparison Tool ]=-"

    let year = if DateTime.Now.Year = 2019 then "2019" else String.Format("2019-{0}", DateTime.Now.Year)
    let copy = String.Format("Copyright (c) {0} Enkomio {1}", year, Environment.NewLine)

    Console.ForegroundColor <- ConsoleColor.Cyan   
    Console.WriteLine(banner)
    Console.WriteLine(copy)
    Console.ResetColor()


[<EntryPoint>]
let main argv = 
    printBanner()

    if argv.Length < 2 then
        Console.WriteLine("Usage: {0} <directory1> <directory2>", Path.GetFileName(Assembly.GetEntryAssembly().Location))
    else
        let directory1 = scanDirectory(argv.[0])
        let directory2 = scanDirectory(argv.[1])
        let printDifference = printResult argv.[0] directory1 argv.[1] directory2
        computeDifferences(directory1, directory2) |> printDifference
    0
