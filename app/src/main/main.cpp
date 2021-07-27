#include "shared/Entrypoint.hpp"

// main program entrypoint function
#if defined(OS_WIN)
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    return shared::wWinMain(hInstance);
}
#else
int main(int argc, char *argv[])
{
    return shared::main(argc, argv);
}
#endif
