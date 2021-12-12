import Foundation
import PlayWave

if CommandLine.argc > 1 {
    try! playWave(file: CommandLine.arguments[1])
} else {
	listAlsadev("pcm")
}

public func playWave(file: String) throws {
    let data = try Data(contentsOf: URL(fileURLWithPath: file))
	let _ = data.withUnsafeBytes { ptr in
    	playWave(ptr.baseAddress!, "default")
	}
}