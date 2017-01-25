
// Previously, all the commands were in a single header.
// This header is a drop-in replacement for that one.
// To improve compile times, this header should be avoided.

#pragma once

#include "command/klsCommand.h"
#include "command/cmdMoveGate.h"
#include "command/cmdMoveWire.h"
#include "command/cmdMoveSelection.h"
#include "command/cmdCreateGate.h"
#include "command/cmdConnectWire.h"
#include "command/cmdDisconnectWire.h"
#include "command/cmdCreateWire.h"
#include "command/cmdDeleteWire.h"
#include "command/cmdDeleteGate.h"
#include "command/cmdDeleteSelection.h"
#include "command/cmdPasteBlock.h"
#include "command/cmdWireSegDrag.h"
#include "command/cmdSetParams.h"
#include "command/cmdDeleteTab.h"
#include "command/cmdAddTab.h"
