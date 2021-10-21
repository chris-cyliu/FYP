## Background

Coroutines is composed of two words: **"co"** (cooperative) and **"routines** (functions). Coroutines are functions that are cooperative, which allow execution to be suspended and resumed.

#### Non-Cooperative Functions
In Operating Systems course, we have learnt that when a function calls a second function, a "frame" is added to the runtime stack and will be removed after the second function has finished executing. In other word, the control remains with the second and the first function cannot continue unless the second function finishes and returns to where it was called.

-- Insert image of the normal stack frame --

#### Cooperative Functions
Unlike the previous example, coroutines allow the control to be transferred from one function to the other function in a way that the *exit* point from the first function and the *entry* point to the second function are remembered.

-- Insert image of coroutine functions "stack" frame --

Hence, each function remembers where it left the execution and where it should be resumed from. This way, we can pass around the values between functions in the middle of the execution.

Subsequently, coroutines are much cheaper than threads and enable us to execute **concurrent** code without much effort.

-- Add Comparison Table Between Subroutines vs Coroutines --

## Properties
- Coroutines consist of:
	1. Promise:
		- Defines overall coroutine behaviour
		- Act as a communicator between the caller & the called coroutine
	2. Awaiter
		- Controls suspension & resumption behaviour
	3. Coroutine handle
		- Controls execution behaviour

## Setup
- Docker
- C++20 Sample Implementation (with comments for each keyword, such as co_await, co_return, co_yield)

References:
- https://medium.com/nerd-for-tech/painless-c-coroutines-part-1-ba90dd101adb
- https://medium.com/codex/painless-coroutines-part2-29b36008b507
- https://www.educative.io/edpresso/what-is-a-coroutine
- http://www.vishalchovatiya.com/cpp20-coroutine-under-the-hood/