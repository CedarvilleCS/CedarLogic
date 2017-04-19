
#include "gl_defs.h"
#include <cmath>
#include <memory>

#include "../MainApp.h"
DECLARE_APP(MainApp)

void defineGLLists() {
	float degInRad = 0;
	
	// Wire end/junction points
	glNewList( CEDAR_GLLIST_CONNECTPOINT, GL_COMPILE );
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(0, 0);
 		for (int z=0; z <= 360; z += 360/POINTS_PER_VERTEX)
		{
			degInRad = z*DEG2RAD;
			glVertex2f(cos(degInRad)*wxGetApp().appSettings.wireConnRadius, sin(degInRad)*wxGetApp().appSettings.wireConnRadius);
		}
		glEnd();
	glEndList();
	
}




namespace {

	// The one opengl context to rule them all.
	std::unique_ptr<wxGLContext> context;

	// Attributes valid between startRenderToWxBitmap(..) and
	// finishRenderToWxBitmap().
	GLuint fb;    // Frame Buffer
	GLuint rb;     // Render Buffer
	int rbWidth;   // Render area width
	int rbHeight;  // Render area height
}

void createGLContext() {
	
	if (context != nullptr) {

		glewInit();
		wxGLCanvas temp(nullptr);
		context = std::make_unique<wxGLContext>(&temp);
	}
}

wxGLContext * getGLContext() {
	return context.get();
}

void startRenderToWxBitmap(int width, int height) {
	
	rbWidth = width;
	rbHeight = height;

	// Create frame buffer and render buffer.
	glGenFramebuffers(1, &fb);
	glGenRenderbuffers(1, &rb);

	// Allocate space for render buffer.
	// We make the buffer RGB because wxWidgets wants that.
	glBindRenderbuffer(GL_RENDERBUFFER, rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height);

	// Make frame buffer current.
	// Attach the render buffer to the frame buffer.
	glBindFramebuffer(GL_RENDERBUFFER, fb);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb);
}

wxBitmap finishRenderToWxBitmap() {

	// Make room for data about to be read from the render buffer.
	// The '3' is for RGB where each channel is 1 byte.
	std::vector<char> pixels(rbWidth * rbHeight * 3);

	// Make sure the correct render buffer is being read from.
	// (in startRenderToWxBitmap, we attach rb to fb through color attachment 0)
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	// Copy row-major, bottom-to-top, rgb, 24bit, pixels into the data vector.
	glReadPixels(0, 0, rbWidth, rbHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// Return to the normal framebuffer for this context.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Delete buffers.
	glDeleteFramebuffers(1, &fb);
	glDeleteRenderbuffers(1, &rb);

	return wxBitmap(pixels.data(), rbWidth, rbHeight);
}
