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
void g3dinstance_setReference ( G3DINSTANCE *ins, 
                                G3DOBJECT   *ref ) {
    ins->ref = ref;
}

/******************************************************************************/
void g3dinstance_setOrientation ( G3DINSTANCE *ins, 
                                  uint32_t     orientation ) {
    ins->orientation = orientation;
}

/******************************************************************************/
void g3dinstance_setMirrored ( G3DINSTANCE *ins ) {
    ((G3DOBJECT*)ins)->flags |= INSTANCEMIRRORED;
}

/******************************************************************************/
void g3dinstance_unsetMirrored ( G3DINSTANCE *ins ) {
    ((G3DOBJECT*)ins)->flags &= (~INSTANCEMIRRORED);
}

/******************************************************************************/
static G3DINSTANCE *g3dinstance_copy ( G3DINSTANCE   *ins, 
                                       uint32_t       id, 
                                       unsigned char *name,
                                       uint64_t       engine_flags ) {
    G3DINSTANCE *cpyins = g3dinstance_new ( ((G3DOBJECT*)ins)->id,
                                            ((G3DOBJECT*)ins)->name );

    cpyins->ref = ins->ref;
    cpyins->orientation = ins->orientation;


    return cpyins;
}

/******************************************************************************/
static uint32_t g3dinstance_draw ( G3DINSTANCE *ins, 
                                   G3DCAMERA   *curcam, 
                                   uint64_t     engine_flags ) {

    if ( ins->ref ) {
        if ( ins->ref->draw ) {
            ins->ref->draw ( ins->ref, curcam, ( engine_flags & (~MODEMASK) ) | VIEWOBJECT );
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dinstance_free ( G3DINSTANCE *ins ) {

}

/******************************************************************************/
static uint32_t g3dinstance_pick ( G3DINSTANCE *ins, 
                                   G3DCAMERA   *curcam, 
                                   uint64_t     engine_flags ) {

    if ( ins->ref ) {
        if ( ins->ref->pick ) {
            ins->ref->pick ( ins->ref, 
                             curcam, 
                             engine_flags & (~MODEMASK) | VIEWOBJECT );
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dinstance_transform ( G3DINSTANCE *ins,
                                    uint64_t     engine_flags ) {

}

/******************************************************************************/
void g3dinstance_init ( G3DINSTANCE *ins, 
                        uint32_t     id, 
                        char        *name ) {
    g3dobject_init ( ins, 
                     G3DINSTANCETYPE,
                     id, 
                     name, 
                     DRAWBEFORECHILDREN,
       DRAW_CALLBACK(g3dinstance_draw),
       FREE_CALLBACK(g3dinstance_free),
       PICK_CALLBACK(g3dinstance_pick),
                     NULL,
       COPY_CALLBACK(g3dinstance_copy),
                     NULL,
                     NULL,
                     NULL,
   ADDCHILD_CALLBACK(NULL),
                     NULL );

    ((G3DOBJECT*)ins)->transform = g3dinstance_transform;

    ins->orientation = INSTANCEYZ;

    /*mes->dump           = g3dmesh_default_dump;*/
}

/******************************************************************************/
G3DINSTANCE *g3dinstance_new ( uint32_t id, 
                               char    *name ) {
    G3DINSTANCE *ins = ( G3DINSTANCE * ) calloc ( 0x01, sizeof ( G3DINSTANCE ) );

    if ( ins == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dinstance_init ( ins, id, name );


    return ins;
}
