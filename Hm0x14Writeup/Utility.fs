namespace Hm0x14Writeup

open System

module Utility =
    let hashChunk(password: String, offset: Int32, rounds: Int32) =
        let mutable result = 1UL
        for i=0 to rounds-1 do
            result <- (result * 0x1FUL) + uint64 password.[i + offset]
        (result <<< 5) &&& 0x00000000FFFFFFFFUL

    let generateKey(password: String) =
        let keys = [
            let size = password.Length >>> 2
            for i=0 to 3 do
                let value = hashChunk(password, i * size, size)
                yield BitConverter.GetBytes(value)
        ]     

        (keys.[1], keys.[0])

    let mangleKey(k0: Int32, k1: Int32, k2: Int32, k3: Int32) = [|
        byte k0 <<< 5
        byte k1 <<< 1
        byte k2 <<< 1
        byte k3 <<< 1
        0uy
        0uy
        0uy
        0uy
    |]

    let getKeys() = seq {
        for i0=0 to 0x7 do
            Console.WriteLine("Start iteration {0} of 7 at {1} ", i0, DateTime.Now)
            for i1=0 to 0x7F do
                for i2=0 to 0x7F do
                    for i3=0 to 0x7F do
                        yield (mangleKey(i0, i1, i2, i3))
    }

