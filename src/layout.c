/*---------------------------------------------------------------------------------------------
 *  Copyright (c) 2022 Emre ÖZÇAKIR  
 *  Licensed under the MIT License. See License file in the project root for more information.
 *-------------------------------------------------------------------------------------------*/

#include "vala-win32.h"
#include <stdio.h>

void win32_edge_list_append (Win32EdgeList *list, Win32Window *item, int edge);
BOOL win32_edge_list_includes (Win32EdgeList *list, Win32Window *item, int edge);
void win32_edge_list_prepend_referred_edges (Win32EdgeList *list, Win32Window *item, int edge);

/* CONSTRUCTOR
------------------------------------------- */
Win32RelativeLayout* win32_relative_layout_new (UINT padding, UINT spacing)
{
    Win32RelativeLayout *relativeLayout = malloc( sizeof(Win32RelativeLayout) );
    memset( relativeLayout, 0, sizeof(Win32RelativeLayout) );
    Win32Layout *layout = (Win32Layout*) relativeLayout;

    // increase reference count
    win32_layout_ref( relativeLayout );

    layout->recalculate = win32_relative_layout_recalculate;
    layout->configure   = win32_relative_layout_configure;
    relativeLayout->vPadding = padding;
    relativeLayout->hPadding = padding;
    relativeLayout->hSpacing = spacing;
    relativeLayout->vSpacing = spacing;
    relativeLayout->scale = 100;

    return relativeLayout;
}


/* METHOD SET SCALE
------------------------------------------- */
Win32RelativeLayout* win32_relative_layout_set_scale(Win32RelativeLayout* self, UINT scale)
{
    self->scale = scale;
    return self;
}


/* METHOD SET SPACING
------------------------------------------- */
Win32RelativeLayout* win32_relative_layout_with_spacing(Win32RelativeLayout* self, UINT vSpacing, UINT hSpacing)
{
    self->vSpacing = vSpacing;
    self->hSpacing = (hSpacing == -1) ? vSpacing : hSpacing;
    return self;
}


/* METHOD SET PADDING
------------------------------------------- */
Win32RelativeLayout* win32_relative_layout_with_padding(Win32RelativeLayout* self, UINT vPadding, UINT hPadding)
{
    self->vPadding = vPadding;
    self->hPadding = (hPadding == -1) ? vPadding : hPadding;
    return self;
}


/* INTERNAL LAYOUT SETUP
------------------------------------------- */
void win32_relative_layout_configure(Win32Container* container)
{
    Win32RelativeLayout *layout = (Win32RelativeLayout*) container->layout;

    Win32Window ** children = container->childWindows.items;
    size_t numChildren      = container->childWindows.length;

    // initialize lists
    free(layout->edgeList.items); // freeing an empty list (i.e. NULL pointer) is OK
    layout->edgeList.length = numChildren * 4;
    layout->edgeList.items = malloc( sizeof(Win32EdgeListItem) * layout->edgeList.length );
    memset( layout->edgeList.items, 0, sizeof(Win32EdgeListItem) * layout->edgeList.length );

    Win32Window *child;
    Win32Window *reference;
    Win32Anchor *anchor, *pair;
    // Re-arrange children
    int edge, pairEdge;
    for (int i=0; i<numChildren; i++)
    {
        child = children[i];

        for ( int n=0; n<4; n++) // Four sides of a window
        {
            edge = 1 << n;
            if ( win32_edge_list_includes( &layout->edgeList, child, edge) ) continue;

            win32_edge_list_prepend_referred_edges (&layout->edgeList, child, edge );
            win32_edge_list_append (&layout->edgeList, child, edge );
        }// END LOOP

    }// END OUTER LOOP
}


