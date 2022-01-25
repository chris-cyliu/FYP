int main {
  auto couroutine = StartCoroutine();

  // internally, goto to the "right" label
  coroutine.hander.resume();

  cout << "in main";

  // goto label different from previous resume
  coroutine.handler.resume();

  return 0;
}


