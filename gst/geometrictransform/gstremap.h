/*
 * GStreamer
 * Copyright (C) 2010 Filippo Argiolas <filippo.argiolas@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
// comment below for development 1.19.2
// uncomment below for renesas RZ/G2L 1.16.3
#define GST_RENESAS

#ifndef __GST_REMAP_H__
#define __GST_REMAP_H__
#include <stdio.h>
#include <stdlib.h>
#include "geometricmath.h"
#include "glib.h"
#include "gstgeometrictransform.h"
#include <gst/gst.h>

G_BEGIN_DECLS
/* #defines don't like whitespacey bits */
#define GST_TYPE_REMAP (gst_remap_get_type())
#define GST_REMAP(obj)                                               \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_REMAP,               \
                                GstRemap))
#define GST_REMAP_CAST(obj) ((GstRemap *)(obj))
#define GST_REMAP_CLASS(klass)                                       \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_REMAP,                \
                             GstRemapClass))
#define GST_IS_REMAP(obj)                                            \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_REMAP))
#define GST_IS_REMAP_CLASS(klass)                                    \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_REMAP))
typedef struct _GstRemap GstRemap;
typedef struct _GstRemapClass GstRemapClass;

#define BUF_SZ 1920*1080
struct _GstRemap {
    GstGeometricTransform element;
float *buf_x;
float *buf_y;
FILE *fptr_x;
FILE *fptr_y;
int mat_width;
int mat_height;    
char *map_x;
char *map_y;
};

struct _GstRemapClass {
    GstGeometricTransformClass parent_class;
};

GType gst_remap_get_type(void);
#ifdef GST_RENESAS
// 1.16.3
gboolean gst_remap_plugin_init(GstPlugin *plugin);
#else
// 1.19.2
GST_ELEMENT_REGISTER_DECLARE(remap);
#endif
G_END_DECLS

static void gst_remap_finalize (GObject * obj);
static gboolean remap_prepare(GstGeometricTransform * trans);
#endif /* __GST_REMAP_H__ */
