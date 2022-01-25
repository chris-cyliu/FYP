#include <bits/stdc++.h>

using namespace std;

#define create_label(name, line) \
  name##line

#define unique_label \
  create_label(label, __LINE__)

#define checkpoint_with_label(text, resume_point, label_name) \
  cout << text << endl; \
  resume_point = &&label_name; \
  label_name: \
  cout << "checkpointed\n";

#define checkpoint(text, resume_point) \
  checkpoint_with_label(text, resume_point, unique_label)

int main() {

  void* g;

  int cnt = 0;

  cout << "a\n";
  checkpoint("b", g);
  cnt++;

  if (cnt < 3)
    goto *g;

  cout << "finished\n";

  cout << sizeof(g) << " " << *((uint64_t*) g) << endl;

  return 0;
}
