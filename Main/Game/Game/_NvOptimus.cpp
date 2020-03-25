#ifdef _WIN32
#include <windows.h>
extern "C" _declspec(dllexport) DWORD NvOptimusEnablement = true;
#endif // _WIN32