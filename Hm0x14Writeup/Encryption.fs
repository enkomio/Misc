namespace Hm0x14Writeup

open System
open System.Security.Cryptography
open System.IO

module Encryption =
    let encrypt(buffer: Byte array, key: Byte array) =
        use provider = 
            new DESCryptoServiceProvider(
                Key = key, 
                IV = Array.zeroCreate<Byte>(8), 
                Mode = CipherMode.ECB, 
                Padding = PaddingMode.Zeros
            )
        use resultStream = new MemoryStream()
        use cryptoStream = new CryptoStream(resultStream, provider.CreateEncryptor(), CryptoStreamMode.Write)
        cryptoStream.Write(buffer, 0, buffer.Length)    
        cryptoStream.FlushFinalBlock()
        resultStream.ToArray()

    let decrypt(buffer: Byte array, key: Byte array) =
        use provider = 
            new DESCryptoServiceProvider(
                Key = key, 
                IV = Array.zeroCreate<Byte>(8), 
                Mode = CipherMode.ECB, 
                Padding = PaddingMode.Zeros
            )
        use cryptoStream = new CryptoStream(new MemoryStream(buffer), provider.CreateDecryptor(), CryptoStreamMode.Read)
        use resultStream = new MemoryStream()
        cryptoStream.CopyTo(resultStream)
        resultStream.ToArray()
    
    let twoDesDecrypt(encryptKey: Byte array, decryptKey: Byte array, buffer: Byte array) =
        let encryptedBuffer = encrypt(buffer , encryptKey)
        decrypt(encryptedBuffer , decryptKey)