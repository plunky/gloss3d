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
typedef struct _VERTEXPOSEEXTENSION {
    G3DVERTEXEXTENSION ext;
    uint32_t           verID;
} VERTEXPOSEEXTENSION;

/******************************************************************************/
static VERTEXPOSEEXTENSION *vertexposeextension_new ( uint32_t extensionName,
                                                      uint32_t verID ) {
    uint32_t size = sizeof ( VERTEXPOSEEXTENSION );
    VERTEXPOSEEXTENSION *vpx = ( VERTEXPOSEEXTENSION * ) calloc ( 0x01, 
                                                                  size );

    if ( vpx == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    g3dvertexextension_init ( ( G3DVERTEXEXTENSION * ) vpx, extensionName );

    vpx->verID  = verID;

    return vpx;
}

/******************************************************************************/
static void vertexposeextension_free ( VERTEXPOSEEXTENSION *vpx ) {
    free ( vpx );
}

/******************************************************************************/
uint32_t g3dmorphermeshpose_getVertexPoseCount ( G3DMORPHERMESHPOSE *mpose ) {
    uint32_t total = 0x00;
    uint32_t i;

    for ( i = 0x00; i < mpose->nbver; i++ ) {
        if ( mpose->vpose[i].enabled ) total++;
    }

    return total;
}

/******************************************************************************/
static void g3dmorphermeshpose_realloc ( G3DMORPHERMESHPOSE *mpose,
                                         uint32_t            nbver ) {
    uint32_t nbnewver = nbver - mpose->nbver;

    mpose->vpose = realloc ( mpose->vpose, 
                             sizeof ( G3DMORPHERVERTEXPOSE ) * nbver );

    memset ( mpose->vpose + mpose->nbver, 
             0x00, 
             sizeof ( G3DMORPHERVERTEXPOSE ) * nbnewver );

    mpose->nbver = nbver;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorphermeshpose_new ( uint32_t nbver, 
                                             char    *name ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMORPHERMESHPOSE ) );
    G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) memarea;

    if ( mpose == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }
 
    mpose->name  = strdup ( name );

    if ( nbver ) {
        g3dmorphermeshpose_realloc ( mpose, nbver );
    }


    return mpose;
}

/******************************************************************************/
static void g3dmorphermeshpose_free ( G3DMORPHERMESHPOSE *mpose ) {
    if ( mpose->vpose ) free ( mpose->vpose );

    free ( mpose );
}

/******************************************************************************/
static void g3dmorpher_removeVertex ( G3DMORPHER *mpr,
                                      G3DVERTEX  *ver ) {
    /*** Note: the vertex keeps its extension because the verID attached ***/
    /*** to it must be kept, as the array of poses is never shrunk ***/
    /*** This eases undo / redo ***/

    list_remove ( &mpr->lver, ver );

    mpr->nbver--;
}

/******************************************************************************/
void g3dmorpher_addVertex ( G3DMORPHER *mpr,
                            G3DVERTEX  *ver ) {
    LIST *ltmpmpose = mpr->lmpose;

    list_insert ( &mpr->lver, ver );

    mpr->nbver++;

    /*** note: each vertex has an allocated slot in all poses, even if ***/
    /*** unused. We then need to reallocate all poses ***/
    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        g3dmorphermeshpose_realloc ( mpose, mpr->verID );

        ltmpmpose = ltmpmpose->next;
    }
}

