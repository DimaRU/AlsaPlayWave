////
/// main.swift
/// Example of library call
//
import Foundation
#if canImport(AlsaPlayWave)
import AlsaPlayWave

public func playWave(file: String) throws {
    let data = try Data(contentsOf: URL(fileURLWithPath: file))
    let result = data.withUnsafeBytes { ptr in
        playWave(ptr.baseAddress!, "default")
    }
    if result == -1 {
        printf("Bad wav file format\n");
    } else if result < 0 {
        let s = getErrorMessage(result)
        let msg = String.init(cString: s)
        print("Error:", msg)
    }
}

#elseif canImport(Cocoa)
import Cocoa

public func playWave(file: String) throws {
    NSSound(contentsOfFile: file, byReference: false)?.play()
    sleep(1)
}

#endif

if CommandLine.argc > 1 {
    try? playWave(file: CommandLine.arguments[1])
} else {
    print("Usage: SoundPlay file.wav")
}
