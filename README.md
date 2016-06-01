# LG TV Serial Controller

This is a small program written in plain C for Linux to control my LG TV from the command line. It has been tested under Ubuntu 16.04 Xenial Xerus only.

## Compiling

Run `make` in the folder. No really, that's it.

## Executing

Run `./lgtv --device=<device> [OPTIONS]`. If you do not get any output from this command, it means no TV is connected to the selected serial port. If a compatible TV is connected, you should see a `X 01 NGSSx` reply, where SS is the Set ID (in Hexadecimal) of your television

### Options

| Short Option | Long Option | Argument | Description |
|--------------|-------------|----------|-------------|
|-e            |--echo       |None      |Echoes the data sent to your TV in the terminal. This can help you quickly see the commands sent to your TV             | 
|-h            |--help       |None      |Shows an help screen for the appliation             |
|-s            |--setid      |Set ID    |Sets the Set ID of the TV with which you want to communicate. If this is not set it defaults to 0, which is the all-call Set ID             |
|-c            |--command      |Command    |See **__Commands__** Section Below            |

### Commands

| Name     | Description             |
|----------|-------------------------|
|power-on  |Turn On television       |
|power-off |Turn Off television      |
|tv		     |Set Input to TV (Cable)  |
|video1		 |Set Input to Video 1     |
|video2		 |Set Input to Video 2     |
|component1|Set Input to Component 1 |
|component2|Set Input to Component 2 |
|rgb		   |Set Input to RGB (VGA)   |
|dvi		   |Set Input to DVI         |

