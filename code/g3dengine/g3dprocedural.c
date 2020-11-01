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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dprocedural_init ( G3DPROCEDURAL *proc, 
                          uint32_t       type,
                          void         (*func)( G3DPROCEDURAL *, 
                                                double, 
                                                double, 
                                                double, 
                                                G3DCOLOR * ) ) {
    proc->type       = type;
    proc->getColor   = func;

    glGenTextures ( 0x01, &proc->image.id );
}

/******************************************************************************/
void g3dprocedural_fill ( G3DPROCEDURAL *proc, uint32_t resx,
                                               uint32_t resy,
                                               uint32_t bpp,
                                               uint32_t bindGL ) {
    uint32_t i, j;

    proc->image.width  = resx;
    proc->image.height = resy;
    proc->image.bytesPerPixel = bpp >> 0x03;
    proc->image.bytesPerLine = proc->image.bytesPerPixel * proc->image.width;
    proc->image.wratio = 1;
    proc->image.hratio = 1;
    proc->image.data   = realloc ( proc->image.data, resx * 
                                                     resy * ( bpp >> 0x03 ) );

    for ( j = 0x00; j < proc->image.height; j++ ) {
        for ( i = 0x00; i < proc->image.width; i++ ) {
            uint32_t offset = ( j * proc->image.width ) + i;
            G3DCOLOR color;
            G3DRGBA rgba;

            proc->getColor ( proc, (double) i / proc->image.width, 
                                   (double) j / proc->image.height,
                                   (double) 0, &color );

            g3dcolor_toRGBA ( &color, &rgba );

            switch ( bpp ) {
                case 0x18 : {
                    unsigned char (*data)[0x03] = (unsigned char (*)[3])proc->image.data;

                    data[offset][0x00] = rgba.r;
                    data[offset][0x01] = rgba.g;
                    data[offset][0x02] = rgba.b;
                } break;

                case 0x08 : {
                    unsigned char (*data)[0x01] = (unsigned char (*)[1])proc->image.data;
                    uint32_t col = ( rgba.r + rgba.g + rgba.b ) / 0x03;

                    data[offset][0x00] = col;
                } break;

                default:
                break;
            }
        }
    }

    if ( bindGL ) g3dimage_bind ( &proc->image );
}