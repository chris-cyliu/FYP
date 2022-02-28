#include <bits/stdc++.h>
#include <concepts>
#include <coroutine>
#include <exception>
#include <chrono>
#include <thread>

#define named_label(a, b) concat(a, b)

#define concat(a, b) a ## b

#define unique_label named_label(label, __LINE__)

#define checkpoint_with_label(label_name) \
  co_yield &&label_name; \
  label_name:

#define checkpoint_suspend() \
  checkpoint_with_label(unique_label)

#define checkpoint_file "checkpoint.data"

using namespace std;

struct CoroutineWithCheckpoint {
  struct Checkpoint {
    void* resume_point;
  };

  static void serialize(Checkpoint &checkpoint, string filename) {
    ostringstream stream;
    stream.write(reinterpret_cast<char*>(&checkpoint.resume_point), sizeof checkpoint.resume_point);
    char null = 0;
    stream.write(&null, 1);

    ofstream storage;
    storage.open(filename);
    storage << stream.str();
    storage.close();
  }

  static void deserialize(Checkpoint& checkpoint, string filename) {
    ifstream storage(filename);
    if (storage.fail()) {
      throw std::runtime_error("File not exist exception");
    }

    storage.read(reinterpret_cast<char*>(&checkpoint.resume_point), sizeof checkpoint.resume_point);
    storage.close();
  }

};

struct MyCoroutineObj : CoroutineWithCheckpoint {
  struct promise_type {
    
    string checkpoint_filename;
    Checkpoint checkpoint;

    promise_type(string& filename) : checkpoint_filename(filename) {
      if (filesystem::exists(checkpoint_filename)) {
        deserialize(checkpoint, checkpoint_filename);
      } 
    }
    
    MyCoroutineObj get_return_object() {
      return {
        ._handle = coroutine_handle<promise_type>::from_promise(*this)
      };
    }

    std::suspend_always initial_suspend() { return {}; } 
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { abort(); }

    // address to serialize
    std::suspend_always yield_value(void* resume_point) {
      checkpoint.resume_point = resume_point; 
      serialize(checkpoint, checkpoint_filename); 
      return {};
    }
  }; 

  coroutine_handle<promise_type> _handle;

  bool finished() {
    return _handle.done();
  }

  void resume() {
    _handle.resume();
  }
};

template<typename PromiseType>
struct GetResumePoint {
  void* return_point;
  bool await_ready() { return false; } // says yes call await_suspend
  bool await_suspend(std::coroutine_handle<PromiseType> h) {
    return_point = h.promise().checkpoint.resume_point;
    return false;     // says no don't suspend coroutine after all
  }
  void* await_resume() { return return_point; }
};

MyCoroutineObj AlphaAsync(std::string filename) {
  // because we can't access promise_type within coroutine body
  // this is a hook to get return point address, then jump to it
  auto resume_point = co_await GetResumePoint<MyCoroutineObj::promise_type> {};
  if (resume_point != nullptr)
    goto *resume_point;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "PHASE 1\n";
  checkpoint_suspend();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "PHASE 2\n";
  checkpoint_suspend();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "PHASE 3 (final!)\n";
  checkpoint_suspend();
}


int main() {

  auto coro = AlphaAsync("checkpoint.data");

  while(!coro.finished()) {
    coro.resume();
    getchar(); // can simulate crash throug at this point
  }
  cout << "exited from coroutine\n";

  return 0;
}
