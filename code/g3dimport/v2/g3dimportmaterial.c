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
#include <g3dimportv2.h>

/******************************************************************************/
void g3dimportmaterial ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_MATERIAL_NAME : {
                char name[0x100] = { 0 };

                g3dimport_fread ( name, chunkSize, 0x01, fsrc );

                gid->currentMaterial = g3dmaterial_new ( name );

                /*** ID should be part of the above function args ***/
                gid->currentMaterial->id = gid->currentMaterialID++;

                g3dscene_addMaterial ( gid->currentScene,
                                       gid->currentMaterial );
            } break;

            case SIG_MATERIAL_DIFFUSE : {
                gid->currentChannel = &gid->currentMaterial->diffuse;
            } break;

            case SIG_MATERIAL_SPECULAR : {
                gid->currentChannel = &gid->currentMaterial->specular;
            } break;

            case SIG_MATERIAL_SPECULAR_SHININESS : {
                 g3dimport_freadf ( &gid->currentMaterial->shininess, fsrc );
            } break;

            case SIG_MATERIAL_SPECULAR_LEVEL : {
                 g3dimport_freadf ( &gid->currentMaterial->specular_level, fsrc );
            } break;

            case SIG_MATERIAL_REFLECTION : {
                gid->currentChannel = &gid->currentMaterial->reflection;
            } break;

            case SIG_MATERIAL_REFRACTION : {
                gid->currentChannel = &gid->currentMaterial->refraction;
            } break;

            case SIG_MATERIAL_BUMP : {
                gid->currentChannel = &gid->currentMaterial->bump;
            } break;

            case SIG_MATERIAL_BUMP_STRENGTH : {
                 g3dimport_freadf ( &gid->currentMaterial->bump_strength, fsrc );
            } break;

            case SIG_MATERIAL_DISPLACEMENT : {
                gid->currentChannel = &gid->currentMaterial->displacement;
            } break;

            case SIG_MATERIAL_DISPLACEMENT_STRENGTH : {
                 g3dimport_freadf ( &gid->currentMaterial->displacement_strength, fsrc );
            } break;

            case SIG_MATERIAL_ALPHA : {
                gid->currentChannel = &gid->currentMaterial->alpha;
            } break;

            case SIG_CHANNEL : {
                if ( chunkSize ) {
                    g3dimportchannel ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
}
