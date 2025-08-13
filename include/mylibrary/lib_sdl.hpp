#ifndef C_TEST_LIB_SDL_HPP
#define C_TEST_LIB_SDL_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>

#include <mylibrary/lib_global.hpp>

namespace my_library {
    enum SDL_TaskOnError {
        PRINT_MASSAGE,
        IGNORE,
        EXIT_PROGRAM
    };

    static void (*func)() = null;
}

namespace {
    void checkInitError(const char *taskName, my_library::SDL_TaskOnError & task, bool isError) {
        if (my_library::func == null) {
            SDL_Log("Release resources function is not set!");
        }

        if (isError) {
            switch (task) {
                case my_library::SDL_TaskOnError::PRINT_MASSAGE:
                    SDL_Log("%s Failed: %s\n", taskName, SDL_GetError());
                    break;
                case my_library::SDL_TaskOnError::EXIT_PROGRAM:
                    SDL_Log("%s Failed, Exit Program: %s\n", taskName, SDL_GetError());

                    if (my_library::func != null) {
                        my_library::func();
                    }

                    exit(EXIT_FAILURE);
                case my_library::SDL_TaskOnError::IGNORE:
                    break;
                default:
                    SDL_Log("Unknown \"task if error\" code: %d\n", task);
            }
        } else {
            SDL_Log("%s...OK\n", taskName);
        }
    }
}

namespace my_library {
    //调用此函数设置资源释放函数
    inline void registerReleaseSDLResources(void (*f)()) {
        func = f;
    }

#define releaseSDLResource(function, taskName) function; SDL_Log(taskName"...Done")
#define expandSDLColor(color) color.r, color.g, color.b, color.a

    //检查SDL函数返回值
    inline void sdlCheckErrorInt(int retVal, const char *taskName, SDL_TaskOnError errorTask) {
        checkInitError(taskName, errorTask, retVal < 0);
    }

    inline void sdlCheckErrorPtr(void *retVal, const char *taskName, SDL_TaskOnError errorTask) {
        checkInitError(taskName, errorTask, retVal == null);
    }
}
#endif //C_TEST_LIB_SDL_HPP
