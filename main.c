#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

char* tv_commands[] = {
  "power",
  "input",
  "aspect",
  "screen-mute",
  "mute",
  "volume-mute",
  "volume"
};

char tv_command_letters[][2] = {
  {'k', 'a'},
  {'k', 'b'},
  {'k', 'c'},
  {'k', 'd'},
  {'k', 'e'},
  {'k', 'e'},
  {'k', 'f'}
};

char* tv_inputs[] = {
  "tv",
  "video1",
  "video2",
  "component1",
  "component2",
  "rgb",
  "dvi"
};

char* tv_aspects[] = {
  "normal",
  "wide",
  "horizon",
  "zoom"
};

int getNumber(char* string, int min, int max)
{
  char* endptr;
  int number = strtol(string, &endptr, 10);

  if(string == endptr)
      //Conversion impossible
      return 0xFF;

  if(number > max)
    return max;
  else if(number < min)
    return min;

  return number;
}

int findStringIndex(char* string, char** collection, int size)
{
  int r = -1;

  for(int i = 0; (i < size) && (r == -1); i++)
  {
    if(strcmp(string, collection[i]) == 0)
      r = i;
  }

  return r;
}

int main(int argc, char *argv[])
{
  char r = 0;
  int option_index = 0, c;

  char* device = NULL;
  char op[] = {'k', 'x'};
  int sid = 0;
  int data = 0xFF;
  char command[9];

  int help = 0, echo = 0;

  static struct option long_options[] = {
    {"device", required_argument, 0, 'd'},
    {"echo", no_argument, 0, 'e'},
    {"help", no_argument, 0, 'h'},
    {"setid", required_argument, 0, 's'},
    {0, 0, 0, 0}
  };

  /* getopt_long stores the option index here. */
  do {
    c = getopt_long (argc, argv, "d:ehs:", long_options, &option_index);

    switch(c)
    {
      case -1:
        //Do nothing, will exit loop
        break;
      case 0:
        //Do nothing
        break;
      case 'd':
        //Specified Device
        device = optarg;
        break;
      case 'e':
        //Echo command sent to TV
        echo = 1;
        break;
      case 'h':
        help = 1;
        break;
      case 's':
        sid = atoi(optarg);
        break;
      case '?':
        /* getopt_long already printed an error message. */
        break;
      default:
        return 1;
    }
  } while(c != -1);

  if(optind < argc)
  {
    int cmd = findStringIndex(argv[optind], tv_commands, sizeof(tv_commands));

    if(cmd != -1)
    {
      op[0] = tv_command_letters[cmd][0];
      op[1] = tv_command_letters[cmd][1];

      if(++optind < argc)
      {
        //2nd Arguments (Command + Value)
        switch(cmd)
        {
          case LGTV_POWER_COMMAND:
          case LGTV_SCREENMUTE_COMMAND:
            //On/Off, Off = 0, On = 1
            if(strcmp(argv[optind], "on") == 0)
              data = 0x01;
            else if(strcmp(argv[optind], "off") == 0)
              data = 0x00;
            break;
          case LGTV_MUTE_COMMAND:
          case LGTV_VOLUMEMUTE_COMMAND:
            //On/Off, Off = 1, On = 0
            if(strcmp(argv[optind], "on") == 0)
              data = 0x00;
            else if(strcmp(argv[optind], "off") == 0)
              data = 0x01;
            break;
          case LGTV_INPUT_COMMAND:
            //Values for Input command
            data = findStringIndex(argv[optind], tv_inputs, sizeof(tv_inputs));
            break;
          case LGTV_ASPECT_COMMAND:
            //Values for Aspect command
            data = findStringIndex(argv[optind], tv_aspects, sizeof(tv_aspects));
            break;
          case LGTV_VOLUME_COMMAND:
            data = getNumber(argv[optind], 0, 100);
            break;
        }
      }
    }
  }

  if(help != 0)
  {
    //Print Help message
    printf("Usage: lgtv [OPTIONS] [COMMAND]\n");
    printf("LG TV Serial Controller, by Tommy Savaria (c)2016\n");
    printf("\nMandatory arguments\n");
    printf(" -d, --device\t\tSpecify device to connect\n");
    printf("\nOptional arguments\n");
    printf(" -e --echo\t\tEcho command sent to TV\n");
    printf(" -h --help\t\tPrint help screen (this)\n");
    printf(" -s --setid [Set ID]\tSpecify Set ID of television\n");
    printf("\nCommands\n");
    printf(" power\t\t\tGet power state (on/off)\n");
    printf(" power on/off\t\tTurn on/off television\n");
    printf(" input\t\t\tGet current TV input\n");
    printf(" input <source>\t\tSet TV input to (tv|video1|video2|component1|component2|rgb|dvi)\n");
    printf(" aspect\t\t\tGet Aspect Ratio of TV\n");
    printf(" aspect <type>\t\tSet Aspect Ratio to (normal|wide|horizon|zoom)\n");
    printf(" screen-mute \t\tGet screen mute state (on/off)\n");
    printf(" screen-mute on/off\tSet screen mute on/off\n");
    printf(" volume-mute\t\tGet volume mute state (on/off)\n");
    printf(" volume-mute on/off\tSet volume mute on/off\n");
    printf(" mute\t\t\tSame as volume mute\n");
    printf(" volume\t\t\tGet TV volume\n");
    printf(" volume 0-100\t\tSet TV volume to value\n");
    printf("\n");
    return 0;
  }
  else if(device == NULL)
  {
    printf("Error: No device specified\nSpecify a device with -d or --device\n");
    return 1;
  }

  int tty = open(device, O_RDWR);

  if(tty == -1)
  {
    printf("Error %d from open: %s\n", errno, strerror(errno));
    return -1;
  }

  set_interface_attribs(tty, 9600, 8, 'N', 1);

  sprintf(command, "%.2s %02X %02X\r", op, sid & 0xFF, data & 0xFF);
  write(tty, command, 9);

  if(echo != 0)
  {
    //Print Send Command
    printf(ANSI_COLOR_GREEN);
    printf("%.9s\n", command);
    printf(ANSI_COLOR_RESET);
  }

  if(tty == -1)
  {
    printf("error %d from open: %s\n", errno, strerror(errno));
    return -1;
  }

  printf(ANSI_COLOR_BLUE);

  do {
    read(tty, &r, 1);
    if(r != -1)
      printf("%c", r);
  } while(r != 'x');

  printf("\n");
  printf(ANSI_COLOR_RESET);

  close(tty);
  return 0;
}
