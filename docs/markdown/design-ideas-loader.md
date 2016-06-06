Design Ideas: Loader                       {#designloader}
=====================


The basic idea is we want a central point that can load all our resource files and allow us to use
same resource - in most cases bitmaps across lots of objects.  We definately dont want unique instances
of the same bitmap being used by 100 sprites.  

The same principal applies to audio files and even text strings.

We also would like to be able to define sprites in a file.

We would like to be able groups collections of sprites into controls and for that matter collections of 
controls into layouts.

in terms of usage it would be nice to register a constructor callback so that everytime we
want a sprite or a control we simply ask it for one and it goes away calls the constructor and passes 
in the appropriate bitmaps.

Internationalisation. If we are to include a string table in this thing it would be nice to have 
the ability to define strings in different languages.  This also applies to bitmaps.

When dealing with bitmaps we really would like to be able to build a texture atlas rather than
lots of individual bitmaps. If we are going to internally create a texture atlas we may as well be 
able to export it.

In order to manage lots of assets it would be nice to be able to use separate files. We have two 
ways of doing this. Simply by importing another file. Alternatively it would be nice to have an 
"include" concept in the JSON that imports the content of one file into another. 