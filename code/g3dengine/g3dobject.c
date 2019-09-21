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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
void g3dobject_updateMeshes_r ( G3DOBJECT *obj, uint32_t engine_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    if ( ( obj->type == G3DMESHTYPE   ) ||
         ( obj->type == G3DSPLINETYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        /*** Rebuild mesh ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              UPDATEVERTEXNORMAL |
                              UPDATEFACENORMAL |
                              RESETMODIFIERS, engine_flags );
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

/*void g3dobject_select ( G3DOBJECT *obj, uint32_t flags ) {
    g3dobject_draw ( obj, flags | SELECTOBJECT );
}*/

/******************************************************************************/
uint32_t g3dobject_drawModifiers ( G3DOBJECT *obj, G3DCAMERA *cam,
                                                     uint32_t engine_flags  ) {
    LIST *ltmpchildren = obj->lchildren;
    uint32_t ret = 0x00;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            if ( ( child->flags & OBJECTINVISIBLE ) == 0x00 ) {
                if ( child->draw ) {
                    G3DMODIFIER *modChild = ( G3DMODIFIER * ) child;

                    glPushMatrix ( );
                    glMultMatrixd ( child->lmatrix );

                    ret = g3dmodifier_draw ( modChild, cam, engine_flags );

                    glPopMatrix ( );
                }
            }
        }

        ltmpchildren = ltmpchildren->next;
    }

    return ret;
}

/******************************************************************************/
uint32_t g3dobject_getNumberOfChildrenByType ( G3DOBJECT *obj, uint32_t type ) {
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
LIST *g3dobject_getChildrenByType ( G3DOBJECT *obj, uint32_t type ) {
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
void g3dobject_localTranslate ( G3DOBJECT *obj, float x,
                                                float y,
                                                float z, uint32_t flags ) {
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

    g3dobject_updateMatrix_r ( obj, flags );
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
void g3dobject_removePositionCurveSegmentsFromKey ( G3DOBJECT *obj, 
                                                    G3DKEY *key ) {
    G3DKEY *prevKey, *nextKey;

    g3dobject_getSurroundingKeys ( obj, key, &prevKey, 
                                             &nextKey, KEYPOSITION );

    if ( prevKey ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( obj->posCurve,
                                                     &prevKey->posCurvePoint,
                                                     &key->posCurvePoint );
        g3dcurve_removeSegment ( obj->posCurve, seg );
    }

    if ( nextKey ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( obj->posCurve,
                                                     &key->posCurvePoint,
                                                     &nextKey->posCurvePoint );
        g3dcurve_removeSegment ( obj->posCurve, seg );
    }

    /* Reconnect */
    if ( prevKey && nextKey ) {
        G3DCURVESEGMENT *csg = g3dcubicsegment_new ( &prevKey->posCurvePoint,
                                                     &nextKey->posCurvePoint,
                                                      0.0f, 
                                                      0.0f,
                                                      0.0f,
                                                      0.0f,
                                                      0.0f,
                                                      0.0f );

        g3dcurve_addSegment ( obj->posCurve, csg );

        g3dcurvepoint_roundCubicSegments ( &prevKey->posCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &nextKey->posCurvePoint );
    }
}

/******************************************************************************/
void g3dobject_removeRotationCurveSegmentsFromKey ( G3DOBJECT *obj, 
                                                    G3DKEY *key ) {
    G3DKEY *prevKey, *nextKey;

    g3dobject_getSurroundingKeys ( obj, key, &prevKey, 
                                             &nextKey, KEYROTATION );

    if ( prevKey ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( obj->rotCurve,
                                                     &prevKey->rotCurvePoint,
                                                     &key->rotCurvePoint );
        g3dcurve_removeSegment ( obj->rotCurve, seg );
    }

    if ( nextKey ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( obj->rotCurve,
                                                     &key->rotCurvePoint,
                                                     &nextKey->rotCurvePoint );
        g3dcurve_removeSegment ( obj->rotCurve, seg );
    }

    /* Reconnect */
    if ( prevKey && nextKey ) {
        G3DCURVESEGMENT *csg = g3dcubicsegment_new ( &prevKey->rotCurvePoint,
                                                     &nextKey->rotCurvePoint,
                                                      0.0f, 
                                                      0.0f,
                                                      0.0f,
                                                      0.0f,
                                                      0.0f,
                                                      0.0f );

        g3dcurve_addSegment ( obj->rotCurve, csg );

        g3dcurvepoint_roundCubicSegments ( &prevKey->rotCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &nextKey->rotCurvePoint );
    }
}

/******************************************************************************/
void g3dobject_removeScalingCurveSegmentsFromKey ( G3DOBJECT *obj, 
                                                   G3DKEY *key ) {
    G3DKEY *prevKey, *nextKey;

    g3dobject_getSurroundingKeys ( obj, key, &prevKey, 
                                             &nextKey, KEYSCALING );

    if ( prevKey ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( obj->scaCurve,
                                                     &prevKey->scaCurvePoint,
                                                     &key->scaCurvePoint );
        g3dcurve_removeSegment ( obj->scaCurve, seg );
    }

    if ( nextKey ) {
        G3DCURVESEGMENT *seg = g3dcurve_seekSegment ( obj->scaCurve,
                                                     &key->scaCurvePoint,
                                                     &nextKey->scaCurvePoint );
        g3dcurve_removeSegment ( obj->scaCurve, seg );
    }

    /* Reconnect */
    if ( prevKey && nextKey ) {
        G3DCURVESEGMENT *csg = g3dcubicsegment_new ( &prevKey->scaCurvePoint,
                                                     &nextKey->scaCurvePoint,
                                                      0.0f, 
                                                      0.0f,
                                                      0.0f,
                                                      0.0f,
                                                      0.0f,
                                                      0.0f );

        g3dcurve_addSegment ( obj->scaCurve, csg );

        g3dcurvepoint_roundCubicSegments ( &prevKey->scaCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &nextKey->scaCurvePoint );
    }
}

/******************************************************************************/
void g3dobject_removeKey ( G3DOBJECT *obj, G3DKEY *key ) {
    g3dobject_removePositionCurveSegmentsFromKey ( obj, key );
    g3dobject_removeRotationCurveSegmentsFromKey ( obj, key );
    g3dobject_removeScalingCurveSegmentsFromKey  ( obj, key );

    list_remove ( &obj->lkey, key );

    obj->nbkey--;
}

/******************************************************************************/
/*** Note: This function does not free() the removed keys. This will be done***/
/*** by the undoredo manager layer ***/
void g3dobject_removeSelectedKeys ( G3DOBJECT *obj ) {
    LIST *ltmpkey = obj->lselkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;
        LIST *ltmpkeynext = ltmpkey->next;

        g3dobject_removeKey ( obj, key );

        ltmpkey = ltmpkeynext;
    }

    list_free ( &obj->lselkey, NULL );
}

/******************************************************************************/
/*** Find orientation based on up vector ( Y-Axis ) ***/
void g3dobject_findOrientation ( G3DOBJECT *obj, G3DVECTOR *vec ) {


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
G3DKEY *g3dobject_getNextKey ( G3DOBJECT *obj, LIST *lkey,
                                               uint32_t key_flags ) {
    LIST *ltmpkey = lkey->next;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( ( key->flags & key_flags ) == key_flags ) {
            return key;
        }

        ltmpkey = ltmpkey->next;
    }

    return ( G3DKEY * ) lkey->data;
}

/******************************************************************************/
G3DKEY *g3dobject_getPrevKey ( G3DOBJECT *obj, LIST *lkey,
                                               uint32_t key_flags ) {
    LIST *ltmpkey = lkey->prev;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( ( key->flags & key_flags ) == key_flags ) {
            return key;
        }

        ltmpkey = ltmpkey->prev;
    }

    return ( G3DKEY * ) lkey->data;
}

/******************************************************************************/
/* wrning: if frame is on a key, prevkey and nextkey = the key we are on */
float g3dobject_getKeys ( G3DOBJECT *obj, float frame, G3DKEY **prevkey,
                                                       G3DKEY **nextkey,
                                                       uint32_t key_flags ) {
    LIST *ltmpkey = obj->lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( ( key->flags & key_flags ) == key_flags ) {
            if ( key->frame <= frame ) {
                /*** Init ***/
                if ( (*prevkey) == NULL ) (*prevkey) = key;

                /*** Find a better candidate ***/
                if ( key->frame > (*prevkey)->frame ) {
                    (*prevkey) = key;
                }
            }

            if ( key->frame >= frame ) {
                /*** Init ***/
                if ( (*nextkey) == NULL ) (*nextkey) = key;

                /*** Find a better candidate ***/
                if ( key->frame <= (*nextkey)->frame ) {
                    (*nextkey) = key;
                }
            }
        }

        ltmpkey = ltmpkey->next;
    }

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

            (*prevkey) = NULL;
            (*nextkey) = NULL;

            /*** Prevent a divide by zero ***/
            if ( v ) {
                /** floating point modulo. We only need the decimal part **/
                decpart = modf ( ( u / v ), &intpart );

                newframe = loopFrame + ( decpart * v );

                return g3dobject_getKeys ( obj, newframe, prevkey,
                                                          nextkey, key_flags );
            }
        }
    }

    return frame;
}

