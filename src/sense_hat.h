#ifndef SENSE_HAT_H
#define SENSE_HAT_H

extern "C" {

#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"
#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

#include <dirent.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>

#include <linux/fb.h>
#include <linux/input.h>

struct fb_t {
  uint16_t pixel[8][8];
};

struct fb_t *fb = nullptr;

static int is_event_device(const struct dirent *dir) {
  return strncmp(EVENT_DEV_NAME, dir->d_name, strlen(EVENT_DEV_NAME) - 1) == 0;
}
static int is_framebuffer_device(const struct dirent *dir) {
  return strncmp(FB_DEV_NAME, dir->d_name, strlen(FB_DEV_NAME) - 1) == 0;
}

static int open_evdev(const char *dev_name) {
  struct dirent **namelist;
  int i, ndev;
  int fd = -1;

  ndev = scandir(DEV_INPUT_EVENT, &namelist, is_event_device, versionsort);
  if (ndev <= 0)
    return ndev;

  for (i = 0; i < ndev; i++) {
    char fname[64];
    char name[256];

    snprintf(fname, sizeof(fname), "%s/%s", DEV_INPUT_EVENT,
             namelist[i]->d_name);
    fd = open(fname, O_RDONLY);
    if (fd < 0)
      continue;
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    if (strcmp(dev_name, name) == 0)
      break;
    close(fd);
  }

  for (i = 0; i < ndev; i++)
    free(namelist[i]);

  return fd;
}

static int open_fbdev(const char *dev_name) {
  struct dirent **namelist;
  int i, ndev;
  int fd = -1;
  struct fb_fix_screeninfo fix_info;

  ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
  if (ndev <= 0)
    return ndev;

  for (i = 0; i < ndev; i++) {
    char fname[64];
    char name[256];

    snprintf(fname, sizeof(fname), "%s/%s", DEV_FB, namelist[i]->d_name);
    fd = open(fname, O_RDWR);
    if (fd < 0)
      continue;
    ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
    if (strcmp(dev_name, fix_info.id) == 0)
      break;
    close(fd);
    fd = -1;
  }
  for (i = 0; i < ndev; i++)
    free(namelist[i]);

  return fd;
}

void render(unsigned short rgb565) {
  if (!fb)
    return;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
      fb->pixel[i][j] = rgb565;
    }
}

int fbfd = 0;

void init_sense_hat() {

  fbfd = open_fbdev("RPi-Sense FB");
  if (fbfd <= 0) {
    printf("Error: cannot open framebuffer device.\n");
    return;
  }

  fb = static_cast<fb_t *>(
      mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0));
  if (!fb) {
    printf("Failed to mmap.\n");
  }
}

void close_sense_hat() {
  munmap(fb, 128);
  close(fbfd);
}
};
#endif // SENSE_HAT_H
