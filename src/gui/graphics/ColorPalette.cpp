
#include "ColorPalette.h"
#include "OpenGL.h"

void ColorPalette::setColor(const Color &color) {
	glColor4f(color.r, color.g, color.b, color.a);
}

void ColorPalette::setClearColor(const Color &color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

Color ColorPalette::getColor() {
	
	GLfloat color[4];
	glGetFloatv(GL_CURRENT_COLOR, color);
	return Color(color[0], color[1], color[2], color[3]);
}

void ColorPalette::save(std::ostream &out) {
	
	out << "SchematicBackground:\n" << SchematicBackground << "\n\n";

	out << "SchematicGrid:\n" << SchematicGrid << "\n\n";

	out << "GateShape:\n" << GateShape << "\n\n";

	out << "GateHotspot:\n" << GateHotspot << "\n\n";

	out << "GateOverlap:\n" << GateOverlap << "\n\n";

	out << "WireHigh:\n" << WireHigh << "\n\n";

	out << "WireLow:\n" << WireLow << "\n\n";

	out << "WireHiZ:\n" << WireHiZ << "\n\n";

	out << "WireUnknown:\n" << WireUnknown << "\n\n";

	out << "WireConflict:\n" << WireConflict << "\n\n";

	out << "KeypadSelection:\n" << KeypadSelection << "\n\n";

	out << "Text:\n" << Text << "\n\n";

	out << "TextSelected:\n" << TextSelected << "\n\n";

	out << "SelectionBoxFill:\n" << SelectionBoxFill << "\n\n";

	out << "SelectionBoxBorder:\n" << SelectionBoxBorder << "\n\n";
}

void ColorPalette::load(std::istream &in) {

	std::string trash;

	in >> trash >> SchematicBackground;
	in >> trash >> SchematicGrid;
	in >> trash >> GateShape;
	in >> trash >> GateHotspot;
	in >> trash >> GateOverlap;
	in >> trash >> WireHigh;
	in >> trash >> WireLow;
	in >> trash >> WireHiZ;
	in >> trash >> WireUnknown;
	in >> trash >> WireConflict;
	in >> trash >> KeypadSelection;
	in >> trash >> Text;
	in >> trash >> TextSelected;
	in >> trash >> SelectionBoxFill;
	in >> trash >> SelectionBoxBorder;
}



Color ColorPalette::SchematicBackground(1.0f, 1.0f, 1.0f);
Color ColorPalette::SchematicGrid(0.0f, 0.0f, 0.08f, 0.08f);

Color ColorPalette::GateShape(0.0f, 0.0f, 0.0f);
Color ColorPalette::GateHotspot(1.0f, 0.0f, 0.0f);
Color ColorPalette::GateOverlap(0.4f, 0.1f, 0.0f, 0.3f);

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