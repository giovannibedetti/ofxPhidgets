ofxPhidgets
========

Naoto HIEDA


Download and install drivers from [Phidgets Support](http://www.phidgets.com/docs/Language_-_C/C%2B%2B#Quick_Downloads)

In Build Settings -> Search Paths -> Header Search Paths, add `/Library/Frameworks/Phidget21.framework/Headers`

In General -> Linked Frameworks and Libraries, add `/Library/Frameworks/Phidget21.framework`


WINDOWS - Visual Studio 2012

Project Properties -> Configuration Properties -> Linker -> Input -> Additional Dependencies -> Edit -> Add C:\Program Files\Phidgets\x86\phidget21.lib
