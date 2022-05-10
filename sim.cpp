#include <fcntl.h>
#include <stdlib.h>
#include "Vvgademo.h"
#include "verilated.h"

#define LOG(...) fprintf(stderr, __VA_ARGS__)

const uint32_t MAX_SIMULATION_TIME = 1024;

bool needDump = false;
bool old_vsync = true;

int main(int argc, char *argv[]) {
    LOG(" [+] starting VGA simulation\n");
    uint64_t tickcount = 0;

    Vmain* gpu = new Vmain;

    uint8_t image[801*526*3];
    memset(image, 'A', sizeof(image));

    uint32_t idx = 0;

    unsigned int count_image = 0;

    for ()
    for ( ; count_image < 10; ) {
        if (tickcount > 10) {
        }
        gpu->clk = 0;
        gpu->eval();

        gpu->clk = 1;
        gpu->eval();
        needDump = (!old_vsync && vga->vsync_out);

        if (needDump) {
            char filename[64];
            snprintf(filename, 63, "frames/frame-%08d.bmp", count_image++);
            LOG(" [-> dumping frame %s at idx %d]\n", filename, idx);
            int fd = creat(filename, S_IRUSR | S_IWUSR);

            if (fd < 0) {
                perror("opening file for frame");
                break;
            }

            char header[] = "P6\n801 526\n255\n"; // [7]

            write(fd, header, sizeof(header));
            write(fd, image, sizeof(image));

            close(fd);

            idx = 0;
        }

        image[idx++] = ((vga->pixel & 1) * 0xff);        // [8]
        image[idx++] = ((vga->pixel & 2) >> 1) * 0xff;
        image[idx++] = ((vga->pixel & 4) >> 2) * 0xff;

        old_vsync = vga->vsync_out;

        tickcount++;
    }

    return EXIT_SUCCESS;
}
