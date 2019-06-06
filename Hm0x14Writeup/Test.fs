namespace Hm0x14Writeup

open System
open System.Linq

module Test =
    open System.Text

    let decryptTest() =
        // strong pwd: qwertyuiopasdfghjklzxcvbnmqwerty
        let encryptedText = "79C01C16354CC0D5EB39FD0ACE55FF53D717109D80BCEDD90AB546C0AD0E5D292BE271E5B3A93E17"
        let ciphertext = 
            Enumerable
                .Range(0, encryptedText.Length / 2)
                .Select(fun x -> Convert.ToByte(encryptedText.Substring(x * 2, 2), 16))
                .ToArray()

        // real keys
        let encryptKey = BitConverter.GetBytes(0xF54E86E0UL)
        let decryptKey = BitConverter.GetBytes(0x10a8d5a0UL)

        // clear the last bit
        encryptKey.[0] <- encryptKey.[0] &&& byte 0xFE
        encryptKey.[1] <- encryptKey.[1] &&& byte 0xFE
        encryptKey.[2] <- encryptKey.[2] &&& byte 0xFE

        decryptKey.[0] <- decryptKey.[0] &&& byte 0xFE
        decryptKey.[1] <- decryptKey.[1] &&& byte 0xFE
        decryptKey.[2] <- decryptKey.[2] &&& byte 0xFE

        let decryptedString = 
            Encryption.twoDesDecrypt(encryptKey, decryptKey, ciphertext) // do an Encrypt and Decrypt
            |> Encoding.UTF8.GetString
        assert(decryptedString.StartsWith("Oggetto:"))
        Console.WriteLine(decryptedString)

    let attackTest() =
        // strong pwd: qwertyuiopasdfghjklzxcvbnmqwerty
        let encryptedText = "79C01C16354CC0D5EB39FD0ACE55FF53D717109D80BCEDD90AB546C0AD0E5D292BE271E5B3A93E17"
        let ciphertext = 
            Enumerable
                .Range(0, encryptedText.Length / 2)
                .Select(fun x -> Convert.ToByte(encryptedText.Substring(x * 2, 2), 16))
                .ToArray().[0..7]
        let plainText = Encoding.UTF8.GetBytes("Oggetto:")

        // real keys
        let encryptKey = BitConverter.GetBytes(0xF54E86E0UL)
        let decryptKey = BitConverter.GetBytes(0x10a8d5a0UL)

        // clear the last bit
        encryptKey.[0] <- encryptKey.[0] &&& byte 0xFE
        encryptKey.[1] <- encryptKey.[1] &&& byte 0xFE
        encryptKey.[2] <- encryptKey.[2] &&& byte 0xFE

        decryptKey.[0] <- decryptKey.[0] &&& byte 0xFE
        decryptKey.[1] <- decryptKey.[1] &&& byte 0xFE
        decryptKey.[2] <- decryptKey.[2] &&& byte 0xFE

        let h1 = Encryption.encrypt(ciphertext, encryptKey) |> BitConverter.ToString
        let h2 = Encryption.encrypt(plainText, decryptKey) |> BitConverter.ToString
        Console.WriteLine("Hash: {0}", h1)
        assert(h1 = h2)

    let doAttackTest(ciphertext: Byte array, plainText: Byte array, encryptKey: Byte array, decryptKey: Byte array) =
        let h1 = Encryption.encrypt(ciphertext, encryptKey) |> BitConverter.ToString
        let h2 = Encryption.encrypt(plainText, decryptKey) |> BitConverter.ToString
        Console.WriteLine("Hash: {0}", h1)
        assert(h1 = h2)