#if defined(_WIN32)
#define SEEBOT_API __declspec(dllexport)
#else
#define SEEBOT_API
#endif

extern "C" SEEBOT_API int SeeBotVersion()
{
    return 1;
}
