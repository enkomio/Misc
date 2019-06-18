namespace ES.WordsGenerator

open System

module Program =
    [<EntryPoint>]
    let main _ = 
        Console.WriteLine("-=[ Start Recursive Generation ]=-")
        RecursiveGenerator.generate("ABCD", 4)
        Console.WriteLine("-=[ End Recursive Generation ]=-")
        Console.WriteLine("-=[ Start Iterative Generation ]=-")
        IterativeGenerator.generate("ABCD", 4)
        Console.WriteLine("-=[ End Iterative Generation ]=-")
        0
