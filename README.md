# Argonaut Map Extractor
This is an extractor specifically tailored for I-Ninja (specifically the PC version), a game made by Argonaut Games released in 2003. This extractor may work with other titles released by Argonaut Games, but I have no clue if it'll work with other titles. This repository comes with:
* Volt Extractor
* Volt Packer
* LCM/LCS Extractor
* Some documentation of the file structures

It would be a dream come true if I can find a way to repack the LCM/LCS files and fit them with custom scripts, but I am no where near being able to repack these files, nor can I figure out the Argonaut Strat Language at this time.

## Setup
Currently, the tools only work with Windows. The easiest way to set up is to
* Clone the Repo
* Open the folder in Visual Studio
* Configure CMake to compile the project **in 32 bit mode** (LCM/LCS Extractor is a little picky)
* Build
## How to Use
Once built, drag the `.vol` and `.avl` files onto `voltextractor.exe` to extract all the levels from their packages. You can highlight all the `.vol` and `.avl` files and drag them onto the executable. 
The extractor will then create two folders for each and dump the contents in those folders.

`Example:
Dragging 09.vol and 09.avl will create the two folders "09/" and "09/audio/" in the same directory as both the vol and avl files. `

Then all of the contents dumped by the volt extractor can then be dragged onto the `argoMapExtractor.exe` and this program will dump the known assets inside of the LCM/LCS files

## Argo Map Extractor
This tool will attempt to extract all audio, textures, and models from `.LCM` and `.LCS`
The tool is fine tuned to work with I-Ninja, but can probably be modified to work with other Argonaut Games titles.

Some things to note:
* Only the audio files have names
* All wav files will be saved as uncompressed 16 bit signed PCM
* Some model files have more than one model inside (I'm assuming theres data somewhere that lets the game know which model is which, or where a specific models indices are)
* Some models look like they haven't extracted correctly (good examples are the static world geometry), I believe they have been and that it's likely the same as the previous issue
* All character models are separated into smaller models, probably due to the way Argonaut Games handled animations
* All static geometry resides in LCM, while mostly entities/scripts/audio/ etc. resides in LCS
## Volt Extractor
This tool reads `.vol` and `.avl` and will extract every file from them.
All the level data (LCM/LCS) will be extracted into a new folder named after the `.vol` file name, and all the audio data (PCM) will be extracted into a new folder named after the `.avl` file name, and into another folder labeled 'audios'.

As long as the files are not tampered with (renaming the files, hand editing them), they should extract fine and will feed in perfectly with the Argo Map Extractor
## Volt Packer
Included just because

Although it's useless for any of the map files, this does make it easier to edit the `cdgame.txt` file inside of `general.vol`

After extracting the contents inside of `general.vol`, go ahead and make the edits. Then you can highlight all of the files inside of the `general/` directory created by the extractor and drag them onto the `voltpacker.exe` and will create a new file called `outVolt.vol`. Renaming this new file to `general.vol`, this file should be read by the game without any problems, given that the game accepts the changes you've made.

*Be careful with this, as you can damage your install of the game and may need to reinstall the game. Keep a backup*

## Extras
* LCM/LCS Documentation (BIGB.md)
* Volt Documentation (Volt.md)


