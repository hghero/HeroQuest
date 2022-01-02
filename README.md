# HeroQuest
An open source C++ implementation of the well-known board game for Windows!

This project has been started already in 2012. My first approach was to use the Allegro engine.
When it came to implementing dialogs, however, I realized that Allegro was not mighty enough, so
I decided to switch to Qt. After having worked with Eclipse CDT a long time, I switched to
VS Code in 2021, because it was a pain to configure the Makefile toolchain together with
the include paths correctly in Eclipse. Furthermore, VS Code supports in-IDE debugging of
MS-compiled stuff (which Eclipse CDT doesn't).

As I don't have much time to push this project, progress is being made very slowly only.

Currently only the first two levels are playable.

# Work and compile
* see "/howto_config/Howto install.txt" on hints how to configure your environment to be able to compile and debug this package
* currently used tools:
  * development and debugging: VS Code
  * compile and link: Visual Studio 2022 compiler and linker, Cygwin (only using make.exe)
  * further libs: Qt 6.2.2

# Version History
* 210116 Initial import.
* 210523 Level 2, equipment cards, buy equipment after level finish.
* 211231 Use VS Code instead of Eclipse CDT, switch to 64 bit compilation, update from Qt5 to Qt6, remove boost, switch to 2022 compiler, switch to C++17.