# Neschgui

Warning: this program will not be maintaned anymore.   
If you want to edit nes roms on linux, please use neschgui-java (check out on my github account)

A program to edit nes/gb character rom on linux  

To install just type:

```
$ make
$ sudo make install
```

## Current modes

```
2bpp - NES (complete)
2bpp - GB/GBC (work in progress)
4bpp - SMS/Game Gear (work in progress)
```

## How to use  

To open a file, type:   

```
$ neschgui rom_name.nes
```

or drag and drop a file inside   

To save a file just press CTRL+S  

## Shortcuts

```
CTRL+S - save
CTRL+R - rotate current sprite
CTRL+I - invert current sprite
CTRL+Z - undo
CTRL+C - copy sprite
CTRL+V - paste sprite
```

## Config file

To create a config file, just type:

```
$ cd ~
$ touch .neschguicfg
```

Here's an example file: 

```
palette:
	255 255 255
	200 200 200
	140 140 140
	0 0 0
```

Each number represents a channel value of the color   
If you want to change it to default colors, just type "default" on the first line of the config file
