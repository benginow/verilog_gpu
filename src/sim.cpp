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
      return;
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
      auto v1 = vertices[triangles[i].v[0]], v2 = vertices[triangles[i].v[1]], v3 = vertices[triangles[i].v[2]];
      data[i].data[0] = round(v1.x * (1 << 8));
      data[i].data[1] = round(v1.y * (1 << 8));
      data[i].data[2] = round(v1.z * (1 << 8));
      data[i].data[4] = round(v2.x * (1 << 8));
      data[i].data[5] = round(v2.y * (1 << 8));
      data[i].data[6] = round(v2.z * (1 << 8));
      data[i].data[8] = round(v3.x * (1 << 8));
      data[i].data[9] = round(v3.y * (1 << 8));
      data[i].data[10] = round(v3.z * (1 << 8));
      OVertex a{v2.x - v1.x, v2.y - v1.y, v2.z - v1.z}, b{v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};
      OVertex n{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
      double norm = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
      if (norm != 0) {
        n.x /= norm;
        n.y /= norm;
        n.z /= norm;
      }
      data[i].data[3] = round(n.x * 256);
      data[i].data[7] = round(n.y * 256);
      data[i].data[11] = round(n.z * 256);
      // cout << data[i].data[3] << " " << data[i].data[7] << " " << data[i].data[11] << "\n";
      data[i].data[12] = 128;
      data[i].data[13] = 128;
      data[i].data[14] = 128;
      data[i].data[15] = 0;
    }
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
public:
  vector<uint32_t> ins;
  Program(vector<uint32_t> ins): ins(ins) {}
};

Program transformationProgram({
    LI(10, 0xffff, 0),
    LI(11, 1024, 0), LI(12, 512, 0),
    LI(8, 0, 0), ADD(8, 0, 8, 0), SRL(8, 16, 8, 0),
    ADD(8, 11, 8, 0),
    LI(9, 0, 0), ADD(9, 0, 9, 0), AND(9, 10, 9, 0),
    ADD(9, 12, 9, 0),
    SLL(8, 16, 8, 0), ADD(8, 9, 0, 0),

    LI(8, 0, 0), ADD(8, 2, 8, 0), SRL(8, 16, 8, 0),
    ADD(8, 11, 8, 0),
    LI(9, 0, 0), ADD(9, 2, 9, 0), AND(9, 10, 9, 0),
    ADD(9, 12, 9, 0),
    SLL(8, 16, 8, 0), ADD(8, 9, 2, 0),

    LI(8, 0, 0), ADD(8, 4, 8, 0), SRL(8, 16, 8, 0),
    ADD(8, 11, 8, 0),
    LI(9, 0, 0), ADD(9, 4, 9, 0), AND(9, 10, 9, 0),
    ADD(9, 12, 9, 0),
    SLL(8, 16, 8, 0), ADD(8, 9, 4, 0),

  LI(10, 0xFFFF, 0),
  LI(11, 7, 0),
  LI(12, 11, 0),

  // save dot in reg 13
  LI(13, 0, 0),
  // do math in reg 14

  // MATH FOR 3
  LI(14, 0, 0),
  ADD(14, 1, 14, 0),
  // SLL(14, 16, 14, 0),
  // SRL(14, 16, 14, 0),
  AND(14, 10, 14, 0),
  SRL(14, 4, 14, 0),
  MUL(14, 11, 14, 0),
  ADD(14, 13, 13, 0),

  // MATH FOR 7
  LI(14, 0, 0),
  ADD(14, 3, 14, 0),
  // //take top bits
  AND(14, 10, 14, 0),
  SRL(14, 4, 14, 0),
  // //multiply by 3
  MUL(14, 11, 14, 0),
  // //add to reg 13
  ADD(14, 13, 13, 0),

  // //MATH FOR 11
  LI(14, 0, 0),
  ADD(14, 5, 14, 0),
  // //take top bits
  // SLL(14, 16, 14, 0),
  // SRL(14, 16, 14, 0),
  AND(14, 10, 14, 0),
  SRL(14, 4, 14, 0),
  // //multiply by 3
  MUL(14, 12, 14, 0),
  // //add to reg 13
  SUB(13, 14, 13, 0),

  //if dot is less than 0, then we set to 0.
  LI(14, 0, 0),
  // NOT(13, 13, 1),

  // SRL(13, 8, 13, 0),
  SETLT(13, 14, 1, 0),
  // LI(13, 0, 1),
  // LI(7, 0, 0),
  // ADD(7, 13, 7, 0),

  LI(14, 0, 0),
  ADD(14, 6, 14, 0),
  SRL(14, 16, 14, 0),
  MUL(14, 13, 14, 0),
  SRL(14, 8, 14, 0),
  SLL(14, 16, 14, 0),

  LI(15, 0, 0),
  ADD(15, 6, 15, 0),
  AND(15, 10, 15, 0),
  // SLL(15, 16, 15, 0),
  // SRL(15, 16, 15, 0),
  MUL(15, 13, 15, 0),
  SRL(15, 8, 15, 0),

  ADD(14, 15, 6, 0),

  LI(14, 0, 0),
  ADD(14, 7, 14, 0),
  AND(14, 10, 14, 0),
  // SLL(15, 16, 15, 0),
  // SRL(15, 16, 15, 0),

  LI(15, 0, 0),
  ADD(15, 7, 15, 0),
  SRL(15, 16, 15, 0),
  MUL(15, 13, 15, 0),
  SRL(15, 8, 15, 0),
  SLL(15, 16, 15, 0),

  ADD(14, 15, 7, 0),

  STOREQI(1, 0), END(0)});
