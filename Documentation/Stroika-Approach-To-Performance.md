# Stroika Approach to System Performance

## Background

How to optimize an application

- First look for the bottlenecks, and then replace them with something more performant.

- Use some profiling tool to see where bottlenecks are. But profilers gives you stack traces along with time measures. To identify which function requires optimization, the optimizing engineer must **understand** the code / call stack, and know what is calling what and why, to idendify the right level of abstraction (call stack) at which to optimize. For example, does the slow function need to be sped up, or just called less often?

- Then you find the bottleneck, and replace it with more performant code.

This is probably all obvious, and non-controversial. What may not have been so obvious, is that the thing that makes it **practical** to optimize, is the **simplicity** and **understandability** of the code. 

It's well known that "Premature optimization is the root of all evil". But the reasons for this, and the implications are less widely understood. 

## Approach

- Write code first and foremost, simply and clearly. Use abstractions without (much) worry for cost.

- Analyze performance and optimize bottlenecks as needed.

- Leverage c++'s powerful abstraction mechanisms to very ***locally*** replace bits of code with different approaches (here is where the earlier abstraction pays dividends). And leverage Stroika's utility classes (e.g. [Foundation::Cache](../Library/Sources/Stroika/Foundation/Cache/ReadMe.md) or alternate container backends [Foundation::Containers](../Library/Sources/Stroika/Foundation/Containers/ReadMe.md#Alternate-Backends-Feature)), or localized use of lower level c++ standard library functions to optimize just the peices that need it).

## Experience

Though this is anecdotal, it may still help. I've been hired dozens of times to fix performance problems in code. Nearly always the effort breaks down as:

- 20-30% of the time, learning and understanding the code
- 1% identifying the bottlenecks
- 60-70% refactoring the code so that the bottlenecks take place in modular, replacable chunks
- 1% - actually rewriting those peices to be optimal

If you can make the 'learning and undestanding the code' bit faster, and the 'refactoring the code so that the bottlenecks take place in modular, replacable chunks' part pre-done, optimizing bottlenecks becomes cheap and easy.

## Why this is unconventional thinking

In C++, there is a heavy emphasis on the idea that all abstractions should be light weight, and (nearly or really) zero cost.

This is not necessarily a bad thing for a language, but for the reasons outlined above, it IS a bad thing (or the wrong emphasis) for application developers.

You want to **first and foremost** keep the code clean and simple. And then only for the 'modularly isolated' parts where performance is critical
drop down to possibly using lower level features and lighter weight abstractions.

This thinking of lowest-possible-cost abstractions permeats most other C++ libraries (e.g. boost). That makes them very good for that
narrow peice of code that you must optimize, but less of a good choice to write the other 99% of your application.