/******************************************************************************/
void g3dobject_anim_position ( G3DOBJECT *obj, float frame, uint32_t flags ) {
    G3DKEY *prevkey = NULL,
           *nextkey = NULL;
    float updframe;

    updframe = g3dobject_getKeys ( obj, frame, &prevkey, 
                                               &nextkey, KEYPOSITION );

    if ( prevkey && nextkey ) {
        /*** we are on the keyframe ***/
        if ( prevkey == nextkey ) {
            obj->pos.x = prevkey->pos.x;
            obj->pos.y = prevkey->pos.y;
            obj->pos.z = prevkey->pos.z;
        } else {
            G3DCUBICSEGMENT *csg = g3dcurve_seekSegment ( obj->posCurve, 
                                                         &prevkey->posCurvePoint,
                                                         &nextkey->posCurvePoint );
            float ratio = ( ( updframe       - prevkey->frame ) /
                            ( nextkey->frame - prevkey->frame ) );

            g3dcubicsegment_getPoint ( csg, ratio, &obj->pos );
        }
    }
}

/******************************************************************************/
void g3dobject_anim_rotation ( G3DOBJECT *obj, float frame, uint32_t flags ) {
    G3DKEY *prevkey = NULL,
           *nextkey = NULL;
    float updframe;

    updframe = g3dobject_getKeys ( obj, frame, &prevkey, 
                                               &nextkey, KEYROTATION );

    if ( prevkey && nextkey ) {
        /*** we are on the keyframe ***/
        if ( prevkey == nextkey ) {
            obj->rot.x = prevkey->rot.x;
            obj->rot.y = prevkey->rot.y;
            obj->rot.z = prevkey->rot.z;
        } else {
            G3DCUBICSEGMENT *csg = g3dcurve_seekSegment ( obj->rotCurve, 
                                                         &prevkey->rotCurvePoint,
                                                         &nextkey->rotCurvePoint );
            float ratio = ( ( updframe       - prevkey->frame ) /
                            ( nextkey->frame - prevkey->frame ) );

            g3dcubicsegment_getPoint ( csg, ratio, &obj->rot );

            glPushMatrix ( );
            glLoadIdentity ( );

            glRotatef ( obj->rot.x, 1.0f, 0.0f, 0.0f );
            glRotatef ( obj->rot.y, 0.0f, 1.0f, 0.0f );
            glRotatef ( obj->rot.z, 0.0f, 0.0f, 1.0f );

            glGetDoublev ( GL_MODELVIEW_MATRIX, obj->rmatrix );
            glPopMatrix ( );
        }
    }
}

