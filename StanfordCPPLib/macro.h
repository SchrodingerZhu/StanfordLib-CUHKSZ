#include <private/mainwrapper.h>
#define __WRAP_MAIN__(m) int main(int argc, char** argv) { return run(argc, argv, m); }

