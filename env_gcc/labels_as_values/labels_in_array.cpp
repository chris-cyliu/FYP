#include <bits/stdc++.h>

using namespace std;


int main() {

  //ptrdiff_t diffs[] = {(int*)&&a_label - (int*)&&a_label, (int*)&&b_label - (int*)&&a_label};

  void* labels[] = {&&a_label, &&b_label};

  int t = 1;
  //goto *(&&a_label + diffs[t]);

  for (int i = 0; i < 2; i++) {
    cout << reinterpret_cast<int*>(labels[i]) << endl;
  }

  auto a_l = reinterpret_cast<int*>(&&a_label);
  auto b_l = reinterpret_cast<int*>(&&b_label);

  int diff = b_l - a_l;
  cout << "diff: " << diff << endl;

  goto *(a_l + diff);

a_label:
  cout << "a\n";
b_label:
  cout << "b\n";

  return 0;
}
