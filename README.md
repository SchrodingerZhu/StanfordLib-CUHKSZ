# StanfordLib-CUHKSZ
[![CircleCI](https://circleci.com/gh/SchrodingerZhu/StanfordLib-CUHKSZ.svg?style=svg)](https://circleci.com/gh/SchrodingerZhu/StanfordLib-CUHKSZ)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/schrodingerzhu/stanfordlib-cuhksz?branch=master&svg=true)](https://ci.appveyor.com/project/SchrodingerZhu/stanfordlib-cuhksz/)

A cmake deliver version of stanford library, specially patched for CSC3002 students in CUHK(SZ)

# Notice
- a new sync library is available now. it provides a mutex/rwlock with rust favor and a array queue and a segment queue.
- All the sources are modified from [stanford-cpp-library](https://github.com/zelenski/stanford-cpp-library). The original author reserves the copyright.
- We hope you can use a more elegant naming and organizing method. Hence instead of the orignal style like `"gfont.h"`, we adapt a modularized way :`<graphics/gfont.h>`
- The orginal stanford library mixes files in all modules and produces cyclic dependency relations, therefore, we have to produce to whole shared library, instead of multiple small libraries for each module.
- The original version takes the advantage of `qmake` toolchain and make some renaming around `main()` function. Here, we want to seperate the library and user's code.
  Therefore, we do not use the library itself to do the renaming. Instead, users should write a entry function (say `start`) by themselves and include `macro.h` and call `__WRAP_MAIN__(start)` to invoke the library.
- The backend of `Map` and `HashMap` has been migrated to `absl::btree_map` and `absl::flat_hash_map` to improve the performance.
  Vast functions in `strlib.h` has been refactored into wrappers around`absl` to reduce the difficulty of maintenance.
  Char checking operation now uses `std::memchr` directly instead of self implementing.
  In most cases, you do not need to care about the changes: they do not require any manual interference on API.
- We also bind the library with [`mimalloc`](https://github.com/microsoft/mimalloc), it is more efficient and it can output some debug messages for you, please check [environment-options](https://github.com/microsoft/mimalloc#environment-options).