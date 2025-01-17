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
uint32_t g3dobject_getNextTagID ( G3DOBJECT *obj ) {
    return obj->tagID++;
}

/******************************************************************************/
G3DTAG *g3dobject_getTagByID ( G3DOBJECT *obj, uint32_t id ) {
    LIST *ltmptag = obj->ltag;

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;

        if ( tag->id == id ) return tag;

        ltmptag = ltmptag->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dobject_transform_tags ( G3DOBJECT *obj,
                                uint64_t   engine_flags ) {
    LIST *ltmptag = obj->ltag;

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;

        if ( tag->transform ) tag->transform ( tag, obj, engine_flags );

        ltmptag = ltmptag->next;
    }
}

/******************************************************************************/
void g3dobject_preanim_tags ( G3DOBJECT *obj, 
                              float      frame, 
                              uint64_t   engine_flags ) {
    LIST *ltmptag = obj->ltag;

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;

        if ( tag->preAnim ) tag->preAnim ( tag, obj, frame, engine_flags );

        ltmptag = ltmptag->next;
    }
}

/******************************************************************************/
void g3dobject_postanim_tags ( G3DOBJECT *obj, 
                               float      frame, 
                               uint64_t   engine_flags ) {
    LIST *ltmptag = obj->ltag;

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;

        if ( tag->postAnim ) tag->postAnim ( tag, obj, frame, engine_flags );

        ltmptag = ltmptag->next;
    }
}

/******************************************************************************/
/*void g3dobject_modify_r ( G3DOBJECT *obj, uint64_t engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT* ) ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            G3DMODIFIER *mod = ( G3DMODIFIER * ) child;

            if ( g3dobject_isActive ( child ) ) {
                if ( mod->modify ) mod->modify ( mod, engine_flags );
            }

            g3dobject_modify_r ( child, engine_flags );
        }

        ltmpchildren = ltmpchildren->next;
    }
}*/