/* INTERNAL LAYOUT UTILITY
------------------------------------------- */
void win32_edge_list_prepend_referred_edges(Win32EdgeList *list, Win32Window *item, int edge)
{
    Win32Window *child = item;
    Win32Anchor *anchor, *pair;
    int pairEdge;

    switch (edge){
        case EDGE_LEFT:
            anchor = child->positioning->left;
            pair   = child->positioning->right;
            pairEdge = EDGE_RIGHT;
            break;
        case EDGE_TOP:
            anchor = child->positioning->top;
            pair   = child->positioning->bottom;
            pairEdge = EDGE_BOTTOM;
            break;
        case EDGE_RIGHT:
            anchor = child->positioning->right;
            pair   = child->positioning->left;
            pairEdge = EDGE_LEFT;
            break;
        case EDGE_BOTTOM:
            anchor = child->positioning->bottom;
            pair   = child->positioning->top;
            pairEdge = EDGE_TOP;
            break;
    }
    // NULL anchor
    if ( !anchor ){
        // LEFT-RIGHT and TOP-BOTTOM edges are considered pairs.
        // If only one element of a pair is NULL, the calculated value of the
        // NULL anchor will be dependent upon the non-null pair.
        if ( pair && !win32_edge_list_includes(list, child, pairEdge) )
        {
            win32_edge_list_prepend_referred_edges(list, child, pairEdge);
            win32_edge_list_append(list, child, pairEdge);
        }
        return;
    }
    // Anchored to the parent
    if (!anchor->reference) return;

    // Anchored to a sibling
    if (anchor->edge == 0){
        switch (edge){
            case EDGE_LEFT: anchor->edge = EDGE_RIGHT;
                break;
            case EDGE_TOP: anchor->edge = EDGE_BOTTOM;
                break;
            case EDGE_RIGHT: anchor->edge = EDGE_LEFT;
                break;
            case EDGE_BOTTOM: anchor->edge = EDGE_TOP;
                break;
        }
    }
    // Check if the anchored window's edge is already in the list
    if ( win32_edge_list_includes(list, anchor->reference, anchor->edge) ) return;

    win32_edge_list_prepend_referred_edges(list, anchor->reference, anchor->edge);
    win32_edge_list_append(list, anchor->reference, anchor->edge);
}


/* INTERNAL LAYOUT UTILITY
------------------------------------------- */
void win32_edge_list_append(Win32EdgeList *list, Win32Window *item, int edge)
{
    // insert the item into the first available spot
    size_t length = list->length;
    for ( int i=0; i<length; i++ ){
        if ( list->items[i].window == NULL ){
            list->items[i].window = item;
            list->items[i].edge = edge;
            break;
        }
    }
}


/* INTERNAL LAYOUT UTILITY
------------------------------------------- */
BOOL win32_edge_list_includes(Win32EdgeList *list, Win32Window *item, int edge)
{
    size_t length = list->length;
    for ( int i=0; i<length; i++ ){
        if ( list->items[i].window == item && list->items[i].edge == edge ) return TRUE;
    }
    return FALSE;
}