/******************************************************************************/
void g3dobject_anim_scaling ( G3DOBJECT *obj, float frame, uint32_t flags ) {
    G3DKEY *prevkey = NULL,
           *nextkey = NULL;
    float updframe;

    updframe = g3dobject_getKeys ( obj, frame, &prevkey, 
                                               &nextkey, KEYSCALING );

    if ( prevkey && nextkey ) {
        /*** we are on the keyframe ***/
        if ( prevkey == nextkey ) {
            obj->sca.x = prevkey->sca.x;
            obj->sca.y = prevkey->sca.y;
            obj->sca.z = prevkey->sca.z;
        } else {
            G3DCUBICSEGMENT *csg = g3dcurve_seekSegment ( obj->scaCurve, 
                                                         &prevkey->scaCurvePoint,
                                                         &nextkey->scaCurvePoint );
            float ratio = ( ( updframe       - prevkey->frame ) /
                            ( nextkey->frame - prevkey->frame ) );

            g3dcubicsegment_getPoint ( csg, ratio, &obj->sca );
        }
    }
}

/******************************************************************************/
void g3dobject_anim_r ( G3DOBJECT *obj, float frame, uint32_t flags ) {
    LIST *ltmp = obj->lchildren;

    /*** We separate transformations because the user might not want to ***/
    /*** keep some transformations for a key but keep it for the next one. ***/
    g3dobject_anim_position ( obj, frame, flags );
    g3dobject_anim_rotation ( obj, frame, flags );
    g3dobject_anim_scaling  ( obj, frame, flags );

    g3dobject_updateMatrix ( obj );

    /*** the transform callback only happen when recursion occurs ***/
    /*** Here we have to call it explicitely. See g3dobject_updateMatrix_r()***/
    if ( obj->transform ) obj->transform ( obj, flags );

    /*** Recurse to children objects ***/
    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        g3dobject_anim_r ( child, frame, flags );

        ltmp = ltmp->next;
    }

    /*if ( obj->anim ) obj->anim ( obj, prevkey, nextkey );*/
}

