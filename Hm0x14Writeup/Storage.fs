namespace Hm0x14Writeup

open System
open System.Linq
open System.Collections.Generic
open System.Text
open System.IO

module Storage =
    let private _storagefilename = "storage.txt"

    let storageExists() =
        File.Exists(_storagefilename)

    let saveEncryptedText(storage: Dictionary<String, Byte array>) =    
        let content = new StringBuilder()
        content.AppendLine("Encrypted plaintext, Used key") |> ignore

        // write content
        storage
        |> Seq.iter(fun kv -> 
            content.AppendFormat("{0},{1}", kv.Key, BitConverter.ToString(kv.Value)).AppendLine() |> ignore
        )

        // write file
        if File.Exists(_storagefilename) then File.Delete(_storagefilename)
        File.WriteAllText(_storagefilename, content.ToString())

        Console.WriteLine("-=[ Storage saved to filesystem ]=-")

    let loadEncryptedText(storage: Dictionary<String, Byte array>) =
        Console.WriteLine("-=[ Populate storage from pre-built table: {0} ]=-", DateTime.Now)
        File.ReadAllLines("storage.txt") 
        |> Array.skip(1)
        |> Array.iter(fun line ->
            let items = line.Split([|','|])
            let encryptedText = items.[1].Trim().Replace("-", String.Empty)
            let buffer =
                Enumerable
                    .Range(0, encryptedText.Length / 2)
                    .Select(fun x -> Convert.ToByte(encryptedText.Substring(x * 2, 2), 16))
                    .ToArray()

            storage.[items.[0].Trim()] <- buffer
        )