/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCGL_H__
#define __CCGL_H__

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

#if CC_OPENGL1_ONLY
#include "GL/gl.h"

#define glBindBuffer(...)
#define glGenBuffers(...)
#define glDeleteBuffers(...)
#define glGenVertexArrays(...)
#define glDeleteVertexArrays(...)
#define glBufferData(...)
#define glVertexAttribPointer(...)
#define glGetUniformLocation(...) 0
#define glCreateProgram(...) 0
#define glAttachShader(...)
#define glBindAttribLocation(...)
#define glGetProgramiv(...) 0
#define glEnableVertexAttribArray(...)
#define glBufferSubData(...)
#define glUniform1i(...)
#define glUniform2i(...)
#define glUniform3i(...)
#define glUniform4i(...)
#define glUniform2iv(...)
#define glUniform3iv(...)
#define glUniform4iv(...)
#define glUniform1f(...)
#define glUniform2f(...)
#define glUniform3f(...)
#define glUniform4f(...)
#define glUniform1fv(...)
#define glUniform2fv(...)
#define glUniform3fv(...)
#define glUniform4fv(...)
#define glUniformMatrix2fv(...)
#define glUniformMatrix3fv(...)
#define glUniformMatrix4fv(...)
#define glGetAttribLocation(...) 0
#define glGetActiveAttrib(...) 0
#define glGetProgramInfoLog(...) 0
#define glGetActiveUniform(...) 0
#define glCreateShader(...) 0
#define glShaderSource(...)
#define glCompileShader(...)
#define glGetShaderiv(...) 0
#define glGetShaderSource(...) 0
#define glLinkProgram(...)
#define glGetShaderiv(...) 0
#define glGetShaderInfoLog(...) 0
#define glDeleteShader(...)
#define glMapBuffer(...) 0
#define glUnmapBuffer(...)
#define glIsBuffer(...) 0
#define glDeleteProgram(...)
#define glUseProgram(...)
#define glBindVertexArray(...)
#define glDisableVertexAttribArray(...)
#define GL_FRAMEBUFFER_BINDING 0
#define GL_DEPTH24_STENCIL8 0
#define CC_GL_DEPTH24_STENCIL8      GL_DEPTH24_STENCIL8

#elif !defined(LINUX_ARM)

#include "GL/glew.h"

#define CC_GL_DEPTH24_STENCIL8      GL_DEPTH24_STENCIL8


#else

#define glClearDepth                glClearDepthf
#define glDeleteVertexArrays        glDeleteVertexArraysOES
#define glGenVertexArrays           glGenVertexArraysOES
#define glBindVertexArray           glBindVertexArrayOES
#define glMapBuffer                 glMapBufferOES
#define glUnmapBuffer               glUnmapBufferOES

#define GL_DEPTH24_STENCIL8         GL_DEPTH24_STENCIL8_OES
#define GL_WRITE_ONLY               GL_WRITE_ONLY_OES

#include <GLES2/gl2platform.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef GL_BGRA
#define GL_BGRA  0x80E1
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#undef None
#undef Bool

//declare here while define in EGLView_android.cpp
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;

#define glGenVertexArraysOES glGenVertexArraysOESEXT
#define glBindVertexArrayOES glBindVertexArrayOESEXT
#define glDeleteVertexArraysOES glDeleteVertexArraysOESEXT

#if !defined(GL_VERTEX_PROGRAM_POINT_SIZE)
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#endif

#endif

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

#endif // __CCGL_H__