/******************************************************************************/
G3DOBJECTEXTENSION *g3dobject_getExtensionByID ( G3DOBJECT *obj, 
                                                 uint32_t   name,
                                                 uint32_t   unit ) {
    LIST *ltmpext = obj->lext;

    while ( ltmpext ) {
        G3DOBJECTEXTENSION *ext = ( G3DOBJECTEXTENSION * ) ltmpext->data;

        if ( ext->id == ( ( name << 16 ) | unit ) ) return ext;

        ltmpext = ltmpext->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dobject_addExtension ( G3DOBJECT          *obj, 
                              G3DOBJECTEXTENSION *ext ) {
    list_insert ( &obj->lext, ext );
}

/******************************************************************************/
void g3dobject_removeExtension ( G3DOBJECT          *obj, 
                                 G3DOBJECTEXTENSION *ext ) {
    list_remove ( &obj->lext, ext );
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
                           float frame, LIST **lbeg, /*** key before frame ***/
                                        LIST **lend, /*** key after  frame ***/
                                        LIST **lequ ) { /*** key on frame ***/
    LIST *ltmp = obj->lkey;
    /*** in case no neighbours is found, this will be NULL ***/
    (*lbeg) = NULL;
    (*lend) = NULL;
    (*lequ) = NULL;

    while ( ltmp ) {
        LIST *lnext = ltmp->next,
             *lprev = ltmp->prev;
        G3DKEY *key = ( G3DKEY * ) ltmp->data,
               *prevkey = ( ltmp->prev ) ? ( G3DKEY *) lprev->data : NULL,
               *nextkey = ( ltmp->next ) ? ( G3DKEY *) lnext->data : NULL;
 
        if ( ( ( frame > key->frame ) && ( nextkey == NULL        ) ) ||
             ( ( frame > key->frame ) && ( frame < nextkey->frame ) ) ) {
            (*lbeg) = ltmp;
            (*lend) = ltmp->next;

            return NULL;
        }

        if ( ( ( frame < key->frame ) && ( prevkey == NULL        ) ) ||
             ( ( frame < key->frame ) && ( frame > prevkey->frame ) ) ) {
            (*lbeg) = ltmp->prev;
            (*lend) = ltmp;

            return NULL;
        }

        if ( key->frame == frame ) {
            (*lbeg) = ltmp->prev;
            (*lequ) = ltmp;
            (*lend) = ltmp->next;

            return key;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DKEY *g3dobject_addKey ( G3DOBJECT *obj, G3DKEY *key ) {
    LIST *lbeg, *lend, *lequ, *lnew;
    G3DKEY *overwrittenKey = g3dobject_getKey ( obj, key->frame, &lbeg, 
                                                                 &lend, 
                                                                 &lequ );
    G3DKEY *beg = ( lbeg ) ? ( G3DKEY * ) lbeg->data : NULL;
    G3DKEY *end = ( lend ) ? ( G3DKEY * ) lend->data : NULL;

    if ( overwrittenKey ) {
        g3dobject_removeKey ( obj, overwrittenKey );
    }

    lnew = list_new ( key );

    if ( lbeg == NULL ) {
        obj->lkey = lnew;
    } else {
        lbeg->next = lnew;
    }

    g3dcurve_addPoint ( obj->posCurve, &key->posCurvePoint );
    g3dcurve_addPoint ( obj->rotCurve, &key->rotCurvePoint );
    g3dcurve_addPoint ( obj->scaCurve, &key->scaCurvePoint );

    if ( lbeg && lend ) {
        G3DCURVESEGMENT *posSeg = g3dcurve_seekSegment ( obj->posCurve,
                                                        &beg->posCurvePoint,
                                                        &end->posCurvePoint ),
                        *rotSeg = g3dcurve_seekSegment ( obj->rotCurve,
                                                        &beg->rotCurvePoint,
                                                        &end->rotCurvePoint ),
                        *scaSeg = g3dcurve_seekSegment ( obj->scaCurve,
                                                        &beg->scaCurvePoint,
                                                        &end->scaCurvePoint );

        g3dcurve_removeSegment ( obj->posCurve, posSeg );
        g3dcurve_removeSegment ( obj->rotCurve, rotSeg );
        g3dcurve_removeSegment ( obj->scaCurve, scaSeg );
    }

    if ( lbeg ) {
        G3DCUBICSEGMENT *posCsg = g3dcubicsegment_new ( &beg->posCurvePoint,
                                                        &key->posCurvePoint,
                                                        0.0f, 
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f ),
                        *rotCsg = g3dcubicsegment_new ( &beg->rotCurvePoint,
                                                        &key->rotCurvePoint,
                                                        0.0f, 
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f ),
                        *scaCsg = g3dcubicsegment_new ( &beg->scaCurvePoint,
                                                        &key->scaCurvePoint,
                                                        0.0f, 
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f );

        g3dcurve_addSegment ( obj->posCurve, posCsg );
        g3dcurve_addSegment ( obj->rotCurve, rotCsg );
        g3dcurve_addSegment ( obj->scaCurve, scaCsg );

        g3dcurvepoint_roundCubicSegments ( &beg->posCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &beg->rotCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &beg->scaCurvePoint );
    }

    if ( lend ) {
        G3DCUBICSEGMENT *posCsg = g3dcubicsegment_new ( &key->posCurvePoint,
                                                        &end->posCurvePoint,
                                                        0.0f, 
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f ),
                        *rotCsg = g3dcubicsegment_new ( &key->rotCurvePoint,
                                                        &end->rotCurvePoint,
                                                        0.0f, 
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f ),
                        *scaCsg = g3dcubicsegment_new ( &key->scaCurvePoint,
                                                        &end->scaCurvePoint,
                                                        0.0f, 
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        0.0f );

        g3dcurve_addSegment ( obj->posCurve, posCsg );
        g3dcurve_addSegment ( obj->rotCurve, rotCsg );
        g3dcurve_addSegment ( obj->scaCurve, scaCsg );

        g3dcurvepoint_roundCubicSegments ( &end->posCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &end->rotCurvePoint );
        g3dcurvepoint_roundCubicSegments ( &end->scaCurvePoint );
    }

    g3dcurvepoint_roundCubicSegments ( &key->posCurvePoint );
    g3dcurvepoint_roundCubicSegments ( &key->rotCurvePoint );
    g3dcurvepoint_roundCubicSegments ( &key->scaCurvePoint );

    lnew->prev = lbeg;
    lnew->next = lend;

    if ( lend ) lend->prev = lnew;

    obj->nbkey++;

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
                         uint32_t    key_flags ) {
    G3DKEY *key = g3dkey_new ( frame, pos, rot, sca, key_flags );

    (*overwrittenKey) = g3dobject_addKey ( obj, key );

    /*** allow objects to set their private datas ***/
    if ( obj->pose ) obj->pose ( obj, key );

    return key;
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
void g3dobject_drawCenter ( G3DOBJECT *obj, uint32_t flags ) {
    /*** no need to draw this in picking mode ***/
    if ( ( flags & SELECTMODE ) == 0x00 ) {
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
                             uint32_t   engine_flags ) {
    list_append ( &obj->lchildren, child );

    child->parent = obj;

    if ( child->type == G3DLIGHTTYPE ) {
        g3dlight_init ( ( G3DLIGHT * ) child );
    }

    if ( obj->addChild    ) obj->addChild  ( obj  , child, engine_flags );
    if ( child->setParent ) obj->setParent ( child, obj  , engine_flags );
}

/******************************************************************************/
void g3dobject_addChild ( G3DOBJECT *obj, 
                          G3DOBJECT *child,
                          uint32_t   engine_flags ) {
    list_insert ( &obj->lchildren, child );

    child->parent = obj;

    if ( child->type == G3DLIGHTTYPE ) {
        g3dlight_init ( ( G3DLIGHT * ) child );
    }

    if ( obj->addChild    ) obj->addChild  ( obj  , child, engine_flags );
    if ( child->setParent ) obj->setParent ( child, obj  , engine_flags );
}

/******************************************************************************/
void g3dobject_removeChild ( G3DOBJECT *obj, 
                             G3DOBJECT *child,
                             uint32_t   engine_flags ) {
    list_remove ( &obj->lchildren, child );

    /*** This field is used in "deleteSelectedItems_undo()" ***/
    /*** we need it not to be NULL, that's why I comment it ***/
    /*child->parent = NULL;*/

    if ( child->type == G3DLIGHTTYPE ) {
        g3dlight_zero ( ( G3DLIGHT * ) child );
    }

    /*if ( child->setParent ) obj->setParent   ( child, NULL , engine_flags );*/
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

    if ( obj->free ) obj->free ( obj );
    if ( obj->name ) free ( obj->name );

    list_free ( &obj->lchildren, (void(*)(void*)) g3dobject_free );

    free ( obj );
}

/******************************************************************************/
void g3dobject_buildRotationMatrix ( G3DOBJECT *obj ) {
    glPushMatrix ( );
    glLoadIdentity ( );
    glRotatef ( obj->rot.x, 1.0f, 0.0f, 0.0f );
    glRotatef ( obj->rot.y, 0.0f, 1.0f, 0.0f );
    glRotatef ( obj->rot.z, 0.0f, 0.0f, 1.0f );
    glGetDoublev ( GL_MODELVIEW_MATRIX, obj->rmatrix );
    glPopMatrix ( );
}

/******************************************************************************/
void g3dobject_updateMatrix_r ( G3DOBJECT *obj, uint32_t flags ) {
    LIST *ltmpobj = obj->lchildren;

    g3dobject_updateMatrix ( obj );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_updateMatrix_r ( child, flags );
        

        ltmpobj = ltmpobj->next;
    }

    if ( obj->transform ) {
        obj->transform ( obj, flags );
    }
}

/******************************************************************************/
void g3dobject_updateMatrix ( G3DOBJECT *obj ) {
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
}

/******************************************************************************/
void g3dobject_drawKeys ( G3DOBJECT *obj, G3DCAMERA *cam, uint32_t flags ) {
    LIST *ltmp = obj->lkey;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glColor3ub ( 0xFF, 0x7F, 0x00 );
    glPointSize ( 2.0f );

    glPushMatrix ( );

    if ( obj->parent ) {
        glMultMatrixd ( obj->parent->wmatrix );
    }

    g3dcurve_draw ( obj->posCurve, cam, flags );

    /*glBegin ( GL_LINE_STRIP );
    while ( ltmp ) {
        G3DKEY *key = ( G3DKEY * ) ltmp->data;

        glVertex3f ( key->pos.x, key->pos.y, key->pos.z );

        ltmp = ltmp->next;
    } 
    glEnd ( );*/
 
    glPopMatrix ( );

    glPopAttrib ( );
}

#define PIOVER180 0.01745329252
/******************************************************************************/
uint32_t g3dobject_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint32_t   engine_flags ) {
    LIST *ltmp = obj->lchildren;

    /*** default color for all objects ***/
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    /* commented out: only works for the local matrix */
    /*if ( ( obj->sca.x != 1.0f ) ||
         ( obj->sca.y != 1.0f ) ||
         ( obj->sca.z != 1.0f ) ) {*/
        glEnable ( GL_RESCALE_NORMAL );
    /*}*/

    glPushMatrix ( );

    glMultMatrixd ( obj->lmatrix );

    if ( engine_flags & SYMMETRYVIEW ) glFrontFace(  GL_CW  );
    else                               glFrontFace(  GL_CCW );

    /*** draw a single dot, then draw the object ***/
    if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
        g3dobject_drawCenter ( obj, engine_flags );
    }

    /*** Modifiers must be explicitely drawn by their parent object ***/
    /*** by using g3dobject_drawModifiers() ***/
    if ( ( obj->type & MODIFIER ) == 0x00 ) {
        if ( obj->flags & DRAWBEFORECHILDREN ) {
            if ( obj->draw && ( ( obj->flags & OBJECTINVISIBLE ) == 0x00 ) ) {
               if ( engine_flags & SELECTMODE ) {
                    glLoadName ( ( GLuint ) obj->id );
                }

                obj->draw ( obj, curcam, engine_flags );
            }
        }
    }

    /*** draw children objects after ***/
    while ( ltmp ) {
        G3DOBJECT *sub = ( G3DOBJECT * ) ltmp->data;

        if ( engine_flags & SYMMETRYVIEW ) { /*** if a symmetry was called ***/
            /*** Do not draw objects that are not     ***/
            /*** concerned by symmetry, e.g modifiers ***/
            if ( ( sub->flags & OBJECTNOSYMMETRY ) == 0x00 ) {
                g3dobject_draw ( sub, curcam, engine_flags );
            }
        } else {
            g3dobject_draw ( sub, curcam, engine_flags );
        }

        ltmp = ltmp->next;
    }

    /*** Modifiers must be explicitely drawn by their parent object ***/
    /*** by using g3dmesh_drawModifiers_r() ***/
    if ( ( obj->type & MODIFIER ) == 0x00 ) {
        if ( obj->flags & DRAWAFTERCHILDREN ) {
            if ( obj->draw && ( ( obj->flags & OBJECTINVISIBLE ) == 0x00 ) ) {
               if ( engine_flags & SELECTMODE ) {
                    glLoadName ( ( GLuint ) obj->id );
                }

                obj->draw ( obj, curcam, engine_flags );
            }
        }
    }

    if ( engine_flags & SYMMETRYVIEW ) glFrontFace(  GL_CCW );
    else                               glFrontFace(  GL_CW  );

    glPopMatrix ( );

    if ( ( obj->sca.x != 1.0f ) ||
         ( obj->sca.y != 1.0f ) ||
         ( obj->sca.z != 1.0f ) ) {
        glDisable ( GL_RESCALE_NORMAL );
    }

    return 0x00;
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
G3DOBJECT *g3dobject_getChildByID ( G3DOBJECT *obj, uint32_t id ) {
    LIST *ltmp = obj->lchildren;
    G3DOBJECT *sel = NULL;

    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        if ( child->id == id ) {

            return child;
        } else {
            if ( sel = g3dobject_getChildByID ( child, id ) ) {

                return sel;
            }
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
/*** Swap a child w/out changing its world matrix, i.e changing the child's ***/
/*** local matrix in such a way that child_local_matrix*parent_local_matrix ***/
/*** leaves the child's world matrix unchanged ***/
void g3dobject_importChild ( G3DOBJECT *newparent, 
                             G3DOBJECT *child, 
                             uint32_t   engine_flags ) {
    double invmatrix[0x10];
    double outmatrix[0x10];

    g3dcore_invertMatrix ( newparent->wmatrix, invmatrix );
    g3dcore_multmatrix   ( child->wmatrix    , invmatrix, outmatrix );

    if ( child->parent ) g3dobject_removeChild ( child->parent, child, engine_flags );

    g3dcore_getMatrixScale       ( outmatrix, &child->sca );
    g3dcore_getMatrixRotation    ( outmatrix, &child->rot );
    g3dcore_getMatrixTranslation ( outmatrix, &child->pos );

    g3dobject_addChild ( newparent, child, engine_flags );

    g3dobject_updateMatrix_r ( child, 0x00 );

}

/******************************************************************************/
G3DOBJECT *g3dobject_copy ( G3DOBJECT *obj, uint32_t    id,
                                            const char *name, 
                                            uint32_t    engine_flags ) {
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
}

/******************************************************************************/
uint32_t g3dobject_default_draw ( G3DOBJECT * obj, G3DCAMERA *cam,
                                                   uint32_t   engine_flags ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
void g3dobject_default_free ( G3DOBJECT *obj ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
void g3dobject_default_pick ( G3DOBJECT *obj, G3DCAMERA *cam, 
                                              uint32_t   engine_flags ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
void g3dobject_default_pose ( G3DOBJECT *obj, G3DKEY *key ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
G3DOBJECT* g3dobject_default_copy ( G3DOBJECT *obj, uint32_t id,
                                                    const char *name,
                                                    uint32_t engine_flags ) {
    G3DOBJECT *cpy = g3dobject_new ( id, name, engine_flags );
    printf("%s unimplemented for %s\n", __func__, obj->name );

    return cpy;
}

/******************************************************************************/
void g3dobject_default_activate ( G3DOBJECT *obj, uint32_t engine_flags ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
void g3dobject_default_deactivate ( G3DOBJECT *obj, uint32_t engine_flags ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
G3DOBJECT* g3dobject_default_commit ( G3DOBJECT *obj, uint32_t id,
                                                      const char *name,
                                                      uint32_t engine_flags ) {
    G3DOBJECT *com = g3dobject_new ( id, name, engine_flags );
    printf("%s unimplemented for %s\n", __func__, obj->name );

    return com;
}

/******************************************************************************/
void g3dobject_default_addChild ( G3DOBJECT *obj, G3DOBJECT *child, 
                                                  uint32_t   engine_flags ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
void g3dobject_default_setParent ( G3DOBJECT *obj, G3DOBJECT *parent, 
                                                   uint32_t   engine_flags ) {
    printf("%s unimplemented for %s\n", __func__, obj->name );
}

/******************************************************************************/
void g3dobject_init ( G3DOBJECT   *obj,
                      uint32_t     type,
                      uint32_t     id,
                      const char  *name,
                      uint32_t     object_flags,
                      uint32_t   (*Draw)      ( G3DOBJECT *, G3DCAMERA *, 
                                                             uint32_t ),
                      void       (*Free)      ( G3DOBJECT * ),
                      void       (*Pick)      ( G3DOBJECT *, G3DCAMERA *, 
                                                             uint32_t ),
                      void       (*Pose)      ( G3DOBJECT *, G3DKEY * ),
                      G3DOBJECT* (*Copy)      ( G3DOBJECT *, uint32_t,
                                                             const char *,
                                                             uint32_t ),
                      void       (*Activate)  ( G3DOBJECT *, uint32_t ),
                      void       (*Deactivate)( G3DOBJECT *, uint32_t ),
                      G3DOBJECT* (*Commit)    ( G3DOBJECT *, uint32_t,
                                                             const char *,
                                                             uint32_t ),
                      void       (*AddChild)  ( G3DOBJECT *, G3DOBJECT *,
                                                             uint32_t ),
                      void       (*SetParent) ( G3DOBJECT *, G3DOBJECT *, 
                                                             uint32_t ) ) {
    obj->type  = type;
    obj->id    = id;
    obj->flags = object_flags;

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

    obj->posCurve = g3dcurve_new ( CUBIC, 0x00 );
    obj->rotCurve = g3dcurve_new ( CUBIC, 0x00 );
    obj->scaCurve = g3dcurve_new ( CUBIC, 0x00 );

    /*** COMMENTED: Because this maybe called before OpenGL initialization ***/
    /*** we dont use OpenGL functions. Instead, we use some home-made func ***/
    /*g3dobject_updateMatrix_r ( obj, 0x00 );*/



    /*g3dquaternion_set ( &obj->rotation, 0.0, 0.0f, 0.0f );*/
}

/******************************************************************************/
uint32_t g3dobject_isActive ( G3DOBJECT *obj ) {
    return ( ( obj->flags & OBJECTINACTIVE ) == 0x00 );
}

/******************************************************************************/
void g3dobject_activate ( G3DOBJECT *obj, uint32_t engine_flags ) {
    obj->flags &= (~OBJECTINACTIVE);

    if ( obj->activate ) obj->activate ( obj, engine_flags );
}

/******************************************************************************/
void g3dobject_deactivate ( G3DOBJECT *obj, uint32_t engine_flags ) {
    obj->flags |= OBJECTINACTIVE;

    if ( obj->deactivate ) obj->deactivate ( obj, engine_flags );
}

/******************************************************************************/
G3DOBJECT *g3dobject_commit ( G3DOBJECT *obj, uint32_t       id,
                                              unsigned char *name,
                                              uint32_t       engine_flags ) {

    if ( obj->commit ) return obj->commit ( obj, id, name, engine_flags );
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
        uint32_t len = strlen ( name ) + 0x01;

        obj->name = realloc ( obj->name, len );

        memcpy ( obj->name, name, len );
    }
}

/******************************************************************************/
void g3dobject_moveAxis ( G3DOBJECT *obj, 
                          double    *PREVMVX, /* previous world matrix */
                          uint32_t   engine_flags ) {
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
