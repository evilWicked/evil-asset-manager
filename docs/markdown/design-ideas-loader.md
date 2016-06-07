Design Ideas: Loader                       {#designloader}
=====================
This file is just a pile of notes I have put together as I go along.  Its a sort of rough requirements and design 
notepad. It will get really messy . Code is pseudo code at best.  So DO NOT use it as a programming guide.

##Wish List

The basic idea is we want a central point that can load all our resource files and allow us to use
same resource - in most cases bitmaps across lots of objects.  We definitely dont want unique instances
of the same bitmap being used by 100 sprites.  

The same principal applies to audio files and even text strings.

We also would like to be able to define sprites in a file. We would like to be able groups collections of sprites into controls and for that matter collections of controls into layouts. In terms of usage it would be nice to be able to register some construction callback so that we can use it as a factory of some sort. Everytime we want a sprite or a control we simply ask it for one and it goes away calls the constructor and passes in the appropriate bitmaps.

Internationalisation. If we are to include a string table in this thing it would be nice to have 
the ability to define strings in different languages.  This also applies to bitmaps. Perhaps just as easy to enable different languages to live in different files that can be loaded if desired.

When dealing with bitmaps we really would like to be able to build a texture atlas rather than
lots of individual bitmaps. If we are going to internally create a texture atlas we may as well be 
able to export it.

In order to manage lots of assets it would be nice to be able to be able to spread things across separate files.

## Implementation

We would like to end up with a json file that looks something like the following.

	{
		"strings":{	"name1":"string test 1",
					"name2":"string test 2"},
					
		"bitmaps":{	"name1":{src:"some path"},
					"name2":{"src":"some path"} },
					
		"spritesheets:{
					   "name1":{"src":"some path","width":100,"height":100,"frames":3},
					   "name1":{"src":"some path","width":100,"height":100,"frames":3}},
					   
		"texture-atlas":{"name":{"src":"xxxx",
								"img-name1":{x:0,y:0,w:100,h:200},
								"img-name2":{x:100,y:0,w:100,h:200},
								"img-name3":{x:200,y:0,w:100,h:200},
								"img-name4":{x:200,y:0,w:100,h:200}
								}}
		
		fonts:{	"main":{"font-family":"berlin_sans_fb_demibold","src":"berlinsansdb/berlin-sans-fb-demi-bold-webfont",
						"font-weight":"normal","font-style":"normal"}
		styles:{
			"btn-txt":{"font":"40px berlin_sans_fb_demibold","fill":"#dddddd","align":"center"},
			"btn-txt-disabled":{"font":"40px berlin_sans_fb_demibold","fill":"#3e3e5d","align":"center"}}
			
		images:{name:{type:spritesheets,}}
		
		"audio_tracks":{
			"dialog-back-track":{"src":"backtune.ogg","from":0,"duration":172.5,"loop":true}
		},
		
		"audio_markers":{
			"effects":{
				  "src":"effects.ogg",
				  "markers":{
					  "beep":{"from":0,"duration":1.300,"loop":false},
					  "bonk":{"from":2.200,"duration":1.500,"loop":false}
				  }
			}		
	}

	//define a sprite
	"definition":{
		"typename":"sprite",
		"attributes":{"key":0,"frame":0,"pos":{x:0,y:0},"anchor":{x:0,y:0},"scale":{x:0,y:0},"angle":0,"alpha":1}
	}

	//define a control built from sprites
	definition:{
		"typename":"control",	
		"knob":{type:sprite,key:"knob_img","anchor":{"x":0.5,"y":0.5}} //override some values of a sprite
		"background":{"type":"sprite","key":"bg","anchor":{"x":0.5,"y":0.5}}
		"attributes":{"pos":{"x":0,"y":0},"anchor":{"x":0,"y":0},"scale":{"x":0,"y":0},"angle":0,"alpha":1}
	}

	definition:{
		"typename":"control2",	
		"inherits":"control"  //start with control and extend it
		"knob":{"key":"knob_img2"},
		"attributes":{"name":"xxxx}, //extend its attributes
		"children":[{type:sprite},{type:sprite},{type:sprite}]  //let it have children
	}

	//define a panel and give it some controls - overriding some default values
	definition:{
		"typename":"panel",
		"c1":{"type":"control","knob":{"key":"knob2_img"},"pos":{"x":0}} 
		"c2":{"type":"control2","knob":{"key":"knob2_img"}}
	}

	instance:{"typename":"panel"}

In the code we would like to be able to do something along the lines of create a panel when we need one

	create("panel",[](JSONobj *def)->PanelClass{
		return new PanelClass(def);
	})

alternatively when it hits an "instance" object it creates the object and passes it to a callback to do something.

	registerConstructor("panel",[&](JSONobj *def){
			panelList.insert(new PanelClass(def));
	})

We would like to have events that get triggered as files load etc etc etc.