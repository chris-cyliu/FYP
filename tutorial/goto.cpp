#include <bits/stdc++.h>

using namespace std;

int main() {

  void* g = &&label;

  cout << sizeof(g) << " " << *((uint64_t*)g) << endl;
  goto *g;

  cout << "a\n";
label:
  cout << "b\n";

  return 0;
}
