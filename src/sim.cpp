#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include "Vgpu.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
using namespace std;

struct WorkData {
  int16_t data[16];
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
        tri.v[1] = verts[j];
        tri.v[2] = verts[j + 1];
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
      data[i].data[4] = round(vertices[triangles[i].v[1]].x * (1 << 8));
      data[i].data[5] = round(vertices[triangles[i].v[1]].y * (1 << 8));
      data[i].data[6] = round(vertices[triangles[i].v[1]].z * (1 << 8));
      data[i].data[8] = round(vertices[triangles[i].v[2]].x * (1 << 8));
      data[i].data[9] = round(vertices[triangles[i].v[2]].y * (1 << 8));
      data[i].data[10] = round(vertices[triangles[i].v[2]].z * (1 << 8));
      data[i].data[3] = (data[i].data[5] - data[i].data[1]) * (data[i].data[10] - data[i].data[2]) - (data[i].data[6] - data[i].data[2]) * (data[i].data[9] - data[i].data[1]);
      data[i].data[7] = 0;
      data[i].data[11] = 0;
      data[i].data[12] = 128;
      data[i].data[13] = 128;
      data[i].data[14] = 128;
      data[i].data[15] = 0;
      // cout << triangles[i].v[1] << " " << triangles[i].v[2] << " " << triangles[i].v[3] << "\n";
      // cout << data[i].data[0] << " " << data[i].data[1] << " " << data[i].data[2] << "\n";
      // cout << data[i].data[4] << " " << data[i].data[5] << " " << data[i].data[6] << "\n";
      // cout << data[i].data[8] << " " << data[i].data[9] << " " << data[i].data[10] << "\n";
    }
    // cout << "got triangles\n";
    return data;
  }
};

struct BmpHeader {
  char bitmapSignatureBytes[2];
  uint32_t sizeOfBitmapFile;
  uint32_t reservedBytes;
  uint32_t pixelDataOffset;
} __attribute__((packed));

struct BmpInfoHeader {
  uint32_t sizeOfThisHeader;
  int32_t width;
  int32_t height;
  uint16_t numberOfColorPlanes;
  uint16_t colorDepth;
  uint32_t compressionMethod;
  uint32_t rawBitmapDataSize;
  int32_t horizontalResolution;
  int32_t verticalResolution;
  uint32_t colorTableEntries;
  uint32_t importantColors;
}__attribute__((packed));

struct Pixel {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  Pixel(uint8_t red, uint8_t green, uint8_t blue): red(red), green(green), blue(blue) {}
}__attribute__((packed));

void writeBitmapImage(vector<vector<Pixel>> &image, string fileName) {
  int32_t width = image.size(), height = width == 0 ? 0 : image[0].size();
  BmpHeader bmpHeader = {{'B', 'M'}, (uint32_t) (54 + width * height * 3), 0, 54};
  BmpInfoHeader bmpInfoHeader = {40, width, height, 1, 24, 0, 0, 3780, 3780, 0, 0};
  ofstream fout(fileName, ios::binary);
  fout.write((char*) &bmpHeader, 14);
  fout.write((char*) &bmpInfoHeader, 40);
  size_t numberOfPixels = width * height;
  for (int i = 0; i < numberOfPixels; i++) {
    // Pixel pix = image[i % width][i / width];
 // int r = pix.red, g = pix.green, b = pix.blue;
    // if (r != 0 || g != 0 || b != 0) {
      // cout << i % width << " " << i / width << " " << r << " " << g << " " << b << "\n";
    // }
    fout.write((char*) &image[i % width][i / width], 3);
  }
  fout.close();
}

// A cheap way to write an assembler with macros
#define LDGPMEM(srcreg, tgtreg, pred) ((pred << 30) | (0 << 24) | (srcreg << 20) | (tgtreg << 16))
#define STGPMEM(srcreg, tgtreg, pred) ((pred << 30) | (1 << 24) | (srcreg << 20) | (tgtreg << 16))
#define MUL(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (2 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define ADD(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (3 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define SUB(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (4 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define SRL(src1reg, src2, tgtreg, pred) ((pred << 30) | (5 << 24) | (src1reg << 20) | (tgtreg << 16) | (src2))
#define SLL(src1reg, src2, tgtreg, pred) ((pred << 30) | (6 << 24) | (src1reg << 20) | (tgtreg << 16) | (src2))
#define AND(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (7 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define NOT(srcreg, tgtreg, pred) ((pred << 30) | (8 << 24) | (srcreg << 20) | (tgtreg << 16))
#define XOR(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (9 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define OR(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (10 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define NAND(src1reg, src2reg, tgtreg, pred) ((pred << 30) | (11 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtreg << 12))
#define LI(tgtreg, src, pred) ((pred << 30) | (12 << 24) | (tgtreg << 20) | (src))
#define SETLT(src1reg, src2reg, tgtpredreg, pred) ((pred << 30) | (13 << 24) | (src1reg << 20) | (src2reg << 16) | (tgtpredreg << 12))
#define STOREQ(srcreg, pred) ((pred << 30) | (14 << 24) | (srcreg << 20))
#define STOREQI(src, pred) ((pred << 30) | (15 << 24) | (src))
#define END(pred) ((pred << 30) | (16 << 24))

