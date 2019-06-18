namespace ES.WordsGenerator

open System

module RecursiveGenerator =
    let rec private generateByLength(alphabet, str, len) =
        alphabet
        |> Seq.map(fun c ->
            let newString = str + c.ToString()
            if newString.Length > len 
            then [newString] :> string seq
            else generateByLength(alphabet, newString, len)
        )
        |> Seq.concat        

    let generate(alphabet: String, passwordLength: Int32) =
        let mutable counter = 0
        for i=0 to passwordLength-1 do
            generateByLength(alphabet, String.Empty, i)             
            |> Seq.iter(fun pwd -> 
                counter <- counter + 1
                Console.WriteLine(String.Format("{0}: {1}", counter, pwd))
            )