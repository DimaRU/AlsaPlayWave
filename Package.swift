// swift-tools-version:5.4

import PackageDescription

let package = Package(
    name: "SoundPlay",
    products: [
        .library(name: "PlayWave", targets: ["PlayWave"]),
        .executable(name: "SoundPlay", targets: ["SoundPlay"])
    ],
    dependencies: [],
    targets: [
        .executableTarget(name: "SoundPlay", dependencies: ["PlayWave"]),
        .target(name: "PlayWave", dependencies: ["CAlsa"]),
        .systemLibrary(name: "CAlsa", pkgConfig: "alsa")
    ]
)
