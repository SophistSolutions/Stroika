# Stroika::Foundation::[Containers](../ReadMe.md)::Factory

- Modules
  - TBD - many - list
  - [Adder.h](Adder.h)


- Design Notes

In Stroika 2.1 and earlier, the factories could be replaced (new defaults) set anytime. But this made
calls to construct new containers more expensive due to the needed locking.

As of Stroika v3, we require calls to container factory 'Register' methods to happen before the first
container is constructed, so that this is no longer an issue, and container construction is
typically more performant.
