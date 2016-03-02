/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   CircuitPrint: Implements a printout object for print and print preview
*****************************************************************************/

#include "CircuitPrint.h"

bool CircuitPrint::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
		DrawPageOne(dc);

        dc->SetDeviceOrigin(0, 0);
        dc->SetUserScale(1.0, 1.0);

        return true;
    }
    else
        return false;
}

bool CircuitPrint::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void CircuitPrint::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool CircuitPrint::HasPage(int pageNum)
{
    return (pageNum == 1);
}

void CircuitPrint::DrawPageOne(wxDC *dc)
{
	// Scale the image to fit within a
	// "margined" area on the printer DC, and
	// keep its same aspect ratio:

    // Get the size of the print DC in pixels
    int w, h;
    dc->GetSize(&w, &h);

		// draw the filename
        wxChar buf[200];
        wxSprintf(buf, filename);
		int textWidth, textHeight;
		dc->GetTextExtent(buf, &textWidth, &textHeight);
        dc->DrawText(buf, w/2-textWidth/2, 10);
    
    float marginX = 150;
    float marginY = 150;
    int maxW = (int)(w - 2 * marginX);
    int maxH = (int)(h - 2 * marginY);
	
	int cw, ch;
	sourceCanvas->GetClientSize( &cw, &ch );
	
	float aspect = (float) ch / (float) cw;

	float imageWidth = 0;
	float imageHeight = 0;
	
	if( aspect * maxW <= maxH ) {
		// If we can fit the maximum width image in the given height,
		// then do it, otherwise take the maximum height.
		imageWidth = maxW;
		imageHeight = aspect * maxW;
	} else {
		imageWidth = (float) maxH / aspect;
		imageHeight = maxH;
	}

/*
    // Calculate a suitable scaling factor
    float scaleX=(float)(w/maxX);
    float scaleY=(float)(h/maxY);

    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX,scaleY);

    // Calculate the position on the DC for centring the graphic
    float posX = (float)((w - (200*actualScale))/2.0);
    float posY = (float)((h - (200*actualScale))/2.0);

    // Set the scale and origin
    dc->SetUserScale(actualScale, actualScale);
    dc->SetDeviceOrigin( (long)posX, (long)posY );
*/

	wxImage viewShot = sourceCanvas->renderToImage((int)imageWidth, (int)imageHeight);
	wxBitmap printBmp(viewShot);

	float posX = ((float) w / 2.0) - imageWidth / 2.0;
	float posY = ((float) h / 2.0) - imageHeight / 2.0;

	dc->DrawBitmap( printBmp, (int)posX, (int)posY );
}
