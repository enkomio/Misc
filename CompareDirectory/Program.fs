open System

open System
open System.Text.RegularExpressions
open System.IO
open System.Security.Cryptography
open System.Reflection
open SsdeepNET

type Hash = {
    Sha1: String
    Ssdeep: String
}

type File = {
    FullPath: String
    Name: String
    Hash: Hash
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

let ssdeep(buffer: Byte array) =
    Hasher.HashBuffer(buffer, buffer.Length)

let hash(content: Byte array) = {
    Sha1 = toSHA1(content)
    Ssdeep = ssdeep(content)
}

let compare(file1: File, file2: File) =
    try
        // this is necessary to bypass an error in compare
        if file1.Hash.Ssdeep.Equals(file2.Hash.Ssdeep, StringComparison.OrdinalIgnoreCase) then Some 100
        elif file1.Length = 0 || file2.Length = 0 then Some 0
        else Comparer.Compare(file1.Hash.Ssdeep, file2.Hash.Ssdeep) |> Some
    with _ ->
        None

let scanDirectory(directory: String) =
    if File.GetAttributes(directory).HasFlag(FileAttributes.Directory) then
        Directory.GetFiles(directory, "*.*", SearchOption.AllDirectories)
        |> Array.map(fun file -> (file, File.ReadAllBytes(file)))
        |> Array.map(fun (file, content) -> {
            FullPath = file
            Name = Regex.Replace(file, "^" + Regex.Escape(directory), String.Empty).Trim('.').Trim(Path.DirectorySeparatorChar)            
            Hash = hash(content)
            Length = content.Length
        })
        |> Array.sortBy(fun file -> file.Name)
    else
        let content = File.ReadAllBytes(directory)
        [|{
            FullPath = directory
            Name = Path.GetFileName(directory).Trim('.').Trim(Path.DirectorySeparatorChar)
            Hash = hash(content)
            Length = content.Length
        }|]

let computeDifferences(directory1: File array, directory2: File array) =
    let fileByName = directory2 |> Array.map(fun file -> (file.Name, file)) |> dict
    let fileByHash = directory2 |> Array.map(fun file -> (file.Hash.Sha1, file)) |> dict

    directory1
    |> Array.map(fun file ->
        let sameName = fileByName.ContainsKey(file.Name)
        let sameHash = fileByHash.ContainsKey(file.Hash.Sha1)
        (file, sameHash, sameName)
    )
    |> Array.filter(fun (_, sameHash, sameName) -> not (sameName && sameHash))
    |> Array.map(fun (file, sameHash, sameName) ->
        if sameName then 
            (file, Some <| fileByName.[file.Name], DifferenceReason.DifferentHash)
        elif sameHash then
            (file, Some <| fileByHash.[file.Hash.Sha1], DifferenceReason.DifferentName)
        else 
            (file, None, DifferenceReason.New)
    )
    |> Array.map(fun (file1, file2, reason) ->
        {
            File1 = file1
            File2 = file2
            Comparison = 
                match file2 with
                | Some file2 -> compare(file1, file2)                    
                | None -> None
            Reason = reason
        }
    )

let printResult (dir1: String) (dir1File: File array) (dir2: String) (dir2File: File array) (difference: Difference array) =
    Console.WriteLine("Files present in '{0}' (#{1}) and not in '{2}' (#{3}): #{4}", dir1, dir1File.Length, dir2, dir2File.Length, difference.Length)
    Console.WriteLine()    

    let differences =
        difference
        |> Array.groupBy(fun difference -> difference.Reason)
        |> Array.map(fun (reason, array) ->
            (reason, 
                array
                |> Array.sortByDescending(fun file ->
                    match file.Comparison with
                    | Some value -> value
                    | None -> 101
                )
            )
        )

    let extract(inputReason: DifferenceReason) = 
        match differences |> Array.tryFind(fun (reason, _) -> reason = inputReason) with
        | Some (_, value) -> value
        | None -> Array.empty
    
    let newFiles = extract(DifferenceReason.New)
    let differentHasehs = extract(DifferenceReason.DifferentHash)
    let differentName = extract(DifferenceReason.DifferentName)

    // print summary
    Console.WriteLine("[++] Summary")
    Console.WriteLine("New Files: #{0}", newFiles.Length)
    Console.WriteLine("Files with difference hash: #{0}", differentHasehs.Length)
    Console.WriteLine("Files that were moved: #{0}", differentName.Length)

    Console.WriteLine()
    Console.WriteLine("[++] Details")

    // print new files
    if newFiles.Length > 0 then
        Console.WriteLine()  
        Console.WriteLine("[+] New Files: #{0}", newFiles.Length)
        newFiles
        |> Array.iter(fun difference ->
            Console.WriteLine("SHA-1: {0} - Len: {1} - File: {2}", difference.File1.Hash.Sha1, difference.File1.Length, difference.File1.Name)
        )

    // print different hash
    if differentHasehs.Length > 0 then
        Console.WriteLine()  
        Console.WriteLine("[+] Files with difference hash: #{0}", differentHasehs.Length)
        differentHasehs
        |> Array.iter(fun difference ->
            let file2 = difference.File2.Value
            let comparison =
                match difference.Comparison with
                | Some v -> v.ToString() + "%"
                | None -> "N/A"

            Console.WriteLine("File: {0} (Len 1: {1} - Len 2: {2}) - Match: {3}", difference.File1.Name, difference.File1.Length, difference.File2.Value.Length, comparison)
            Console.WriteLine("File 1 SHA-1: {0}", difference.File1.Hash.Sha1)
            Console.WriteLine("File 2 SHA-1: {0}", file2.Hash.Sha1)
            Console.WriteLine("File 1 SSDEEP: {0}", difference.File1.Hash.Ssdeep)
            Console.WriteLine("File 2 SSDEEP: {0}", file2.Hash.Ssdeep)
            Console.WriteLine("+-----------------------------------------------+")
        )

    // print different names
    if differentName.Length > 0 then
        Console.WriteLine()  
        Console.WriteLine("[+] Files that were moved: #{0}", differentName.Length)
        differentName
        |> Array.filter(fun difference -> difference.File1.Length > 0)
        |> Array.iter(fun difference ->
            Console.WriteLine("File 1: {0}", difference.File1.Name)
            Console.WriteLine("File 2: {0}", difference.File2.Value.Name)
            Console.WriteLine("+-----------------------------------------------+")
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
