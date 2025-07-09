#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include "hps.h"
#include <pthread.h>

#define LW_BRIDGE_BASE    0xFF200000
#define LW_BRIDGE_SPAN    0x00005000

/* Offsets conforme MpuMain: */
#define PIO_CMD_OFFSET    0x00  /* HPS → FPGA (pio_out) */
#define PIO_STAT_OFFSET   0x10  /* FPGA → HPS (pio_in) */

#define N_BYTES           25    /* 200 bits / 8 */
#define N_BITS            (N_BYTES * 8)

int open_dev(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open(/dev/mem)");
        exit(1);
    }
    return fd;
}

void *map_hps(int fd)
{
    void *base = mmap(NULL,
                      LW_BRIDGE_SPAN,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      fd,
                      LW_BRIDGE_BASE);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }
    return base;
}

void unmap_hps(void *base)
{
    if (munmap(base, LW_BRIDGE_SPAN) != 0) {
        perror("munmap");
        exit(1);
    }
}



void bayer_grbg_to_rgb(uint8_t *bayer, uint8_t *rgb, uint8_t *greyscale, int width, int height) {
    int x, y;

    #define PIX(x, y) bayer[(y) * width + (x)]
    #define CLAMP(v) ((v) < 0 ? 0 : ((v) > 255 ? 255 : (v)))

    for (y = 1; y < height - 1; y++) {
        for (x = 1; x < width - 1; x++) {
            int r = 0, g = 0, b = 0;

            int idx = y * width + x;
            int out_idx = idx * 3;

            if (y % 2 == 0) {
                if (x % 2 == 0) {
                    // G pixel (even row, even col)
                    g = PIX(x, y);
                    r = (PIX(x-1, y) + PIX(x+1, y)) / 2;
                    b = (PIX(x, y-1) + PIX(x, y+1)) / 2;
                } else {
                    // R pixel
                    r = PIX(x, y);
                    g = (PIX(x-1, y) + PIX(x+1, y) + PIX(x, y-1) + PIX(x, y+1)) / 4;
                    b = (PIX(x-1, y-1) + PIX(x+1, y-1) + PIX(x-1, y+1) + PIX(x+1, y+1)) / 4;
                }
            } else {
                if (x % 2 == 0) {
                    // B pixel
                    b = PIX(x, y);
                    g = (PIX(x-1, y) + PIX(x+1, y) + PIX(x, y-1) + PIX(x, y+1)) / 4;
                    r = (PIX(x-1, y-1) + PIX(x+1, y-1) + PIX(x-1, y+1) + PIX(x+1, y+1)) / 4;
                } else {
                    // G pixel (odd row, odd col)
                    g = PIX(x, y);
                    r = (PIX(x, y-1) + PIX(x, y+1)) / 2;
                    b = (PIX(x-1, y) + PIX(x+1, y)) / 2;
                }
            }
	    
            rgb[out_idx + 0] = CLAMP(b);
            rgb[out_idx + 1] = CLAMP(g);
            rgb[out_idx + 2] = CLAMP(r);
	    
	    greyscale[idx] = (0.2125 * CLAMP(r)) + (0.7154 * CLAMP(g)) + (0.0721 * CLAMP(b));
        }
    }
}

int main(void)
{
    uint8_t *image = malloc(512*512*sizeof(uint8_t));
    volatile int *pio_cmd;
    volatile int *pio_pixel_color;
    volatile int *pio_instruction;
    volatile int *pio_stat;
    void               *hps_base;
    int                 fd;
    int                 bit_pos, byte_idx, bit_idx;
    uint32_t            word, status;
    uint8_t             A[5][5], B[5][5], R[5][5];

    /* 1) Abre /dev/mem e mapeia lightweight bridge */
    fd       = open_dev();
    hps_base = map_hps(fd);

    /* 2) Ajusta ponteiros para comando e status */
    pio_cmd  = (volatile int *)((char*)hps_base + PIO_LED_BASE);
    pio_pixel_color = pio_cmd + 4;
    pio_instruction = pio_cmd;
    int count = 0;
    int temp = 0, t2 = 0;
    int beyer;

    printf("algo1\n");
    while(count < 65536){
	temp = (count<<4) + 15;
        *pio_instruction = temp;
        beyer = *pio_pixel_color;
	t2 = count<<2;
        image[t2] = beyer & 255;
        image[t2+1] = (beyer >> 8) & 255;
        image[t2+2] = (beyer >> 16) & 255;
        image[t2+3] = (beyer >> 24) & 255;
        printf("%x %d %d\n", beyer, count, *pio_instruction>>4); //debug
        count = count + 1;
    }
    //for (int i = 0; i<10; i++) {
    	*pio_instruction = 14;
    //}
    *pio_instruction = 0;

    printf("algo\n");
    uint8_t *rgb_img = malloc(512 * 512 * 3);
    uint8_t *gs = malloc(512 * 512);
    bayer_grbg_to_rgb(image, rgb_img, gs, 512, 512);
    stbi_write_png("outputRGB.png", 512, 512, 3, rgb_img, 512*3);
    stbi_write_png("output.png", 512, 512, 1, gs, 512);

    //stbi_write_png("teste.png", 512, 480, 3, image_rgb, 512*3);

    /* 8) Cleanup */
    unmap_hps(hps_base);
    close(fd);
    return 0;
}
