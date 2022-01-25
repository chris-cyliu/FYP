#include <iostream>
#include <concepts>
#include <coroutine>
#include <exception>

using namespace std;

struct MyCoroutine {
  struct promise_type {
    int val;

    MyCoroutine get_return_object() {
      return this;
    }

    suspend_always initial_suspend() noexcept {
      return suspend_always{};
    }

    suspend_always final_suspend() noexcept {
      return suspend_always{};
    }

    suspend_always yield_value(int val) {
      this->val = val;
      return suspend_always{};
    }

    suspend_always return_value(int val) {
      this->val = val;
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

  void resume() {
    m_handle.resume();
  }

  int get_value() {
    return m_handle.promise().val;  
  }

  coroutine_handle<promise_type> m_handle;
};


MyCoroutine gen(int n) {
  for (int i = 0; i < n; i++) {
    co_yield i;
  }
  co_return 0;
}

int main() {

  int n = 5;

  auto coroutine = gen(n);
  for (int i = 0; i < n; i++) {
    coroutine.m_handle.resume();
    cout << "in the main: " << i << ", value from coroutine: " << coroutine.get_value() << endl;
  }
  coroutine.m_handle.resume();
  cout << "Finishing coroutine, exit code: " << coroutine.get_value() << endl;

  return 0;
}
