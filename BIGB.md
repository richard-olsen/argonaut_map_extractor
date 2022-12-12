# BIGB/WAD Files
These are the LCM/LCS files that contain the level data for the game. I call them BIGB because the first four bytes are literally BIGB

There is still a lot unknown with this file, but it does contain important information such as the wav files (with a non-standard chunk), texture data, model data, collision data, scripts, etc. 

## Header
The size of the entire header is 400 bytes
`Fun Fact: I-Ninja doesn't even bother with the first 16 bytes. The first 16 bytes can literally all be 'f', and the game will run just fine.`

| Offset | Size | Default Value? | Description |
| ------ | ---- | -------------- | ----------- |
| 0x00 | 0x04 | 'BIGB' | Magic Value |
| 0x04 | 0x04 | 384 | Location to the start of the compressed data chunk (400 - 16) |
| 0x08 | 0x04 | 118 (on PC) | This is the WAD version of the BIGB file, as noted in 10-00.LCS.LOG |
| 0x0C | 0x04 | 1 | Unknown as to what this value is |
| 0x10 | 0x40 |  | Name of the BIGB file |
| 0x50 | 0x28 |  | Name of the Argonaut Games employee who compiled the level |
| 0x78 | 0x04 |  | Rounding this value up to align it with 0x0800 bytes gives the offset to find some audio files |
| 0x7C | 0x04 |  | Unknown, might be the total amount of encoded audio samples |
| 0x80 | 0x04 |  | Location to the Resource Initialization Table in the decompressed data |
| 0x84 | 0x04 |  | Resource Initialization Table size in bytes |
| 0x88 | 0x04 |  | Unknown |
| 0x8C | 0x04 |  | Unknown |

The resource initialization table is always at the end on the PC version of the game. This makes it easy to calculate the total size of the decompressed data. 
`Table Location + Table Size`

Even though the last two values are unknown, they both can be added together to figure out the size of the data while the data is compressed!

## Resource Initialization Table
According to I-Ninja executable disassembly, the game loads in it's resources through this table. I only know some of these values, and they can get quite erratic. The game reads through this table iteratively. It reads in 4 bytes, determines the resource type, then reads the next 4 - unknown amount of bytes for the location of these resources in the decompressed data.

I will leave two tables, one with all the values I've seen so far, and one with the ones I know for sure

Known:
| Value | Locations Count | Description |
| ----- | --------------- | ----------- |
| 0x80000001 | 2 | Texture resource |
| 0x80000023 | 3 | Model data without Texture Coordinates |
| 0x80000024 | 3 | Model data with Texture Coordinates |
| 0x8000010A | 3 | Collision Data |
| 0x80000113 | 1 | Audio Header |
| 0x80000115 | 2 | Locations point to a spot in the audio table... which points to an audio header |

All that have been seen:
* 0x80000001
* 0x80000004
* 0x80000006
* 0x80000007
* 0x80000008
* 0x8000000A
* 0x80000020
* 0x80000021
* 0x80000023
* 0x80000024
* 0x8000010A
* 0x8000010B
* 0x8000010C
* 0x8000010E
* 0x80000113
* 0x80000115
* 0x80000402
