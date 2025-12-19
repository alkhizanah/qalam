#ifdef _WIN32
#include "platform_windows.c"
#elif __APPLE__
#include "platform_macos.c"
#elif __linux
#include "platform_linux.c"
#endif
