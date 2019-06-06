namespace Hm0x14Writeup

open System
open System.Text
open System.IO
open System.Collections.Generic
open System.Reflection

module Program =    
    let buildEncryptedTextTable(plainText: Byte array, storage: Dictionary<String, Byte array>) =
        Console.WriteLine("-=[ Start encrypt plaintext: {0} ]=-", DateTime.Now)
        let mutable i = 0
        Utility.getKeys()
        |> Seq.iter(fun key ->
            i <- i + 1
            try
                let encryptedBuffer = Encryption.encrypt(plainText, key)
                storage.[BitConverter.ToString(encryptedBuffer)] <- key                
            with e -> Console.WriteLine(e)
        )

        Console.WriteLine("-=[ End encrypt plaintext: {0} ]=-", DateTime.Now)
        Console.WriteLine()

    let populateStorage(storage: Dictionary<String, Byte array>) =
        if not <| Storage.storageExists() then
            let plainText = Encoding.UTF8.GetBytes("Oggetto:")
            buildEncryptedTextTable(plainText, storage)
            Storage.saveEncryptedText(storage) 
        else
            Storage.loadEncryptedText(storage)

    let findKey(encryptedText: Byte array, storage: Dictionary<String, Byte array>) =
        Console.WriteLine("-=[ Start identify key: {0} ]=-", DateTime.Now)
        let mutable (encKey, decKey) = (Array.empty<Byte>, Array.empty<Byte>)

        Utility.getKeys()
        |> Seq.exists(fun key ->
            try
                let encryptedBuffer = Encryption.encrypt(encryptedText, key) |> BitConverter.ToString
                if storage.ContainsKey(encryptedBuffer) then
                    decKey <- storage.[encryptedBuffer]
                    encKey <- key
                    Console.WriteLine("Encrypt Password found: " + BitConverter.ToString(encKey))
                    Console.WriteLine("Decrypt Password found: " + BitConverter.ToString(decKey))
                    true
                else 
                    false
            with _ -> 
                false
        ) 
        |> fun keyFound ->
            Console.WriteLine("-=[ End identify key: {0} ]=-", DateTime.Now)
            
            if keyFound 
            then Some (encKey, decKey)
            else None

    let getCipherText() =
        let curDir = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location)
        File.ReadAllBytes(Path.Combine(curDir, "4DES_SEGRETO"))

    [<EntryPoint>]
    let main argv = 
        let storage = new Dictionary<String, Byte array>(0xFFFFFF)
        populateStorage(storage)

        // decrypt the cipher text
        let ciphertext = getCipherText()     
        match findKey(ciphertext.[0..7], storage) with
        | Some (encKey, decKey) -> 
            // print the cipherText
            let secretMessage = Encryption.twoDesDecrypt(encKey, decKey, ciphertext)            
            Console.WriteLine("-=[ Secret message: {0} ]=-", DateTime.Now)
            Console.WriteLine(secretMessage |> Encoding.UTF8.GetString)
        | _ -> ()
        0
