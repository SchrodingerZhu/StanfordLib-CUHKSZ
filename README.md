# StanfordLib-CUHKSZ
A cmake deliver version of stanford library, specially patched for CSC3002 students in CUHK(SZ)

# Notice
- All the sources are modified from [stanford-cpp-library](https://github.com/zelenski/stanford-cpp-library). The original author reserves the copyright.
- We hope you can use a more elegant naming and organizing method. Hence instead of the orignal style like `"gfont.h"`, we adapt a modularized way :`<graphics/gfont.h>`
- The orginal stanford library mixes files in all modules and produces cyclic dependency relations, therefore, we have to produce to whole shared library, instead of multiple small libraries for each module.
- The original version takes the advantage of `qmake` toolchain and make some renaming around `main()` function. Here, we want to seperate the library and user's code.
  Therefore, we do not use the library itself to do the renaming. Instead, users should write a entry function (say `start`) by themselves and include `macro.h` and call `__WRAP_MAIN__(start)` to invoke the library.