/******************************************************************************/
void g3dobject_browse ( G3DOBJECT  *obj,
                        uint32_t (*action)(G3DOBJECT *, void *, uint64_t ),
                        void       *data,
                        uint64_t    engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    if ( action ) {
        if ( action ( obj, data, engine_flags ) == 0x01 ) {
            return;
        }
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_browse ( child, action, data, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void g3dobject_updateMeshes_r ( G3DOBJECT *obj, uint64_t engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    /*** Note: program crashes if condition is obj->type & MESH ***/
    /*** when loading a g3d file ***/
    if ( obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        mes->obj.update_flags |= ( UPDATEVERTEXNORMAL |
                                   UPDATEFACENORMAL |
                                   RESETMODIFIERS );

        /*** Rebuild mesh ***/
        g3dmesh_update ( mes, engine_flags );
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_updateMeshes_r ( child, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void g3dobject_renumber_r ( G3DOBJECT *obj, uint32_t *id ) {
    LIST *ltmpchildren = obj->lchildren;
    obj->id = (*id)++;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_renumber_r ( child, id );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
G3DOBJECT *g3dobject_getActiveParentByType ( G3DOBJECT *obj, uint32_t type ) {
    G3DOBJECT *parent = obj->parent;

    while ( parent ) {
        if ( g3dobject_isActive ( parent ) ) {
            if ( ( parent->type & type ) == type ) {
                return parent;
            }
        }

        parent = parent->parent;
    }

    return NULL;
}

/******************************************************************************/
void g3dobject_printCoordinates ( G3DOBJECT *obj ) {
    printf ( "Position: X:%f Y:%f Z:%f\n", obj->pos.x, obj->pos.y, obj->pos.z );
    printf ( "Rotation: X:%f Y:%f Z:%f\n", obj->rot.x, obj->rot.y, obj->rot.z );
    printf ( "Scaling : X:%f Y:%f Z:%f\n", obj->sca.x, obj->sca.y, obj->sca.z );
}

/*void g3dobject_select ( G3DOBJECT *obj, uint64_t engine_flags ) {
    g3dobject_draw ( obj, flags | SELECTOBJECT );
}*/

/******************************************************************************/
uint32_t g3dobject_getNumberOfChildrenByType ( G3DOBJECT *obj,
                                               uint64_t   type ) {
    LIST *ltmpobj = obj->lchildren;
    uint32_t nb = 0x00;

    while ( ltmpobj ) {
        G3DOBJECT *chi = ( G3DOBJECT * ) ltmpobj->data;

        if ( chi->type == type ) {
            nb++;
        }

        ltmpobj = ltmpobj->next;
    }

    return nb;
}

/******************************************************************************/
LIST *g3dobject_getChildrenByType ( G3DOBJECT *obj, 
                                    uint64_t   type ) {
    LIST *ltmpobj = obj->lchildren;
    LIST *ltype = NULL;

    while ( ltmpobj ) {
        G3DOBJECT *chi = ( G3DOBJECT * ) ltmpobj->data;

        if ( chi->type == type ) {
            list_insert ( &ltype, chi );
        }

        ltmpobj = ltmpobj->next;
    }

    return ltype;
}

/******************************************************************************/
/*** Get all Object and children object matching the type passed as argument **/
/******************************************************************************/
void g3dobject_getObjectsByType_r ( G3DOBJECT *obj, uint32_t type,
                                                    LIST **lobj ){
    LIST *ltmp = obj->lchildren;

    if ( ( type == 0x00 ) || ( obj->type == type ) ) {
        list_append ( lobj, obj );
    }

    while ( ltmp ) {
        G3DOBJECT *chi = ( G3DOBJECT * ) ltmp->data;

        g3dobject_getObjectsByType_r ( chi, type, lobj );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
/*** Translate object based on object orientation. Float args are absolute  ***/
/*** coordinates into object's parent coordinates system, not the object's. ***/
void g3dobject_localTranslate ( G3DOBJECT *obj, 
                                float      x,
                                float      y,
                                float      z,
                                uint64_t   engine_flags ) {
    static G3DVECTOR vecx = { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f },
                     vecy = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f },
                     vecz = { .x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f };
    G3DVECTOR movvecx, movvecy, movvecz;

    g3dvector_matrix ( &vecx, obj->rmatrix, &movvecx );
    g3dvector_matrix ( &vecy, obj->rmatrix, &movvecy );
    g3dvector_matrix ( &vecz, obj->rmatrix, &movvecz );

    obj->pos.x += ( movvecx.x * x );
    obj->pos.y += ( movvecx.y * x );
    obj->pos.z += ( movvecx.z * x );

    obj->pos.x += ( movvecy.x * y );
    obj->pos.y += ( movvecy.y * y );
    obj->pos.z += ( movvecy.z * y );

    obj->pos.x += ( movvecz.x * z );
    obj->pos.y += ( movvecz.y * z );
    obj->pos.z += ( movvecz.z * z );

    g3dobject_updateMatrix_r ( obj, engine_flags );
}

/******************************************************************************/
void g3dobject_getSurroundingKeys ( G3DOBJECT *obj,
                                    G3DKEY    *key,
                                    G3DKEY   **prevKey,
                                    G3DKEY   **nextKey,
                                    uint32_t   key_flags ) {
    LIST *ltmpkey = obj->lkey;


    (*prevKey) = NULL;
    (*nextKey) = NULL;

    while ( ltmpkey ) {
        G3DKEY *currentKey = ( G3DKEY * ) ltmpkey->data;

        if ( currentKey == key ) {
            if ( ( currentKey->flags & key_flags ) == key_flags ) {
                if ( ltmpkey->prev ) {
                    (*prevKey) = ( G3DKEY * ) ltmpkey->prev->data;
                }

                if ( ltmpkey->next ) {
                    (*nextKey) = ( G3DKEY * ) ltmpkey->next->data;
                }
            }
        }

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dobject_removeKey ( G3DOBJECT *obj, 
                           G3DKEY    *key, 
                           uint32_t   stitch,
                           LIST     **lremovedPosSegments,
                           LIST     **lremovedRotSegments,
                           LIST     **lremovedScaSegments,
                           LIST     **laddedPosSegments,
                           LIST     **laddedRotSegments,
                           LIST     **laddedScaSegments ) {
    /*g3dobject_removePositionCurveSegmentsFromKey ( obj, key );
    g3dobject_removeRotationCurveSegmentsFromKey ( obj, key );
    g3dobject_removeScalingCurveSegmentsFromKey  ( obj, key );

    g3dcurve_removePoint ( obj->curve[0x00], &key->curvePoint[0x00] );
    g3dcurve_removePoint ( obj->curve[0x01], &key->curvePoint[0x01] );
    g3dcurve_removePoint ( obj->curve[0x02], &key->curvePoint[0x02] );*/

    /*** we backup the flags that are gonna be erased by ***/
    /*** g3dobject_unsetKeyTransformations(). This way, its easier to ***/
    /*** undo redo the operation. ***/
    uint32_t keyFlags = key->flags;


    list_remove ( &obj->lkey, key );

    obj->nbkey--;

    g3dobject_unsetKeyTransformations ( obj, 
                                        key,
                                        KEYPOSITION | 
                                        KEYROTATION | 
                                        KEYSCALING,
                                        lremovedPosSegments,
                                        lremovedRotSegments,
                                        lremovedScaSegments );

    if ( stitch ) {
        g3dobject_stitchTransformations ( obj,
                                          laddedPosSegments,
                                          laddedRotSegments,
                                          laddedScaSegments );
    }

    key->flags = keyFlags;
}

/******************************************************************************/
/*** Note: This function does not free() the removed keys. This will be done***/
/*** by the undoredo manager layer ***/
void g3dobject_removeSelectedKeys ( G3DOBJECT *obj,
                                    LIST     **lremovedKeys,
                                    LIST     **lremovedPosSegments,
                                    LIST     **lremovedRotSegments,
                                    LIST     **lremovedScaSegments,
                                    LIST     **laddedPosSegments,
                                    LIST     **laddedRotSegments,
                                    LIST     **laddedScaSegments ) {
    LIST *ltmpkey = obj->lselkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;
        LIST *ltmpkeynext = ltmpkey->next;

        g3dobject_removeKey ( obj, 
                              key, 
                              0x00,
                              lremovedPosSegments,
                              lremovedRotSegments,
                              lremovedScaSegments,
                              laddedPosSegments,
                              laddedRotSegments,
                              laddedScaSegments );

        if ( lremovedKeys ) list_insert ( lremovedKeys, key );

        ltmpkey = ltmpkeynext;
    }

    g3dobject_stitchTransformations ( obj,
                                      laddedPosSegments,
                                      laddedRotSegments,
                                      laddedScaSegments );

    list_free ( &obj->lselkey, NULL );
}

/******************************************************************************/
/*** Find orientation based on up vector ( Y-Axis ) ***/
void g3dobject_findOrientation ( G3DOBJECT *obj, G3DVECTOR *vec ) {


}

/******************************************************************************/
void g3dobject_selectKeyRange ( G3DOBJECT *obj,
                                float      firstFrame, 
                                float      lastFrame ) {
    LIST *ltmpkey = obj->lkey;

    list_free ( &obj->lselkey, NULL );

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( ( key->frame >= firstFrame ) && 
             ( key->frame <= lastFrame  ) ) {
            g3dobject_selectKey ( obj, key );
        }

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dobject_selectKey ( G3DOBJECT *obj, G3DKEY *key ) {
    list_insert ( &obj->lselkey, key );

    g3dkey_setSelected ( key );
}

/******************************************************************************/
void g3dobject_selectAllKeys ( G3DOBJECT *obj ) {
    /*** Clear the list first ***/
    list_free ( &obj->lselkey, NULL );

    obj->lselkey = list_copy ( obj->lkey );

    list_exec ( obj->lselkey, (void(*)(void*))g3dkey_setSelected );
}

/******************************************************************************/
void g3dobject_unselectKey ( G3DOBJECT *obj, G3DKEY *key ) {
    list_remove ( &obj->lselkey, key );

    g3dkey_unsetSelected ( key );
}

/******************************************************************************/
void g3dobject_unselectAllKeys ( G3DOBJECT *obj ) {
    list_exec ( obj->lselkey, (void(*)(void*))g3dkey_unsetSelected );

    list_free ( &obj->lselkey, NULL );
}

/******************************************************************************/
uint32_t g3dobject_nbkeyloop ( G3DOBJECT *obj ) {
    LIST *ltmp = obj->lkey;
    uint32_t nb = 0x00;    

    while ( ltmp ) {
        G3DKEY *key = ( G3DKEY * ) ltmp->data;

        if ( key->flags & KEYLOOP ) nb++;

        ltmp = ltmp->next;
    }


    return nb;
}

/******************************************************************************/
uint32_t g3dobject_findKeyDuplicate ( G3DOBJECT *obj, G3DKEY *key ) {
    LIST *ltmpkey = obj->lkey;

    while ( ltmpkey ) {
        G3DKEY *dup = ( G3DKEY * ) ltmpkey->data;

        if ( key != dup ) {
            if ( ( int ) key->frame == ( int ) dup->frame ) return 0x01;
        }

        ltmpkey = ltmpkey->next;
    }

    return 0x00;
}

/******************************************************************************/
typedef struct _MOVEDKEY {
    uint32_t flags; /*** Need to backup flags, they'll be erased ***/
                    /*** by unsetTransformations ***/
    float curFrame;
    float newFrame;
    G3DKEY *key;
    G3DKEY *nextKey[0x03];
    G3DKEY *prevKey[0x03];
    uint32_t makeCurve[0x03];
} MOVEDKEY;

/******************************************************************************/
static void g3dobject_moveSelectedKeys ( G3DOBJECT *obj,
                                         void     (*func) ( G3DKEY *funcKey,
                                                            void   *funcData ), 
                                         void      *funcData,
                                         LIST     **lremovedKeys,
                                         LIST     **lremovedPosSegments,
                                         LIST     **lremovedRotSegments,
                                         LIST     **lremovedScaSegments,
                                         LIST     **laddedPosSegments,
                                         LIST     **laddedRotSegments,
                                         LIST     **laddedScaSegments ) {
    /*** at first I thought omly selected keys were concerned by the        ***/
    /*** segment creation/deletion task, but all keys are, b/c they are     ***/
    /*** cases were a not-selected key must reconnect with a selected key   ***/
    /*** and disconnect from a not-selected key, because the stitching goes ***/
    /***  from t to t+1, not ominidirectionnal  ***/
    uint32_t nbkey = list_count ( obj->lkey );
    MOVEDKEY *movedKey = ( MOVEDKEY * ) calloc ( nbkey, sizeof ( MOVEDKEY ) );
    int OP[0x03] = { KEYPOSITION, KEYROTATION, KEYSCALING };
    LIST *ltmpkey = obj->lkey;
    uint32_t i = 0x00, j;

    /*** first pass ***/
    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        movedKey[i].curFrame   = key->frame;
        movedKey[i].flags      = key->flags;
        movedKey[i].key        = key;

        for ( j = 0x00; j < 0x03; j++ ) {
            if ( key->flags & OP[j] ) {
                G3DKEY *prevKey  = NULL,
                       *frameKey = NULL,
                       *nextKey  = NULL;

                g3dobject_getKeys ( obj,
                                    key->frame,
                                   &frameKey,
                                   &prevKey,
                                   &nextKey,
                                    OP[j],
                                    0x01 );

                movedKey[i].prevKey[j] = prevKey;
                movedKey[i].nextKey[j] = nextKey;
            }
        }

        i++;

        ltmpkey = ltmpkey->next;
    }

    /*** second pass ***/
    for ( i = 0x00; i < nbkey; i++ ) {
        if ( movedKey[i].key->flags & KEYSELECTED ) {
            if ( func ) func ( movedKey[i].key, funcData );
        }
    }

    /*** Third pass ****/
    for ( i = 0x00; i < nbkey; i++ ) {
        for ( j = 0x00; j < 0x03; j++ ) {
            if ( movedKey[i].flags & OP[j] ) {
                G3DKEY *key = movedKey[i].key;
                LIST *ltmpseg = key->curvePoint[j].lseg;
                G3DKEY *movedPrevKey  = NULL,
                       *movedFrameKey = NULL,
                       *movedNextKey  = NULL;

                g3dobject_getKeys ( obj,
                                    movedKey[i].key->frame,
                                   &movedFrameKey,
                                   &movedPrevKey,
                                   &movedNextKey,
                                    OP[j],
                                    0x01 );

                while ( ltmpseg ) {
                    G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
                    LIST *ltmpsegnext = ltmpseg->next;
                    LIST **lremovedSegments[0x03] = { lremovedPosSegments,
                                                      lremovedRotSegments,
                                                      lremovedScaSegments };

                    if ( movedKey[i].prevKey[j] ) {
                        if ( movedPrevKey != movedKey[i].prevKey[j] ) {
                            G3DKEY *prevKey = movedKey[i].prevKey[j];

                            if ( ( seg->pt[0x01] == &prevKey->curvePoint[j] ) ||
                                 ( seg->pt[0x00] == &prevKey->curvePoint[j] ) ) {
                                g3dcurve_removeSegment ( obj->curve[j], seg );

                                list_insert ( lremovedSegments[j], seg );
                            }
                        }
                    }

                    if ( movedKey[i].nextKey[j] ) {
                        if ( movedNextKey != movedKey[i].nextKey[j] ) {
                            G3DKEY *nextKey = movedKey[i].nextKey[j];

                            if ( ( seg->pt[0x01] == &nextKey->curvePoint[j] ) ||
                                 ( seg->pt[0x00] == &nextKey->curvePoint[j] ) ) {
                                g3dcurve_removeSegment ( obj->curve[j], seg );

                                list_insert ( lremovedSegments[j], seg );
                            }
                        }
                    }

                    ltmpseg = ltmpsegnext;
                }
            }
        }

        if ( g3dobject_findKeyDuplicate ( obj, movedKey[i].key ) ) {
            g3dobject_unselectKey ( obj, movedKey[i].key );

            g3dobject_removeKey ( obj, 
                                  movedKey[i].key,
                                  0x00,
                                  lremovedPosSegments,
                                  lremovedRotSegments,
                                  lremovedScaSegments,
                                  laddedPosSegments,
                                  laddedRotSegments,
                                  laddedScaSegments );

            list_insert ( lremovedKeys, movedKey[i].key );
        }
    }

    g3dobject_stitchTransformations ( obj,
                                      laddedPosSegments,
                                      laddedRotSegments,
                                      laddedScaSegments );

    if ( nbkey ) free ( movedKey );
}

/******************************************************************************/
typedef struct _SCALEKEYDATA {
    float factor;
    float reference;
} SCALEKEYDATA;

static void scaleKeyFunc ( G3DKEY *key, SCALEKEYDATA *skd ) {
    key->frame += ( int ) ( ( key->frame - skd->reference ) * skd->factor );
}

void g3dobject_scaleSelectedKeys ( G3DOBJECT *obj,
                                   float      factor, 
                                   float      reference,
                                   LIST     **lremovedKeys,
                                   LIST     **lremovedPosSegments,
                                   LIST     **lremovedRotSegments,
                                   LIST     **lremovedScaSegments,
                                   LIST     **laddedPosSegments,
                                   LIST     **laddedRotSegments,
                                   LIST     **laddedScaSegments ) {
    SCALEKEYDATA skd = { .factor = factor,
                         .reference = reference };

    g3dobject_moveSelectedKeys ( obj, 
                  OBJECTKEY_FUNC(scaleKeyFunc), 
                                &skd,
                                 lremovedKeys,
                                 lremovedPosSegments,
                                 lremovedRotSegments,
                                 lremovedScaSegments,
                                 laddedPosSegments,
                                 laddedRotSegments,
                                 laddedScaSegments );
}

/******************************************************************************/
typedef struct _DRIFTKEYDATA {
    float drift;
} DRIFTKEYDATA;

static void driftKeyFunc ( G3DKEY *key, DRIFTKEYDATA *dkd ) {
    key->frame = ( int ) ( key->frame + dkd->drift );
}

void g3dobject_driftSelectedKeys ( G3DOBJECT *obj,
                                   float      drift,
                                   LIST     **lremovedKeys,
                                   LIST     **lremovedPosSegments,
                                   LIST     **lremovedRotSegments,
                                   LIST     **lremovedScaSegments,
                                   LIST     **laddedPosSegments,
                                   LIST     **laddedRotSegments,
                                   LIST     **laddedScaSegments ) {
    DRIFTKEYDATA dkd = { .drift = drift };

    g3dobject_moveSelectedKeys ( obj, 
                  OBJECTKEY_FUNC(driftKeyFunc), 
                                &dkd,
                                 lremovedKeys,
                                 lremovedPosSegments,
                                 lremovedRotSegments,
                                 lremovedScaSegments,
                                 laddedPosSegments,
                                 laddedRotSegments,
                                 laddedScaSegments );
}

/******************************************************************************/
float g3dobject_getKeys ( G3DOBJECT *obj,
                          float      frame,
                          G3DKEY   **framekey,
                          G3DKEY   **prevkey,
                          G3DKEY   **nextkey,
                          uint32_t   key_flags,
                          uint32_t   ignoreLoop ) {
    LIST *ltmpkey = obj->lkey;

    (*framekey) = NULL;
    (*prevkey)  = NULL;
    (*nextkey)  = NULL;
    
    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( ( key->flags & key_flags ) == key_flags ) {
            if ( key->frame == frame ) (*framekey) = key;

            if ( key->frame < frame ) {
                /*** Init ***/
                if ( (*prevkey) == NULL ) (*prevkey) = key;

                /*** Find a better candidate ***/
                if ( key->frame > (*prevkey)->frame ) {
                    (*prevkey) = key;
                }

            }

            if ( key->frame > frame ) {
                /*** Init ***/
                if ( (*nextkey) == NULL ) (*nextkey) = key;

                /*** Find a better candidate ***/
                if ( key->frame < (*nextkey)->frame ) {
                    (*nextkey) = key;
                }
            }
        }

        ltmpkey = ltmpkey->next;
    }

    if ( ignoreLoop == 0x00 ) {
        if ( (*prevkey) ) {
        /*** loop to previous key if needed ***/
                                              /*** Prevents an infinite loop ***/
            if ( ( (*prevkey)->flags & KEYLOOP ) && 
                 ( (*prevkey)->frame != frame  ) ) {
                float v = ( (*prevkey)->frame - (*prevkey)->loopFrame );
                float u = (             frame - (*prevkey)->loopFrame );
                double intpart, decpart;
                float newframe;
                float loopFrame = (*prevkey)->loopFrame;

                (*framekey) = NULL;
                (*prevkey) = NULL;
                (*nextkey) = NULL;

                /*** Prevent a divide by zero ***/
                if ( v ) {
                    /** floating point modulo. We only need the decimal part **/
                    decpart = modf ( ( u / v ), &intpart );

                    newframe = loopFrame + ( decpart * v );

                    return g3dobject_getKeys ( obj, 
                                               newframe, 
                                               framekey,
                                               prevkey,
                                               nextkey, 
                                               key_flags,
                                               0x00 );
                }
            }
        }
    }

    return frame;
}

/******************************************************************************/
void g3dobject_anim_position ( G3DOBJECT *obj,
                               float      frame,
                               uint64_t   engine_flags ) {
    G3DKEY *prevkey = NULL,
           *nextkey = NULL,
           *framekey = NULL;
    float updframe;

    updframe = g3dobject_getKeys ( obj, 
                                   frame, 
                                  &framekey,
                                  &prevkey, 
                                  &nextkey, 
                                   KEYPOSITION,
                                   0x00 );

    if ( framekey ) {
        memcpy ( &obj->pos, &framekey->pos, sizeof ( G3DVECTOR ) );
    } else {
        if ( prevkey && nextkey ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * )  g3dcurve_seekSegment ( obj->curve[0x00], 
                                                                                &prevkey->curvePoint[0x00],
                                                                                &nextkey->curvePoint[0x00] );

            float ratio = ( ( updframe       - prevkey->frame ) /
                            ( nextkey->frame - prevkey->frame ) );

            g3dcubicsegment_getPoint ( csg, ratio, &obj->pos );
        }
    }

/*printf("%s %f\n", __func__, frame );
printf("g3dcoords:");g3dvector_print ( &obj->pos );*/
}

/******************************************************************************/
void g3dobject_anim_rotation ( G3DOBJECT *obj,
                               float      frame, 
                               uint64_t   engine_flags ) {
    G3DKEY *prevkey = NULL,
           *nextkey = NULL,
           *framekey = NULL;
    double RMX[0x10];
    float updframe;

    updframe = g3dobject_getKeys ( obj, 
                                   frame, 
                                  &framekey,
                                  &prevkey, 
                                  &nextkey, 
                                   KEYROTATION,
                                   0x00 );

    if ( framekey ) {
        memcpy ( &obj->rot, &framekey->rot, sizeof ( G3DVECTOR ) );
    } else {
        if ( prevkey && nextkey ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) g3dcurve_seekSegment ( obj->curve[0x01], 
                                                                               &prevkey->curvePoint[0x01],
                                                                               &nextkey->curvePoint[0x01] );
            float ratio = ( ( updframe       - prevkey->frame ) /
                            ( nextkey->frame - prevkey->frame ) );
            G3DQUATERNION qsrc, qdst, qout;
            G3DDOUBLEVECTOR prevrot = { prevkey->rot.x,
                                        prevkey->rot.y,
                                        prevkey->rot.z },
                            nextrot = { nextkey->rot.x,
                                        nextkey->rot.y,
                                        nextkey->rot.z };
            G3DDOUBLEVECTOR rot;

            g3dcore_eulerInDegreesToQuaternion ( &prevrot,
                                                 &qsrc );

            g3dcore_eulerInDegreesToQuaternion ( &nextrot,
                                                 &qdst );

            g3dcubicsegment_getPoint ( csg, ratio, &obj->rot );

            g3dquaternion_slerp ( &qsrc, &qdst, ratio, &qout );

/*printf("out\n");
g3dquaternion_print ( &qout );
g3dquaternion_toEulerInDegrees ( &qout, &rot );
g3ddoublevector_print ( &rot ); */

            g3dquaternion_convert ( &qout, obj->rmatrix );

            g3dquaternion_toEulerInDegrees ( &qout, &rot );

            obj->rot.x = rot.x;
            obj->rot.y = rot.y;
            obj->rot.z = rot.z;
        }
    }
}

/******************************************************************************/
void g3dobject_anim_scaling ( G3DOBJECT *obj,
                              float      frame,
                              uint64_t   engine_flags ) {
    G3DKEY *prevkey = NULL,
           *nextkey = NULL,
           *framekey = NULL;
    float updframe;

    updframe = g3dobject_getKeys ( obj, 
                                   frame, 
                                  &framekey,
                                  &prevkey, 
                                  &nextkey, 
                                   KEYSCALING,
                                   0x00 );

    if ( framekey ) {
        memcpy ( &obj->sca, &framekey->sca, sizeof ( G3DVECTOR ) );
    } else {
        if ( prevkey && nextkey ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) g3dcurve_seekSegment ( obj->curve[0x02], 
                                                                               &prevkey->curvePoint[0x02],
                                                                               &nextkey->curvePoint[0x02] );
            float ratio = ( ( updframe       - prevkey->frame ) /
                            ( nextkey->frame - prevkey->frame ) );

            g3dcubicsegment_getPoint ( csg, ratio, &obj->sca );
        }
    }
}

/******************************************************************************/
static void g3dobject_anim_r_internal ( G3DOBJECT  *obj,
                                        float       frame,
                                        G3DOBJECT **animobj,
                                        uint32_t   *animobjID,
                                        uint32_t    maxanimobj,
                                        uint64_t    engine_flags ) {
    LIST *ltmp = obj->lchildren;

    /*** We separate transformations because the user might not want to ***/
    /*** keep some transformations for a key but keep it for the next one. ***/
    g3dobject_anim_position ( obj, frame, engine_flags );
    g3dobject_anim_rotation ( obj, frame, engine_flags );
    g3dobject_anim_scaling  ( obj, frame, engine_flags );

    g3dobject_preanim_tags ( obj, frame, engine_flags );

    g3dobject_updateMatrix ( obj, engine_flags );

    if ( obj->anim ) obj->anim ( obj, frame, engine_flags );

    /*if ( (*animobjID) < maxanimobj ) {
        animobj[(*animobjID)] = obj;

        (*animobjID)++;
    }*/

    /*** Recurse to children objects ***/
    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        g3dobject_anim_r ( child, frame, engine_flags );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
uint32_t g3dobject_hasRiggedBone_r ( G3DOBJECT *obj ) {
    LIST *ltmpobj = obj->lchildren;

    if ( obj->type == G3DBONETYPE ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        if ( bon->lrig ) return 0x01;
    }

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( g3dobject_hasRiggedBone_r ( ( G3DOBJECT * ) child ) ) return 0x01;

        ltmpobj = ltmpobj->next;
    }

    return 0x00;
}

/******************************************************************************/
/*** This is not per-se a recursive function, but I had to do like that b/c ***/
/*** the anim callbacks must be called after all matrices have been updated ***/
void g3dobject_anim_r  ( G3DOBJECT *obj,
                         float      frame,
                         uint64_t   engine_flags ) {
#define MAXANIMOBJ 0x400
    static G3DOBJECT *animobj[MAXANIMOBJ];
    uint32_t animobjID = 0x00;
    uint32_t i;

    g3dobject_anim_r_internal ( obj,
                                frame,
                                animobj,
                               &animobjID,
                                MAXANIMOBJ,
                                engine_flags );

    for ( i = 0x00; i < animobjID; i++ ) {
        /*if ( animobj[i]->anim ) {
            animobj[i]->anim ( animobj[i], frame, engine_flags );
        }*/

        g3dobject_postanim_tags ( animobj[i], frame, engine_flags );
    }
}

/******************************************************************************/
G3DKEY *g3dobject_getKeyByFrame ( G3DOBJECT *obj, float frame ) {
    LIST *ltmpkey = obj->lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( key->frame == frame ) return key;

        ltmpkey = ltmpkey->next;
    }

    return NULL;
}

/******************************************************************************/
G3DKEY *g3dobject_getKey ( G3DOBJECT *obj,
                           float      frame ) {
    LIST *ltmp = obj->lkey;

    while ( ltmp ) {
        G3DKEY *key = ( G3DKEY * ) ltmp->data;
 
        if ( key->frame == frame ) return key;

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
/*** COMMENTED OUT: unused ***/
/*G3DKEY *g3dobject_getNextKey ( G3DOBJECT *obj,
                               G3DKEY    *key ) {
    G3DKEY *retKey = NULL;
    LIST *ltmpkey = obj->lkey;

    while ( ltmpkey ) {
        G3DKEY *nextKey = ( G3DKEY * ) ltmpkey->data;
 
        if ( nextKey != key ) {
            if ( retKey == NULL ) retKey = nextKey;

            if ( ( nextKey->frame > key->frame    ) &&
                 ( nextKey->frame < retKey->frame ) ) {
                retKey = nextKey;
            }
        }

        ltmpkey = ltmpkey->next;
    }

    return retKey;
}*/

/******************************************************************************/
/*     g3dobject_getKeys ( G3DOBJECT *obj, float frame, G3DKEY **prevkey,
                                                       G3DKEY **nextkey,
                                                       uint32_t key_flags ); */

/******************************************************************************/
static void curve_addTransformation ( G3DCURVE      *curve, 
                                      G3DCURVEPOINT *pt, 
                                      G3DCURVEPOINT *prevPt, 
                                      G3DCURVEPOINT *nextPt,
                                      LIST         **laddedSegments,
                                      LIST         **lremovedSegments ) {

    if ( prevPt && nextPt ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( curve, prevPt, nextPt );

        g3dcurve_insertPointWithinSegment ( curve, 
                                            pt,
                                            seg,
                                            laddedSegments, 
                                            lremovedSegments );
    } else {
        if ( ( prevPt == NULL ) &&
             ( nextPt == NULL ) ) {
            g3dcurve_addPoint ( curve, pt );
        }

        if ( prevPt ) {
            G3DCUBICSEGMENT *seg = g3dcubicsegment_new ( prevPt,
                                                         pt,
                                                         0.0f, 
                                                         0.0f,
                                                         0.0f,
                                                         0.0f,
                                                         0.0f,
                                                         0.0f );

            g3dcurve_addPoint   ( curve, pt );
            g3dcurve_addSegment ( curve, ( G3DCURVESEGMENT * ) seg );
            g3dcurvepoint_roundCubicSegments ( prevPt );

            if ( laddedSegments ) list_insert ( laddedSegments, seg );
        }

        if ( nextPt ) {
            G3DCUBICSEGMENT *seg = g3dcubicsegment_new ( pt,
                                                         nextPt,
                                                         0.0f, 
                                                         0.0f,
                                                         0.0f,
                                                         0.0f,
                                                         0.0f,
                                                         0.0f );
            g3dcurve_addPoint   ( curve, pt );
            g3dcurve_addSegment ( curve, ( G3DCURVESEGMENT * ) seg );
            g3dcurvepoint_roundCubicSegments ( nextPt );

            if ( laddedSegments ) list_insert ( laddedSegments, seg );
        }

        g3dcurvepoint_roundCubicSegments ( pt );
    }
}

/******************************************************************************/
void g3dobject_setKeyTransformations ( G3DOBJECT *obj, 
                                       G3DKEY    *key,
                                       uint32_t   keyFlags,
                                       LIST     **laddedPosSegments,
                                       LIST     **laddedRotSegments,
                                       LIST     **laddedScaSegments,
                                       LIST     **lremovedPosSegments,
                                       LIST     **lremovedRotSegments,
                                       LIST     **lremovedScaSegments ) {
    G3DKEY *prevKey = NULL;
    G3DKEY *nextKey = NULL;
    G3DKEY *frameKey = NULL;

    key->flags |= keyFlags;

    if ( keyFlags & KEYPOSITION ) {
        if ( list_seek ( obj->curve[0x00]->lpt, &key->curvePoint[0x00] ) == NULL ) {
            g3dobject_getKeys ( obj, 
                                key->frame, 
                               &frameKey, 
                               &prevKey, 
                               &nextKey, 
                                KEYPOSITION,
                                0x01 );

            curve_addTransformation ( obj->curve[0x00], 
                                     &key->curvePoint[0x00],
                       ( prevKey ) ? &prevKey->curvePoint[0x00] : NULL,
                       ( nextKey ) ? &nextKey->curvePoint[0x00] : NULL,
                                      laddedPosSegments,
                                      lremovedPosSegments );
        }
    }

    if ( keyFlags & KEYROTATION ) {
        if ( list_seek ( obj->curve[0x01]->lpt, &key->curvePoint[0x01] ) == NULL ) {
            g3dobject_getKeys ( obj, 
                                key->frame, 
                               &frameKey, 
                               &prevKey, 
                               &nextKey, 
                                KEYROTATION,
                                0x01 );

            curve_addTransformation ( obj->curve[0x01], 
                                     &key->curvePoint[0x01],
                       ( prevKey ) ? &prevKey->curvePoint[0x01] : NULL,
                       ( nextKey ) ? &nextKey->curvePoint[0x01] : NULL,
                                      laddedRotSegments,
                                      lremovedRotSegments );
        }
    }

    if ( keyFlags & KEYSCALING ) {
        if ( list_seek ( obj->curve[0x02]->lpt, &key->curvePoint[0x02] ) == NULL ) {
            g3dobject_getKeys ( obj, 
                                key->frame, 
                               &frameKey, 
                               &prevKey, 
                               &nextKey, 
                                KEYSCALING,
                                0x01 );

            curve_addTransformation ( obj->curve[0x02], 
                                     &key->curvePoint[0x02],
                       ( prevKey ) ? &prevKey->curvePoint[0x02] : NULL,
                       ( nextKey ) ? &nextKey->curvePoint[0x02] : NULL,
                                      laddedScaSegments,
                                      lremovedScaSegments );
        }
    }
}

/******************************************************************************/
static void g3dobject_stitchCurve ( G3DOBJECT *obj, 
                                    uint32_t   curveID,
                                    LIST    **laddedSegments ) {
    LIST *ltmpkey = obj->lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;
        G3DKEY *frameKey = NULL, *prevKey = NULL, *nextKey = NULL;

        g3dobject_getKeys ( obj, 
                            key->frame, 
                           &frameKey, 
                           &prevKey, 
                           &nextKey, 
     ( ( curveID == 0x00 ) ? KEYPOSITION : 0x00 ) |
     ( ( curveID == 0x01 ) ? KEYROTATION : 0x00 ) |
     ( ( curveID == 0x02 ) ? KEYSCALING  : 0x00 ) ,
                            0x01 );

        if ( key && nextKey ) {
            G3DCURVESEGMENT *csg = g3dcurve_seekSegment ( obj->curve[curveID],
                                                         &key->curvePoint[curveID],
                                                     &nextKey->curvePoint[curveID]  );

            if ( csg == NULL ) {
                csg = ( G3DCURVESEGMENT * ) g3dcubicsegment_new ( &key->curvePoint[curveID],
                                                                  &nextKey->curvePoint[curveID],
                                                                       0.0f, 
                                                                       0.0f,
                                                                       0.0f,
                                                                       0.0f,
                                                                       0.0f,
                                                                       0.0f );

                g3dcurve_addSegment ( obj->curve[curveID], csg );

                g3dcurvepoint_roundCubicSegments (     &key->curvePoint[curveID] );
                g3dcurvepoint_roundCubicSegments ( &nextKey->curvePoint[curveID] );

                if ( laddedSegments ) list_insert ( laddedSegments, csg );
            }
        }

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dobject_stitchPositionCurve  ( G3DOBJECT  *obj, 
                                      LIST      **laddedSegments ) {
    g3dobject_stitchCurve ( obj, 0x00, laddedSegments );
}

/******************************************************************************/
void g3dobject_stitchRotationCurve  ( G3DOBJECT  *obj, 
                                      LIST      **laddedSegments ) {
    g3dobject_stitchCurve ( obj, 0x01, laddedSegments );
}

/******************************************************************************/
void g3dobject_stitchScalingCurve  ( G3DOBJECT  *obj, 
                                     LIST      **laddedSegments ) {
    g3dobject_stitchCurve ( obj, 0x02, laddedSegments );
}

/******************************************************************************/
void g3dobject_stitchTransformations ( G3DOBJECT *obj,
                                       LIST     **laddedPosSegments,
                                       LIST     **laddedRotSegments,
                                       LIST     **laddedScaSegments ) {
    g3dobject_stitchPositionCurve  ( obj, laddedPosSegments );
    g3dobject_stitchRotationCurve  ( obj, laddedRotSegments );
    g3dobject_stitchScalingCurve   ( obj, laddedScaSegments );
}

/******************************************************************************/
void g3dobject_unsetKeyTransformations ( G3DOBJECT *obj,
                                         G3DKEY    *key,
                                         uint32_t   keyFlags,
                                         LIST     **lremovedPosSegments,
                                         LIST     **lremovedRotSegments,
                                         LIST     **lremovedScaSegments ) {
    if ( keyFlags & KEYPOSITION ) {
        g3dcurve_removePoint ( obj->curve[0x00], 
                              &key->curvePoint[0x00], lremovedPosSegments );

        /*** flag must be unset before connectPositionSegmentFromFrame ***/
        /*** because the latter will search for flagged keys ***/
        key->flags &= (~KEYPOSITION);
    }

    if ( keyFlags & KEYROTATION ) {
        g3dcurve_removePoint ( obj->curve[0x01], 
                              &key->curvePoint[0x01], lremovedRotSegments );

        /*** flag must be unset before connectPositionSegmentFromFrame ***/
        /*** because the latter will search for flagged keys ***/
        key->flags &= (~KEYROTATION);
    }

    if ( keyFlags & KEYSCALING ) {
        g3dcurve_removePoint ( obj->curve[0x02], 
                              &key->curvePoint[0x02], lremovedScaSegments );

        /*** flag must be unset before connectPositionSegmentFromFrame ***/
        /*** because the latter will search for flagged keys ***/
        key->flags &= (~KEYSCALING);
    }
}

/******************************************************************************/
G3DKEY *g3dobject_addKey ( G3DOBJECT *obj,
                           G3DKEY    *key,
                           LIST     **laddedPosSegments,
                           LIST     **laddedRotSegments,
                           LIST     **laddedScaSegments,
                           LIST     **lremovedPosSegments, 
                           LIST     **lremovedRotSegments, 
                           LIST     **lremovedScaSegments ) {
    G3DKEY *overwrittenKey = g3dobject_getKey ( obj, key->frame );

    if ( overwrittenKey ) {
        g3dobject_removeKey ( obj, 
                              overwrittenKey, 
                              0x01,
                              lremovedPosSegments, 
                              lremovedRotSegments, 
                              lremovedScaSegments,
                              laddedPosSegments, 
                              laddedRotSegments, 
                              laddedScaSegments );
    }

    list_insert ( &obj->lkey, key );

    obj->nbkey++;

    g3dobject_setKeyTransformations ( obj,
                                      key,
                                      KEYPOSITION |
                                      KEYROTATION |
                                      KEYSCALING,
                                      laddedPosSegments,
                                      laddedRotSegments,
                                      laddedScaSegments,
                                      lremovedPosSegments,
                                      lremovedRotSegments,
                                      lremovedScaSegments );

    return overwrittenKey;
}

/******************************************************************************/
/*** Sure, we could read  Position/Rotation/Scale values from the object, ***/
/*** but this is more convenient to do it that way, especially when reading ***/
/*** the keys from a file. This way I dont have to order the way keys are ***/
/*** aved into the file. g3dobject_pose does the ordering by itself ***/
G3DKEY *g3dobject_pose ( G3DOBJECT  *obj, 
                         float       frame,
                         G3DVECTOR  *pos,
                         G3DVECTOR  *rot,
                         G3DVECTOR  *sca, 
                         G3DKEY    **overwrittenKey,
                         uint32_t    key_flags,
                         LIST     **laddedPosSegments, 
                         LIST     **laddedRotSegments, 
                         LIST     **laddedScaSegments, 
                         LIST     **lremovedPosSegments, 
                         LIST     **lremovedRotSegments, 
                         LIST     **lremovedScaSegments ) {
    G3DKEY *key = g3dkey_new ( frame, pos, rot, sca, key_flags );

    (*overwrittenKey) = g3dobject_addKey ( obj, 
                                           key,
                                           laddedPosSegments, 
                                           laddedRotSegments, 
                                           laddedScaSegments, 
                                           lremovedPosSegments, 
                                           lremovedRotSegments, 
                                           lremovedScaSegments  );

    /*** allow objects to set their private datas ***/
    if ( obj->pose ) obj->pose ( obj, key );

    return key;
}

/******************************************************************************/
uint32_t g3dobject_hasSelectedParent ( G3DOBJECT *obj ) {
    while ( obj->parent ) {
        if ( obj->parent->flags & OBJECTSELECTED ) return 0x01;

        obj = obj->parent;
    }

    return 0x00;
}

/******************************************************************************/
void g3dobject_setSelected ( G3DOBJECT *obj ) {
    obj->flags |= OBJECTSELECTED;
}

/******************************************************************************/
void g3dobject_unsetSelected ( G3DOBJECT *obj ) {
    obj->flags &= (~OBJECTSELECTED);
}

/******************************************************************************/
void g3dobject_setInactive ( G3DOBJECT *obj ) {
    obj->flags |= OBJECTINACTIVE;
}

/******************************************************************************/
void g3dobject_setActive ( G3DOBJECT *obj ) {
    obj->flags &= (~OBJECTINACTIVE);
}

/******************************************************************************/
G3DOBJECT *g3dobject_getRandomChild ( G3DOBJECT *obj ) {
    uint32_t nbchildren = list_count ( obj->lchildren );
    LIST *ltmpobj = obj->lchildren;

    if ( nbchildren ) {
        uint32_t id = rand () % nbchildren;

        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

            if ( id-- == 0x00 ) return child;

            ltmpobj = ltmpobj->next;
        }
    }

    return NULL;
}

/******************************************************************************/
void g3dobject_drawCenter ( G3DOBJECT *obj,
                            uint64_t engine_flags ) {
    /*** no need to draw this in picking mode ***/
    if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_LIGHTING );
        glColor3ub  ( 0xFF, 0xFF, 0xFF );
        glPointSize ( 3.0f );

        glBegin ( GL_POINTS );
        glVertex3f ( 0.0f, 0.0f, 0.0f );
        glEnd ( );

        glPopAttrib ( );
    }

    /*glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0xFF );
    glBegin ( GL_LINES );
    glVertex3f ( 0.0f, 0.0, 0.0f );
    glVertex3f ( 1.0f, 0.0, 0.0f );

    glVertex3f ( 0.0f, 0.0, 0.0f );
    glVertex3f ( 0.0f, 1.0, 0.0f );

    glVertex3f ( 0.0f, 0.0, 0.0f );
    glVertex3f ( 0.0f, 0.0, 1.0f );

    glEnd ( );
    glPopAttrib ( );*/
}

/******************************************************************************/
void g3dobject_appendChild ( G3DOBJECT *obj, 
                             G3DOBJECT *child,
                             uint64_t   engine_flags ) {
    G3DOBJECT *oldParent = child->parent;

    list_append ( &obj->lchildren, child );

    child->parent = obj;

    if ( obj->addChild ) obj->addChild  ( obj, 
                                          child,
                                          engine_flags );

    if ( child->parent != oldParent ) {
        if ( child->setParent ) child->setParent ( child, 
                                                   obj, 
                                                   oldParent, 
                                                   engine_flags );
    }
}

/******************************************************************************/
void g3dobject_addChild ( G3DOBJECT *obj, 
                          G3DOBJECT *child,
                          uint64_t   engine_flags ) {
    G3DOBJECT *oldParent = child->parent;

    list_insert ( &obj->lchildren, child );

    child->parent = obj;

    child->flags &= (~OBJECTORPHANED);

    if ( obj->addChild ) obj->addChild  ( obj, 
                                          child,
                                          engine_flags );

    if ( child->parent != oldParent ) {
        if ( child->setParent ) child->setParent ( child, 
                                                   obj, 
                                                   oldParent, 
                                                   engine_flags );
    }
}

/******************************************************************************/
void g3dobject_removeChild ( G3DOBJECT *obj, 
                             G3DOBJECT *child,
                             uint64_t   engine_flags ) {
    G3DOBJECT *oldParent = child->parent;

    list_remove ( &obj->lchildren, child );

    /*** This field is used in "deleteSelectedItems_undo()" ***/
    /*** we need it not to be NULL, that's why I comment it ***/
    /*child->parent = NULL;*/

    child->flags |= OBJECTORPHANED;

    if ( obj->removeChild ) obj->removeChild  ( obj,
                                                child,
                                                engine_flags );

    if ( child->setParent ) child->setParent ( child, 
                                               NULL, 
                                               oldParent, 
                                               engine_flags );
}

/******************************************************************************/
void g3dobject_addTag ( G3DOBJECT *obj, 
                        G3DTAG    *tag ) {
    list_insert ( &obj->ltag, tag );

    if ( tag->add ) tag->add ( tag, obj, 0x00 );
}

/******************************************************************************/
void g3dobject_removeTag ( G3DOBJECT *obj, 
                           G3DTAG    *tag ) {
    list_remove ( &obj->ltag, tag );

    if ( tag->remove ) tag->remove ( tag, obj, 0x00 );

    /*** this should be in some dedicated func ***/
    obj->seltag = NULL;
}

/******************************************************************************/
void g3dobject_importTransformations ( G3DOBJECT *dst, G3DOBJECT *src ) {
    /*** import world matrix ***/
    memcpy ( dst->wmatrix , src->wmatrix , sizeof ( dst->wmatrix  ) );
    /*** import inverse world matrix ***/
    memcpy ( dst->iwmatrix, src->iwmatrix, sizeof ( dst->iwmatrix ) );

    /*** import local matrix ***/
    memcpy ( dst->lmatrix , src->lmatrix , sizeof ( dst->lmatrix  ) );
    /*** import inverse local matrix ***/
    memcpy ( dst->ilmatrix, src->ilmatrix, sizeof ( dst->ilmatrix ) );

    /*** import transformation vectors ***/
    memcpy ( &dst->pos, &src->pos, sizeof ( dst->pos ) );
    memcpy ( &dst->rot, &src->rot, sizeof ( dst->rot ) );
    memcpy ( &dst->sca, &src->sca, sizeof ( dst->sca ) );

    g3dobject_updateMatrix_r ( dst, 0x00 );
}

/******************************************************************************/
void g3dobject_free ( G3DOBJECT *obj ) {
    printf ( "freeing memory for object %s\n", obj->name );

    list_free ( &obj->lchildren, (void(*)(void*)) g3dobject_free );
    list_free ( &obj->lkey, (void(*)(void*)) g3dkey_free );
    list_free ( &obj->ltag, (void(*)(void*)) g3dtag_free );

    if ( obj->free ) obj->free ( obj );
    if ( obj->name ) free ( obj->name );

    list_free ( &obj->lselkey, NULL );

    free ( obj );
}

/******************************************************************************/
void g3dobject_buildRotationMatrix ( G3DOBJECT *obj ) {
    double TMPX[0x10];
    G3DQUATERNION qua;
    G3DVECTOR rotX = { 1.0f, 0.0f, 0.0f, 1.0f }, matX,
              rotY = { 0.0f, 1.0f, 0.0f, 1.0f }, matY,
              rotZ = { 0.0f, 0.0f, 1.0f, 1.0f }, matZ;

    glPushMatrix ( );
    glLoadIdentity ( );

    glRotatef ( obj->rot.z, 0.0f, 0.0f, 1.0f );
    glRotatef ( obj->rot.y, 0.0f, 1.0f, 0.0f );
    glRotatef ( obj->rot.x, 1.0f, 0.0f, 0.0f );

    glGetDoublev ( GL_MODELVIEW_MATRIX, obj->rmatrix );
    glPopMatrix ( );

    /*printf("%s origin\n", __func__);
    g3dcore_printMatrix ( obj->rmatrix, 4, 4 );*/

    /*g3dcore_printMatrix ( obj->rmatrix, 4, 4 );
    g3dcore_printMatrix ( TMPX, 4, 4 );*/
}

/******************************************************************************/
void g3dobject_updateMatrix_r ( G3DOBJECT *obj, 
                                uint64_t   engine_flags ) {
    LIST *ltmpobj = obj->lchildren;

    g3dobject_updateMatrix ( obj, engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_updateMatrix_r ( child, engine_flags );
        

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3dobject_updateMatrix ( G3DOBJECT *obj, uint64_t engine_flags ) {
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    glTranslatef ( obj->pos.x, obj->pos.y, obj->pos.z );

    g3dobject_buildRotationMatrix ( obj );

    glMultMatrixd ( obj->rmatrix );

    glScalef  ( obj->sca.x, obj->sca.y, obj->sca.z );

    glGetDoublev ( GL_MODELVIEW_MATRIX, obj->lmatrix );

    g3dcore_invertMatrix ( obj->lmatrix, obj->ilmatrix );

    if ( obj->parent ) {
        g3dcore_multmatrix ( obj->lmatrix, obj->parent->wmatrix, obj->wmatrix );

        g3dcore_invertMatrix ( obj->wmatrix, obj->iwmatrix );
    } else {
        memcpy ( obj->wmatrix , obj->lmatrix , sizeof ( obj->wmatrix  ) );

        memcpy ( obj->iwmatrix, obj->ilmatrix, sizeof ( obj->iwmatrix ) );
    }

    glPopMatrix ( );

    if ( obj->transform ) obj->transform ( obj, engine_flags );

    g3dobject_transform_tags ( obj, engine_flags );
}

/******************************************************************************/
void g3dobject_drawKeys ( G3DOBJECT *obj, 
                          G3DCAMERA *cam, 
                          uint64_t   engine_flags ) {
    LIST *ltmp = obj->lkey;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glColor3ub ( 0xFF, 0x7F, 0x00 );
    glPointSize ( 2.0f );

    if ( engine_flags & VIEWPATH ) {
        g3dcurve_drawSegments ( obj->curve[0x00], engine_flags );
        g3dcurve_drawHandles  ( obj->curve[0x00], engine_flags );
        g3dcurve_drawPoints   ( obj->curve[0x00], engine_flags );
    }

    g3dcurve_draw ( obj->curve[0x00], engine_flags );

    /*glBegin ( GL_LINE_STRIP );
    while ( ltmp ) {
        G3DKEY *key = ( G3DKEY * ) ltmp->data;

        glVertex3f ( key->pos.x, key->pos.y, key->pos.z );

        ltmp = ltmp->next;
    } 
    glEnd ( );*/

    glPopAttrib ( );
}

#define PIOVER180 0.01745329252

/******************************************************************************/
uint32_t g3dobject_pickPath ( G3DOBJECT *obj, 
                              G3DCAMERA *curcam, 
                              uint64_t   engine_flags ) {
    g3dcurve_pickPoints  ( obj->curve[0x00], engine_flags );
    g3dcurve_pickHandles ( obj->curve[0x00], engine_flags );

    return 0x00;
}

/******************************************************************************/
uint32_t g3dobject_pick ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint64_t   engine_flags ) {
    double MVX[0x10], PARENTMVX[0x10];

    glPushMatrix ( );

    glGetDoublev ( GL_MODELVIEW_MATRIX, PARENTMVX );

    glMultMatrixd ( obj->lmatrix );

    glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );

    /*** if we are in the UVMAPEDITOR, we must not change the modelview ***/
    /*** matrix ***/
    if ( engine_flags & VIEWVERTEXUV ) g3dcore_identityMatrix ( MVX );
    if ( engine_flags & VIEWFACEUV   ) g3dcore_identityMatrix ( MVX );

    g3dpick_setModelviewMatrix ( MVX );

    if ( ( obj->flags & OBJECTINVISIBLE ) == 0x00 ) {
        if ( obj->pick ) obj->pick ( obj, curcam, engine_flags );
    }

    if ( engine_flags & VIEWPATH ) {
        if ( obj->flags & OBJECTSELECTED ) {
            g3dpick_setModelviewMatrix ( PARENTMVX );

            if ( ( obj->flags & OBJECTINVISIBLE ) == 0x00 ) {
                g3dobject_pickPath ( obj, curcam, engine_flags );
            }
        }
    }

    glPopMatrix ( );

    return 0x00;
}

/******************************************************************************/
uint32_t g3dobject_pick_r ( G3DOBJECT *obj, 
                            G3DCAMERA *curcam, 
                            uint64_t   engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;
    double MVX[0x10];

    g3dobject_pick ( obj, curcam, engine_flags );

    glPushMatrix ( );
    glMultMatrixd ( obj->lmatrix );

    while ( ltmpchildren ) {
        G3DOBJECT *sub = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_pick_r ( sub, curcam, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }

    glPopMatrix ( );

    return 0x00;
}

/******************************************************************************/
uint32_t g3dobject_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint64_t   engine_flags ) {
    uint32_t ret = 0x00;

    /*** default color for all objects ***/
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    /*** commented out: we rescale normals in all cases, because parents 
         could be scaled too ***/
    /*if ( ( obj->sca.x != 1.0f ) ||
         ( obj->sca.y != 1.0f ) ||
         ( obj->sca.z != 1.0f ) ) {*/
        glEnable ( GL_RESCALE_NORMAL );
    /*}*/

    glPushMatrix ( );
    glMultMatrixd ( obj->lmatrix );

    if ( engine_flags & SYMMETRYVIEW ) glFrontFace(  GL_CW  );
    else                               glFrontFace(  GL_CCW );

    if ( ( obj->flags & OBJECTINVISIBLE ) == 0x00 ) {
        if ( obj->draw ) ret = obj->draw ( obj, curcam, engine_flags );
    }

    if ( engine_flags & SYMMETRYVIEW ) glFrontFace(  GL_CCW );
    else                               glFrontFace(  GL_CW  );

    glPopMatrix ( );

    /*if ( ( obj->sca.x != 1.0f ) ||
         ( obj->sca.y != 1.0f ) ||
         ( obj->sca.z != 1.0f ) ) {*/
        glDisable ( GL_RESCALE_NORMAL );
    /*}*/

    return ret;
}

/******************************************************************************/
uint32_t g3dobject_draw_r ( G3DOBJECT *obj, 
                            G3DCAMERA *curcam, 
                            uint64_t   engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;
    uint32_t ret = 0x00;

    ret = g3dobject_draw ( obj, curcam, engine_flags );

    glPushMatrix ( );

    glMultMatrixd ( obj->lmatrix );

    /*** draw children objects after ***/
    while ( ltmpchildren ) {
        G3DOBJECT *sub = ( G3DOBJECT * ) ltmpchildren->data;

        if ( engine_flags & SYMMETRYVIEW ) { /*** if a symmetry was called ***/
            /*** Do not draw objects that are not     ***/
            /*** concerned by symmetry, e.g modifiers ***/
            if ( ( sub->flags & OBJECTNOSYMMETRY ) == 0x00 ) {
                ret |= g3dobject_draw_r ( sub, curcam, engine_flags );
            }
        } else {
            ret |= g3dobject_draw_r ( sub, curcam, engine_flags );
        }

        ltmpchildren = ltmpchildren->next;
    }

    glPopMatrix ( );

    return ret;
}

/******************************************************************************/
void g3dobject_update ( G3DOBJECT *obj,
                        uint64_t   engine_flags ) {
    if ( obj->update ) {
        obj->update ( obj, engine_flags );
    }

    obj->update_flags = 0x00;
}

/******************************************************************************/
void g3dobject_update_r ( G3DOBJECT *obj,
                          uint64_t   engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    g3dobject_update ( obj, engine_flags );

    /*** draw children objects after ***/
    while ( ltmpchildren ) {
        G3DOBJECT *sub = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_update_r ( sub, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
uint32_t g3dobject_isChild ( G3DOBJECT *parent, G3DOBJECT *obj ) {
    LIST *ltmp = parent->lchildren;

    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        if ( child == obj ) {

            return 0x01;
        } else {
            if ( g3dobject_isChild ( child, obj ) == 0x01 ) {
                return 0x01;
            }
        }

        ltmp = ltmp->next;
    }

    return 0x00;
}

/******************************************************************************/
void g3dobject_treeToList_r ( G3DOBJECT *obj, LIST **lis ) {
    LIST *ltmpobj = obj->lchildren;

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_treeToList_r ( child, lis );

        ltmpobj = ltmpobj->next;
    }

    list_insert ( lis, obj );
}

/******************************************************************************/
G3DOBJECT *g3dobject_getChildByType ( G3DOBJECT *obj, 
                                      uint64_t   type ) {
    LIST *ltmp = obj->lchildren;

    if ( obj->type == type ) {
        return obj;
    } else {
        while ( ltmp ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;
            G3DOBJECT *match = g3dobject_getChildByType ( child, type );

            if ( match ) return match;

            ltmp = ltmp->next;
        }
    }

    return NULL;
}

/******************************************************************************/
G3DOBJECT *g3dobject_getChildByID ( G3DOBJECT *obj, uint32_t id ) {
    LIST *ltmp = obj->lchildren;

    if ( obj->id == id ) {
        return obj;
    } else {
        while ( ltmp ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;
            G3DOBJECT *match = g3dobject_getChildByID ( child, id );

            if ( match ) return match;

            ltmp = ltmp->next;
        }
    }

    return NULL;
}

/******************************************************************************/
/*** Swap a child w/out changing its world matrix, i.e changing the child's ***/
/*** local matrix in such a way that child_local_matrix*parent_local_matrix ***/
/*** leaves the child's world matrix unchanged ***/
void g3dobject_importChild ( G3DOBJECT *newparent, 
                             G3DOBJECT *child, 
                             uint64_t   engine_flags ) {
    double invmatrix[0x10];
    double outmatrix[0x10];

    g3dcore_invertMatrix ( newparent->wmatrix, invmatrix );
    g3dcore_multmatrix   ( child->wmatrix    , invmatrix, outmatrix );

    if ( child->parent ) g3dobject_removeChild ( child->parent, child, engine_flags );

    g3dcore_getMatrixScale       ( outmatrix, &child->sca );
    g3dcore_getMatrixRotation    ( outmatrix, &child->rot );
    g3dcore_getMatrixTranslation ( outmatrix, &child->pos );

    g3dobject_addChild ( newparent, child, engine_flags );

    g3dobject_updateMatrix_r ( child, engine_flags );

}

/******************************************************************************/
G3DOBJECT *g3dobject_copy ( G3DOBJECT  *obj, 
                            uint32_t    id,
                            const char *name, 
                            uint64_t    engine_flags ) {
    G3DOBJECT *cpyobj = ( G3DOBJECT * ) obj->copy ( obj, id, name, engine_flags );
    LIST *ltmpchildren = obj->lchildren;

    g3dobject_importTransformations ( cpyobj, obj );

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_addChild ( cpyobj, g3dobject_copy ( child, child->id,
                                                             child->name,
                                                             engine_flags ), engine_flags );

        ltmpchildren = ltmpchildren->next;
    }


    return cpyobj;
}

/******************************************************************************/
void g3dobject_initMatrices ( G3DOBJECT *obj ) {
    uint32_t i;

    /*** set identity matrix ***/
    for ( i = 0x00; i < 0x10; i++ ) {
        if ( ( i % 0x05 ) == 0x00 ) {
            obj->lmatrix[i] = obj->ilmatrix[i] = 
            obj->wmatrix[i] = obj->iwmatrix[i] = obj->rmatrix[i] = 1.0f;
        } else {
            obj->lmatrix[i] = obj->ilmatrix[i] = 
            obj->wmatrix[i] = obj->iwmatrix[i] = obj->rmatrix[i] = 0.0f;
        }
    }

    obj->rotXAxis.x = obj->rotYAxis.y = obj->rotZAxis.z = 1.0f;

    obj->rotXAxis.y = obj->rotYAxis.x = obj->rotZAxis.x =
    obj->rotXAxis.z = obj->rotYAxis.z = obj->rotZAxis.y = 0.0f;
}

/******************************************************************************/
uint32_t g3dobject_default_draw ( G3DOBJECT * obj, G3DCAMERA *cam,
                                                   uint64_t engine_flags ) {
    /*** commented out : too verbose ***/
    /*if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }*/

    return 0;
}

/******************************************************************************/
void g3dobject_default_free ( G3DOBJECT *obj ) {
    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }
}

/******************************************************************************/
uint32_t g3dobject_default_pick ( G3DOBJECT *obj, G3DCAMERA *cam, 
                                              uint64_t engine_flags ) {
    if ( obj->type != G3DSCENETYPE) {
        /*printf("%s unimplemented for %s\n", __func__, obj->name );*/
    }
}

/******************************************************************************/
void g3dobject_default_pose ( G3DOBJECT *obj, G3DKEY *key ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
G3DOBJECT* g3dobject_default_copy ( G3DOBJECT  *obj,
                                    uint32_t    id,
                                    const char *name,
                                    uint64_t    engine_flags ) {
    G3DOBJECT *cpy = g3dobject_new ( id, name, engine_flags );

    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }

    return cpy;
}

/******************************************************************************/
void g3dobject_default_activate ( G3DOBJECT *obj, uint64_t engine_flags ) {
    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }
}

/******************************************************************************/
void g3dobject_default_deactivate ( G3DOBJECT *obj, uint64_t engine_flags ) {
    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }
}

/******************************************************************************/
G3DOBJECT* g3dobject_default_commit ( G3DOBJECT *obj, uint32_t id,
                                                      const char *name,
                                                      uint64_t engine_flags ) {
    G3DOBJECT *com = g3dobject_new ( id, name, engine_flags );

    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }

    return com;
}

