# ZeroMQ PoC

* This PoC requires two components:
  1. `libzmq`, the underlying ZeroMQ library: `apt install libzmq3-dev`
  1. [cppzmq](https://github.com/zeromq/cppzmq), a C++ binding for `libzmq`.

* Note that `cppzmq` is included in `apt install libzmq3-dev`, we don't need
to install it manually.

* There are multiple C++ bindings, `cppzmq` sets itself apart from others
as the official documentation's examples usually use it as the bindings
for C++.

* To build cppzmq, one may need to add `-DENABLE_DRAFTS=off` to `cmake`, to
make the build script consistent with `apt`'s version.