# README #

## Introduction ##
LibMonitorSwap is a simple Win32 library designed to make it easy 
for applications to change the primary monitor. 

All exported functions use a WinAPI style calling conventions, 
and can be used from any language that can consume a DLL.

## Background ##
The library was designed out of frustration with certain games,
which do not offer a mechanism to control which monitor they use. 
With this library, it is easy to incorporate a monitor selection
option without modifying any other code. 

Alternatively, an executable will be added to the solution at
a later date that will provide a mechanism to query and change
default monitors from the command line. If an application doesn't
support multiple monitors, you can write a simple batch file, 
calling the exe before starting the application.

## Building ##
The library was designed and tested using Visual Studio 2013
(Community Edition). 

The library's code is pure C, and should not require any 
non-standard libraries to compile (though you will need the
Windows SDK). If you install Visual Studio Community Edition,
you will have everything you need.

## Tests ##
The solution also has a unit test library, which uses the Visual Studio
unit testing framework. Though the tests are not exhaustive,
they do provide a simple sanity check to ensure everything is working.

## Documentation ##
Doxygen documentation is currently a work in progress. For the moment,
please refer to the comments in each function, and the included unit 
test project for information on how to use library functions.

## License ##
This code is licensed under the ISC license - feel free to use whatever you
find useful!



