# NPC Voice Changer

This is an SKSE plugin for Skyrim Special Edition that allows the user to change
the voice type of NPCs during runtime. Settings are saved to a JSON file and
loaded every time the user loads a game.

## Introduction

[Proteus](https://www.nexusmods.com/skyrimspecialedition/mods/62934)
by phenderix is an incredible mod with many unique features.
However, if you try to use it to change an NPC's voice type, the NPC will revert
to their previous voice type when the game is reloaded. This left me dissatisfied
and that's where the idea for NPC Voice Changer was born. This mod aims to provide
a functional voice type changer menu with
[SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
and persistent settings.

## Description

To change an NPC's voice type, simply select them with the console, enter the
SKSE menu with your hotkey (defaults to F1), click the NPC selection button,
select the desired voice type, and click the save button. The NPC's new voice
type will be saved in a settings file with the path 
`SKSE/Plugins/NPCVoiceChanger.json` (found in Overwrite folder for MO2 users)
and will be loaded every time you load into a game. To remove an NPC from the
settings, simply click the "X" button next to their name in the Modified NPCs
list.

The menu will allow you to choose from every voice type in the game, including
ones added by other mods. However, the same caveats that apply to Proteus'
voice changer also apply to this mod:

* Assigning a different voice type to custom voiced followers may interfere with
some scripts/dialogues
* Assigning a creature's voice to a humanoid NPC will cause dialogue options
to not appear
* Assigning a voice type may result in silent dialogue because the new voice
type does not have recorded voice lines for the NPC's dialogue and/or some
dialogues may become unavailable. This isn't really a problem if you are
assigning "generic" voice types to NPCs that already have a "generic" voice
type (MaleEvenToned, FemaleYoungEager, MaleElfHaughty, just to name a few.
Full list [here](https://en.uesp.net/wiki/Category:Skyrim-VoiceTypes)).

NPCs will be saved via their editor IDs. If they do not have one for some reason,
the mod will attempt to use their form ID. This will not work for NPCs with
dynamic forms, however. The form ID is a dynamic form if it starts with "FF".
Examples of dynamic forms include guards and NPCs that spawn from random
encounters. You can still change the voice type of the NPC, they just won't be
saved to the settings file.

Pairs well with mods that add generic voice types like 
[Simply Open Source Voice Pack](https://www.nexusmods.com/skyrimspecialedition/mods/92904)
and
[Multi Language Wife](https://www.nexusmods.com/skyrimspecialedition/mods/73786).
[Instantly Skip Dialogue NG](https://www.nexusmods.com/skyrimspecialedition/mods/89163)
is also recommended otherwise you may get stuck in dialogue if you assign an
incompatible voice type. See also
[NPC Visual Editor - NVE](https://www.nexusmods.com/skyrimspecialedition/mods/176063).
As of version 1.2.1, NVE has a voice changer, so pick either that or this mod.

## Build Requirements
* [XMake](https://xmake.io) [3.0.0+]
* C++23 Compiler (MSVC, Clang-CL)
* [CommonLibSSE NG](https://github.com/alandtse/CommonLibVR)
* [CLibUtil](https://github.com/powerof3/CLibUtil)
* [SKSE Menu Framework](https://github.com/QTR-Modding/SKSE-Menu-Framework-3/blob/master/resources/SKSEMenuFramework.h)
* [nlohmann/json](https://github.com/nlohmann/json/blob/develop/include/nlohmann/json.hpp)

## Runtime Requirements
* [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
* [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
* [powerofthree's Tweaks](https://www.nexusmods.com/skyrimspecialedition/mods/51073)

## Credits
qudix's [CommonLibSSE-NG Plugin Template](https://github.com/libxse/commonlibsse-ng-template)  
zfroggyman's [Mysterious Stranger](https://www.nexusmods.com/skyrimspecialedition/mods/180400) source code  
RavenKZP's [In-Game Patcher](https://github.com/RavenKZP/In-Game_Patcher) source code
