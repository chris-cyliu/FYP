//#include <bits/stdc++.h>
#include <iostream>

using namespace std;


int main() {

  void* label = &&b_label;
  goto *label;

a_label:
  cout << "a\n";
b_label:
  cout << "b\n";;

  return 0;
}
