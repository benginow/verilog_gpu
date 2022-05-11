#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Vvgademo.h"
#include "verilated.h"
using namespace std;

struct WorkData {
  uint16_t data[16];
};

class Object {
private:
  struct OVertex {
    double x, y, z;
  };
  struct OTriangle {
    int v[3];
  };
  string errorMessage;
  int numVertices;
  int numFaces;
  int numEdges;

  vector<OVertex> vertices;
  vector<OTriangle> triangles;
public:
  Object(string inputFile) {
    ifstream file(inputFile);
    string line;
    getline(file, line);
    if (line != "OFF") {
      errorMessage = "Invalid header format";
    }

    getline(file, line);
    stringstream ss(line);
    ss >> numVertices >> numFaces >> numEdges;
    vertices.resize(numVertices);

    for (int i = 0; i < numVertices; i++) {
      getline(file, line);
      ss = stringstream(line);
      ss >> vertices[i].x >> vertices[i].y >> vertices[i].z;
    }

    for (int i = 0; i < numFaces; i++) {
      getline(file, line);
      ss = stringstream(line);
      int K; ss >> K;
      vector<int> verts;
      for (int j = 0; j < K; j++) {
        int v; ss >> v;
        verts.push_back(v);
      }
      for (int j = 1; j < (int) verts.size() - 1; j++) {
        OTriangle tri;
        tri.v[0] = verts[0];
        tri.v[1] = verts[i];
        tri.v[2] = verts[i + 1];
        triangles.push_back(tri);
      }
    }
  }

  vector<WorkData> getTriangles() {
    vector<WorkData> data(triangles.size());
    for (int i = 0; i < (int) triangles.size(); i++) {
      data[i].data[0] = round(vertices[triangles[i].v[0]].x * (1 << 8));
      data[i].data[1] = round(vertices[triangles[i].v[0]].y * (1 << 8));
      data[i].data[2] = round(vertices[triangles[i].v[0]].z * (1 << 8));
      data[i].data[3] = 0;
      data[i].data[4] = round(vertices[triangles[i].v[1]].x * (1 << 8));
      data[i].data[5] = round(vertices[triangles[i].v[1]].y * (1 << 8));
      data[i].data[6] = round(vertices[triangles[i].v[1]].z * (1 << 8));
      data[i].data[7] = 0;
      data[i].data[8] = round(vertices[triangles[i].v[2]].x * (1 << 8));
      data[i].data[9] = round(vertices[triangles[i].v[2]].y * (1 << 8));
      data[i].data[10] = round(vertices[triangles[i].v[2]].z * (1 << 8));
      data[i].data[11] = 0;
      data[i].data[12] = (1 << 15);
      data[i].data[13] = (1 << 15);
      data[i].data[14] = (1 << 15);
      data[i].data[15] = 0;
    }
    return data;
  }
};

class Simulator {
private:
  Object object;
public:
  Simulator(string inputFile): object(inputFile) {
  }
};

/*
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
*/