Program lightingProgram({
  // LI(10, 0xFFFF, 0),
  // LI(11, 7, 0),
  // LI(12, 11, 0),

  //save dot in reg 13
  // LI(13, 0, 0),
  //do math in reg 14

  //MATH FOR 3
  // LI(14, 0, 0),
  // ADD(14, 1, 14, 0),
  //shift down
  // SRL(14, 20, 14, 0),
  // MUL(14, 11, 14, 0),
  // ADD(14, 13, 13, 0),

  //MATH FOR 7
  // LI(14, 0, 0),
  // ADD(14, 3, 14, 0),
  // //take top bits
  // SRL(14, 20, 14, 0),
  // //multiply by 3
  // MUL(14, 11, 14, 0),
  // //add to reg 13
  // ADD(14, 13, 13, 0),

  // //MATH FOR 11
  // LI(14, 0, 0),
  // ADD(14, 5, 14, 0),
  // //take top bits
  // SRL(14, 20, 14, 0),
  // //multiply by 3
  // MUL(14, 12, 14, 0),
  // //add to reg 13
  // ADD(14, 13, 13, 0),

  // //if dot is less than 0, then we set to 0.
  // LI(14, 0, 0),
  // SETLT(13, 14, 1, 0),
  // LI(14, 0, 1),


  //inp.data[12] *= dot;
  //inp.data[12] >>= 8;
  //inp.data[13] *= dot;
  //inp.data[13] >>= 8;
  // LI(14, 0, 0),
  // ADD(14, 6, 14, 0),
  // AND(14, 10, 14, 0),
  // MUL(14, 13, 14, 0),
  // SRL(14, 8, 14, 0),
  
  // LI(15, 0, 0),
  // ADD(15, 6, 15, 0),
  // SRL(15, 16, 15, 0),
  // MUL(15, 13, 15, 0),
  // SRL(15, 8, 15, 0),
  // SLL(15, 16, 15, 0),

  // ADD(14, 15, 6, 0),
  // //14 and 15
  // LI(14, 0, 0),
  // ADD(14, 7, 14, 0),
  // AND(14, 10, 14, 0),
  // MUL(14, 13, 14, 0),
  // SRL(14, 8, 14, 0),
  
  // LI(15, 0, 0),
  // ADD(15, 7, 15, 0),
  // SRL(15, 16, 15, 0),
  // SLL(15, 16, 15, 0),

  // ADD(14, 15, 7, 0),

  //zero out top bits
  
  STOREQI(2, 0), END(0)});
