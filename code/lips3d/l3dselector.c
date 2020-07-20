/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GLOSS3D.  If not, see http://www.gnu.org/licenses/.            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <lips3d/lips3d.h>

#define SNAPDISTANCE 0x40

#define SAMESIDEBIT   0x80
#define LINEBODYBIT   0x40
#define LINETIPBIT    0x20
#define LINEFORCEBIT  0x10
#define LINEBITMASK   0x0F

static int l3dselector_press ( L3DOBJECT     *obj,
                               L3DPATTERN    *pattern,
                               uint32_t       fgcolor,
                               uint32_t       bgcolor,
                               int32_t        x,
                               int32_t        y,
                               unsigned char *buffer, 
                               uint32_t       width, 
                               uint32_t       height,
                               uint32_t       bpp,
                               unsigned char *mask,
                               unsigned char *zbuffer,
                               uint32_t       engineFlags );
static int l3dselector_move ( L3DOBJECT     *obj,
                              L3DPATTERN    *pattern,
                              uint32_t       fgcolor,
                              uint32_t       bgcolor,
                              int32_t        x,
                              int32_t        y,
                              unsigned char *buffer, 
                              uint32_t       width, 
                              uint32_t       height,
                              uint32_t       bpp,
                              unsigned char *mask,
                              unsigned char *zbuffer,
                              int32_t       *updx,
                              int32_t       *updy,
                              int32_t       *updw,
                              int32_t       *updh,
                              uint32_t       engineFlags );
static int l3dselector_release ( L3DOBJECT     *obj,
                                 L3DPATTERN    *pattern,
                                 uint32_t       fgcolor,
                                 uint32_t       bgcolor,
                                 int32_t        x,
                                 int32_t        y,
                                 unsigned char *buffer, 
                                 uint32_t       width, 
                                 uint32_t       height,
                                 uint32_t       bpp,
                                 unsigned char *mask,
                                 unsigned char *zbuffer,
                                 uint32_t       engineFlags );

/******************************************************************************/
static void l3dselectorpoint_free ( L3DSELECTORPOINT *pt ) {
    free ( pt );
}

/******************************************************************************/
L3DSELECTORPOINT* l3dselectorpoint_new ( int32_t  x, 
                                         int32_t  y,
                                         uint32_t width,
                                         uint32_t height ) {
    uint32_t structSize = sizeof ( L3DSELECTORPOINT );
    void *memarea = calloc ( 0x01, structSize );
    L3DSELECTORPOINT *spt = ( L3DSELECTORPOINT * ) memarea;

    if ( spt == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    spt->x = x;
    spt->y = y;
    spt->u = ( width  ) ? ( float ) x / width  : 0.0f;
    spt->v = ( height ) ? ( float ) y / height : 0.0f;

    return spt;
}

/******************************************************************************/
static void l3dselectorline_free ( L3DSELECTORLINE *lin ) {
    free ( lin );
}

/******************************************************************************/
static void line_putpixel ( int32_t        x, 
                            int32_t        y, 
                            uint32_t       width, 
                            uint32_t       height,
                            unsigned char *mask,
                            uint32_t       flags ) {
    uint32_t offset = ( y * width ) + x;
    unsigned char sideb =   mask[offset] & SAMESIDEBIT;
    unsigned char nblin = ( mask[offset] & LINEBITMASK ) + 0x01;

    /*** detect intersections other than at line tips ***/
    if (   ( mask[offset] & LINEBODYBIT ) &&
         ( ( mask[offset] & LINETIPBIT  ) == 0x00 ) ) {
        mask[offset] = 0x00;
    } else { 
        mask[offset] = nblin | sideb | flags;
    }
}

/******************************************************************************/
static void line_trace ( int32_t          x1,
                         int32_t          y1,
                         int32_t          x2,
                         int32_t          y2,
                         uint32_t         width,
                         uint32_t         height,
                         unsigned char   *mask,
                         uint32_t         sameSide ) {
    int32_t dx  = ( x2 - x1 ),
            dy  = ( y2 - y1 );
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x, y;
    int i;

    if ( dy ) {
        float coef = ( float ) dx / dy;

        for ( i = 0x00, y = 0x00; i <= abs ( dy ); i++, y += py ) {
            x = ( y * coef );
            uint32_t flags =   ( y == 0x00  ) ?
                             ( ( sameSide ) ? SAMESIDEBIT : 
                                              LINEBODYBIT ) : LINEBODYBIT;
            if ( y == 0x00 ) flags |= LINETIPBIT;
            if ( y == dy   ) flags |= LINETIPBIT;

            line_putpixel ( x + x1, y + y1, width, height, mask, flags );
        }
    } else {
        for ( i = 0x00, x = 0x00; i <= abs ( dx ); i++, x += px ) {
            line_putpixel ( x + x1, y1, width, height, mask, LINEFORCEBIT );
        }
    }
}

/******************************************************************************/
#ifdef UNUSED
static void line_trace ( int32_t          x1,
                         int32_t          y1,
                         int32_t          x2,
                         int32_t          y2,
                         uint32_t         width,
                         uint32_t         height,
                         unsigned char   *mask,
                         uint32_t         sameSide ) {
    int32_t dx  = ( x2 - x1 ),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ) + 1,
            dy  = ( y2 - y1 ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ) + 1;
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = x1, 
            y = y1;
    int i, cumul = 0x00;
    uint32_t offset;
    uint32_t onex = 0x01;
    unsigned char nblin, side;

    if ( ddx > ddy ) {
        for ( i = 0x00; i < ddx; i++ ) {
            uint32_t flags =   ( x == x1  ) ?
                             ( ( sameSide ) ? SAMESIDEBIT : 
                                              LINEBODYBIT ) : LINEBODYBIT;

            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;

                onex = 0x01;
            }

            if ( onex ) {
                line_putpixel ( x, y, width, height, mask, flags );

                onex = 0x00;
            }

            cumul += ddy;
            x     += px;
        }
    } else {
        for ( i = 0x00; i < ddy; i++ ) {
            uint32_t flags =   ( x == x1  ) ?
                             ( ( sameSide ) ? SAMESIDEBIT : 
                                              LINEBODYBIT ) : LINEBODYBIT;

            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            line_putpixel ( x, y, width, height, mask, flags );

            cumul += ddx;
            y     += py;
        }
    }
}
#endif

