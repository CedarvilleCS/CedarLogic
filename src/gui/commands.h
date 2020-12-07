/*****************************************************************************
   Project: CEDAR Logic Simulator
   Copyright 2006 Cedarville University, Benjamin Sprague,
                     Matt Lewellyn, and David Knierim
   All rights reserved.
   For license information see license.txt included with distribution.   

   commands: Implements a klsCommand object for each user interface command
*****************************************************************************/

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
#include "command/cmdMergeWire.h"
