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
    Usage: kartograf_cli world-number [options]
       or: kartograf_cli directory-name [options]

    Options:
     -h [ --help ]                         produce help message

     -m [ --render-mode ] arg (=top-view)  'top-view'  or '0'
                                           'oblique'   or '1'
                                           'isometric' or '2'

     -q [ --shadow-quality ] arg (=normal) 'normal'    or '0'
                                           'high'      or '1'
                                           'ultra'     or '2'

     -d [ --sun-direction ] arg (=NW)      where the sun light comes from (note:
                                           this direction is already relative to
                                           the rotated map):
                                           'NW', 'W', 'SW', 'S',
                                           'SE', 'E', 'NE', 'N'

     -s [ --shadow-strength ] arg (=60)    set to 0 to disable shadows
                                           set higher to make shadows stronger

     -l [ --relief-strength ] arg (=10)    set to 0 to disable relief effect
                                           set higher to make relief effect
                                           stronger

     -r [ --rotation ] arg (=N)            the top of the map points to:
                                           'N', 'W', 'S', 'E'

     -n [ --night-mode ]                   enables night mode
     --height-map-mono                     renders monochrome height map
     --height-map-color                    renders color height map
