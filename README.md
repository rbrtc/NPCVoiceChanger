# NPC Voice Changer

This is an SKSE plugin for Skyrim Special Edition that allows the user to change
the voice type of NPCs during runtime. Settings are saved to a JSON file and it
is read every time the user loads into a game.

A more detailed description can be found at the [Nexus mod page](https://www.nexusmods.com/skyrimspecialedition/mods/182334).

## Build Requirements
* [XMake](https://xmake.io) [3.0.0+]
* C++23 Compiler (MSVC, Clang-CL)
* [CommonLibSSE NG](https://github.com/alandtse/CommonLibVR)
* [CLibUtil](https://github.com/powerof3/CLibUtil)
* [SKSE Menu Framework](https://github.com/QTR-Modding/SKSE-Menu-Framework-3/blob/master/resources/SKSEMenuFramework.h)
* [simpleini](https://github.com/brofield/simpleini/blob/master/SimpleIni.h)
* [nlohmann/json](https://github.com/nlohmann/json/blob/develop/include/nlohmann/json.hpp)

## End User Requirements
* [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
* [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
* [powerofthree's Tweaks](https://www.nexusmods.com/skyrimspecialedition/mods/51073)

## Credits
qudix's [CommonLibSSE-NG Plugin Template](https://github.com/libxse/commonlibsse-ng-template)  
zfroggyman's [Mysterious Stranger](https://www.nexusmods.com/skyrimspecialedition/mods/180400) source code  
RavenKZP's [In-Game Patcher](https://github.com/RavenKZP/In-Game_Patcher) source code
