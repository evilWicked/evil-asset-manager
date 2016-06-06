evil loader
============

Placeholder while I am building.
   
About
-----

Provides a combined, resource loader, bitmap and spritesheet management tool. Simply put it loads files. 

More extensively the evil loader provides a centralized assets and resource management tool.  You provide a list of files in JSON format and it goes away and loads them using as many threads as it can gets its greedy little hands on signalling you when its done.  Assets can then be accessed by keyval.

It will optionally pack images into a texture atlas using a skyline packing algorithm. 

For a final trick it enables you to generate object hierarchies.  You register callbacks with it and it will call these as it loads to construct objects the parameters to the callback.   This is intended to be used to allow gui's for an audio processing suite to be defined in JSON, but can be used for anything.

Documentation
-------------

No ready yet. It will be doxygen available here on git

Installation
------------


Compiler Options
----------------
Nothing special. Just treat it as your own code and optimise to suit your own needs.
 
Dependencies
------------
Other than other evil dependencies (included in the source) it utilises Sean Barrett's STB libraries https://github.com/nothings/stb
also included in the source.

Built using C++11 std libraries.

Examples
--------

Yep there will be some of these

Usage
-----

Yep I will tell you how to use it

Multithreading
--------------

It will use multithreading and allow resources to be access in a threadsafe manner.

Releases
--------

0.0.0  Still in development


License
-------

BSD like.