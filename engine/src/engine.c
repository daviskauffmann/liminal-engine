#include <engine/engine.h>

int engine_init(void)
{
    // init GLEW
    {
        GLenum glewError = glewInit();

        if (glewError != GLEW_OK)
        {
            printf("Error: %s\n", glewGetErrorString(glewError));

            return 1;
        }
    }

    printf("GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("OpenGL %s\n", glGetString(GL_VERSION));
    printf("Vendor %s\n", glGetString(GL_VENDOR));
    printf("Renderer %s\n", glGetString(GL_RENDERER));
    printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // load assets now that GLEW is initialized
    {
        int error = assets_load();

        if (error != 0)
        {
            printf("Error: Couldn't initialize assets\n");

            return error;
        }
    }

    return 0;
}