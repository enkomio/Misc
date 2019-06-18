namespace ES.WordsGenerator

open System

module IterativeGenerator =

    let increment(currentPassword: Byte array, alphabet: String) =
        let mutable incrementNext = true        
        for i=0 to currentPassword.Length-1 do
            if incrementNext then
                currentPassword.[i] <- currentPassword.[i] + 1uy
                if currentPassword.[i] >= byte alphabet.Length then            
                    currentPassword.[i] <- 0uy
                else
                    incrementNext <- false

    let getNumOfPasswords(alphabet: String, length: Int32) =
        Math.Pow(float alphabet.Length, float length) |> int32

    let rec getPasswords(currentPassword: Byte array, alphabet: String) = seq {        
        for i=0 to getNumOfPasswords(alphabet, currentPassword.Length)-1 do
            yield currentPassword
            increment(currentPassword, alphabet)
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