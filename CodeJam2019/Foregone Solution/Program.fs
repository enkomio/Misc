open System


let rec computeNumbers(inputA: Int32, inputB: Int32) =
    let mutable a = inputA
    let mutable b = inputB
    let mutable exponent = 0
    let mutable reDo = false

    let aStr = a.ToString().ToCharArray() |> Array.rev
    let bStr = b.ToString().ToCharArray() |> Array.rev

    for j=0 to aStr.Length-1 do
        if not reDo then
            let ac = aStr.[j]
            let bc = bStr.[j]

            if ac = '4' || bc = '4' then
                let increment = 1 * (int32 <| Math.Pow(10., float exponent))
                a <- a + increment
                b <- b - increment
                reDo <- true

            exponent <- exponent + 1

    if reDo 
    then computeNumbers(a, b)
    else (new String(aStr |> Array.rev), new String(bStr |> Array.rev))

[<EntryPoint>]
let main argv = 
    let testCases = Int32.Parse(Console.ReadLine().Trim())
    for i=0 to testCases-1 do
        let N = Int32.Parse(Console.ReadLine().Trim())        
        let mutable a = N / 2
        let mutable b = a

        if a+b < N then
            a <- a + 1

        let (aStr, bStr) = computeNumbers(a, b)
        Console.WriteLine("Case #{0}: {1} {2}", i+1, aStr, bStr)
    0
