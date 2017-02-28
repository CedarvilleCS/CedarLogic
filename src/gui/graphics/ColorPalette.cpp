
#include "ColorPalette.h"
#include "gl_wrapper.h"

void ColorPalette::setColor(const Color &color) {
	glColor4f(color.r, color.g, color.b, color.a);
}

Color ColorPalette::getColor() {
	
	GLfloat color[4];
	glGetFloatv(GL_CURRENT_COLOR, color);
	return Color(color[0], color[1], color[2], color[3]);
}

Color ColorPalette::SchematicBackground(1.0f, 1.0f, 1.0f);
Color ColorPalette::SchematicGrid(0.0f, 0.0f, 0.08f, 0.08f);

Color ColorPalette::GateShape(0.0f, 0.0f, 0.0f);
Color ColorPalette::GateHotspot(1.0f, 0.0f, 0.0f);

Color ColorPalette::WireHigh(1.0f, 0.0f, 0.0f);
Color ColorPalette::WireLow(0.0f, 0.0f, 0.0f);
Color ColorPalette::WireHiZ(0.0f, 0.78f, 0.0f);
Color ColorPalette::WireUnknown(0.3f, 0.3f, 1.0f);
Color ColorPalette::WireConflict(0.0f, 1.0f, 1.0f);

Color ColorPalette::KeypadSelection(0.0f, 0.4f, 1.0f, 0.3f);

Color ColorPalette::Text(0.0f, 0.0f, 0.0f);
Color ColorPalette::TextSelected(1.0f, 0.0f, 0.0f, 0.7f);

Color ColorPalette::SelectionBoxFill(0.0f, 0.4f, 1.0f, 0.3f);
Color ColorPalette::SelectionBoxBorder(0.0f, 0.4f, 1.0f);