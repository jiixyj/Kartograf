Kartograf
=========

Kartograf is a Minecraft map rendering tool. Its goal is to create pixel-perfect
World renders.
It is redistributed under the MIT license. See LICENSE file for details.

Features
--------

* Runs under Linux, Windows, OS X and FreeBSD (possible others)
* Shadow rendering
* Biome rendering (using Minecraft Biome Extractor by Donkey Kong)
* Tuned for multi-core CPUs
* Top-down, oblique and isometric projection
* CLI and GUI Interface
* Different rendering modes (night, height map)


Requirements
------------

You'll need the following libraries to compile Kartograf:

* Boost
* Qt4
* libpng
* Intel Threading Building Blocks


Installation
-----------

In the root folder, type:

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make


Usage
-----

If you'd like to have your biomes rendered, you must run Minecraft Biome
Extractor first to create a "EXTRACTEDBIOMES" dir in your world dir.

#### GUI
    ./kartograf_qt4

#### CLI
At this time, the CLI does not support many arguments. Use:

    ./kartograf_cli [world number]

to render a world. You can also specify a folder:

    ./kartograf_cli [folder name]
