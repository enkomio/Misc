namespace ES.WordsGenerator

open System

module IterativeGenerator =

    let rec tryIncrement(buffer: Byte array, index: Int32, alphabet: String) =
        if index >= buffer.Length then 
            false
        else
            buffer.[index] <- buffer.[index] + 1uy
            if buffer.[index] >= byte alphabet.Length then            
                buffer.[index] <- 0uy
                tryIncrement(buffer, index + 1, alphabet)
            else
                true

    let rec getPasswords(buffer: Byte array, alphabet: String) = seq {
        yield buffer
        if tryIncrement(buffer, 0, alphabet) then
            yield! getPasswords(buffer, alphabet)
    }

    let generate(alphabet: String, passwordLength: Int32) =
        let mutable counter = 0
        for curLen=1 to passwordLength do
            let buffer = Array.zeroCreate<Byte>(curLen)
            getPasswords(buffer, alphabet)
            |> Seq.map(fun buffer -> buffer |> Seq.map(fun b -> alphabet.[int32 b]))
            |> Seq.map(fun pwdChars -> new String(pwdChars |> Seq.toArray |> Array.rev))
            |> Seq.iter(fun pwd -> 
                counter <- counter + 1
                Console.WriteLine(String.Format("{0}: {1}", counter, pwd))
            )