class Program {
private:
  vector<uint32_t> ins;
public:
  Program(vector<uint32_t> ins): ins(ins) {}
};

Program transformation({XOR(8, 8, 8, 0), OR(0, 0, 8, 0), LI(12, 0xffff, 0), AND(8, 12, 8, 0)});

/*
Program rasterization(

                      // get the current job status
                      LI(15, 0, 0),
                      OR(7, 7, 15, 0),
                      LI(14, 0xffff, 0)
                      AND(15, 14, 15, 0),

                      // compare first two triangles
                      LI(8, 0, 0),
                      OR(0, 0, 8, 0),
                      SRL(8, 16, 8, 0),
                      LI(9, 0, 0),
                      OR(2, 2, 9, 0),
                      SRL(9, 16, 9, 0),
                      SETLT(9, 8, 1, 0),
                      XOR(0, 2, 0, 1),
                      XOR(0, 2, 2, 1),
                      XOR(0, 2, 0, 1),
                      XOR(1, 3, 1, 1),
                      XOR(1, 3, 3, 1),
                      XOR(1, 3, 1, 1),

                      // compare last two triangles
                      LI(8, 0, 0),
                      OR(2, 2, 8, 0),
                      SRL(8, 16, 8, 0),
                      LI(9, 0, 0),
                      OR(4, 4, 0),
                      SRL(9, 16, 9, 0),
                      SETLT(9, 8, 1, 0),
                      XOR(2, 4, 2, 1),
                      XOR(2, 4, 4, 1),
                      XOR(2, 4, 2, 1),
                      XOR(3, 5, 3, 1),
                      XOR(3, 5, 5, 1),
                      XOR(3, 5, 3, 1),

                      // compare first two triangles
                      LI(8, 0, 0),
                      OR(0, 0, 8, 0),
                      SRL(8, 16, 8, 0),
                      LI(9, 0, 0),
                      OR(2, 2, 9, 0),
                      SRL(9, 16, 9, 0),
                      SETLT(9, 8, 1, 0),
                      XOR(0, 2, 0, 1),
                      XOR(0, 2, 2, 1),
                      XOR(0, 2, 0, 1),
                      XOR(1, 3, 1, 1),
                      XOR(1, 3, 3, 1),
                      XOR(1, 3, 1, 1),

                      // check if need to split
                      LI(8, 0, 0),
                      OR(0, 0, 8, 0),
                      SRL(8, 16, 8, 0),
                      LI(9, 0, 0),
                      OR(2, 2, 9, 0),
                      SRL(9, 16, 9, 0),

                      LI(10, 0, 0),
                      SETLT(8, 9, 1, 0),
                      LI(10, 1, 1),

                      LI(8, 0, 0),
                      OR(4, 4, 8, 0),
                      SRL(8, 16, 8, 0),

                      LI(11, 0, 0),
                      SETLT(9, 8, 1, 0),
                      LI(11, 1, 1),

                      AND(10, 11, 10, 0), // r10 is true if need to split

                      LI(11, 0, 0),
                      SETLT(11, 10, 1, 0), // p1 is true if we need to split

                      // if we don't need to split do something


                      // look at the long segment

                      // calculate slope
                      LI(8, 0, 1)
                      OR(0, 0, 8, 1)
                      SRL(8, 16, 8, 1)



                      );
*/

class Simulator {
private:
  Object object;
  string outputFile;

  struct State {
    int x, y, z, p1, p2, dx, dy, dz, xs, ys, zs, x1, y1, z1, x2, y2, z2;

    State(int x1, int y1, int z1, int x2, int y2, int z2): x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2) {
      x = x1;
      y = y1;
      z = z1;
      xs = x2 > x1 ? 1 : -1;
      ys = y2 > y1 ? 1 : -1;
      zs = z2 > z1 ? 1 : -1;
      dx = abs(x1 - x2);
      dy = abs(y1 - y2);
      dz = abs(z1 - z2);

      if (dx >= dy && dx >= dz) {
        p1 = 2 * dy - dx;
        p2 = 2 * dz - dx;
      }
      else if (dy >= dx && dy >= dz) {
        p1 = 2 * dx - dy;
        p2 = 2 * dz - dy;
      }
      else {
        p1 = 2 * dx - dz;
        p2 = 2 * dy - dz;
      }
    }