Program projectionProgram({
    // LI(7, 0, 0),
    //set reg 10
    LI(10, 0xFFFF, 0),
    LI(11, 16, 0),
    LI(12, 2, 0),
    LI(13, 1, 0),

    //PROCESS 0 and 1
    //process upper bits
    LI(8, 0, 0), ADD(8, 0, 8, 0), SRL(8, 16, 8, 0),
    //logic for division and all idk
    SRL(8, 2, 8, 0), ADD(8, 13, 8, 0),
    //process lower bits
    LI(9, 0, 0), ADD(9, 0, 9, 0), AND(9, 10, 9, 0),
    //math logig.
    SRL(9, 2, 9, 0), ADD(9, 13, 9, 0),
    //put them together
    SLL(8, 16, 8, 0), ADD(8, 9, 0, 0),

    //PROCESS 3
    //save upper bits
    LI(8, 0, 0), ADD(8, 1, 8, 0), SRL(8, 16, 8, 0),
    //process lower bits
    LI(9, 1, 0), ADD(9, 1, 9, 0), AND(9, 10, 9, 0),
    //math logig.
    SRL(9, 2, 9, 0), ADD(9, 13, 9, 0),
    //put them together
    SLL(8, 16, 8, 0), ADD(8, 9, 1, 0),

    //PROCESS 4 and 5
    //process upper bits
    LI(8, 0, 0), ADD(8, 2, 8, 0), SRL(8, 16, 8, 0),
    //logic for division and all idk
    SRL(8, 2, 8, 0), ADD(8, 13, 8, 0),
    //process lower bits
    LI(9, 0, 0), ADD(9, 2, 9, 0), AND(9, 10, 9, 0),
    //math logig.
    SRL(9, 2, 9, 0), ADD(9, 13, 9, 0),
    //put them together
    SLL(8, 16, 8, 0), ADD(8, 9, 2, 0),

    //PROCESS 6
    //save upper bits
    LI(8, 0, 0), ADD(8, 3, 8, 0), SRL(8, 16, 8, 0),
    //process lower bits
    LI(9, 1, 0), ADD(9, 3, 9, 0), AND(9, 10, 9, 0),
    //math logig.
    SRL(9, 2, 9, 0), ADD(9, 13, 9, 0),
    //put them together
    SLL(8, 16, 8, 0), ADD(8, 9, 3, 0),

    //PROCESS 8 and 9
    //process upper bits
    LI(8, 0, 0), ADD(8, 4, 8, 0), SRL(8, 16, 8, 0),
    //logic for division and all idk
    SRL(8, 2, 8, 0), ADD(8, 13, 8, 0),
    //process lower bits
    LI(9, 0, 0), ADD(9, 4, 9, 0), AND(9, 10, 9, 0),
    //math logig.
    SRL(9, 2, 9, 0), ADD(9, 13, 9, 0),
    //put them together
    SLL(8, 16, 8, 0), ADD(8, 9, 4, 0),

    //PROCESS 10
    //save upper bits
    LI(8, 0, 0), ADD(8, 5, 8, 0), SRL(8, 16, 8, 0),
    //process lower bits
    LI(9, 1, 0), ADD(9, 5, 9, 0), AND(9, 10, 9, 0),
    //math logig.
    SRL(9, 2, 9, 0), ADD(9, 13, 9, 0),
    //put them together
    SLL(8, 16, 8, 0), ADD(8, 9, 5, 0),

    STOREQI(3, 0), END(0)
  });

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

    if (inp.data[12] > 130) return queue;
      // cout << inp.data[12] << " " << inp.data[13] << " " << inp.data[14] <<  " " << inp.data[15] << "\n";

    // cout << x1 << " " << y1 << " " << z1 << " " << x2 << " " << y2 << "  " << z2 << "\n" ;

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
    // double lx = sqrt(1.0 / 6), ly = sqrt(1.0 / 6), lz = sqrt(1.0 / 2);
    // int x = round(lx * 16), y = round(ly * 16), z = round(lz * 16);
    // cout << x << " " << y << " " << z << "\n";
    auto dot = (inp.data[3] / 16) * 7 + (inp.data[7] / 16) * 7 - (inp.data[11] / 16) * 11;
    if (dot < 0) dot = 0;
    inp.data[12] *= dot;
    inp.data[12] >>= 8;
    inp.data[13] *= dot;
    inp.data[13] >>= 8;
    inp.data[14] *= dot;
    inp.data[14] >>= 8;
    return inp;
  }

  WorkData projection(WorkData inp) {
    int screenWidth = 1, screenHeight = 1, screenDepth = 1;
    inp.data[0] = (((inp.data[0] / 4) + 1)) * screenWidth;
    inp.data[1] = (((inp.data[1] / 4) + 1)) * screenHeight;
    inp.data[2] = (((inp.data[2] / 4) + 1)) * screenDepth;
    inp.data[4] = (((inp.data[4] / 4) + 1)) * screenWidth;
    inp.data[5] = (((inp.data[5] / 4) + 1)) * screenHeight;
    inp.data[6] = (((inp.data[6] / 4) + 1)) * screenDepth;
    inp.data[8] = (((inp.data[8] / 4) + 1)) * screenWidth;
    inp.data[9] = (((inp.data[9] / 4) + 1)) * screenHeight;
    inp.data[10] = (((inp.data[10] / 4) + 1)) * screenDepth;
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

  void setPrograms(Vgpu *gpu) {
    int idx = 0;
    gpu->t_pc = idx;
    for (auto v : transformationProgram.ins)
      gpu->gpu__DOT__instr_mem__DOT__data[idx++] = v;
    gpu->l_pc = idx;
    for (auto v : lightingProgram.ins)
      gpu->gpu__DOT__instr_mem__DOT__data[idx++] = v;
    gpu->p_pc = idx;
    for (auto v : projectionProgram.ins)
      gpu->gpu__DOT__instr_mem__DOT__data[idx++] = v;
  }

  void transferTriangles(Vgpu *gpu) {
    vector<WorkData> tris = object.getTriangles();
    // vector<WorkData> tris;
    // for (auto t : first_tris) {
    //   tris.push_back(t);
    // }
    int size = tris.size();
    gpu->gpu__DOT__transformation__DOT__front = 0;
    gpu->gpu__DOT__transformation__DOT__back = size;
    gpu->gpu__DOT__transformation__DOT__size_ = size;
    for (int i = 0; i < size; i++) {
      auto item = tris[i];
      for (int j = 0; j < 8; j++)
        gpu->gpu__DOT__transformation__DOT__data[i][8 - j - 1] = (item.data[2 * j] << 16) + item.data[2 * j + 1];
    }
  }

  void checkRasterization(Vgpu *gpu) {
    bool should = gpu->gpu__DOT__proc1__DOT__request_new_pc_ && (gpu->gpu__DOT__rasterization__DOT__size_ > 0);
    if (should) {

      WorkData data;
      for (int j = 0; j < 8; j++) {
        data.data[j * 2] = (int16_t) (gpu->gpu__DOT__rasterization__DOT__data[gpu->gpu__DOT__rasterization__DOT__front][8 - j - 1] >> 16);
        data.data[j * 2 + 1] = (int16_t) (gpu->gpu__DOT__rasterization__DOT__data[gpu->gpu__DOT__rasterization__DOT__front][8 - j - 1] % (1 << 16));
      }
      gpu->gpu__DOT__rasterization__DOT__size_ -= 1;
      gpu->gpu__DOT__rasterization__DOT__front += 1;

      // for (int i = 0; i < 16; i++) cout << data.data[i] << " ";
      // cout << "\n";

      vector<WorkData> next = rasterization(data);
      for (WorkData unit : next) {
        for (int j = 0; j < 8; j++)
          gpu->gpu__DOT__zbuffer_queue__DOT__data[gpu->gpu__DOT__zbuffer_queue__DOT__back][8 - j - 1] = (unit.data[2 * j] << 16) + unit.data[2 * j + 1];
        gpu->gpu__DOT__zbuffer_queue__DOT__back += 1;
        gpu->gpu__DOT__zbuffer_queue__DOT__size_ += 1;
      }

    }
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
    Vgpu *gpu = new Vgpu;
    VerilatedVcdC *m_trace = new VerilatedVcdC;
    gpu->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    gpu->clk = 0;
    gpu->eval();

    setPrograms(gpu);
    transferTriangles(gpu);

    uint32_t idx = 0;

    while ((uint32_t) gpu->terminated != 1 && idx < 10000000) {
      gpu->clk = 0;
      gpu->eval();
      m_trace->dump((idx + 1) * 10 - 2);

      gpu->clk = 1;
      gpu->eval();
      m_trace->dump((idx + 1) * 10);

      checkRasterization(gpu);

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
