# Volt File
The Volt File is a basic archive file containing more files. For I-Ninja, they are named as follows
* 09.vol / 09.avl --- Menus, Starting Level
* 10.vol / 10.avl --- Robot Beach
* 20.vol / 20.avl --- Bomb Bay
* 30.vol / 30.avl --- Jungle Falls
* 40.vol / 40.avl --- Mountain Gorge
* 50.vol / 50.avl --- Moon Base
* 70.vol / 70.avl --- Mini Games, Challenges, Battle Arena
* general.vol

The `.avl` contains all of the PCM files (which are IMA ADPCM encoded audio banks), and `.vol` contains level data and game files such as `cdgame.txt`

The file is comprised of 4 sections
* Header
* Directory Table
* File Table
* File Data

## Header
| Offset | Size | Default Value? | Description |
| 0x00 | 0x04 | 'VOLT' | A magic value |
| 0x04 | 0x04 | 2 | An unknown value, but it's always 2|
| 0x08 | 0x04 | | How many files exist inside this archive |
| 0x0C | 0x04 | | The size, in bytes, of the file table section |

## Directory Table
The Directory Table is an array of Directory Entries. This array holds the same amount of Directory Entries as the File Count in the Header.

### Directory Entry
| Offset | Size | Default Value? | Description |
| 0x00 | 0x04 | | A CRC encoding of the file name |
| 0x04 | 0x04 | 1 | An unknown value, but it's always 1 (referred to fileTypeID in the extractor/packer) |
| 0x08 | 0x04 | | An accumulation of the file entry size |

## File Table
The File Table is a little odd. The File Table is constructed of File Entries, though the File Entry size depends on the file name itself. This is where the size, in bytes, from the header comes into play as it helps the game with loading the file. The extractor doesn't use this value as the amount of File Entries is the same as the amount of files inside the volt archive

### File Entry
| Offset | Size | Default Value? | Description |
| 0x00 | 0x04 | | The file's data location in the archive |
| 0x04 | 0x04 | 0 | Unknown, possibly a 64 bit extension? |
| 0x08 | 0x04 | | The file's size |
| 0x0C | 0x04 | | Unknown, possibly a 64 bit extension? |
| 0x10 | 0xXX | | A NULL-terminated string for the file name |

## File Data
Contains all the data for each file
All files **MUST** be aligned by 0x800 bytes! The game **WILL** crash if the files are not aligned!