    bool done() {
      // cout << dx << " " << dy << " " << dz << " d\n";
      if (dx >= dy && dx >= dz)
        return x == x2;
      else if (dy >= dx && dy >= dz)
        return y == y2;
      else
        return z == z2;
    }

    void update() {
      if (done()) return;
      if (dx >= dy && dx >= dz) {
        x += xs;
        if (p1 >= 0)
          y += ys, p1 -= 2 * dx;
        if (p2 >= 0)
          z += zs, p2 -= 2 * dx;
        p1 += 2 * dy;
        p2 += 2 * dz;
      }
      else if (dy >= dx && dy >= dz) {
        y += ys;
        if (p1 >= 0)
          x += xs, p1 -= 2 * dy;
        if (p2 >= 0)
          z += zs, p2 -= 2 * dy;
        p1 += 2 * dx;
        p2 += 2 * dz;
      }
      else {
        z += zs;
        if (p1 >= 0)
          x += xs, p1 -= 2 * dz;
        if (p2 >= 0)
          y += ys, p2 -= 2 * dz;
        p1 += 2 * dx;
        p2 += 2 * dy;
      }
    }

    void shifty() {
      int cy = y;
      while (!done()) {
        update();
        if (y != cy) break;
      }
    }

    void shiftx() {
      int cx = x;
      while (!done()) {
        update();
        if (x != cx) break;
      }
    }
  };

  vector<WorkData> rasterization(WorkData inp) {
    vector<WorkData> queue;
    WorkData res;
    res.data[12] = inp.data[12];
    res.data[13] = inp.data[13];
    res.data[14] = inp.data[14];
    res.data[15] = inp.data[15];
    int x1 = inp.data[0], y1 = inp.data[1], z1 = inp.data[2], a1 = inp.data[3];
    int x2 = inp.data[4], y2 = inp.data[5], z2 = inp.data[6], a2 = inp.data[7];
    int x3 = inp.data[8], y3 = inp.data[9], z3 = inp.data[10], a3 = inp.data[11];

    if (y2 < y1) swap(x1, x2), swap(y1, y2), swap(z1, z2), swap(a1, a2);
    if (y3 < y2) swap(x2, x3), swap(y2, y3), swap(z2, z3), swap(a2, a3);
    if (y2 < y1) swap(x1, x2), swap(y1, y2), swap(z1, z2), swap(a1, a2);

    cout << x1 << " " << y1 << " " << z1 << " " << x2 << " " << y2 << "  " << z2 << "\n" ;

    State s1(x1, y1, z1, x2, y2, z2), s2(x1, y1, z1, x3, y3, z3);

    while (true) {
      State s3(s1.x, s1.y, s1.z, s2.x, s2.y, s2.z);

      // cout << s1.x << " " << s1.y << " " << s1.z << "\n";

      while (true) {
        WorkData next = res;
        next.data[0] = s3.x, next.data[1] = s3.y, next.data[2] = s3.z;
        queue.push_back(next);
        if (s3.done()) break;
        s3.shiftx();
      }

      // cout << s1.done() << " " << s2.done() << "\n";
      if (s1.done() || s2.done()) break;
      s1.shifty(); s2.shifty();
    }

    if (!s2.done()) swap(s1, s2);
    if (s1.done()) return queue;

    s2 = State(x2, y2, z2, x3, y3, z3);
    s1.shifty(); s2.shifty();
    while (true) {
      State s3(s1.x, s1.y, s1.z, s2.x, s2.y, s2.z);

      while (true) {
        WorkData next = res;
        next.data[0] = s3.x, next.data[1] = s3.y, next.data[2] = s3.z;
        queue.push_back(next);
        if (s3.done()) break;
        s3.shiftx();
      }

      if (s1.done() || s2.done()) break;
      s1.shifty(); s2.shifty();
    }
    return queue;
  }

  WorkData transformation(WorkData inp) {
    inp.data[0] += 1024;
    inp.data[1] += 512;
    inp.data[4] += 1024;
    inp.data[5] += 512;
    inp.data[8] += 1024;
    inp.data[9] += 512;
    return inp;
  }

  WorkData lighting(WorkData inp) {
    return inp;
  }

  WorkData projection(WorkData inp) {
    int screenWidth = 2, screenHeight = 2, screenDepth = 2;
    inp.data[0] = (((inp.data[0] / 4) + 1) / 2) * screenWidth;
    inp.data[1] = (((inp.data[1] / 4) + 1) / 2) * screenHeight;
    inp.data[2] = (((inp.data[2] / 4) + 1) / 2) * screenDepth;
    inp.data[4] = (((inp.data[4] / 4) + 1) / 2) * screenWidth;
    inp.data[5] = (((inp.data[5] / 4) + 1) / 2) * screenHeight;
    inp.data[6] = (((inp.data[6] / 4) + 1) / 2) * screenDepth;
    inp.data[8] = (((inp.data[8] / 4) + 1) / 2) * screenWidth;
    inp.data[9] = (((inp.data[9] / 4) + 1) / 2) * screenHeight;
    inp.data[10] = (((inp.data[10] / 4) + 1) / 2) * screenDepth;
    return inp;
  }

  queue<WorkData> getZBufferQueue() {
    vector<WorkData> q1 = object.getTriangles();
    queue<WorkData> q2, q3, q4, q5;
    while (true) {
      if (!q4.empty()) {
        vector<WorkData> next = rasterization(q4.front()); q4.pop();
        for (auto n : next) q5.push(n);
      }
      else if (!q3.empty()) {
        WorkData next = projection(q3.front()); q3.pop();
        q4.push(next);
      }
      else if (!q2.empty()) {
        WorkData next = lighting(q2.front()); q2.pop();
        q3.push(next);
      }
      else if (!q1.empty()) {
        auto next = q1.back();
        q2.push(transformation(q1.back())); q1.pop_back();
      }
      else break;
    }
    return q5;
  }

