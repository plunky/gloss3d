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
#include <g3dexportv3.h>

/******************************************************************************/
static uint32_t g3dexportv3material_alphaOpacity ( G3DEXPORTV3DATA *ged, 
                                                   G3DMATERIAL   *mat, 
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** displacement strength is stored as solid color ***/
    size += g3dexportv3_fwritef ( &mat->alphaOpacity, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_alpha ( G3DEXPORTV3DATA *ged, 
                                         G3DMATERIAL   *mat, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_ALPHA_OPACITY,
                   EXPORTV3_CALLBACK(g3dexportv3material_alphaOpacity),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->alpha,
                                     EXPORTV3CHANNEL_IMAGECOLOR |
                                     EXPORTV3CHANNEL_PROCEDURAL,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_displacementStrength ( G3DEXPORTV3DATA *ged, 
                                                         G3DMATERIAL   *mat, 
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** displacement strength is stored as solid color ***/
    size += g3dexportv3_fwritef ( &mat->displacement.solid.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_displacement ( G3DEXPORTV3DATA *ged, 
                                               G3DMATERIAL   *mat, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_DISPLACEMENT_STRENGTH,
                   EXPORTV3_CALLBACK(g3dexportv3material_displacementStrength),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->displacement,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_bumpStrength ( G3DEXPORTV3DATA *ged, 
                                                 G3DMATERIAL   *mat, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** bump strength is stored as solid color ***/
    size += g3dexportv3_fwritef ( &mat->bump.solid.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_bump ( G3DEXPORTV3DATA *ged, 
                                        G3DMATERIAL   *mat, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_BUMP_STRENGTH,
                   EXPORTV3_CALLBACK(g3dexportv3material_bumpStrength),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->bump,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_refraction ( G3DEXPORTV3DATA *ged, 
                                              G3DMATERIAL   *mat, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->refraction,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_reflection ( G3DEXPORTV3DATA *ged, 
                                              G3DMATERIAL   *mat, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->reflection,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_specularLevel ( G3DEXPORTV3DATA *ged, 
                                                  G3DMATERIAL   *mat, 
                                                  uint32_t       flags, 
                                                  FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &mat->specular_level, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_specularShininess ( G3DEXPORTV3DATA *ged, 
                                                      G3DMATERIAL   *mat, 
                                                      uint32_t       flags, 
                                                      FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &mat->shininess, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_specular ( G3DEXPORTV3DATA *ged, 
                                            G3DMATERIAL   *mat, 
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_SPECULAR_SHININESS,
                   EXPORTV3_CALLBACK(g3dexportv3material_specularShininess),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_SPECULAR_LEVEL,
                   EXPORTV3_CALLBACK(g3dexportv3material_specularLevel),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->specular,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_diffuse ( G3DEXPORTV3DATA *ged, 
                                            G3DMATERIAL   *mat, 
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL,
                   EXPORTV3_CALLBACK(g3dexportv3channel),
                                     ged,
                                    &mat->diffuse,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_name ( G3DEXPORTV3DATA *ged, 
                                         G3DMATERIAL   *mat, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( mat->name, strlen ( mat->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3material_flags ( G3DEXPORTV3DATA *ged, 
                                          G3DMATERIAL   *mat, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mat->flags, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3material  ( G3DEXPORTV3DATA *ged, 
                              G3DMATERIAL   *mat, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_NAME,
                   EXPORTV3_CALLBACK(g3dexportv3material_name),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_MATERIAL_FLAGS,
                   EXPORTV3_CALLBACK(g3dexportv3material_flags),
                                     ged,
                                     mat,
                                     0xFFFFFFFF,
                                     fdst );

    if ( mat->flags & DIFFUSE_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_DIFFUSE,
                       EXPORTV3_CALLBACK(g3dexportv3material_diffuse),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mat->flags & SPECULAR_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_SPECULAR,
                       EXPORTV3_CALLBACK(g3dexportv3material_specular),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mat->flags & REFLECTION_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_REFLECTION,
                       EXPORTV3_CALLBACK(g3dexportv3channel_reflection),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mat->flags & REFRACTION_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_REFRACTION,
                       EXPORTV3_CALLBACK(g3dexportv3channel_refraction),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mat->flags & BUMP_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_BUMP,
                       EXPORTV3_CALLBACK(g3dexportv3channel_bump),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mat->flags & DISPLACEMENT_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_DISPLACEMENT,
                       EXPORTV3_CALLBACK(g3dexportv3channel_displacement),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mat->flags & ALPHA_ENABLED ) {
        size += g3dexportv3_writeChunk ( SIG_MATERIAL_ALPHA,
                       EXPORTV3_CALLBACK(g3dexportv3channel_alpha),
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}
