// swift-tools-version:5.4

import PackageDescription

let package = Package(
    name: "AlsaPlayWave",
    products: [
        .library(name: "AlsaPlayWave", targets: ["AlsaPlayWave"]),
        .executable(name: "SoundPlay", targets: ["SoundPlay"])
    ],
    targets: [
        .executableTarget(name: "SoundPlay", dependencies: [
            .target(name: "AlsaPlayWave", condition: .when(platforms: [.linux]))
        ]),
        .target(name: "AlsaPlayWave", dependencies: ["CAlsa"]),
        .systemLibrary(name: "CAlsa", pkgConfig: "alsa")
    ]
)