/* INTERNAL LAYOUT CHILDREN
------------------------------------------- */
void win32_relative_layout_recalculate(Win32Container* container)
{
    Win32RelativeLayout *layout = (Win32RelativeLayout*) container->layout;
    Win32Window * window = (Win32Window*) container;

    double scale = (double) layout->scale;
    UINT halfSpacing_L = layout->vSpacing / 2 + (layout->vSpacing % 2);
    UINT halfSpacing_R = layout->vSpacing / 2;
    UINT halfSpacing_T = layout->hSpacing / 2 + (layout->hSpacing % 2);
    UINT halfSpacing_B = layout->hSpacing / 2;

    RECT rect;
    GetClientRect( window->hwnd, &rect);
    int containerWidth  = rect.right - rect.left - 2 * layout->vPadding + layout->vSpacing;
    int containerHeight = rect.bottom - rect.top - 2 * layout->hPadding + layout->hSpacing;

    Win32EdgeList *edgeList = &layout->edgeList;

    Win32Window *child;
    Win32Window *reference;
    int *left, *top, *bottom, *right, withPadding;
    for (int i=0; i< edgeList->length; i++)
    {
        child = edgeList->items[i].window;

        switch ( edgeList->items[i].edge )
        {
        case EDGE_LEFT:
            left = &child->positioning->_left;

            if ( child->positioning->left ){
                // Positioning relative to a sibling
                // [!] Assignment is intentional
                if ( reference = child->positioning->left->reference ){
                    if ( child->positioning->left->edge == EDGE_RIGHT ){
                        *left = reference->positioning->_right + layout->vSpacing;;
                    } else if ( child->positioning->left->edge == EDGE_LEFT ){
                        *left = reference->positioning->_left;
                    }
                // Positioning relative to the parent
                } else {
                    *left = child->positioning->left->ratio / scale * containerWidth;
                    // Add spacing
                    *left += halfSpacing_R;
                }
            // No anchor is provided
            } else if (child->positioning->right){
                *left = child->positioning->_right - child->width;
            } else {
                *left = halfSpacing_R; // 0
            }
            break;

        case EDGE_TOP:
            top = &child->positioning->_top;

            if ( child->positioning->top ){
                // Positioning relative to a sibling
                // [!] Assignment is intentional
                if ( reference = child->positioning->top->reference ){
                    if ( child->positioning->top->edge == EDGE_BOTTOM ){
                        *top = reference->positioning->_bottom + layout->hSpacing;
                    } else if ( child->positioning->top->edge == EDGE_TOP ){
                        *top = reference->positioning->_top;
                    }
                // Positioning relative to the parent
                } else {
                    *top = child->positioning->top->ratio / scale * containerHeight;
                    // Add spacing
                    *top += halfSpacing_B;
                }
            // No anchor is provided
            } else if (child->positioning->bottom){
                *top = child->positioning->_bottom - child->height;
            } else {
                *top = halfSpacing_B; // 0
            }
            break;

        case EDGE_RIGHT:
            right = &child->positioning->_right;

            if ( child->positioning->right ){
                // Positioning relative to a sibling
                // [!] Assignment is intentional
                if ( reference = child->positioning->right->reference ){
                    if ( child->positioning->right->edge == EDGE_RIGHT ){
                        *right = reference->positioning->_right;
                    } else if ( child->positioning->right->edge == EDGE_LEFT ){
                        *right = reference->positioning->_left - layout->vSpacing;
                    }
                // Positioning relative to the parent
                } else {
                    *right = child->positioning->right->ratio / scale * containerWidth;
                    // Add spacing
                    *right -= halfSpacing_L;
                }
            // No anchor is provided
            } else *right = child->positioning->_left + child->width;

            break;

        case EDGE_BOTTOM:
            bottom = &child->positioning->_bottom;

            if ( child->positioning->bottom ){
                // Positioning relative to a sibling
                // [!] Assignment is intentional
                if ( reference = child->positioning->bottom->reference ){
                    if ( child->positioning->bottom->edge == EDGE_BOTTOM ){
                        *bottom = reference->positioning->_bottom;
                    } else if ( child->positioning->bottom->edge == EDGE_TOP ){
                        *bottom = reference->positioning->_top - layout->hSpacing;
                    }
                // Positioning relative to the parent
                } else {
                    *bottom = child->positioning->bottom->ratio / scale * containerHeight;
                    // Add spacing
                    *bottom -= halfSpacing_T;
                }
            // No anchor is provided
            } else *bottom = child->positioning->_top + child->height;

            break;
        }
    }// END LOOP

    int width, height;
    size_t numChildren = container->childWindows.length;
    for (int i=0; i<numChildren; i++ )
    {
        child = container->childWindows.items[i];

        width = child->positioning->_right - child->positioning->_left;
        height = child->positioning->_bottom - child->positioning->_top;

        win32_window_move_and_resize  ( (Win32Window*) child, child->positioning->_left + layout->vPadding - halfSpacing_R,
                                  child->positioning->_top + layout->hPadding - halfSpacing_B, width, height);

    }
}


/* INTERNAL REF LAYOUT
------------------------------------------- */
void* win32_layout_ref (void* instance)
{
    Win32Layout * self = instance;
    g_atomic_int_inc (&self->ref_count);
    return self;
}


/* INTERNAL UNREF LAYOUT
------------------------------------------- */
void win32_layout_unref (void* instance)
{
    Win32Layout * self = instance;
    if (g_atomic_int_dec_and_test (&self->ref_count)) {
        free (self);
    }
}

/*┌────────────────────────────────────────────────────────┐
  │ ANCHOR                                                 │
  └────────────────────────────────────────────────────────┘*/

/* CONSTRUCT
------------------------------------------- */
Win32Anchor *win32_anchor_to_parent( UINT ratio, int offset )
{
    Win32Anchor* anchor = malloc( sizeof(Win32Anchor) );
    memset( anchor, 0, sizeof(Win32Anchor) );

    // increase reference count
    win32_anchor_ref( anchor );

    anchor->ratio = ratio;
    anchor->offset = offset;

    return anchor;
}


/* CONSTRUCT
------------------------------------------- */
Win32Anchor *win32_anchor_to_sibling( Win32Window *sibling, int offset )
{
    Win32Anchor* anchor = malloc( sizeof(Win32Anchor) );
    memset( anchor, 0, sizeof(Win32Anchor) );

    // increase reference count
    win32_anchor_ref( anchor );

    anchor->reference = sibling;
    anchor->offset = offset;

    return anchor;
}