/******************************************************************************/
L3DSELECTORLINE* l3dselectorline_new ( L3DSELECTORPOINT *srcpt, 
                                       L3DSELECTORPOINT *dstpt ) {
    uint32_t structSize = sizeof ( L3DSELECTORLINE );
    void *memarea = calloc ( 0x01, structSize );
    L3DSELECTORLINE *lin = ( L3DSELECTORLINE * ) memarea;

    if ( lin == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    lin->srcpt = srcpt;
    lin->dstpt = dstpt;

    return lin;
}

/******************************************************************************/
L3DSELECTOR* l3dselector_new ( ) {
    uint32_t structSize = sizeof ( L3DSELECTOR );
    L3DSELECTOR *sel = ( L3DSELECTOR * ) calloc ( 0x01, structSize );

    if ( sel == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    l3dobject_init ( sel,
                     0.0f,
                     l3dselector_press,
                     l3dselector_move,
                     l3dselector_release );

    return sel;
}

/******************************************************************************/
static void l3dselector_generateMask (  L3DSELECTOR   *sel,
                                        uint32_t       width, 
                                        uint32_t       height,
                                        unsigned char *mask ) {
    uint32_t selWidth  = abs ( sel->xmax - sel->xmin ) + 0x01,
             selHeight = abs ( sel->ymax - sel->ymin ) + 0x01;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    if ( selWidth && selHeight ) {
        uint32_t maskSize = selWidth * selHeight;
        unsigned char *selMask = ( unsigned char * ) calloc ( 0x01, maskSize );
        unsigned char (*selJPG)[0x03]; /*** for debugging purpose ***/
        unsigned char (*selWRF)[0x03]; /*** for debugging purpose ***/
        LIST *ltmplin = sel->llines;
        int32_t mx, my;
        uint32_t i, j;

        memset ( mask, 0x00, width * height );

        if ( sysinfo->debug ) {
            selJPG = ( unsigned char * ) calloc ( 0x03, maskSize );
            selWRF = ( unsigned char * ) calloc ( 0x03, maskSize );
        }

        while ( ltmplin ) {
            L3DSELECTORLINE *pLine = ( ltmplin->prev ) ? 
                                     ( L3DSELECTORLINE * ) ltmplin->prev->data : 
                                                           sel->firstLine;
            L3DSELECTORLINE *cLine = ( L3DSELECTORLINE * ) ltmplin->data;
            uint32_t sameSide = 0x00;

            if ( ( cLine->srcpt->y <= cLine->dstpt->y ) && 
                 ( pLine->dstpt->y <= cLine->dstpt->y ) ) {
                sameSide = 0x01;
            }

            if ( ( cLine->srcpt->y >= cLine->dstpt->y ) && 
                 ( pLine->dstpt->y >= cLine->dstpt->y ) ) {
                sameSide = 0x01;
            }

            /*** Note: we adjust coordinates to the containing square ***/
            line_trace ( pLine->srcpt->x - sel->xmin,
                         pLine->srcpt->y - sel->ymin,
                         pLine->dstpt->x - sel->xmin,
                         pLine->dstpt->y - sel->ymin,
                         selWidth,
                         selHeight,
                         selMask,
                         sameSide );

            ltmplin = ltmplin->next;
        }

        for ( i = 0x00, my = sel->ymin; i < selHeight; i++, my++ ) {
            unsigned char code = 0x00;

            for ( j = 0x00, mx = sel->xmin; j < selWidth; j++, mx++ ) {
                uint32_t offset  = ( i * selWidth ) + j;

                if ( selMask[offset] & LINEFORCEBIT ) {
                    code = 0xFF;
                }

                if ( ( selMask[offset]   & LINEBITMASK ) == 0x01 ) {
                    code = ( code == 0x00 ) ? 0xFF : 0x00;
                }

                if ( ( ( selMask[offset] & LINEBITMASK ) >= 0x02 ) && 
                     ( ( selMask[offset] & SAMESIDEBIT ) == 0x00 ) ) {
                    code = ( code == 0x00 ) ? 0xFF : 0x00;
                }

                if ( sysinfo->debug ) {
                    selWRF[offset][0] = (selMask[offset] & LINEBITMASK) * 100;
                    selWRF[offset][1] = (selMask[offset] & LINEBITMASK) * 100;
                    selWRF[offset][2] = (selMask[offset] & LINEBITMASK) * 100;

                    selJPG[offset][0] = code;
                    selJPG[offset][1] = code;
                    selJPG[offset][2] = code;
                }

                selMask[offset] = code;

                if ( selMask[offset] ) {
                    if ( ( mx > 0x00 ) && ( mx < width  ) &&
                         ( my > 0x00 ) && ( my < height ) ) {
                        uint32_t moffset = ( my * width ) + mx;

                        mask[moffset] = 0xFF;
                    }
                }
            }
        }

        free ( selMask );

        if ( sysinfo->debug ) {
            g3dcore_writeJpeg ( "mask.jpg", selWidth, selHeight, 24, selJPG );
            g3dcore_writeJpeg ( "wire.jpg", selWidth, selHeight, 24, selWRF );

            free ( selJPG );
            free ( selWRF );
        }
    }
}

/******************************************************************************/
static int l3dselector_reset ( L3DSELECTOR *sel ) {
    sel->closed = 0x00;

    sel->firstPoint = sel->lastPoint = NULL;
    sel->firstLine  = sel->lastLine  = NULL;

    list_free ( &sel->llines , l3dselectorline_free  );
    list_free ( &sel->lpoints, l3dselectorpoint_free );

    return 0x00;
}

/******************************************************************************/
static int l3dselector_press ( L3DOBJECT     *obj,
                               L3DPATTERN    *pattern,
                               uint32_t       fgcolor,
                               uint32_t       bgcolor,
                               int32_t        x,
                               int32_t        y,
                               unsigned char *buffer, 
                               uint32_t       width, 
                               uint32_t       height,
                               uint32_t       bpp,
                               unsigned char *mask,
                               unsigned char *zbuffer,
                               uint32_t       engineFlags ) {
    L3DSELECTOR *sel = ( L3DSELECTOR * ) obj;

    if ( sel->closed ) {
        sel->closed = 0x00;

        sel->firstPoint = sel->lastPoint = NULL;

        sel->firstLine = sel->lastLine = NULL;

        list_free ( &sel->llines , l3dselectorline_free  );
        list_free ( &sel->lpoints, l3dselectorpoint_free );
    }

    return 0x00;
}

/******************************************************************************/
static int l3dselector_move ( L3DOBJECT     *obj,
                              L3DPATTERN    *pattern,
                              uint32_t       fgcolor,
                              uint32_t       bgcolor,
                              int32_t        x,
                              int32_t        y,
                              unsigned char *buffer, 
                              uint32_t       width, 
                              uint32_t       height,
                              uint32_t       bpp,
                              unsigned char *mask,
                              unsigned char *zbuffer,
                              int32_t       *updx,
                              int32_t       *updy,
                              int32_t       *updw,
                              int32_t       *updh,
                              uint32_t       engineFlags ) {
    L3DSELECTOR *sel = ( L3DSELECTOR * ) obj;

    if ( sel->closed == 0x00 ) {
        if ( sel->lastPoint ) {
            int32_t distX = sel->firstPoint->x - x,
                    distY = sel->firstPoint->y - y;

            if ( ( ( distX * distX ) + ( distY * distY ) ) < SNAPDISTANCE ) {
                sel->lastPoint->x = sel->firstPoint->x;
                sel->lastPoint->y = sel->firstPoint->y;
                sel->lastPoint->u = sel->firstPoint->u;
                sel->lastPoint->v = sel->firstPoint->v;
            } else {
                sel->lastPoint->x = x;
                sel->lastPoint->y = y;
                sel->lastPoint->u = ( width  ) ? ( float ) x / width  : 0.0f;
                sel->lastPoint->v = ( height ) ? ( float ) y / height : 0.0f;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
#ifdef __linux__
#include <sys/time.h>
static void gettime ( uint64_t *milliseconds ) {
    struct timeval tp;

    gettimeofday ( &tp, NULL );

    (*milliseconds) = ( tp.tv_sec * 1000 ) + ( tp.tv_usec / 1000 );
}
#endif
#ifdef __MINGW32__
#include <windows.h>
static void gettime ( uint64_t *milliseconds ) {
    SYSTEMTIME time;

    GetSystemTime ( &time );

   (*milliseconds) = ( time.wSecond * 1000 ) + time.wMilliseconds;
}
#endif

/******************************************************************************/
static void l3dselector_findMinMax ( L3DSELECTOR *sel ) {
    LIST *ltmppt = sel->lpoints;
    uint32_t init = 0x00;

    while ( ltmppt ) {
        L3DSELECTORPOINT *pt = ( L3DSELECTORPOINT * ) ltmppt->data;

        if ( init == 0x00 ) {
            sel->xmax = sel->xmin = pt->x;
            sel->ymax = sel->ymin = pt->y;

            init = 0x01;
        } else {
            if ( pt->x > sel->xmax ) sel->xmax = pt->x;
            if ( pt->x < sel->xmin ) sel->xmin = pt->x;
            if ( pt->y > sel->ymax ) sel->ymax = pt->y;
            if ( pt->y < sel->ymin ) sel->ymin = pt->y;
        }

        ltmppt = ltmppt->next;
    }
}

/******************************************************************************/
static int l3dselector_release ( L3DOBJECT     *obj,
                                 L3DPATTERN    *pattern,
                                 uint32_t       fgcolor,
                                 uint32_t       bgcolor,
                                 int32_t        x,
                                 int32_t        y,
                                 unsigned char *buffer, 
                                 uint32_t       width, 
                                 uint32_t       height,
                                 uint32_t       bpp,
                                 unsigned char *mask,
                                 unsigned char *zbuffer,
                                 uint32_t       engineFlags ) {
    L3DSELECTOR *sel = ( L3DSELECTOR * ) obj;
    static uint64_t oldmilliseconds;
    static int32_t oldx, oldy;
    uint32_t doubleClicked = 0x00;

    /*** detect double click ***/
    if ( ( x == oldx ) &&
         ( y == oldy ) ) {
        uint64_t milliseconds;

        gettime ( &milliseconds );

        if ( ( milliseconds - oldmilliseconds ) < 250 ) {
            doubleClicked = 0x01;
        }
    }

    if ( doubleClicked ) {
        if ( sel->lastLine ) {
            sel->lastLine->dstpt = sel->firstPoint;

            sel->closed = 0x01;
        }
    } else {
        if ( sel->closed == 0x00 ) {
            L3DSELECTORPOINT *pt;
            L3DSELECTORLINE *lin;

            if ( sel->firstPoint == NULL ) {
                sel->firstPoint = l3dselectorpoint_new ( x, y, width, height );

                list_insert ( &sel->lpoints, sel->firstPoint );

                pt = l3dselectorpoint_new ( x, y, width, height );

                list_insert ( &sel->lpoints, pt );

                sel->firstLine = l3dselectorline_new ( sel->firstPoint, pt );
                sel->lastLine  = sel->firstLine;

                list_insert ( &sel->llines, sel->lastLine );

                sel->lastPoint = pt;

                sel->closed = 0x00;
            } else {
                int32_t distX = sel->firstPoint->x - x,
                        distY = sel->firstPoint->y - y;

                if ( ( ( distX * distX ) + 
                       ( distY * distY ) ) < SNAPDISTANCE ) {
                    list_remove ( &sel->lpoints, sel->lastLine->dstpt );

                    l3dselectorpoint_free ( sel->lastLine->dstpt );

                    sel->lastLine->dstpt = sel->firstPoint;

                    sel->closed = 0x01;
                } else {
                    pt = l3dselectorpoint_new ( x, y, width, height );

                    list_insert ( &sel->lpoints, pt );

                    sel->lastLine = l3dselectorline_new ( sel->lastPoint, pt );

                    list_insert ( &sel->llines, sel->lastLine );

                    sel->lastPoint = pt;

                    sel->closed = 0x00;
                }
            }
        }
    }

    if ( sel->closed ) {
        /*** if no area was defined, then the whole mask is set for writing ***/
        if ( list_count ( sel->llines ) <= 0x02 ) {
            memset ( mask, 0xFF, width * height );

            l3dselector_reset ( sel );
        } else {
            l3dselector_findMinMax ( sel );
            l3dselector_generateMask ( sel, width, height, mask );
        }
    }

    oldx = x;
    oldy = y;
    gettime ( &oldmilliseconds );

    return 0x00;
}
