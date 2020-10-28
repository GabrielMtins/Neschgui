# Neschgui

A program to edit nes/gb character rom on linux  

To install just type:

```
$ make
$ sudo make install
```

## How to use  

To open a file, type:   

```
$ neschgui rom_name.nes
```

To save a file just press S  

## Config file

To create a config file, just type:

```
$ cd ~
$ touch .neschguicfg
```

Here's a default config file: 

```
palette:
	255 255 255
	200 200 200
	140 140 140
	0 0 0
```

Each number represents a channel value of the color   
If you want to change it to default colors, just type "default" on the first line of the config file
