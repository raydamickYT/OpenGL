// ErrorCheck.h
#pragma once
#include <glad/glad.h>
#include <iostream>

void checkOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error " << err << ", at " << fname << ":" << line << " - for " << stmt << std::endl;
        abort();
    }
}

#define GL_CHECK(stmt) do { \
    stmt; \
    checkOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)
