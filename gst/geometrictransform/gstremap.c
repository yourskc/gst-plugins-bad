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

/**
 * SECTION:element-remap
 * @title: remap
 * @see_also: geometrictransform
 *
 * Remap is a geometric image transform element. It transform a MOIL
 * fisheye to an remap image .
 *
 * ## Example launch line
 * |[
 * gst-launch-1.0 -v videotestsrc ! remap ! videoconvert !
 * autovideosink
 * ]|
 *
 */
#include "glib-object.h"
#include "glib.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include <math.h>

#include "gstremap.h"

GST_DEBUG_CATEGORY_STATIC(gst_remap_debug);
#define GST_CAT_DEFAULT gst_remap_debug

enum
{
  PROP_0,
  PROP_MAPX,
  PROP_MAPY
};

#define DEFAULT_MAPX "EquimatX"
#define DEFAULT_MAPY "EquimatY"  

#define gst_remap_parent_class parent_class
G_DEFINE_TYPE(GstRemap, gst_remap,
              GST_TYPE_GEOMETRIC_TRANSFORM);

#ifndef GST_RENESAS
// 1.19.2
GST_ELEMENT_REGISTER_DEFINE_WITH_CODE(
    remap, "remap", GST_RANK_NONE, GST_TYPE_REMAP,
    GST_DEBUG_CATEGORY_INIT(gst_remap_debug, "remap", 0,
                            "remap"));
#endif

