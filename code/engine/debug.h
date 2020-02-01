GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
void APIENTRY glDebugOutput(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar *Message, const void *UserParam);

GLenum 
glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  printf("INVALID_ENUM"); break;
            case GL_INVALID_VALUE:                 printf("INVALID_VALUE"); break;
            case GL_INVALID_OPERATION:             printf("INVALID_OPERATION"); break;
            case GL_OUT_OF_MEMORY:                 printf("OUT_OF_MEMORY"); break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: printf("INVALID_FRAMEBUFFER_OPERATION"); break;
        }
    }
    return errorCode;
}

void APIENTRY
glDebugOutput(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar *Message, const void *UserParam)
{
    // NOTE(insolence): ignore non-significant error/warning codes
    if (ID == 131169 || ID == 131185 || ID == 131218 || ID == 131204) return; 

    printf("--------------- \n");
    printf("Debug message (%d): %s \n", ID, Message);

    switch (Source)
    {
        case GL_DEBUG_SOURCE_API:             printf("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     printf("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           printf("Source: Other"); break;
    } printf("\n");

    switch (Type)
    {
        case GL_DEBUG_TYPE_ERROR:               printf("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         printf("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         printf("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              printf("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           printf("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               printf("Type: Other"); break;
    } printf("\n");
    
    switch (Severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         printf("Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       printf("Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          printf("Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Severity: notification"); break;
    } printf("\n");
    printf("\n");
}