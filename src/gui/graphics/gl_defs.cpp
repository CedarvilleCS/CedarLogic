
#include "gl_defs.h"
#include "gl_text.h"
#include <cmath>
#include <memory>
#include <wx/rawbmp.h>

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

void createGLContext(wxGLCanvas &canvas) {
	
	if (context == nullptr) {

		context = std::make_unique<wxGLContext>(&canvas);
		makeGLCanvasCurrent(canvas);

		glewExperimental = GL_TRUE;
		GLenum status = glewInit();
		if (status != GLEW_OK) {
			;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// Load the font texture
		gl_text::loadFont(wxGetApp().appSettings.textFontFile);
	}
}

void makeGLCanvasCurrent(const wxGLCanvas &canvas) {
	context->SetCurrent(canvas);
}

void startRenderToWxImage(int width, int height) {
	
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

wxImage finishRenderToWxImage() {

	// Make room for data about to be read from the render buffer.
	// The '3' is for RGB where each channel is 1 byte.
	std::vector<unsigned char> pixels(rbWidth * rbHeight * 3);

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

	wxImage result;
	result.Create(pixels.data(), wxBitmapType::wxBITMAP_TYPE_ANY, rbWidth, rbHeight);

	


	for (int row = 0; row < rbHeight; row++) {
		for (int col = 0; col < rbWidth; col++) {
			result.GetData()[row * rbWidth + col] = pixels[row *rbWidth + col];
		}
	}

	return result;
}
