# C++20 Coroutine Tutorial

## Background

#### Introduction
Coroutines is composed of two words: **"co"** (cooperative) and **"routines** (functions). Coroutines are functions that are cooperative, which allow execution to be suspended and resumed.

Several use cases of coroutines are web servers which handle multiple simultaneous connections and game programming to time-slice computations.

#### Non-Cooperative Functions
From your operating systems course, you may have learnt that when a function calls a second function, a "frame" is added to the runtime stack and will be removed after the second function has finished executing.

![](https://i.imgur.com/s6o9LPi.png)

Source: <a href="https://mymusing.co/stack-process-control-block/">Stack in Process Control Block</a>

In other words, the control remains with the second and the first function cannot continue unless the second function finishes and returns to where it was called.

![](https://i.imgur.com/liTaxwA.png)

Source: <a href="https://www.educative.io/edpresso/what-is-a-coroutine">Educative.io - What is a Coroutine?</a>


#### Cooperative Functions
Unlike the previous example, coroutines allow the control to be transferred from one function to the other function in a way that the *exit* point from the first function and the *entry* point to the second function are remembered.

Hence, each function remembers where it left the execution and where it should be resumed from. This way, we can pass around the values between functions in the middle of the execution.

![](https://i.imgur.com/G5X6rKw.png)

Source: <a href="https://www.educative.io/edpresso/what-is-a-coroutine">Educative.io - What is a Coroutine?</a>

Subsequently, coroutines are much cheaper than threads and enable us to execute **concurrent** code without much effort.

As mentioned in the introduction, game programming uses coroutine features. For example, we need to maintain a consistent frame rate, e.g., 60fps and several ms to execute code in each frame in the game. This includes physics simulation, input processing and drawing/rendering. Coroutines make it possible to time slice the computation so that it runs a little bit in each frame -- it allows the method to `yield` the computation back to the "caller" (in this case the game loop) so that the next time the method is called it resumes from where it left off.

Here is the table comparison between <b>subroutines</b> and <b>coroutines</b>:
<table>
  <tr>
    <th/>
    <th>Subroutines</th>
    <th>Coroutines</th>
  </tr>
  <tr>
    <td><b>Call</b></td>
    <td>Calling Convention i.e., foo()</td>
    <td>Calling Convention i.e., foo()</td>
  </tr>
  <tr>
    <td><b>Suspend</b></td>
    <td>Can't suspend</td>
    <td>co_await / co_yield expression</td>
  </tr>
  <tr>
    <td><b>Resume</b></td>
    <td>Can't resume</td>
    <td>coroutine_handle<>::resume()</td>
  </tr>
  <tr>
    <td><b>Return</b></td>
    <td>return statement</td>
    <td>co_return statement</td>
  </tr>
</table> 

## Properties
- Coroutines consist of:
	1. Promise i.e. `promise_type`:
		- Defines overall coroutine behaviour.
		- Contains special methods like `get_return_object()`, `initial_suspend()`, `final_suspend()`, etc. that compiler use in coroutine transformation.
		- Act as a communicator between the caller & the called coroutine.
	2. Awaiter
		- Controls suspension & resumption behaviour
	3. Coroutine handle
		- Controls execution behaviour
		- Manipulated outside of the coroutine

- Coroutines cannot use variadict arguments (e.g., `int sum(int... numbers)`), plain `return` statement, or placeholder return types (e.g., `auto`)


## How C++20 Coroutine Works Internally

To understand how C\+\+20 Coroutine works internally, you need to know some details on how context switch operates in the OS. Refer to these articles for detailed informations on how C++20 Coroutines work internally:
- [C++20 Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- [Coroutine in C Language](http://www.vishalchovatiya.com/coroutine-in-c-language)
- [C++20 Coroutines: Under the Hood](https://blog.feabhas.com/2021/09/c20-coroutines)

## Setup

We have prepared a docker image to run the sample code. If you don't have docker engine installed on your own machine, please refer to [https://docs.docker.com/get-docker/](https://docs.docker.com/get-docker/)

Launch a docker container with our image `askhatzhalgasov/gcc_image`

```bash=
docker run -it --rm askhatzhalgasov/gcc_image
```

Compile the code 
```bash=
g++-10 -fcoroutines -std=c++20 -Wall -Werror -o <filename> <filename.cpp>
``` 

Execute the binary file
```bash=
./filename
```

## Practice
It is effective to learn any new subject by practice. 
Therefore, further you will find some code examples which display how C++ implement coroutine feature.
1. **Resume/suspend** 
```cpp=
#include <iostream>
#include <concepts>
#include <coroutine>
#include <exception>

using namespace std;

struct MyCoroutine {
  struct promise_type {
    MyCoroutine get_return_object() {
      return MyCoroutine(this);
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
```
#### Output: 
```=
start coroutine
in the coroutine: 0
in the main: 0
in the coroutine: 1
in the main: 1
in the coroutine: 2
in the main: 2
in the coroutine: 3
in the main: 3
in the coroutine: 4
in the main: 4
ending coroutine
```
### Note
The provided code above shows the basic usage of coroutines in the program. 

Coroutine ```print(int n)``` takes `n` as the parameter to print every number from `0 ... n`, meanwhile, it suspends execution after each iteration.

There are multiple ways to write programs with coroutines, however, this one should be easier to understand for newcomers. 

#### Let's examine the code more closely:
* The logic within the `main` body is self-explanory. Program spins up the coroutine `print` and iterativelly prints out the values from either `main` function or `print` coroutine. All the interesting things happen in the`MyCoroutine` class.
```cpp=8
struct MyCoroutine {
  ...  
};
```
* **Return Object**: you can refer the defined `MyCoroutine` class as the abstraction which encapsulates all implementation details and provide the interface for user.

```cpp=9
struct promise_type {
  ...
}
```

* **Promise**: Under the hood of `MyCoroutine` class there is a `promise_type` structure which is hard requirement from the compiler. Internally, it controls the coroutine behavior, i.e, suspension condition, `yielding` mechanic and etc. 


    1. `initial_suspend()` - as the name suggests controls the suspension when a coroutine just started. Returns the **awaiter** object `suspend_never` used by function `co_await`. More on that will be discussed further, as of now, you can assume coroutine is never suspended at the beginning. 
    2. `final_suspend()` - similarly to `initial_suspend()` controls the suspension at the end of a coroutine's life. However, with `suspend_always` coroutine will be suspended before finishing. 
    3. `unhandled_exception()` - is a function which need to be implemented. For our simple example, it is empty. Still, without it code cannot compile.
    4. Finally, `get_return_object` - is the function, value of which will be returned to the caller.
    
        Citing [C++ reference page](https://en.cppreference.com/w/cpp/language/coroutines) when coroutine starts execution, it: 

        > calls promise.get_return_object() and keeps the result in a local variable. The result of that call will be returned to the caller when the coroutine first suspends. 
        
         In our case, `get_return_object()` will return `MyCoroutine` object instantiated by `promise_type` pointer constructor. 
         
         Notice that there is a constructor accepting `promise_type` pointer. It will then instantiate `corotuine_handle` *m_handle* variable using library `corotuine_handle<promise_type>::from_promise` primitive. 
         
         `m_handle` - is a corotuine handle which enables a caller to resume execution. It can be decoupled from `MyCoroutine` class, however, subjectively it will only complicate the code.
    
    After learning basics (i.e., `initial_suspend()`, `get_return_object`) about promise type, to better grasp what happens, you can imagine that compiler will augment `print` coroutine code as following:
    ```cpp=
    MyCoroutine print(int n) {
      //compiler will add smth similar
      auto _promise = new MyCoroutine::promise_type;
      auto return_object = _promise->get_return_object();
      co_await _promise->initial_suspend();

      ...
          
      co_await _promise->final_suspend();
      delete _promise
    }
    ```
    This is not an actual code during execution, but more or less describes the behaviour. Yet, it can help to build an intuition about coroutines.

* *MyCoroutine* **constructor**
  ```c++=27
  MyCoroutine(promise_type* p) 
    : m_handle(coroutine_handle<promise_type>::from_promise(*p)) 
  {}
  ``` 
  instantiates `MyCoroutine` object meanwhile creating the handler `m_handle` which later used to resume (and getting value from) coroutine.
* Coroutine **resumption**
  ```c++=37
  void resume() {
    m_handle.resume();
  }
  ```
  Just wrapper which triggers resumption from handler.
  
* **co_await** - unary operator which accepts the **awaitable** or **awaiter** object. Basically, suspends the coroutine execution. On the code level:
  ```cpp=
  co_await suspend_always{};
  ```
  You can conceptually see the above code line behaves as:
  ```cpp=
  {
    auto&& awaiter = suspend_always{};
    // if condition is mainly for performance
    if (awaiter.await_ready() == false) {
      awaiter.await_suspend(handle);
    }
    awaiter.await_resume();
  }
  ```

  *Note*: There might be an ambiguity of the difference between **awaitable** and **awaiter**. **Awaitable** is the object which supports *co_await* operator. Sample signature of **awaitable** class:
  ```cpp=
  struct/class SomeObject {
    ...
  public:
    some_awaiter operator co_await() {
        return some_awaiter{};
    } 
    ...
  };
  ```
  Meanwhile, **awaiter** should implement certain interface:
  ```cpp=
  struct some_awaiter {
    bool await_ready();
    void await_suspend(coroutine_handle<>);
    void await_resume();
  };
  ```
  Library, already provides some sample awaiter implementatinos such as `suspend_always` or `suspend_never`.

* Coroutine **destructor**
  ```cpp=31
  ~MyCoroutine() {
    m_handle.destroy();
  }
  ```
  In our example, the life of *coroutine* is bounded by the lifetime of `main` function, in other words, *coroutine* finishes once everything in `main` is executed. It is a good practice to release resources when there is no need in them. 
  
  This is done in `MyCoroutine` destructor with handle's `destroy()` call. 
  
  You can view the `coroutine_handle` as the pointer that  points to the *coroutine*'s resources and cannot **own** them. If the pointer stop to exists, it does not imply the pointed data also released.
 
  *Coroutine* can live longer than its handlers, because memory for it is allocated on the heap. It also enables, to access coroutines across different threads/processes.
  
  Therefore, to destroy *coroutine* from the handle you need to call `destroy()` method. 
  
2. **Pass values** with from coroutine:
  ```cpp=
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
    coroutine.resume();
    cout << "in the main: " << i << ", value from coroutine: " << coroutine.get_value() << endl;
  }
  coroutine.resume();
  cout << "Finishing coroutine, exit code: " << coroutine.get_value() << endl;

  return 0;
}
  ```
  This example is almost identical to the one in a previous section, apart from newly added keywords such `co_return` and `co_yield`. 
  
  The `gen` coroutine generates values from `0 .. n-1` and yielding them one by one. At the end *coroutine* returns `exit_code`. 
  
  * `co_yield value` expression works as it would be something like:
    ```cpp=61
    _promise->return_value(value);
    ```
    which behaves like `initial_suspend()`, `final_suspend()` functions by returning **awaiter** object (you already observed how **awaiter** behaves in previous section).

    Similarly, `co_return` internally calls `return_value()` function in `promise_type` structure.

    As you may already notice, values are not passed directly to the caller, but rather **stored** and **modified** within the coroutine's `promise_type` structure. 

    Such indirection allows to read the variable within the coroutine multiple times and enables several functions to modify it. In fact `return_value()` and `yield_value()` already do that. 
	
  * Returning the value from handler
  ```cpp=51
  int get_value() {
    return m_handle.promise().val;  
  }
  ```
  The wrapper which refers the handler to return variable from `promise_type` structure. 

### References:
- https://medium.com/nerd-for-tech/painless-c-coroutines-part-1-ba90dd101adb
- https://medium.com/codex/painless-coroutines-part2-29b36008b507
- https://www.educative.io/edpresso/what-is-a-coroutine
- http://www.vishalchovatiya.com/cpp20-coroutine-under-the-hood
- http://www.vishalchovatiya.com/coroutine-in-c-language/
- https://en.cppreference.com/w/cpp/language/coroutines
