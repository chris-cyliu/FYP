#include <iostream>
#include <boost/fiber/all.hpp>

using namespace std;

uint64_t pt;

int main() {

  pt = (uint64_t) sbrk(0);

  cout << pt << endl;

  return 0;
}