/******************************************************************************/
void g3dobject_default_addChild ( G3DOBJECT *obj, G3DOBJECT *child, 
                                                  uint64_t engine_flags ) {
    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }
}

/******************************************************************************/
void g3dobject_default_setParent ( G3DOBJECT *obj, 
                                   G3DOBJECT *parent, 
                                   G3DOBJECT *oldParent, 
                                   uint64_t   engine_flags ) {
    if ( obj->type ) {
        printf("%s unimplemented for %s\n", __func__, obj->name );
    }
}

/******************************************************************************/
void g3dobject_init ( G3DOBJECT   *obj,
                      uint32_t     type,
                      uint32_t     id,
                      const char  *name,
                      uint32_t     object_flags,
                      uint32_t   (*Draw)      ( G3DOBJECT *, G3DCAMERA *, 
                                                             uint64_t ),
                      void       (*Free)      ( G3DOBJECT * ),
                      uint32_t   (*Pick)      ( G3DOBJECT *, G3DCAMERA *, 
                                                             uint64_t ),
                      void       (*Pose)      ( G3DOBJECT *, G3DKEY * ),
                      G3DOBJECT* (*Copy)      ( G3DOBJECT *, uint32_t,
                                                             const char *,
                                                             uint64_t ),
                      void       (*Activate)  ( G3DOBJECT *, uint64_t ),
                      void       (*Deactivate)( G3DOBJECT *, uint64_t ),
                      G3DOBJECT* (*Commit)    ( G3DOBJECT *, uint32_t,
                                                             const char *,
                                                             uint64_t ),
                      void       (*AddChild)  ( G3DOBJECT *, G3DOBJECT *,
                                                             uint64_t ),
                      void       (*SetParent) ( G3DOBJECT *, 
                                                G3DOBJECT *, 
                                                G3DOBJECT *, 
                                                uint64_t ) ) {
    obj->type  = type;
    obj->id    = id;
    obj->flags = object_flags;

    if ( obj->parent == NULL ) obj->flags |= OBJECTORPHANED;

    if ( name ) {
        uint32_t len = strlen ( name );

        obj->name = ( char * ) calloc ( ( len + 0x01 ), sizeof ( char ) );
        strncpy ( obj->name, name, len );
    } else {
        fprintf ( stderr, "WARNING: Object has no name!\n" );

        obj->name = NULL;
    }

    obj->draw       = ( Draw       ) ? Draw       : g3dobject_default_draw;
    obj->free       = ( Free       ) ? Free       : g3dobject_default_free;
    obj->pick       = ( Pick       ) ? Pick       : g3dobject_default_pick;
    obj->pose       = ( Pose       ) ? Pose       : g3dobject_default_pose;
    obj->copy       = ( Copy       ) ? Copy       : g3dobject_default_copy;
    obj->activate   = ( Activate   ) ? Activate   : g3dobject_default_activate;
    obj->deactivate = ( Deactivate ) ? Deactivate : g3dobject_default_deactivate;
    obj->commit     = ( Commit     ) ? Commit     : g3dobject_default_commit;
    obj->addChild   = ( AddChild   ) ? AddChild   : g3dobject_default_addChild;
    obj->setParent  = ( SetParent  ) ? SetParent  : g3dobject_default_setParent;

    g3dvector_init ( &obj->pos, 0.0f, 0.0f, 0.0f, 1.0f );
    g3dvector_init ( &obj->rot, 0.0f, 0.0f, 0.0f, 1.0f );
    g3dvector_init ( &obj->sca, 1.0f, 1.0f, 1.0f, 1.0f );

    g3dobject_initMatrices ( obj );

    obj->curve[0x00] = g3dcurve_new ( CUBIC, 0x00 );
    obj->curve[0x01] = g3dcurve_new ( CUBIC, 0x00 );
    obj->curve[0x02] = g3dcurve_new ( CUBIC, 0x00 );

    /*** COMMENTED: Because this maybe called before OpenGL initialization ***/
    /*** we dont use OpenGL functions. Instead, we use some home-made func ***/
    /*g3dobject_updateMatrix_r ( obj, 0x00 );*/



    /*g3dquaternion_set ( &obj->rotation, 0.0, 0.0f, 0.0f );*/
}

