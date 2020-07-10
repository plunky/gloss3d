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

/******************************************************************************/
int l3dpattern_paint ( L3DPATTERN    *pattern,
                       uint32_t       color,
                       float          pressure,
                       int32_t        x,
                       int32_t        y,
                       unsigned char *buffer,
                       uint32_t       width,
                       uint32_t       height,
                       uint32_t       bpp,
                       unsigned char *mask,
                       unsigned char *zbuffer,
                       uint32_t       engineFlags ) {
    int32_t x1, x2, y1, y2, xm, ym, xp, yp;
    uint8_t A = ( color & 0xFF000000 ) >> 24;
    uint8_t R = ( color & 0x00FF0000 ) >> 16;
    uint8_t G = ( color & 0x0000FF00 ) >>  8;
    uint8_t B = ( color & 0x000000FF ) >>  0;

    if ( pattern->size == 0x01 ) {
        l3dcore_paintPoint( pattern,
                            color,
                            pressure,
                            x,
                            y,
                            buffer,
                            width,
                            height, 
                            bpp, 
                            mask,
                            zbuffer,
                            engineFlags );
    } else {
        int32_t halfSize = pattern->size / 0x02;

        if ( ( pattern->size % 0x02 ) == 0x01 ) { /*** uneven size ***/
            x1 = x - halfSize;
            y1 = y - halfSize;
            x2 = x + halfSize;
            y2 = y + halfSize;

        } else { /*** even size ***/
            x1 = x - halfSize + 1;
            y1 = y - halfSize + 1;
            x2 = x + halfSize;
            y2 = y + halfSize;
        }

        for ( ym = y1, yp = 0x00; ym <= y2; ym++, yp++ ) {
            for ( xm = x1, xp = 0x00; xm <= x2; xm++, xp++ ) {

                if ( ( xm > 0x00 ) && ( xm < width  ) &&
                     ( ym > 0x00 ) && ( ym < height ) ) {
                    /*** image buffer offset ***/
                    uint32_t boffset = ( ym * width         ) + xm;
                    /*** pattern offset ***/
                    uint32_t poffset = ( yp * pattern->size ) + xp;

                    if ( zbuffer[boffset] < pattern->buffer[poffset] ) {
                        unsigned char diff = pattern->buffer[poffset] - zbuffer[boffset];

                        if ( pattern->buffer[poffset] > 0x00 ) {
                            unsigned char pval = /*pattern->buffer[poffset]*/diff;
                            float patternPressure = ( float ) pval / 255.0f;
                            /*** Note: opacity and pressure are the same thing. ***/
                            /*** I just needed different names. ***/
                            float P = pressure * patternPressure;
                            float invP = 1.0f - P;

                            switch ( bpp ) {
                                case 0x20 :
                                break;

                                case 0x18 : {
                                    unsigned char (*b24)[0x03] = buffer;
                                    unsigned char BR = b24[boffset][0x00],
                                                  BG = b24[boffset][0x01],
                                                  BB = b24[boffset][0x02];

                                    b24[boffset][0x00] = ( R * P ) + ( BR * invP );
                                    b24[boffset][0x01] = ( G * P ) + ( BG * invP );
                                    b24[boffset][0x02] = ( B * P ) + ( BB * invP );
                                } break;

                                default :
                                    fprintf ( stderr, "Unsupported depth\n");
                                break;
                            }

                            zbuffer[boffset] = pattern->buffer[poffset];
                        }
                    }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void l3dpattern_generatePlainRectangle ( L3DPATTERN *pattern ) {
    memset ( pattern->buffer, 0xFF, pattern->size * pattern->size );
}

/******************************************************************************/
void l3dpattern_generatePlainCircle ( L3DPATTERN *pattern ) {
    uint32_t radius = pattern->size / 0x02;
    uint32_t xp = radius, yp = radius;
    uint32_t x, y, rdiff = ( radius * radius );
    uint32_t x1 = xp - radius, x2 = xp + radius;
    uint32_t y1 = yp - radius, y2 = yp + radius;

    for ( y = y1; y <= y2; y++ ) {
        int32_t ydiff = ( y - yp )  * ( y - yp );

        for ( x = x1; x <= x2; x++ ) {
            int32_t xdiff = ( x - xp ) * ( x - xp );

            uint32_t offset = ( y * pattern->size ) + x;

            if ( ( xdiff + ydiff ) <= rdiff ) {
                pattern->buffer[offset] = 0xFF;
            }
        }
    }
}

/******************************************************************************/
void l3dpattern_generateFadedCircle ( L3DPATTERN *pattern ) {
    uint32_t radius = pattern->size / 0x02;
    uint32_t xp = radius, yp = radius;
    uint32_t x, y, rdiff = ( radius * radius );
    uint32_t x1 = 0x00, x2 = pattern->size;
    uint32_t y1 = 0x00, y2 = pattern->size;

    for ( y = 0x00; y < pattern->size; y++ ) {
        int32_t ydiff = ( y - yp )  * ( y - yp );

        for ( x = 0x00; x < pattern->size; x++ ) {
            int32_t xdiff = ( x - xp ) * ( x - xp );

            uint32_t offset = ( y * pattern->size ) + x;

            if ( ( xdiff + ydiff ) <= rdiff ) {
                float fade = 1.0f - ( sqrt ( xdiff + ydiff ) / sqrt ( rdiff ) );

                pattern->buffer[offset] = 0xFF * fade;
            }
        }
    }
}

/******************************************************************************/
void l3dpattern_resize ( L3DPATTERN *pattern, uint32_t size ) {
    if ( size ) {
        pattern->size = size;

        pattern->buffer = realloc ( pattern->buffer, pattern->size * 
                                                     pattern->size );

        memset ( pattern->buffer, 0x00, pattern->size * pattern->size );

        pattern->generate ( pattern );
    }
}

/******************************************************************************/
void l3dpattern_init ( L3DPATTERN *pattern,
                       uint32_t    size,
                       void(*generate)(L3DPATTERN*)) {
    pattern->generate = generate;

    l3dpattern_resize ( ( L3DPATTERN * ) pattern, size );
}

/******************************************************************************/
L3DPLAINRECTANGLEPATTERN *l3dplainrectanglepattern_new ( uint32_t size ) {
    uint32_t structSize = sizeof ( L3DPLAINRECTANGLEPATTERN );
    L3DPLAINRECTANGLEPATTERN *prp;

    prp = ( L3DPLAINRECTANGLEPATTERN * ) calloc ( 0x01, structSize );

    if ( prp == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    l3dpattern_init ( ( L3DPATTERN * ) prp, 
                                       size, 
                                       l3dpattern_generateFadedCircle );


    return prp;
}
