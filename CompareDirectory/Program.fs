open System

open System
open System.Text.RegularExpressions
open System.IO
open System.Security.Cryptography
open System.Reflection
open SsdeepNET

[<CustomEquality; CustomComparison>]
type File = {
    FullPath: String
    Name: String
    Hash: String   
    Ssdeep: String
} with
    override this.Equals(f: Object) =
        match f with
        | :? File as f ->
            f.Name.Equals(this.Name, StringComparison.Ordinal)
            && f.Hash.Equals(this.Hash, StringComparison.OrdinalIgnoreCase)
        | _ -> false

    override this.GetHashCode() =
        this.Hash.GetHashCode()

    interface IComparable with
        member this.CompareTo(o: Object) =
            match o with
            | :? File as f -> StringComparer.CurrentCulture.Compare(f.Name, this.Name)
            | _ -> 1
            

type Difference = {
    File1: File
    File2: File option
    Comparison: Int32 option
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
            Name = Regex.Replace(file, "$" + directory, String.Empty).Trim('.').Trim(Path.DirectorySeparatorChar)
            Hash = toSHA1(content)
            Ssdeep = Hasher.HashBuffer(content, content.Length)
        })
        |> Array.sortBy(fun file -> file.Name)
    else
        let content = File.ReadAllBytes(directory)
        [|{
            FullPath = directory
            Name = Path.GetFileName(directory).Trim('.').Trim(Path.DirectorySeparatorChar)
            Hash = toSHA1(content)
            Ssdeep = Hasher.HashBuffer(content, content.Length)
        }|]

let computeDifferences(directory1: File array, directory2: File array) =
    let set1 = Set.ofArray directory1
    let set2 = Set.ofArray directory2
    
    Set.difference set1 set2
    |> Seq.map(fun file ->
        match directory2 |> Array.tryFind(fun f -> f.Name.Equals(file.Name)) with
        | Some file2 -> ({
            File1 = file
            File2 = Some file2
            Comparison = Comparer.Compare(file.Ssdeep, file2.Ssdeep) |> Some
        })
        | None -> ({
            File1 = file
            File2 = None
            Comparison = None
        })
    )
    |> Seq.toArray

let printResult (dir1: String) (dir1File: File array) (dir2: String) (dir2File: File array) (difference: Difference array) =
    Console.WriteLine("Files present in '{0}' [#{1}] and not in '{2}' [#{3}]: #{4}", dir1, dir1File.Length, dir2, dir2File.Length, difference.Length)
    Console.WriteLine()

    difference 
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
            Console.WriteLine("File '{0}' sha-1: {1} is not present", difference.File1.Name, difference.File1.Hash)
        Console.WriteLine("-----------------------------------------------")
    )

[<EntryPoint>]
let main argv = 
    if argv.Length < 2 then
        Console.WriteLine("Usage: {0} <directory1> <directory2>", Path.GetFileName(Assembly.GetEntryAssembly().Location))
    else
        Console.WriteLine("-=[ Comparison Tool ]=-")
        let directory1 = scanDirectory(argv.[0])
        let directory2 = scanDirectory(argv.[1])
        let printDifference = printResult argv.[0] directory1 argv.[1] directory2
        computeDifferences(directory1, directory2) |> printDifference
    0
