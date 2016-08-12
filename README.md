evil asset manager
============

Placeholder while I am building.
   
About
-----

Provides a centralized resource loader and asset management tool. Simply put it loads files
and and enables you to access them from a central point.

You provide a list of files in JSON format and it goes away and loads them using as many threads as it can gets its greedy little hands on signalling you when its done.  Assets can then be accessed by keyval.

It will optionally pack images into a texture atlas using a skyline packing algorithm. 

For a final trick it provides the basis for an object factory. You define the assets that will be associated with a class in JSON format and register callbacks with it and it will call these as it loads or on demand to construct objects using the supplied assets.

The original purpose is to construct gui's for an audio processing suite where there is a lot of reuse of controls, but could be used in games, scene graphs or other data driven configuration.

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

Other than other evil dependencies (included in the source) it utilises some of Sean Barrett's STB libraries https://github.com/nothings/stb and json for modern c++ from https://github.com/nlohmann/json which have also been included in the source.

Built using C++11 std libraries.

Examples
--------

Yep there will be some of these

Usage
-----

Yep I will tell you how to use it

Multithreading
--------------

It will use multithreading and allow resources to be accessed in a threadsafe manner.

Releases
--------

0.0.0  Still in development


License
-------

BSD like.