/******************************************************************************/
uint32_t g3dobject_isSelected ( G3DOBJECT *obj ) {
    return ( obj->flags & OBJECTSELECTED ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t g3dobject_isActive ( G3DOBJECT *obj ) {
    return ( obj->flags & OBJECTINACTIVE ) ? 0x00 : 0x01;
}

/******************************************************************************/
void g3dobject_activate ( G3DOBJECT *obj,
                          uint64_t   engine_flags ) {
    if ( obj->flags & OBJECTINACTIVE ) {
        g3dobject_setActive ( obj );

        if ( obj->activate ) obj->activate ( obj, engine_flags );
    }
}

/******************************************************************************/
void g3dobject_deactivate ( G3DOBJECT *obj, 
                            uint64_t   engine_flags ) {
    if ( ( obj->flags & OBJECTINACTIVE ) == 0x00 ) {
        g3dobject_setInactive ( obj );

        if ( obj->deactivate ) obj->deactivate ( obj, engine_flags );
    }
}

/******************************************************************************/
G3DOBJECT *g3dobject_commit ( G3DOBJECT     *obj, 
                              uint32_t       id,
                              unsigned char *name,
                              uint64_t       engine_flags ) {

    if ( obj->commit ) return obj->commit ( obj, id, name, engine_flags );

    return NULL;
}

/******************************************************************************/
G3DOBJECT *g3dobject_getSelectedChild ( G3DOBJECT *obj ) {
    LIST *ltmp = obj->lchildren;
    G3DOBJECT *sel = NULL;

    while ( ltmp ) {
        G3DOBJECT *kid = ( G3DOBJECT * ) ltmp->data;

        if ( kid->flags & OBJECTSELECTED ) {

            return kid;
        } else {
            sel = g3dobject_getSelectedChild ( kid );

            if ( sel ) return sel;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dobject_countChildren_r ( G3DOBJECT *obj ) {
    LIST *ltmp = obj->lchildren;
    uint32_t nbkid = 0x00;

    while ( ltmp ) {
        G3DOBJECT *kid = ( G3DOBJECT * ) ltmp->data;

        nbkid += g3dobject_countChildren_r ( kid );

        nbkid++;

        ltmp = ltmp->next;
    }

    return nbkid;
}

/******************************************************************************/
uint32_t g3dobject_getID ( G3DOBJECT *obj ) {
    return obj->id;
}

/******************************************************************************/
char *g3dobject_getName ( G3DOBJECT *obj ) {
    return obj->name;
}

/******************************************************************************/
void g3dobject_name ( G3DOBJECT *obj, const char *name ) {
    if ( name ) {
        if ( obj->name ) free ( obj->name );

        obj->name = strdup ( name );
    }
}

/******************************************************************************/
void g3dobject_moveAxis ( G3DOBJECT *obj, 
                          double    *PREVMVX, /* previous world matrix */
                          uint64_t engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;
    double DIFFMVX[0x10];

    g3dcore_multmatrix ( PREVMVX, obj->iwmatrix, DIFFMVX );

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;
        double prvChildWorldMatrix[0x10];
        double newChildLocalMatrix[0x10];

        g3dcore_multmatrix ( child->lmatrix, PREVMVX, prvChildWorldMatrix );
        g3dcore_multmatrix ( prvChildWorldMatrix, obj->iwmatrix, newChildLocalMatrix );

        g3dcore_getMatrixTranslation ( newChildLocalMatrix, &child->pos );
        g3dcore_getMatrixRotation    ( newChildLocalMatrix, &child->rot );

        g3dobject_updateMatrix_r ( child, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
uint32_t g3dobject_seekByPtr_r ( G3DOBJECT *obj, 
                                 G3DOBJECT *ptr ) {
    LIST *ltmpchildren = obj->lchildren;

    if ( obj == ptr ) return 0x01;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        if ( g3dobject_seekByPtr_r ( child, ptr ) ) return 0x01;

        ltmpchildren = ltmpchildren->next;
    }

    return 0x00;
} 

/******************************************************************************/
G3DOBJECT *g3dobject_new ( uint32_t id, const char *name, uint32_t object_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) calloc ( 0x01, sizeof ( G3DOBJECT ) );

    if ( obj == NULL ) {
        fprintf ( stderr, "g3dobject_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DOBJECTTYPE, id, name, object_flags,
                                            NULL, 
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL );

    return obj;
}
