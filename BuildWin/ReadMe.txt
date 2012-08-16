Use the .mk files here with nmake:

nmake clean.mk

Erases all the build files (Debug, Release, DebugMemory and Distrib), plus 
side files normally not erased by VC++ clean: *.sdf and /ipch.



nmake rebuild.mk

Builds all projects in all four configurations.



nmake publish.mk

Rebuilds (clean and build) all projects in the Distrib configuration.
Then creates all the zip files for upload to the site.  

This makefile will create Demos, Games and Milligram directories under 
this directory (_BuildWin).  These are for the distributable demos, and 
can be erased after the zip files are created.