static void
gst_remap_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstRemap *remap;
  GstGeometricTransform *gt;
  const char *v;

  gt = GST_GEOMETRIC_TRANSFORM_CAST (object);
  remap = GST_REMAP_CAST (gt);

  GST_OBJECT_LOCK (remap);
  switch (prop_id) {
    case PROP_MAPX:
      v = g_value_get_string (value);
      if (strcmp( v,remap->map_x)) {
        fprintf(stdout, "mapx : %s \n",v);
        strcpy(remap->map_x, v);
        gst_geometric_transform_set_need_remap (gt);
      }
      break;
    case PROP_MAPY:
      v = g_value_get_string (value);
      if (strcmp( v,remap->map_y)) {
        fprintf(stdout, "mapy : %s \n",v);
        strcpy(remap->map_y, v);
        gst_geometric_transform_set_need_remap (gt);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (remap);
}

static void
gst_remap_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstRemap *remap;
  GstGeometricTransform *gt;

  gt = GST_GEOMETRIC_TRANSFORM_CAST (object);
  remap = GST_REMAP_CAST (gt);

  switch (prop_id) {
    case PROP_MAPX:
      g_value_set_string (value, remap->map_x);
      fprintf(stdout, "Get %s \n",g_value_get_string (value));
      break;
    case PROP_MAPY:
      g_value_set_string (value, remap->map_y);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}



static gboolean remap_map(GstGeometricTransform *gt, gint x, gint y,
                                    gdouble *in_x, gdouble *in_y) {
#ifndef GST_DISABLE_GST_DEBUG
    GstRemap *remap = GST_REMAP_CAST(gt);
#endif    
    gdouble norm_x;
    gdouble norm_y;
    gdouble r;

    gdouble width = gt->width;
    gdouble height = gt->height;

    gint displacement = ((y - 1) * (int)width + x);
    displacement = CLAMP (displacement, 0, BUF_SZ - 1);        
    if (( gt->width == remap->mat_width ) && ( gt->height == remap->mat_height )){
    *in_x = (gdouble)remap->buf_x[displacement];
    *in_y = (gdouble)remap->buf_y[displacement];
    }
    else { // missing maps
    }
    return TRUE;
}

static void gst_remap_class_init(GstRemapClass *klass) {

    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    GstGeometricTransformClass *gstgt_class;

    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *)klass;
    gstgt_class = (GstGeometricTransformClass *)klass;

    gst_element_class_set_static_metadata(
        gstelement_class, "remap", "Transform/Effect/Video",
        "Remap images",
        "skc <skc1125@gmail.com>");

  gobject_class->set_property = gst_remap_set_property;
  gobject_class->get_property = gst_remap_get_property;

  g_object_class_install_property (gobject_class, PROP_MAPX,
      g_param_spec_string ("mapx", "mapx",
          "Filename of the mapX",
          DEFAULT_MAPX,
          GST_PARAM_CONTROLLABLE | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_MAPY,
      g_param_spec_string ("mapy", "mapy",
          "Filename of the mapY",
          DEFAULT_MAPY,
          GST_PARAM_CONTROLLABLE | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
    gobject_class->finalize = gst_remap_finalize;
    gstgt_class->prepare_func = remap_prepare;
    gstgt_class->map_func = remap_map;

}

static void gst_remap_init(GstRemap *filter) {
    GstGeometricTransform *gt = GST_GEOMETRIC_TRANSFORM(filter);

    filter->map_x = malloc(100);
    filter->map_y = malloc(100);
    strcpy(filter->map_x, DEFAULT_MAPX);
    strcpy(filter->map_y, DEFAULT_MAPY);
    
    gt->off_edge_pixels = GST_GT_OFF_EDGES_PIXELS_CLAMP;
}

gboolean gst_remap_plugin_init(GstPlugin *plugin) {
    GST_DEBUG_CATEGORY_INIT(gst_remap_debug, "remap", 0,
                            "remap");

    return gst_element_register(plugin, "remap", GST_RANK_NONE,
                                GST_TYPE_REMAP);
}

/* Clean up */
static void
gst_remap_finalize (GObject * obj)
{
    fprintf(stdout, "Finalize! \n");
    GstRemap *remap = GST_REMAP_CAST (obj);

    g_free (remap->buf_x);
    g_free (remap->buf_y);

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}


static gboolean
remap_prepare(GstGeometricTransform * trans) {
  GstRemap *remap = GST_REMAP_CAST (trans);
    fprintf(stdout, "mapx: %s mapy: %s\n",remap->map_x, remap->map_y);
    // remap->fptr_x = fopen("EquimatX", "rb");
    // remap->fptr_y = fopen("EquimatY", "rb");

    remap->fptr_x = fopen(remap->map_x, "rb");
    remap->fptr_y = fopen(remap->map_y, "rb");
 
    if ((remap->fptr_x != NULL) && (remap->fptr_y != NULL)) {
        int Buf_Size = BUF_SZ;
        remap->buf_x = malloc(sizeof(float) * Buf_Size);
        remap->buf_y = malloc(sizeof(float) * Buf_Size);
        int rows, cols, type, channels;
        fread(&remap->mat_height, 1, sizeof(int), remap->fptr_x);
        fread(&remap->mat_width, 1, sizeof(int), remap->fptr_x);
        fread(&type, 1, sizeof(int), remap->fptr_x);
        fread(&channels, 1, sizeof(int), remap->fptr_x);
        fread(remap->buf_x, Buf_Size, sizeof(float), remap->fptr_x);
        fread(&rows, 1, sizeof(int), remap->fptr_y);
        fread(&cols, 1, sizeof(int), remap->fptr_y);
        fread(&type, 1, sizeof(int), remap->fptr_y);
        fread(&channels, 1, sizeof(int), remap->fptr_y);
        fread(remap->buf_y, Buf_Size, sizeof(float), remap->fptr_y);
        
        if (( remap->mat_height == rows ) && ( remap->mat_width == cols )){
        fprintf(stdout, "( rows, cols ) = ( %d, %d ) \n", rows, cols);
        fprintf(stdout, "X, Y Mats Loaded! \n");
    } else {
        if (errno != 0)
            fprintf(stderr, "Could not open mat file, for this reason: %s\n!",
                    strerror(errno));
        else
            fprintf(stderr, "Unknown errors.\n");
    }   
    fclose(remap->fptr_x);
    fclose(remap->fptr_y);
    }
    return TRUE;
}
