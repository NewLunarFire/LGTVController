# LG TV Serial Controller

This is a small program written in plain C for Linux to control my LG TV from the command line. It has been tested under Ubuntu 16.04 Xenial Xerus only.

## Compiling

Run `make` in the folder. No really, that's it.

## Executing

Run `./lgtv --device=<device> [OPTIONS]`. If you do not get any output from this command, it means no TV is connected to the selected serial port. If a compatible TV is connected, you should see a `X 01 NGSSx` reply, where SS is the Set ID (in Hexadecimal) of your television

### Options

| Short Option | Long Option  | Argument   | Description |
|--------------|--------------|------------|-------------|
|-d            |--device      |File Device |Selects the file device used to communicate with the TV           |
|-e            |--echo        |None        |Echoes the data sent to your TV in the terminal. This can help you quickly see the commands sent to your TV             |
|-h            |--help        |None      |Shows an help screen for the appliation             |
|-s            |--setid       |Set ID    |Sets the Set ID of the TV with which you want to communicate. If this is not set it defaults to 0, which is the all-call Set ID             |

### Commands


| Name           | Values                                             | Description                          |
|----------------|----------------------------------------------------|--------------------------------------|
|&lt;none&gt;    |&lt;none&gt;                                        |Send invalid command to TV (identify) |
|power           |&lt;none&gt;                                        |Get TV is powered on                  |
|power           |on, off                                             |Power TV (On/Off)                     |
|input  	       |&lt;none&gt;                                        |Get current input source              |
|input  	       |tv, video1, video2, component1, component2, rgb, dvi|Select input source                   |
|aspect          |&lt;none&gt;                                        |Get current aspect ratio              |
|aspect          |normal, wide, horizon, zoom                         |Change aspect ratio                   |
|screen-mute     |&lt;none&gt;                                        |Get screen mute (on/off)              |
|screen-mute     |on, off                                             |Set screen mute (on/off)              |
|volume-mute     |&lt;none&gt;                                        |Get volume mute (on/off)              |
|volume-mute     |on, off                                             |Set volume mute (on/off)              |
|mute            |&lt;none&gt;, on, off                               |Same as volume mute                   |
