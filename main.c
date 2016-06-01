#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct tv_command
{
  char op1;
  char op2;
  int data;
};

struct cli_command
{
  char* name;
  struct tv_command tvcmd;
};

struct cli_command cmds[] = {
  {"power-on", {'k', 'a', 1}},
  {"power-off", {'k', 'a', 0}},
  {"tv", {'k', 'b', 0}},
  {"video1", {'k', 'b', 1}},
  {"video2", {'k', 'b', 2}},
  {"component1", {'k', 'b', 3}},
  {"component2", {'k', 'b', 4}},
  {"rgb", {'k', 'b', 5}},
  {"dvi", {'k', 'b', 6}}
};

void send(int fd, char op1, char op2, unsigned int sid, unsigned int data)
{
  char command[9];
  sprintf(command, "%c%c %02X %02X\r", op1, op2, sid & 0xFF, data & 0xFF);
  write(fd, command, 9);
}

int set_interface_attribs(int fd, int speed, int data_bits, int parity, int stop_bits)
{
  struct termios tty;
  memset(&tty, 0, sizeof(tty));

  if(tcgetattr(fd, &tty) != 0)
  {
    printf("error %d from tcgetattr: %s\n", errno, strerror(errno));
    return -1;
  }

  //Set Data Bits
  tty.c_cflag &= ~CSIZE;
  switch(data_bits)
  {
    case 5:
      tty.c_cflag |= CS5;
      break;
    case 6:
      tty.c_cflag |= CS6;
      break;
    case 7:
      tty.c_cflag |= CS7;
      break;
    case 8:
    default:
    tty.c_cflag |= CS8;
      break;
  }

  //Set Parity
  switch(parity)
  {
    //No Parity
    case 'N':
    case 'n':
    default:
      tty.c_cflag &= ~(PARENB|PARODD|CMSPAR);
      break;
    //Even Parity
    case 'E':
    case 'e':
      tty.c_cflag |= PARENB;
      tty.c_cflag &= ~(PARODD|CMSPAR);
      break;
    //Odd Parity
    case 'O':
    case 'o':
      tty.c_cflag |= PARENB|PARODD;
      tty.c_cflag &= ~CMSPAR;
      break;
    //Mark Parity
    case 'M':
    case 'm':
      tty.c_cflag |= PARENB|PARODD|CMSPAR;
      break;
    //Space Parity
    case 'S':
    case 's':
      tty.c_cflag |= PARENB|CMSPAR;
      tty.c_cflag &= ~PARODD;
      break;
  }

  if(stop_bits == 2)
    //2 Stop Bits
    tty.c_cflag |= CSTOPB;
  else
    //1 Stop Bit
    tty.c_cflag &= ~CSTOPB;

  //Set Non-canocial Read
  cfmakeraw(&tty);

  //Set to speed
  cfsetspeed(&tty, speed);

  tty.c_lflag = 0; // no signaling chars, no echo, no canonical processing
  tty.c_oflag = 0; // no remapping, no delays
  tty.c_cc[VMIN] = 0; // read doesn't block
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

  /* Flush Port, then applies attributes */
  tcflush(fd, TCIFLUSH);

  if(tcsetattr (fd, TCSANOW, &tty ) != 0)
  {
    printf("error %d from tcsetattr: %s\n", errno, strerror(errno));
    return -1;
  }
}

int main(int argc, char *argv[])
{
  char r = 0;
  int option_index = 0, c;

  char* device = NULL;
  char op[] = {'k', 'x'};
  int sid = 0;
  int data = 0xFF;

  int help = 0, echo = 0;

  static struct option long_options[] = {
    {"command", required_argument, 0, 'c'},
    {"device", required_argument, 0, 'd'},
    {"echo", no_argument, 0, 'e'},
    {"help", no_argument, 0, 'h'},
    {"setid", required_argument, 0, 's'},
    {0, 0, 0, 0}
  };

   /* getopt_long stores the option index here. */
   do {
     c = getopt_long (argc, argv, "d:", long_options, &option_index);

     switch(c)
     {
      case -1:
        //Do nothing, will exit loop
        break;
      case 0:
        //Do nothing
        break;
      case 'c':
        for(int i = 0; i < sizeof(cmds); i++)
        {
            if(strcmp(cmds[i].name, optarg) == 0)
            {
              op[0] = cmds[i].tvcmd.op1;
              op[1] = cmds[i].tvcmd.op2;
              data = cmds[i].tvcmd.data;
              break;
            }
        }
        //Command
        if(strcmp("power-on", optarg) == 0)
        {
          //Power-On TV
          op[0] = 'k';
          op[1] = 'a';
          data = 1;
        }
        else if(strcmp("power-off", optarg) == 0)
        {
          //Power-Off TV
          op[0] = 'k';
          op[1] = 'a';
          data = 0;
        }
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

   if(help != 0)
   {
    printf("Usage: lgtv [OPTIONS]\n");
    printf("LG TV Serial Controller, by Tommy Savaria (c)2016\n");
    printf("\nMandatory options\n");
    printf(" -d, --device\tSpecify device to connect\n");
    printf("\nOptional arguments\n");
    printf(" -e --echo\t\tEcho command sent to TV\n");
    printf(" -h --help\t\tPrint help screen (this)\n");
    printf(" -s --setid [Set ID]\tSpecify Set ID of television\n");
    printf(" -c --command [Command]\tRun Command\n");
    printf("\nList of commands (after --command option)\n");
    printf(" power-on\tTurn On television\n");
    printf(" power-off\tTurn Off television\n");
    printf(" tv\t\tSet Input to TV (Cable)\n");
    printf(" video1\t\tSet Input to Video 1\n");
    printf(" video2\t\tSet Input to Video 2\n");
    printf(" component1\tSet Input to Component 1\n");
    printf(" component2\tSet Input to Component 2\n");
    printf(" rgb\t\tSet Input to RGB (VGA)\n");
    printf(" dvi\t\tSet Input to DVI\n");
    printf("\n");
    return 0;
   }
   else if(device == NULL)
   {

     printf("Error: No device specified\nSpecify a device with -d or --device");
     return 1;
   }

  int tty = open(device, O_RDWR);

  if(tty == -1)
  {
    printf("Error %d from open: %s\n", errno, strerror(errno));
    return -1;
  }

  set_interface_attribs(tty, B9600, 8, 'N', 1);
  if(echo != 0)
  {
    printf(ANSI_COLOR_GREEN);
    printf("%c%c %02X %02X\\r\n", op[0], op[1], sid & 0xFF, data & 0xFF);
    printf(ANSI_COLOR_RESET);
  }

  send(tty, op[0], op[1], sid, data);

  if(tty == -1)
  {
    printf("error %d from open: %s\n", errno, strerror(errno));
    return -1;
  }

  printf(ANSI_COLOR_BLUE);

  do {
    read(tty, &r, 1);
    printf("%c", r);
  } while(r != 'x');

  printf("\n");
  printf(ANSI_COLOR_RESET);

  close(tty);
  return 0;
}