/* METHOD TO EDGE
------------------------------------------- */
Win32Anchor *win32_anchor_to_edge(Win32Anchor *self, int edge)
{
    self->edge = edge;
    return self;
}


/* METHOD WITH OFFSET
------------------------------------------- */
Win32Anchor *win32_anchor_with_offset(Win32Anchor *self, int offset)
{
    self->offset = offset;
    return self;
}


/* INTERNAL REF ANCHOR
------------------------------------------- */
void* win32_anchor_ref (void* instance)
{
    Win32Anchor * self = (Win32Anchor *) instance;
    g_atomic_int_inc (&self->ref_count);
    return self;
}


/* INTERNAL UNREF ANCHOR
------------------------------------------- */
void win32_anchor_unref (void* instance)
{
    Win32Anchor * self = instance;
    if (g_atomic_int_dec_and_test (&self->ref_count)) {
        free (self);
    }
}


/*┌────────────────────────────────────────────────────────┐
  │ LAYOUT DATA                                            │
  └────────────────────────────────────────────────────────┘*/
Win32LayoutData *win32_layout_data_new (void)
{
    Win32LayoutData *instance;

    instance = malloc( sizeof(Win32LayoutData) );
    memset( instance, 0, sizeof(Win32LayoutData) );
    win32_layout_data_ref (instance);

    return instance;
}

/* PROPERTY SET LEFT
------------------------------------------- */
void  win32_layout_data_set_left (Win32LayoutData *instance, Win32Anchor* anchor)
{
    if ( instance->left != NULL ){
        if (instance->left == anchor ) return;
        // decrease the reference count of the existing anchor
        win32_anchor_unref( instance->left );
    }

    // increase reference count
    win32_anchor_ref( anchor );

    instance->left = anchor;
}


/* PROPERTY GET LEFT
------------------------------------------- */
Win32Anchor* win32_layout_data_get_left (Win32LayoutData *instance)
{
    return instance->left;
}


/* PROPERTY SET TOP
------------------------------------------- */
void  win32_layout_data_set_top (Win32LayoutData *instance, Win32Anchor* anchor)
{
    if ( instance->top != NULL ){
        if (instance->top == anchor ) return;
        // decrease the reference count of the existing anchor
        win32_anchor_unref( instance->top );
    }

    // increase reference count
    win32_anchor_ref( anchor );

    instance->top = anchor;
}


/* PROPERTY GET TOP
------------------------------------------- */
Win32Anchor* win32_layout_data_get_top (Win32LayoutData *instance)
{
    return instance->top;
}


/* PROPERTY SET RIGHT
------------------------------------------- */
void  win32_layout_data_set_right (Win32LayoutData *instance, Win32Anchor* anchor)
{
    if ( instance->right != NULL ){
        if (instance->right == anchor ) return;
        // decrease the reference count of the existing anchor
        win32_anchor_unref( instance->right );
    }

    // increase reference count
    win32_anchor_ref( anchor );

    instance->right = anchor;
}


/* PROPERTY GET RIGHT
------------------------------------------- */
Win32Anchor* win32_layout_data_get_right (Win32LayoutData *instance)
{
    return instance->right;
}


/* PROPERTY SET BOTTOM
------------------------------------------- */
void  win32_layout_data_set_bottom (Win32LayoutData *instance, Win32Anchor* anchor)
{
    if ( instance->bottom != NULL ){
        if (instance->bottom == anchor ) return;
        // decrease the reference count of the existing anchor
        win32_anchor_unref( instance->bottom );
    }

    // increase reference count
    win32_anchor_ref( anchor );

    instance->bottom = anchor;
}


/* PROPERTY GET BOTTOM
------------------------------------------- */
Win32Anchor* win32_layout_data_get_bottom (Win32LayoutData *instance)
{
    return instance->bottom;
}


/* INTERNAL REF LAYOUT DATA
------------------------------------------- */
void* win32_layout_data_ref (void* instance)
{
    Win32LayoutData * self = instance;
    g_atomic_int_inc (&self->ref_count);
    return self;
}


/* INTERNAL UNREF LAYOUT DATA
------------------------------------------- */
void win32_layout_data_unref (void* instance)
{
    Win32LayoutData * self = instance;
    if (g_atomic_int_dec_and_test (&self->ref_count)) {
        free (self);
    }
}
