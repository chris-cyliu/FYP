#include <iostream>
#include <concepts> 
#include <coroutine>
#include <exception>

using namespace std;

struct MyCoroutine {
  struct promise_type {
    MyCoroutine get_return_object() {
      return this;
    }

    suspend_never initial_suspend() noexcept {
      return suspend_never{};
    }

    suspend_always final_suspend() noexcept {
      return suspend_always{};
    }

    void unhandled_exception() {
      abort();
    }
  };

  MyCoroutine(promise_type* p) 
    : m_handle(coroutine_handle<promise_type>::from_promise(*p)) 
  {}

  ~MyCoroutine() {
    m_handle.destroy();
  }

  coroutine_handle<promise_type> m_handle;

  void resume() {
    m_handle.resume();
  }
};


MyCoroutine print(int n) {
  cout << "start coroutine\n";
  for (int i = 0; i < n; i++) {
    cout << "in the coroutine: " << i << endl;
    co_await std::suspend_always{};
  }
  cout << "ending coroutine\n";
}

int main() {

  int n = 5;

  auto coroutine = print(n);
  for (int i = 0; i < n; i++) {
    cout << "in the main: " << i << endl;
    coroutine.resume();
  }
  return 0;
}