public:
  Simulator(string inputFile, string outputFile): object(inputFile), outputFile(outputFile) {}

  void softwareRun() {
    vector<vector<int>> zbuffer(640, vector<int>(480, 256));
    vector<vector<Pixel>> image(640, vector<Pixel>(480, Pixel(0, 0, 0)));
    queue<WorkData> q5 = getZBufferQueue();
    while (true) {
      if (!q5.empty()) {
        WorkData t = q5.front(); q5.pop();
        int x = t.data[0], y = t.data[1], z = t.data[2];
        if (x >= 0 && y >= 0 && x < 640 && y < 480) {
          if (z >= 0 && z < zbuffer[x][y]) {
            zbuffer[x][y] = z;
            image[x][y] = Pixel(t.data[12], t.data[13], t.data[14]);
          }
        }
      }
      else break;
    }
    writeBitmapImage(image, outputFile);
  }

  void run() {
    vector<vector<Pixel>> image(640, vector<Pixel>(480, Pixel(0, 0, 0)));

    Verilated::traceEverOn(true);
    Vgpu* gpu = new Vgpu;
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    gpu->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    gpu->clk = 0;
    gpu->eval();

    queue<WorkData> zbuffer = getZBufferQueue();
    int size = zbuffer.size();
    gpu->gpu__DOT__zbuffer_queue__DOT__front = 0;
    gpu->gpu__DOT__zbuffer_queue__DOT__back = size;
    gpu->gpu__DOT__zbuffer_queue__DOT__size_ = size;
    for (int i = 0; i < size; i++) {
      auto item = zbuffer.front(); zbuffer.pop();
      for (int j = 0; j < 8; j++) {
        gpu->gpu__DOT__zbuffer_queue__DOT__data[i][8 - j - 1] = (item.data[2 * j] << 16) + item.data[2 * j + 1];
      }
      if (i == 0) {
        for (int j = 0; j < 16; j++) {
          cout << item.data[j] << " ";
        }
        cout << "\n";
        for (int j = 0; j < 8; j++) {
          cout << gpu->gpu__DOT__zbuffer_queue__DOT__data[i][j] << " ";
        }
        cout << "\n";
      }
    }

    uint32_t idx = 0;

    while ((uint32_t) gpu->terminated != 1 && idx < 10000000) {
      gpu->clk = 0;
      gpu->eval();

      m_trace->dump((idx + 1) * 10 - 2);

      gpu->clk = 1;
      gpu->eval();

      m_trace->dump((idx + 1) * 10);
      gpu->gpu__DOT__zbuffer_queue__DOT__back = size;


      Pixel &pixel = image[gpu->counterX][gpu->counterY];
      pixel.red = ((gpu->pixel & 0xff0000) >> 16);
      pixel.green = ((gpu->pixel & 0xff00) >> 8);
      pixel.blue = (gpu->pixel & 0xff);

      idx++;
    }

    m_trace->close();
    delete gpu;

    writeBitmapImage(image, outputFile);
  }
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Usage: ./demo object.off output.bmp\n";
    return 1;
  }
  string inputFile = string(argv[1]);
  string outputFile = string(argv[2]);

  Simulator sim(inputFile, outputFile);
  sim.run();

  return 0;
}
