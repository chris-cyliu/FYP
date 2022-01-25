/*
 * gcc-10 -std=c++20 -Wall -Werror -o test simple_example.cpp 
*/
#include <iostream>
#include <string>

using namespace std;

int main() {
  void* labels[3] = {&&s0, &&s1, &&s2};

  int n;
  cin >> n;

  goto *labels[n];

  s0:
    cout << "s0\n";
  s1:
    cout << "s1\n";
  s2:
    cout << "s2\n";

  return 0;
}
