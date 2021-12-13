////
/// main.swift
/// Example of library call
//
import Foundation
#if canImport(AlsaPlayWave)
import AlsaPlayWave

public func playWave(file: String) throws {
    let data = try Data(contentsOf: URL(fileURLWithPath: file))
    let _ = data.withUnsafeBytes { ptr in
        playWave(ptr.baseAddress!, "default")
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