/******************************************************************************/
uint64_t g3dmorpher_getAvailableSlot ( G3DMORPHER *mpr ) {
    uint32_t i;

    for ( i = 0x00; i < 64; i++ ) {
        uint64_t slotBit = mpr->meshPoseSlots & ( 1 << i );

        if ( slotBit == 0x00 ) {
            return ( uint64_t ) 1 << i;
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dmorpher_removeMeshPose ( G3DMORPHER         *mpr, 
                                 G3DMORPHERMESHPOSE *mpose ) {
    mpr->meshPoseSlots &= (~mpose->slotBit);

    list_remove ( &mpr->lmpose, mpose );

    mpose->slotBit = 0x00;
}

/******************************************************************************/
void g3dmorpher_addMeshPose ( G3DMORPHER         *mpr, 
                              G3DMORPHERMESHPOSE *mpose,
                              uint64_t            slotBit ) {
    mpose->slotBit = slotBit;

    list_insert ( &mpr->lmpose, mpose );

    mpr->meshPoseSlots |= mpose->slotBit;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_createMeshPose ( G3DMORPHER *mpr, 
                                                char       *name ) {
    uint64_t slotBit = g3dmorpher_getAvailableSlot ( mpr );

    if ( slotBit ) {
        G3DMORPHERMESHPOSE *mpose = g3dmorphermeshpose_new ( mpr->verID, name );

        g3dmorpher_addMeshPose ( mpr, mpose, slotBit );

        return  mpose;
    }

    return NULL;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_getMeshPoseByRank ( G3DMORPHER *mpr,
                                                   uint32_t    rank ) {
    LIST *ltmpmpose = mpr->lmpose;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( (rank--) == 0x00 ) {
            return mpose;
        }

        ltmpmpose = ltmpmpose->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dmorpher_selectMeshPoseByRank ( G3DMORPHER *mpr,
                                       uint32_t    rank ) {
    G3DMORPHERMESHPOSE *mpose = g3dmorpher_getMeshPoseByRank ( mpr, rank );

    if ( mpose ) {
        mpr->selmpose = mpose;
    }
}

/******************************************************************************/
void g3dmorpher_selectMeshPose ( G3DMORPHER         *mpr,
                                 G3DMORPHERMESHPOSE *mpose ) {
    mpr->selmpose = mpose;
}

/******************************************************************************/
G3DMORPHERVERTEXPOSE *g3dmorpher_getVertexPose ( G3DMORPHER         *mpr,
                                                 G3DVERTEX          *ver,
                                                 G3DMORPHERMESHPOSE *mpose ) {
    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                            mpr->extensionName );

        if ( vxt ) {
            if ( mpose->vpose[vxt->verID].enabled ) {
                return &mpose->vpose[vxt->verID];
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void g3dmorpher_addVertexPose ( G3DMORPHER         *mpr,
                                G3DVERTEX          *ver,
                                G3DVECTOR          *vpos ) {
    VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                        mpr->extensionName );

    if ( vxt == NULL ) {
        vxt = vertexposeextension_new ( mpr->extensionName, mpr->verID++ );

        g3dvertex_addExtension ( ver, vxt );

        g3dmorpher_addVertex ( mpr, ver );
    }

    if ( mpr->selmpose ) {
        mpr->selmpose->vpose[vxt->verID].pos.x = vpos->x;
        mpr->selmpose->vpose[vxt->verID].pos.y = vpos->y;
        mpr->selmpose->vpose[vxt->verID].pos.z = vpos->z;

        mpr->selmpose->vpose[vxt->verID].enabled = 0x01;
    }
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_getSelectedMeshPose ( G3DMORPHER *mpr ) {
    return mpr->selmpose;
}

/******************************************************************************/
static void g3dmorpher_anim ( G3DMORPHER *mpr, float frame ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;
    LIST *ltmpver = mpr->lver;
    G3DKEY *prevKey = NULL,
           *nextKey = NULL,
           *currKey = NULL;

    g3dobject_getKeys ( obj, frame, &currKey,
                                    &prevKey, 
                                    &nextKey, KEYDATA );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                            mpr->extensionName );\
        LIST *ltmpmpose = mpr->lmpose;
        uint32_t nbCurrPose = 0x00;
        uint32_t nbPrevPose = 0x00;
        uint32_t nbNextPose = 0x00;
        G3DVECTOR currpos, currnor;
        G3DVECTOR prevpos, prevnor;
        G3DVECTOR nextpos, nextnor;

        memset ( &currpos, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &currnor, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &prevpos, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &prevnor, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &nextpos, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &nextnor, 0x00, sizeof ( G3DVECTOR ) );

        while ( ltmpmpose ) {
            G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

            /*** if framekey not NULL, we are on the frame ***/
            if ( currKey ) {
                if ( currKey->data.u64 & mpose->slotBit ) {
                    if ( mpose->vpose[vxt->verID].enabled ) {
                        currpos.x += mpose->vpose[vxt->verID].pos.x;
                        currpos.y += mpose->vpose[vxt->verID].pos.y;
                        currpos.z += mpose->vpose[vxt->verID].pos.z;

                        currnor.x += mpose->vpose[vxt->verID].nor.x;
                        currnor.y += mpose->vpose[vxt->verID].nor.y;
                        currnor.z += mpose->vpose[vxt->verID].nor.z;

                        nbCurrPose++;
                    }
                }
             } else {
                if ( prevKey && nextKey ) {
                    if ( prevKey->data.u64 & mpose->slotBit ) {
                        if ( mpose->vpose[vxt->verID].enabled ) {
                            prevpos.x += mpose->vpose[vxt->verID].pos.x;
                            prevpos.y += mpose->vpose[vxt->verID].pos.y;
                            prevpos.z += mpose->vpose[vxt->verID].pos.z;

                            prevnor.x += mpose->vpose[vxt->verID].nor.x;
                            prevnor.y += mpose->vpose[vxt->verID].nor.y;
                            prevnor.z += mpose->vpose[vxt->verID].nor.z;

                            nbPrevPose++;
                        }
                    }

                    if ( nextKey->data.u64 & mpose->slotBit ) {
                        if ( mpose->vpose[vxt->verID].enabled ) {
                            nextpos.x += mpose->vpose[vxt->verID].pos.x;
                            nextpos.y += mpose->vpose[vxt->verID].pos.y;
                            nextpos.z += mpose->vpose[vxt->verID].pos.z;

                            nextnor.x += mpose->vpose[vxt->verID].nor.x;
                            nextnor.y += mpose->vpose[vxt->verID].nor.y;
                            nextnor.z += mpose->vpose[vxt->verID].nor.z;

                            nbNextPose++;
                        }
                    }
                }
            }

            ltmpmpose = ltmpmpose->next;
        }

        if ( currKey ) {
            if ( nbCurrPose ) {
                currpos.x /= nbCurrPose;
                currpos.y /= nbCurrPose;
                currpos.z /= nbCurrPose;

                currnor.x /= nbCurrPose;
                currnor.y /= nbCurrPose;
                currnor.z /= nbCurrPose;

                memcpy ( &ver->pos, &currpos, sizeof ( G3DVECTOR ) );
                memcpy ( &ver->nor, &currnor, sizeof ( G3DVECTOR ) );
            }
        } else {
            if ( prevKey && nextKey ) {
                float pRatio = (          frame - nextKey->frame ) / 
                               ( prevKey->frame - nextKey->frame ),
                      nRatio = 1.0f - pRatio;

                if ( nbPrevPose ) {
                    prevpos.x /= nbPrevPose;
                    prevpos.y /= nbPrevPose;
                    prevpos.z /= nbPrevPose;

                    prevnor.x /= nbPrevPose;
                    prevnor.y /= nbPrevPose;
                    prevnor.z /= nbPrevPose;
                }

                if ( nbNextPose ) {
                    nextpos.x /= nbNextPose;
                    nextpos.y /= nbNextPose;
                    nextpos.z /= nbNextPose;

                    nextnor.x /= nbNextPose;
                    nextnor.y /= nbNextPose;
                    nextnor.z /= nbNextPose;
                }

                if ( nbPrevPose && nbNextPose ) {
                    ver->pos.x = ( prevpos.x * pRatio ) + ( nextpos.x * nRatio );
                    ver->pos.y = ( prevpos.y * pRatio ) + ( nextpos.y * nRatio );
                    ver->pos.z = ( prevpos.z * pRatio ) + ( nextpos.z * nRatio );

                    ver->nor.x = ( prevnor.x * pRatio ) + ( nextnor.x * nRatio );
                    ver->nor.y = ( prevnor.y * pRatio ) + ( nextnor.y * nRatio );
                    ver->nor.z = ( prevnor.z * pRatio ) + ( nextnor.z * nRatio );
                }
            }
        }

        ltmpver = ltmpver->next;
    }

    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;

        g3dmesh_update ( mes,
                         mpr->lver,
                         NULL,
                         NULL,
                         UPDATEFACEPOSITION |
                         UPDATEFACENORMAL   |
                         UPDATEVERTEXNORMAL |
                         RESETMODIFIERS, 0x00 );
    }
}

/******************************************************************************/
static void g3dmorpher_setParent ( G3DMORPHER *mpr, 
                                   G3DOBJECT  *parent,
                                   uint64_t    engine_flags ) {
    /*if ( g3dobject_isActive ( (G3DOBJECT*) sdr ) ) {
        g3dsubdivider_activate ( sdr, engine_flags );
    }*/
}

/******************************************************************************/
static G3DMESH *g3dmorpher_commit ( G3DMORPHER    *mpr, 
                                    uint32_t       commitMeshID,
                                    unsigned char *commitMeshName,
                                    uint64_t       engine_flags ) {

}

/******************************************************************************/
static G3DMORPHER *g3dmorpher_copy ( G3DMORPHER *mpr,
                                     uint64_t engine_flags ) {
}

/******************************************************************************/
static void g3dmorpher_startUpdate ( G3DMORPHER *mpr,
                                     uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_update ( G3DMORPHER *mpr,
                                uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_endUpdate ( G3DMORPHER *mpr,
                                   uint64_t engine_flags ) {

}

/******************************************************************************/
static uint32_t g3dmorpher_modify ( G3DMORPHER *mpr,
                                    uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_activate ( G3DMORPHER *mpr,
                                  uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_deactivate ( G3DMORPHER *mpr,
                                    uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_free ( G3DMORPHER *mpr ) {
    LIST *ltmpver = mpr->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX* ) ltmpver->data;
        G3DVERTEXEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                           mpr->extensionName );

        g3dvertex_removeExtension ( ver, vxt );

        vertexposeextension_free ( vxt );

        ltmpver = ltmpver->next;
    }

    list_free ( &mpr->lver  , NULL );
    list_free ( &mpr->lmpose, g3dmorphermeshpose_free  );
}

/******************************************************************************/
static void g3dmorpher_pickVertices ( G3DMORPHER *mpr,
                                      uint64_t    engine_flags ) {
    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;
        LIST *ltmpver = mes->lver;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
            G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr,
                                                                     ver,
                                                                     NULL );

            g3dpick_setName ( ( uint64_t ) ver );

            /*if ( ver->flags & VERTEXSKINNED ) {
                g3dpick_drawPoint ( ver->skn.x, 
                                    ver->skn.y, 
                                    ver->skn.z );
            } else {
                g3dpick_drawPoint ( ver->pos.x, 
                                    ver->pos.y, 
                                    ver->pos.z );
            }*/

            if ( vpose ) {
                g3dpick_drawPoint ( vpose->pos.x, 
                                    vpose->pos.y, 
                                    vpose->pos.z );
            } else {
                g3dpick_drawPoint ( ver->pos.x, 
                                    ver->pos.y, 
                                    ver->pos.z );
            }


            ltmpver = ltmpver->next;
        }
    }
}

/******************************************************************************/
static uint32_t g3dmorpher_pick ( G3DMORPHER *mpr, 
                                  G3DCAMERA  *curcam, 
                                  uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;

    if ( obj->flags & OBJECTSELECTED ) {
        if ( engine_flags & VIEWVERTEX   ) {
            g3dmorpher_pickVertices ( mpr, engine_flags );
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dmorpher_pose ( G3DMORPHER *mpr, 
                              G3DKEY     *key ) {

}

/******************************************************************************/
static uint32_t g3dmorpher_draw ( G3DMORPHER *mpr, 
                                  G3DCAMERA  *cam,
                                  uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;

    if ( obj->parent ) {
        if ( obj->parent->type == G3DMESHTYPE ) {
            if ( obj->flags & OBJECTSELECTED ) {
                G3DMESH *mes = ( obj->parent );
                LIST *ltmpfac = mes->lfac;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    uint32_t i;

                    if ( fac->nbver == 0x03 ) {
                        glBegin ( GL_TRIANGLES );
                    } else {
                        glBegin ( GL_QUADS );
                    }

                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        G3DMORPHERVERTEXPOSE *vpose;

                        /*** small optimization to avoid func call ***/
                        vpose = ( fac->ver[i]->lext ) ? g3dmorpher_getVertexPose ( mpr, fac->ver[i], NULL ) : NULL;

                        if ( vpose && vpose->enabled )  {
                            glNormal3fv ( &vpose->nor );
                            glVertex3fv ( &vpose->pos );
                        } else {
                            glNormal3fv ( &fac->ver[i]->nor );
                            glVertex3fv ( &fac->ver[i]->pos );
                        }
                    }

                    glEnd ( );

                    ltmpfac = ltmpfac->next;
                }

                if ( engine_flags & VIEWVERTEX   ) {
                    if ( mpr->selmpose ) {
                        LIST *ltmpver = mes->lver;

                        glPushAttrib( GL_ALL_ATTRIB_BITS );

                        glDisable   ( GL_LIGHTING );
                        glPointSize ( 4.0f );

                        glBegin ( GL_POINTS );
                        while ( ltmpver ) {
                            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                            G3DMORPHERVERTEXPOSE *vpose;

                            /*** small optimization to avoid func call ***/
                            vpose = ( ver->lext ) ? g3dmorpher_getVertexPose ( mpr, ver, NULL ) : NULL;

                            if ( vpose && vpose->enabled ) {
                                glColor3ub  ( 0x00, 0xFF, 0x00 );
                                glVertex3fv ( ( GLfloat * ) &vpose->pos );
                            } else {
                                glColor3ub  ( 0x00, 0x00, 0xFF );
                                glVertex3fv ( ( GLfloat * ) &ver->pos );
                            }

                            ltmpver = ltmpver->next;
                        }
                        glEnd ( );

                        glPopAttrib ( );
                    }
                }

                return MODIFIERTAKESOVER;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dmorpher_init ( G3DMORPHER *mpr, 
                              uint32_t    id, 
                              char       *name, 
                              uint64_t engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) mpr;

    g3dmodifier_init ( mod, 
                       G3DMORPHERTYPE, 
                       id, 
                       name, 
                       DRAWBEFORECHILDREN  | 
                       OBJECTNOTRANSLATION | 
                       OBJECTNOROTATION    |
                       OBJECTNOSCALING,
         DRAW_CALLBACK(g3dmorpher_draw),
         FREE_CALLBACK(g3dmorpher_free),
         PICK_CALLBACK(g3dmorpher_pick),
         POSE_CALLBACK(g3dmorpher_pose),
         COPY_CALLBACK(NULL),
     ACTIVATE_CALLBACK(g3dmorpher_activate),
   DEACTIVATE_CALLBACK(g3dmorpher_deactivate),
       COMMIT_CALLBACK(NULL),
                       NULL,
    SETPARENT_CALLBACK(g3dmorpher_setParent),
       MODIFY_CALLBACK(g3dmorpher_modify),
  STARTUPDATE_CALLBACK(g3dmorpher_startUpdate),
       UPDATE_CALLBACK(g3dmorpher_update),
    ENDUPDATE_CALLBACK(g3dmorpher_endUpdate) );

    ((G3DOBJECT*)mpr)->anim = ANIM_CALLBACK(g3dmorpher_anim);

    /*** we need an extension name to be able to install an extension to ***/
    /*** the desired vertices ***/
    mpr->extensionName = g3dsysinfo_requestExtensionName ( );
}

/******************************************************************************/
G3DMORPHER *g3dmorpher_new ( uint32_t id, 
                             char    *name, 
                             uint64_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DMORPHER );
    G3DMORPHER *mpr = ( G3DMORPHER * ) calloc ( 0x01, structSize );

    if ( mpr == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dmorpher_init ( mpr, id, name, engine_flags );

    return mpr;
}
