#include <iostream>

using namespace std;


int main() {
  register int i asm("esp");
  printf("0x00000000 <= Low Memory Address\n");
  printf("%p <= main() function\n", &main);
  printf("%#010x <= esp", i);

  return 0;
}
