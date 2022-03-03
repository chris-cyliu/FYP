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

struct Checkpoint {
  std::string filename;
  void* resume_point;
  struct Data { int x; int y; int z; } data;

  Checkpoint(std::string filename) : filename(filename) {}
  
  virtual void serialize() {
    std::ostringstream stream;
    stream.write(reinterpret_cast<char*>(&resume_point), sizeof resume_point);
    stream.write(reinterpret_cast<char*>(&data), sizeof data);
    char null = 0;
    stream.write(&null, 1);

    std::ofstream storage;
    storage.open(filename);
    storage << stream.str();
    storage.close();
  }

  virtual void deserialize() {
    std::ifstream storage(filename);
    if (storage.fail()) {
      throw std::runtime_error("File not exist exception");
    }

    storage.read(reinterpret_cast<char*>(&resume_point), sizeof resume_point);
    storage.read(reinterpret_cast<char*>(&data), sizeof data);
    storage.close();
  }

  bool exists() {
    return std::filesystem::exists(filename);
  }
};

// helper function to extract checkpoint
template<typename PromiseType, typename CheckpointType>
struct GetCheckpoint {
  CheckpointType* _checkpoint;
  bool await_ready() { return false; } // says yes call await_suspend
  bool await_suspend(std::coroutine_handle<PromiseType> h) {
    _checkpoint = &h.promise().checkpoint;
    return false;     // says no don't suspend coroutine after all
  }
  auto await_resume() { return _checkpoint; }
};

struct Task {
  struct promise_type {
    Checkpoint checkpoint;
    promise_type(std::string filename) : checkpoint(filename) {
      if (checkpoint.exists()) {
        checkpoint.deserialize();
      }
    }

    Task get_return_object() {
      return {
        ._handle = std::coroutine_handle<promise_type>::from_promise(*this)
      };
    }

    std::suspend_always initial_suspend() { return {}; } 
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { abort(); }

    // address to serialize
    std::suspend_always yield_value(void* resume_point) {
      checkpoint.resume_point = resume_point; 
      checkpoint.serialize();
      return {};
    }
  };

  std::coroutine_handle<promise_type> _handle;

  bool finished() {
    return _handle.done();
  }

  void resume() {
    _handle.resume();
  }

  ~Task() {
    // clean up the coroutine object
    _handle.destroy();
  }
};

Task AlphaAsync(std::string filename) {
  // because we can't access checkpoint within coroutine body
  // this is a hook to get return point address, then jump to it
  auto checkpoint = co_await GetCheckpoint<Task::promise_type, Checkpoint> {};
  if (checkpoint->resume_point != nullptr)
    goto *checkpoint->resume_point;

  std::cout << "PHASE 1\n";
  checkpoint->data.x = 1;
  checkpoint->data.y = 2;
  checkpoint->data.z = 3;
  checkpoint_suspend();

  std::cout << "PHASE 2\n";
  std::cout << "data: " << checkpoint->data.x << " " << checkpoint->data.y << " " << checkpoint->data.z << std::endl;
  for (int i = checkpoint->data.x; i <= checkpoint->data.y; i++)
    checkpoint->data.z += i;
  checkpoint_suspend();

  std::cout << "PHASE 3 (final!)\n";
  std::cout << "data: " << checkpoint->data.x << " " << checkpoint->data.y << " " << checkpoint->data.z << std::endl;
  checkpoint->data.x *= checkpoint->data.y;
  checkpoint_suspend();

  std::cout << "final suspend\n";
}


int main() {

  auto task = AlphaAsync("checkpoint.data");

  std::cout << "entering the loop\n";

  while(!task.finished()) {
    task.resume();
    getchar(); // can simulate crash through at this point
  }

  std::cout << "exited from coroutine\n";

  return 0;
}
