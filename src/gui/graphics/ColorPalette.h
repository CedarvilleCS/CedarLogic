
#pragma once
#include "Color.h"

struct ColorPalette {

	// Set OpenGL draw color.
	static void setColor(const Color &color);

	// Set OpenGL draw color.
	static void setClearColor(const Color &color);

	// Get OpenGL draw color.
	static Color getColor();

	// ===================================================================
	// Colors used throughout CedarLogic.
	// When new colors are used, they should be put here and referenced.
	// ===================================================================

	static Color SchematicBackground;
	static Color SchematicGrid;

	static Color GateShape;
	static Color GateHotspot;
	static Color GateOverlap;

	static Color WireHigh;
	static Color WireLow;
	static Color WireHiZ;
	static Color WireUnknown;
	static Color WireConflict;

	static Color KeypadSelection;

	static Color Text;
	static Color TextSelected;

	static Color SelectionBoxFill;
	static Color SelectionBoxBorder;
};