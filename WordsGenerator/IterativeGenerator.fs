namespace ES.WordsGenerator

open System

module IterativeGenerator =

    let rec tryIncrement(currentPassword: Byte array, index: Int32, alphabet: String) =
        if index >= currentPassword.Length then 
            false
        else
            currentPassword.[index] <- currentPassword.[index] + 1uy
            if currentPassword.[index] >= byte alphabet.Length then            
                currentPassword.[index] <- 0uy
                tryIncrement(currentPassword, index + 1, alphabet)
            else
                true

    let rec getPasswords(currentPassword: Byte array, alphabet: String) = seq {
        yield currentPassword
        if tryIncrement(currentPassword, 0, alphabet) then
            yield! getPasswords(currentPassword, alphabet)
    }

    let generate(alphabet: String, passwordLength: Int32) =
        let mutable counter = 0
        for curLen=1 to passwordLength do
            let currentPassword = Array.zeroCreate<Byte>(curLen)
            getPasswords(currentPassword, alphabet)
            |> Seq.map(fun buffer -> buffer |> Seq.map(fun b -> alphabet.[int32 b]))
            |> Seq.map(fun pwdChars -> new String(pwdChars |> Seq.toArray |> Array.rev))
            |> Seq.iter(fun pwd -> 
                counter <- counter + 1
                Console.WriteLine(String.Format("{0}: {1}", counter, pwd))
            )