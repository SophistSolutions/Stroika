# Stroika::[Foundation](../../)::[Math](../)::LinearAlgebra

This folder contains all the Stroika Library [Foundation](../../)::[Math](../)::LinearAlgebra source code.

## Status: WEAK - expect major revision or removal in Stroika 3.1

This code is fairly immature, and the section as a whole is due a decision rather than incremental
patching - see [issue #1168](https://github.com/SophistSolutions/Stroika/issues/1168). It will either
undergo major revision in Stroika 3.1, or be removed.

Do not build new code on it without reading that issue first. Two things to know:

- The central design question - a functional/immutable API versus a mutation-based one - is still open
  (see [TODO.md](TODO.md)). Today's mutation API carries a real trap: on a non-const Vector,
  `Vector<T>::operator[]` returns a writeback proxy with an *implicit* conversion, so passing `v[0]`
  through varargs (`printf` and friends) compiles, is undefined, and prints garbage.
- Removal would not be a simple delete. Nothing else in Stroika uses `Vector`/`Matrix` except
  [Optimization/](../Optimization/) - but **that optimization code is used by other projects**, so
  whatever is decided here has to keep it working, or port it.

## Modules

- [Matrix.h](Matrix.h)
- [Vector.h](Vector.h)
