# Stroika Approach to System Performance

(ROUGH INCOMPLETE DRAFT)

## Background

How to optimize an application

- Obviously, you first look for the bottlenecks, and then replace them with something more performant.

- Use some profiling tool to see where bottlenecks are. But profiler gives you stack traces along with time measures. To identify which function requires optimization, the optimizing engineer must **understand** the code / call stack, and know what is calling what and why, to idendify the right level of abstraction (call stack) at which to optimize.

- Then you find the bottleneck, and replace it with more performant code

This is probably all obvious, and non-controversial. What make not have been so obvious, is that the thing that makes it **practical** to optimize, is the **simplicity** and **understandability** of the code.

## Approach

- Write code first and foremost, simply and clearly. Use abstractions without (much) worry for cost.

- Analyze performance and optimize bottlenecks as needed.

- Leverage c++'s powerful abstraction mechanisms to very locally replace bits of code with different approaches (here is where the earlier abstraction pays dividends). And Leverage Stroika's utility classes (e.g. )