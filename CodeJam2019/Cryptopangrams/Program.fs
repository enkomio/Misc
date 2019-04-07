open System
open System.Collections.Generic

let getPrimes(N: UInt64) = [|
    for i=2UL to N do
        let mutable ok = true
        for j=2UL to i-1UL do            
            if ok &&  i % j = 0UL then
                ok <- false
        if ok then
            yield uint64 i
|]

let primes = getPrimes(10000UL)

let alphabet = [
    'A'; 'B'; 'C'; 'D'; 'E'; 'F'; 'G'; 'H'; 'I'; 
    'J'; 'K'; 'L'; 'M'; 'N'; 'O'; 'P'; 'Q'; 'R'; 
    'S'; 'T'; 'U'; 'V'; 'W'; 'X'; 'Y'; 'Z'
]

type Decomposition = {
    FirstFactor: UInt64
    SecondFactor: UInt64
}

let factorize(number: UInt64, primes: UInt64 seq, N: UInt64) =
    primes
    |> Seq.filter(fun n -> n <= N)
    |> Seq.map(fun n ->
        let reminder = ref(int64 0)
        let result = Math.DivRem(int64 number, int64 n, reminder) |> uint64
        if !reminder = int64 0 && result <= N then        
            let decomposition = {FirstFactor = n; SecondFactor = result}
            Some decomposition
        else
            None
    )
    |> Seq.filter(Option.isSome)    
    |> Seq.map(Option.get)
    |> Seq.head

let getFirstChar(numbers: UInt64 array, N: UInt64) =
    let decomposition1 = factorize(numbers.[0], primes, N)
    let decomposition1Set = [decomposition1.FirstFactor; decomposition1.SecondFactor]
    let decomposition2 = factorize(numbers.[1], decomposition1Set, N)
    let decomposition2Set = [decomposition2.FirstFactor; decomposition2.SecondFactor]

    // check which is the first character
    if decomposition2Set |> List.contains decomposition1.FirstFactor 
    then decomposition1.SecondFactor
    else decomposition1.FirstFactor
  
let tryDecomposeAllNumbers(numbers: UInt64 array, firstChar: UInt64, N: UInt64) = 
    let mutable success = true
    ([|
        // check which is the first character
        let mutable previousChar = firstChar
        yield previousChar

        // return all remaining chars
        for number in numbers do
            if success then
                let factor = number / previousChar |> uint64
                if factor > N || primes |> Array.contains factor |> not then
                     success <- false

                yield factor
                previousChar <- factor
    |], success)
  
let decomposeAllNumbers(numbers: UInt64 array, N: UInt64) =
    let firstChar = getFirstChar(numbers, N)
    match tryDecomposeAllNumbers(numbers, firstChar, N) with
    | (decomposition, true) -> decomposition
    | _ -> 
        // something went wrong, try the other char
        let secondChar = (numbers.[0] / firstChar)
        tryDecomposeAllNumbers(numbers, secondChar, N) |> fst

let extractKeys(kb: UInt64 array) =
    kb
    |> Seq.distinct
    |> Seq.sort
    |> Seq.mapi(fun i key -> (key, alphabet.[i]))
    |> dict

let decrypt(keys: IDictionary<UInt64, Char>, clearText: UInt64 array) =
    clearText
    |> Seq.map(fun number -> keys.[number])
    |> fun l -> (new String(l |> Seq.toArray))

let decryptText(numberList: String, N: UInt64) =
    let numbers = numberList.Trim().Split(' ') |> Array.map(UInt64.Parse)
    let clearText = decomposeAllNumbers(numbers, N)
    let keys = extractKeys(clearText)
    decrypt(keys, clearText)

[<EntryPoint>]
let main argv =    
    let testCases = Int32.Parse(Console.ReadLine().Trim())
    for i=0 to testCases-1 do
        let items = Console.ReadLine().Trim().Split(' ')
        let (N, L) = (UInt64.Parse(items.[0]), Int32.Parse(items.[1]))
        let numbers = Console.ReadLine()
        let decryptedText = decryptText(numbers, N)
       
        Console.WriteLine("Case #{0}: {1}", i+1, decryptedText)
    0

