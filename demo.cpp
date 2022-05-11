#include <iostream>
#include <string>
#include "sim.cpp"
using namespace std;

int main(int argc, char *argv[]) {
  cout << argc << "\n";
  if (argc != 2) {
    cout << "Usage: ./demo object.off\n";
    return 1;
  }
  string fileName = string(argv[1]);
  cout << "Rendering " << fileName << "\n";

  Simulator sim(fileName);

  return 0;
}
