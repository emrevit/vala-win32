/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#ifndef _WIN32_LAYOUT_H_
#define _WIN32_LAYOUT_H_

#include <windows.h>
#include <glib-object.h>

#define EDGE_LEFT     1
#define EDGE_TOP      2
#define EDGE_RIGHT    4
#define EDGE_BOTTOM   8

#define _win32_anchor_unref0(var)   ((var == NULL) ? NULL : (var = (win32_anchor_unref (var), NULL)))
#define _win32_layout_unref0(var) ((var == NULL) ? NULL : (var = (win32_layout_unref (var), NULL)))

/* CLASS Anchor
------------------------------------------- */
typedef struct _Win32Anchor {
    volatile int ref_count;
    Win32Window *reference;
    UINT ratio;
    int offset;
    int edge;
} Win32Anchor;

Win32Anchor *win32_anchor_to_parent( UINT ratio, int offset );
Win32Anchor *win32_anchor_to_sibling( Win32Window *sibling, int offset );

Win32Anchor *win32_anchor_to_edge(Win32Anchor *instance, int edge);
Win32Anchor *win32_anchor_with_offset(Win32Anchor *instance, int offset);

/* INTERNAL */
void* win32_anchor_ref   (void*);
void  win32_anchor_unref (void*);

/* CLASS LayoutData
------------------------------------------- */
typedef struct _Win32LayoutData {
    volatile int ref_count;
    Win32Anchor *left;
    Win32Anchor *top;
    Win32Anchor *right;
    Win32Anchor *bottom;
    int _left;
    int _top;
    int _right;
    int _bottom;
    int _width;
    int _height;
} Win32LayoutData;

Win32LayoutData* win32_layout_data_new (void);

void  win32_layout_data_set_left (Win32LayoutData *instance, Win32Anchor* anchor);
Win32Anchor* win32_layout_data_get_left (Win32LayoutData *instance);

void  win32_layout_data_set_top (Win32LayoutData *instance, Win32Anchor* anchor);
Win32Anchor* win32_layout_data_get_top (Win32LayoutData *instance);

void  win32_layout_data_set_right (Win32LayoutData *instance, Win32Anchor* anchor);
Win32Anchor* win32_layout_data_get_right (Win32LayoutData *instance);

void  win32_layout_data_set_bottom (Win32LayoutData *instance, Win32Anchor* anchor);
Win32Anchor* win32_layout_data_get_bottom (Win32LayoutData *instance);

/* INTERNAL */
void* win32_layout_data_ref   (void*);
void  win32_layout_data_unref (void*);


typedef struct _Win32Layout {
    volatile int ref_count;
    void (*recalculate)(Win32Container *container);
    void (*configure)  (Win32Container *container);
} Win32Layout;

/* INTERNAL */
void* win32_layout_ref   (void*);
void  win32_layout_unref (void*);


/* CLASS RelativeLayout
------------------------------------------- */
typedef struct _Win32EdgeListItem {
    Win32Window * window;
    int edge;
} Win32EdgeListItem;

typedef struct _Win32EdgeList {
    Win32EdgeListItem * items;
    size_t length;
} Win32EdgeList;

typedef struct _Win32RelativeLayout {
    Win32Layout layout;
    UINT vPadding;
    UINT hPadding;
    UINT hSpacing;
    UINT vSpacing;
    UINT scale;
    Win32EdgeList edgeList;
} Win32RelativeLayout;

Win32RelativeLayout* win32_relative_layout_new (UINT padding, UINT spacing);
Win32RelativeLayout* win32_relative_layout_set_scale(Win32RelativeLayout* self, UINT scale);
Win32RelativeLayout* win32_relative_layout_with_spacing(Win32RelativeLayout* self, UINT vSpacing, UINT hSpacing);
Win32RelativeLayout* win32_relative_layout_with_padding(Win32RelativeLayout* self, UINT vPadding, UINT hPadding);
void win32_relative_layout_recalculate(Win32Container* container);
void win32_relative_layout_configure(Win32Container* container);


#endif
