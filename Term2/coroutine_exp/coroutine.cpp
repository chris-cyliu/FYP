/*
 * Compilation: g++-10 -fcoroutines -std=c++20 coroutine.cpp
 *
*/

#include <bits/stdc++.h>
#include <concepts>
#include <coroutine>
#include <exception>

using namespace std;

/* some macros magic to generate labels */

#define named_label(a, b) concat(a, b)

#define concat(a, b) a ## b

#define unique_label named_label(label, __LINE__)

#define checkpoint_with_label(expr, chk, label_name) \
  chk.resume_point = &&label_name; \
  serialize(chk, checkpoint_file); \
  co_await expr; \
  label_name: 

#define checkpoint(expr, chk) \
  checkpoint_with_label(expr, chk, unique_label) 

#define checkpoint_file "checkpoint.data"

struct Checkpoint {
  void* resume_point;
  string payload;
} global_check;

void serialize(Checkpoint checkpoint, string filename) {

  ostringstream stream;
  stream.write(reinterpret_cast<char*>(&checkpoint.resume_point), sizeof checkpoint.resume_point);
  char null = 0;
  stream.write(&null, 1);

  ofstream storage;
  storage.open(filename);
  storage << stream.str();
  storage.close();
}

void deserialize(Checkpoint& checkpoint, string filename) {
  ifstream storage(filename);
  if (storage.fail()) {
    cout << "no file with name: " << filename << endl;
    return;
  }

  storage.read(reinterpret_cast<char*>(&checkpoint.resume_point), sizeof checkpoint.resume_point);
  storage.close();

  cout << "deserialized!\n";
}

struct MyCoroutine {
  struct promise_type {
    ~promise_type() {
      cout << "destroying promise_type\n";
    }

    MyCoroutine get_return_object() {
      return MyCoroutine(this);
    }

    suspend_never initial_suspend() noexcept {
      return suspend_never{};
    }

    suspend_always final_suspend() noexcept {
      return {};
    }

    void unhandled_exception() {
      abort();
    }
  };

  MyCoroutine(promise_type* p) 
    : m_handle(coroutine_handle<promise_type>::from_promise(*p)) 
  {}

  ~MyCoroutine() {
    cout << "destroying coroutine\n";
    m_handle.destroy();
  }

  coroutine_handle<promise_type> m_handle;
void resume() {
    m_handle.resume();
  }
};

MyCoroutine print() {

  if (global_check.resume_point != nullptr)
    goto *global_check.resume_point;

  int var = 2;
  cout << "start coroutine\n";
  checkpoint(suspend_always{}, global_check);  

  int var1 = 3;
  cout << "in the middle1 of coroutine\n";
  checkpoint(suspend_always{}, global_check);

  int var2 = 4;
  cout << "in the middle2 of coroutine\n";
  checkpoint(suspend_always{}, global_check);

  int var3 = 5;
  cout << "ending coroutine\n";
}

int main() {

  deserialize(global_check, checkpoint_file);

  auto coroutine = print();

  while(coroutine.m_handle.done() == false) { // check whether coroutine is finished executing
    cout << "in the main: " << endl;
    getchar(); // crash point imitation
    coroutine.resume();
  }

  cout << "finished\n";

  return 0;
}
