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
void g3dchannel_getNormal ( G3DCHANNEL *cha, 
                            float       u,
                            float       v,
                            G3DVECTOR  *nor,
                            uint32_t    repeat ) {
    uint32_t channelFlags = cha->flags;

    g3dvector_init ( nor, 0.0f, 0.0f, 1.0f, 1.0f );

    if ( repeat == 0x00 ) {
        if ( ( u < 0.0f ) || ( u > 1.0f ) ||
             ( v < 0.0f ) || ( v > 1.0f ) ) {
            channelFlags = USESOLIDCOLOR;
        }
    }

    if ( channelFlags & USEIMAGECOLOR ) {
        G3DIMAGE *colimg = cha->image;
        if ( colimg && colimg->width && colimg->height ) {
            int32_t imgx = ((int32_t)((float)u * colimg->width  )) % colimg->width;
            int32_t imgy = ((int32_t)((float)v * colimg->height )) % colimg->height;

            if ( imgx < 0x00 ) imgx = colimg->width  - imgx;
            if ( imgy < 0x00 ) imgy = colimg->height - imgy;

            g3dimage_getNormal ( colimg, imgx, imgy, nor );
        }
    }

    if ( channelFlags & USEPROCEDURAL ) {
        G3DPROCEDURAL *proc = cha->proc;
        G3DCOLOR color;

        if ( proc ) {
            g3dprocedural_getNormal ( proc, u, v, nor, 0.005f );
        }
    }
}

/******************************************************************************/
G3DPROCEDURAL *g3dchannel_setProcedural ( G3DCHANNEL    *cha, 
                                          G3DPROCEDURAL *proc ) {
    G3DPROCEDURAL *previous = cha->proc;

    cha->proc = proc;

    return previous;
}

/******************************************************************************/
void g3dchannel_enableProcedural ( G3DCHANNEL *cha ) {
        cha->flags &= (~USECHANNELMASK); /*** clear flags first ***/

        cha->flags |= USEPROCEDURAL;
}

/******************************************************************************/
void g3dchannel_enableImageColor ( G3DCHANNEL *cha ) {
        cha->flags &= (~USECHANNELMASK); /*** clear flags first ***/

        cha->flags |= USEIMAGECOLOR;
}

/******************************************************************************/
void g3dchannel_enableSolidColor ( G3DCHANNEL *cha ) {
        cha->flags &= (~USECHANNELMASK); /*** clear flags first ***/

        cha->flags |= USESOLIDCOLOR;
}

/******************************************************************************/
void g3dchannel_getColor ( G3DCHANNEL *cha, float    u,
                                            float    v,
                                            G3DRGBA *rgba,
                                            uint32_t repeat ) {
    uint32_t channelFlags = cha->flags;

    if ( repeat == 0x00 ) {
        if ( ( u < 0.0f ) || ( u > 1.0f ) ||
             ( v < 0.0f ) || ( v > 1.0f ) ) {
            channelFlags = USESOLIDCOLOR;
        }
    }

    if ( channelFlags & USESOLIDCOLOR ) {
        g3dcolor_toRGBA ( &cha->solid, rgba );
    }

    if ( channelFlags & USEIMAGECOLOR ) {
        G3DIMAGE *colimg = cha->image;
        if ( colimg && colimg->width && colimg->height ) {
            int32_t imgx = ((int32_t)((float)u * colimg->width  )) % colimg->width;
            int32_t imgy = ((int32_t)((float)v * colimg->height )) % colimg->height;

            if ( imgx < 0x00 ) imgx = colimg->width  - imgx;
            if ( imgy < 0x00 ) imgy = colimg->height - imgy;

            uint32_t offset = ( imgy * colimg->bytesPerLine  ) +
                              ( imgx * colimg->bytesPerPixel );

            rgba->r = colimg->data[offset+0x00];
            rgba->g = colimg->data[offset+0x01];
            rgba->b = colimg->data[offset+0x02];
        } else {
            g3dcolor_toRGBA ( &cha->solid, rgba );
        }
    }

    if ( channelFlags & USEPROCEDURAL ) {
        G3DPROCEDURAL *proc = cha->proc;
        G3DCOLOR color;

        if ( proc ) {
            proc->getColor ( proc, u, v, 0.0f, &color );

            g3dcolor_toRGBA ( &color, rgba );
        } else {
            g3dcolor_toRGBA ( &cha->solid, rgba );
        }
    }
}

/******************************************************************************/
void g3dchannel_getBumpVector ( G3DCHANNEL *cha,
                                G3DVECTOR  *ray,
                                float       u,
                                float       v,
                                G3DVECTOR  *vout ) {
    G3DIMAGE *image = NULL;

    if ( cha->flags & USEIMAGECOLOR ) {
        image = cha->image;
    }

    if ( cha->flags & USEPROCEDURAL ) {
        if ( cha->proc ) {
            image = &cha->proc->image;
        }
    }

    if ( image ) {
        if ( image->width && image->height ) {
            int32_t imgx = ((int32_t)((float)u * image->width  )) % image->width;
            int32_t imgy = ((int32_t)((float)v * image->height )) % image->height;
            int32_t imgxn = ( imgx + 0x01 ) % image->width;
            int32_t imgyn = ( imgy + 0x01 ) % image->height;

            if ( imgx  < 0x00 ) imgx  = image->width  - imgx;
            if ( imgy  < 0x00 ) imgy  = image->height - imgy;
            if ( imgxn < 0x00 ) imgxn = image->width  - imgxn;
            if ( imgyn < 0x00 ) imgyn = image->height - imgyn;

            uint32_t offset = ( imgy * image->bytesPerLine  ) +
                              ( imgx * image->bytesPerPixel );

            uint32_t offsetxn = ( imgy  * image->bytesPerLine  ) +
                                ( imgxn * image->bytesPerPixel );

            uint32_t offsetyn = ( imgyn * image->bytesPerLine  ) +
                                ( imgx  * image->bytesPerPixel );

            uint32_t col = ( ( uint32_t ) image->data[offset+0x00] + 
                                          image->data[offset+0x01] + 
                                          image->data[offset+0x02] ) / 0x03;

            uint32_t colxn = ( ( uint32_t ) image->data[offsetxn+0x00] + 
                                            image->data[offsetxn+0x01] + 
                                            image->data[offsetxn+0x02] ) / 0x03;

            uint32_t colyn = ( ( uint32_t ) image->data[offsetyn+0x00] + 
                                            image->data[offsetyn+0x01] + 
                                            image->data[offsetyn+0x02] ) / 0x03;

            G3DVECTOR tvec = { 1, 0, (float) colxn - col, 1.0f };
            G3DVECTOR bvec = { 0, 1, (float) colyn - col, 1.0f };
            G3DVECTOR nvec;

            g3dvector_cross ( &tvec, &bvec, &nvec );

            double    TBN[0x10] = { tvec.x, bvec.x, nvec.x, 0.0f,
                                    tvec.y, bvec.y, nvec.y, 0.0f,  
                                    tvec.z, bvec.z, nvec.z, 0.0f,  
                                    0.0f  , 0.0f  , 0.0f  , 1.0f };
            G3DVECTOR rout;

            g3dvector_matrix ( ray, TBN, &rout );

            vout->x = rout.x * nvec.x;
            vout->y = rout.y * nvec.y;
            vout->z = rout.z * nvec.z;

            g3dvector_normalize ( vout, NULL );
        }
    }
}
