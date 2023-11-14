#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#define DEVICE_FB_PATHNAME  "/dev/fb0"

int main() {
    int fb_fd;
    struct fb_var_screeninfo vinfo;
    size_t fbmem_size;
    void *p_fbmem = NULL;

    if (-1 == (fb_fd = open(DEVICE_FB_PATHNAME, O_RDWR))) {
        perror("open");
        return -1;
    }

    if (-1 == ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("ioctl");
        return -1;
    }

    printf("screen width   : %u px\n", vinfo.xres);
    printf("screen height  : %u px\n", vinfo.yres);
    printf("bits per pixel : %u bits\n", vinfo.bits_per_pixel);
    fbmem_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    p_fbmem = mmap(NULL, fbmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd
, 0);
    if (p_fbmem == (void *)-1) {
        perror("mmap");
        return -1;
    }

    // 将屏幕设置为红色
    uint16_t *p_pixel = p_fbmem;
    size_t num_pixels = vinfo.xres * vinfo.yres;

    for (size_t i = 0; i < num_pixels; i++) {
        p_pixel[i] = 0xF800; // 红色值：0xF800
    }

    munmap(p_fbmem, fbmem_size);
    close(fb_fd);

    return 0;
}

