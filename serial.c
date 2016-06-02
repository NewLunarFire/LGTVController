#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

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
  cfsetspeed(&tty, B9600);

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
