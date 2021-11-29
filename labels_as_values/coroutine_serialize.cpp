#include <bits/stdc++.h>
#include <coroutine>

using namespace std;

#define create_label(name, line) \
  name##line

#define unique_label \
  create_label(label, __LINE__)

#define checkpoint_with_label(expr, resume_point, label_name) \
  co_await expr; \
  resume_point = &&label_name; \
  label_name: \
  cout << "checkpointed\n";

#define checkpoint(expr, chk) \
  checkpoint_with_label(expr, chk.resume_point, unique_label) \
  serialize(chk, checkpoint_file);

#define checkpoint_file "checkpoint.data"

struct Checkpoint {
  void* resume_point;
  string payload;
} global_check;

void serialize(Checkpoint checkpoint, string filename) {
  ostringstream stream;
  stream.write(reinterpret_cast<char*>(checkpoint.resume_point), sizeof checkpoint.resume_point);
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

  storage.read(reinterpret_cast<char*>(checkpoint.resume_point), sizeof checkpoint.resume_point);
  storage.close();

  checkpoint.resume_point_present = true;
  cout << "deserialized!\n";
}

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
  if (global_check.resume_point != nullptr)
    goto *global_check.resume_point;

  cout << "start coroutine\n";
  cout << "before checkpoint\n";
  checkpoint(suspend_always{}, global_check);
  cout << "after checkpoint\n";
}

int main() {

  deserialize(global_check, checkpoint_file); 

  int n = 5;
  {
    auto coroutine = print(n);

    coroutine.resume();

    cout << "finished\n";
  }

  return 0;
}

