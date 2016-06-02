#ifndef MAIN_H
#define MAIN_H

#define ANSI_COLOR_RED		 "\x1b[31m"
#define ANSI_COLOR_GREEN	 "\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_RESET	 "\x1b[0m"

#define LGTV_POWER_COMMAND 0
#define LGTV_INPUT_COMMAND 1
#define LGTV_ASPECT_COMMAND 2
#define LGTV_SCREENMUTE_COMMAND 3
#define LGTV_MUTE_COMMAND 4
#define LGTV_VOLUMEMUTE_COMMAND 5
#define LGTV_VOLUME_COMMAND 6
#define LGTV_UNKNOWN_COMMAND -1

int set_interface_attribs(int fd, int speed, int data_bits, int parity, int stop_bits);

#endif //MAIN_H
