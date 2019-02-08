let alphabet = "ABCD"

let rec generate(str, len) =
    alphabet
    |> Seq.map(fun c ->
        let newString = str + c.ToString()
        if newString.Length > len 
        then [newString] :> string seq
        else generate(newString, len)
    )
    |> Seq.concat

[<EntryPoint>]
let m _ = 
    for i=0 to alphabet.Length-1 do
        generate("", i) 
        |> Seq.iter(printfn "%s")
    0
