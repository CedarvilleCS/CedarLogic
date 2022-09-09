#include "glToImage.h"

#include "MainApp.h"
#include "wx/glcanvas.h"
#include "klsMiniMap.h"

#include "paramDialog.h"

glImageCtx::glImageCtx(int a_width, int a_height, wxWindow *parent) {
	width = a_width;
	height = a_height;

#ifdef _WINDOWS
	// This only works in MS Windows because of the DIB Section OpenGL
	// rendering.

	// Create a DIB section.
	// (The Windows wxBitmap implementation will create a DIB section for a bitmap if you set
	// a color depth of 24 or greater.)
	theBM = wxBitmap(width, height, 32);

	// Get a memory hardware device context for writing to the bitmap DIB Section:
	myDC = new wxMemoryDC();
	myDC->SelectObject(theBM);
	theHDC = myDC->GetHDC();

	// The basics of setting up OpenGL to render to the bitmap are found at:
	// http://www.nullterminator.net/opengl32.html
	// http://www.codeguru.com/cpp/g-m/opengl/article.php/c5587/

    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    // set the pixel format for the DC
    ::ZeroMemory( &pfd, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ::ChoosePixelFormat( (HDC) theHDC, &pfd );
    ::SetPixelFormat( (HDC) theHDC, iFormat, &pfd );

    // create and enable the render context (RC)
    hRC = ::wglCreateContext( (HDC) theHDC );
    oldhRC = ::wglGetCurrentContext();
    oldDC = ::wglGetCurrentDC();
    ::wglMakeCurrent( (HDC) theHDC, hRC );
#endif
}

wxImage glImageCtx::getImage() {
#ifdef _WINDOWS
	// convert the DIB Section into a wxImage to return to the caller
	return theBM.ConvertToImage();
#elif __linux__
	uint8_t* pixels = new uint8_t[3 * width * height];
	uint8_t* flipped = new uint8_t[3 * width * height];

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Reverse the orders of rows in the image we read.
	// glReadPixels gives us the bottom row first, but we need the top first.
	// We copy the rows one at a time to the correct loation in a new buffer.
	//  * We can get the the stat of a row bu multiplying the width (in bytes) by
	//    the row number we want.
	//  * by adding the offset to the pointer to the start of the pixel buffer
	//    we can get a pointer to the start of a row
	int row_size = width*3; // The width of a row in bytes; one for each color
	for (int y=0; y < height; y++) {
		memcpy(&flipped[y*row_size], &pixels[(height-y)*row_size], row_size);
	}

/* 	for (int y=0; y < height; y++) { */
/* 		for (int x=0; x < (width*3); x++) { */
/* 			flipped[(y*width*3) + x] = pixels[((height-y)*width*3) + x]; */
/* 		} */
/* 	} */

	wxImage mapImage(width, height, true);
	mapImage.SetData(flipped);

	return mapImage;
#endif
};

glImageCtx::~glImageCtx() {
#ifdef _WINDOWS
	// Destroy the OpenGL rendering context, release the memDC
	::wglMakeCurrent( oldDC, oldhRC );
	::wglDeleteContext( hRC );
	myDC->SelectObject(wxNullBitmap);
#endif
};
