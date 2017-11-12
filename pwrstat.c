///////////////////////////
//
// gcc -o pwrstat pwrstat.c
// display C.H.I.P power and battery status
// AXP209-internal-temperature USB-voltage USB-current battery-voltage [+-]battery-current
//
// https://bbs.nextthing.co/t/pwrstat-c-talk-to-axp209-over-i2c-in-c/6710?u=yoursunny
//
// Modified by: Michael E. O'Connor <gmikeoc@gmail.com>
//
// Date: 11/10/17
//
///////////////////////////

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

int readReg(int fd, int reg)
{
  uint8_t regbuf[1];
	regbuf[0] = reg  & 0xFF;
	int res = write(fd, regbuf, sizeof(regbuf));
	if (res < 0) {
	  printf("cannot read register\n");
	  exit(1);
	}

  uint8_t valbuf[1];
  res = read(fd, valbuf, sizeof(valbuf));
  if (res < 0) {
	  printf("cannot read register\n");
	  exit(1);
	}

  return valbuf[0];
}

void writeReg(int fd, int reg, uint8_t val)
{
  uint8_t cmdbuf[2];
	cmdbuf[0] = reg  & 0xFF;
	cmdbuf[1] = val;
	int res = write(fd, cmdbuf, sizeof(cmdbuf));
	if (res < 0) {
	  printf("cannot write register\n");
	  exit(1);
	}
}

int readAdc(int fd, int regH, int regL, int lowBits)
{
  int h = readReg(fd, regH);
  int l = readReg(fd, regL);
  return (h << lowBits) + l;
}

//
// Main Routine
//

int main(int argc, char** argv) {
    int fd = open("/dev/i2c-0", O_RDWR);
    if (fd < 0) {
        perror("cannot open I2C bus");
        return 1;
    }

  int ret = ioctl(fd, I2C_SLAVE_FORCE, 0x34);
  if (ret < 0) {
      printf("cannot talk to AXP209\n");
      return 1;
      }

  int adcEnable1 = readReg(fd, 0x82);
  int adcEnable2 = readReg(fd, 0x83);
  bool needWait = false;
  if ((adcEnable1 & 0xcc) != 0xcc) {
      adcEnable1 |= 0xcc;
      writeReg(fd, 0x82, adcEnable1);
      needWait = true;
      }

  if ((adcEnable2 & 0x80) != 0x80) {
      adcEnable2 |= 0x80;
      writeReg(fd, 0x83, adcEnable2);
      needWait = true;
      }

  if (needWait) {
      sleep(1);
      }

  int powerStatus = readReg(fd, 0x00);

  printf("\n");

// Get internal temp (and convert to Fahrenheit)

  int temp = readAdc(fd, 0x5E, 0x5F, 4);
  printf("Internal Temp:\t\t %0.1f F\n", (0.1 * temp - 144.7) * 1.8 + 32);

// Check Status of USB Power

  bool hasVbus = (powerStatus & 0x20) != 0;
  if (hasVbus) {
      int vbusVolt = readAdc(fd, 0x5A, 0x5B, 4);
      printf("USB Charge Voltage:\t %0.1fmV \n", 1.7 * vbusVolt);

      int vbusCurrent = readAdc(fd, 0x5C, 0x5D, 4);
      printf("USB Charge Current:\t %0.1fmA \n", 0.375 * vbusCurrent);
      }
  else {
      printf("USB Power:\t\t No USB Power!\n");
      }

// Check to see if Battery Backup is attached

  int powerMode = readReg(fd, 0x01);
  bool hasBatt = (powerMode & 0x20) != 0;
  if (hasBatt) {
      int battVolt = readAdc(fd, 0x78, 0x79, 4);
      printf("Battery Voltage:\t %0.1fmV \n", 1.1 * battVolt);

      bool isDischarging = (powerStatus & 0x04) == 0;
      bool isCharging = (powerMode & 0x40) != 0;
      if (isDischarging) {
        int dischargeCurrent = readAdc(fd, 0x7C, 0x7D, 5);
        printf("Battery Discharging at:\t -%0.1fmA \n", 0.5 * dischargeCurrent);
        }
      else if (isCharging) {
        int chargeCurrent = readAdc(fd, 0x7A, 0x7B, 4);
        printf("Battery Charging at:\t +%0.1fmA \n", 0.5 * chargeCurrent);
        }
      else {
        printf("Battery:\t Fully Charged\n");
        }
    }
  else {
    printf("Battery:\t\t Not Present!");
    }

  printf("\n");
  return 0;
}
