/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef WIN32_APPLICATION_H
#define WIN32_APPLICATION_H

#define UNICODE
#define _UNICODE

#define WINVER 0x0601 // Windows 7

#include <windows.h>
#include <glib-object.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_LIST_SIZE 16  // Initial size and also the growing size of the event list
#define INITIAL_QUEUE_SIZE 2  // the same for the callback list

#define  FM_COMMAND    0x4000      // FM: Forwarded Message
#define  FM_CLICKED    FM_COMMAND

typedef struct _Win32Window Win32Window;
typedef struct _Win32Container Win32Container;

#include "utilities.h"
#include "clipboard.h"
#include "wrappers.h"
#include "device-context.h"
#include "layout.h"
#include "window.h"
#include "container.h"
#include "application-window.h"
#include "control.h"
#include "button.h"
#include "label.h"
#include "edit.h"

#endif
