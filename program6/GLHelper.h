#ifndef __GLHELPER
#define __GLHELPER

static void checkGLError(char const * ident = "")
{
	GLenum errCode;
	const GLubyte *errString;
	
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error (%s): %s\n", ident, errString);
	}
}

#endif