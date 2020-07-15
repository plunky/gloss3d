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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static int move_tool ( G3DMOUSETOOL *mou, 
                       G3DSCENE *sce, 
                       G3DCAMERA *cam,
                       G3DURMANAGER *urm, 
                       uint32_t flags, 
                       G3DEvent *event );
static int moveUV_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE *sce, 
                         G3DCAMERA *cam,
                         G3DURMANAGER *urm, 
                         uint32_t flags, 
                         G3DEvent *event );

/******************************************************************************/
G3DMOUSETOOLMOVE *g3dmousetoolmove_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLMOVE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLMOVE *mv =  ( G3DMOUSETOOLMOVE * ) memarea;

    if ( mv == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( mv,
                        MOVETOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        move_tool,
                        0x00 );

    return mv;
}

/******************************************************************************/
G3DMOUSETOOLMOVEUV *g3dmousetoolmoveUV_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLMOVEUV );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLMOVEUV *mv =  ( G3DMOUSETOOLMOVEUV * ) memarea;

    if ( mv == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( mv,
                        MOVEUVTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        moveUV_tool,
                        0x00 );

    return mv;
}

/******************************************************************************/
static int move_spline ( G3DSPLINE    *spl,
                         G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm,
                         uint32_t      eflags, 
                         G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    static URMMOVEPOINT *ump;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            glMatrixMode ( GL_PROJECTION );
            glPushMatrix ( );
            glLoadIdentity ( );
            g3dcamera_project ( cam, eflags );

            glMatrixMode ( GL_MODELVIEW );
            glPushMatrix ( );
            g3dcamera_view ( cam, HIDEGRID ); 
            glMultMatrixd ( obj->wmatrix );

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            glPopMatrix ( );
            glMatrixMode ( GL_PROJECTION );
            glPopMatrix ( );

            mouseXpress = bev->x;
            mouseYpress = bev->y;

            if ( eflags & VIEWVERTEX ) {
                G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                   bev->x, VPX[0x03] - bev->y },
                                        .only_visible = 0x01,
                                        .weight = 0.0f,
                                        .radius = PICKMINRADIUS };
                uint32_t ctrlClick = ( bev->state & G3DControlMask ) ? 1 : 0;

                /*** simulate click and release ***/
                pick_Item ( &pt, sce, cam, ctrlClick, eflags );

                /*** MUST be called after pick_item because based on ***/
                /*** currently selected points ***/
                ump = g3durm_spline_movePoint ( urm, 
                                                spl, 
                                                UMPSAVESELECTEDPOINTS |
                                                UMPSAVECURRENTHANDLE,
                                                REDRAWVIEW );
                urmmovepoint_saveState ( ump, UMPSAVESTATEBEFORE );

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) bev->x,
                               ( GLdouble ) VPX[0x03] - bev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &orix, &oriy, &oriz );
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( mev->state & G3DButton1Mask ) {
                if ( eflags & VIEWVERTEX ) {
                    LIST *ltmppt = spl->curve->lselpt;
                    double difx, dify, difz;

                    gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &newx, &newy, &newz );

                    difx = ( newx - orix );
                    dify = ( newy - oriy );
                    difz = ( newz - oriz );

                    if ( spl->curve->curhan ) {
                        spl->curve->curhan->pos.x += difx;
                        spl->curve->curhan->pos.y += dify;
                        spl->curve->curhan->pos.z += difz;
                    }

                    while ( ltmppt ) {
                        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

                        pt->pos.x += difx;
                        pt->pos.y += dify;
                        pt->pos.z += difz;

                        ltmppt = ltmppt->next;
                    }

                	g3dspline_update ( spl,
                                       NULL,
                                       UPDATEMODIFIERS, eflags );

                    orix = newx;
                    oriy = newy;
                    oriz = newz;
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( eflags & VIEWVERTEX ) {
                urmmovepoint_saveState ( ump, UMPSAVESTATEAFTER );
            }

            spl->curve->curhan = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
int moveUV_tool ( G3DMOUSETOOL *mou, 
                  G3DSCENE     *sce, 
                  G3DCAMERA    *cam,
                  G3DURMANAGER *urm,
                  uint32_t      eflags, 
                  G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    G3DOBJECT *obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );
    static GLdouble MVX[0x10], PJX[0x10];
    static int VPX[0x04];
    static float widthFactor;
    static float heightFactor;
    static double xold, yold;
    static G3DUV *olduv, *newuv;
    static LIST *lseluv;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

            if ( uvmap ) {
                switch ( event->type ) {
                    case G3DButtonPress : {
                        G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                        glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
                        glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                        glGetIntegerv ( GL_VIEWPORT, VPX );

                        gluUnProject ( bev->x, 
                                       bev->y, 
                                       0.0f,
                                       MVX,
                                       PJX,
                                       VPX,
                                      &orix,
                                      &oriy,
                                      &oriz );

                        mouseXpress = xold = bev->x;
                        mouseYpress = yold = bev->y;

                        if ( eflags & VIEWVERTEXUV ) lseluv = list_copy ( uvmap->lseluv );
                        if ( eflags & VIEWFACEUV   ) lseluv = g3duvset_getUVsFromList ( uvmap->lseluvset );

                        /*** remember coords for undo-redo ***/
                        g3duv_copyUVFromList ( lseluv, &olduv );
                    } return REDRAWVIEW;

                    case G3DMotionNotify : {
                        G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

                        if ( mev->state & G3DButton1Mask ) {
                            gluUnProject ( mev->x, 
                                           mev->y, 
                                           0.0f,
                                           MVX,
                                           PJX,
                                           VPX,
                                          &newx,
                                          &newy,
                                          &newz );

                            if ( ( eflags & VIEWVERTEXUV ) || 
                                 ( eflags & VIEWFACEUV   ) ) {
                                float udiff = ( float ) ( newx - orix ),
                                      vdiff = ( float ) ( newy - oriy );
                                LIST *ltmpuv = lseluv;

                                while ( ltmpuv ) {
                                    G3DUV *uv = ( G3DUV * ) ltmpuv->data;

                                    uv->u += udiff;
                                    uv->v -= vdiff;

                                    ltmpuv = ltmpuv->next;
                                }
                            }

                            orix = newx;
                            oriy = newy;
                        }
                    } return REDRAWUVMAPEDITOR;

                    case G3DButtonRelease : {
                        G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                        /*** simulate click and release ***/
                        if ( ( bev->x == mouseXpress ) && 
                             ( bev->y == mouseYpress ) ) {
                            G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                               bev->x, VPX[0x03] - bev->y },
                                                    .only_visible = 0x00,
                                                    .weight = 0.0f,
                                                    .radius = PICKMINRADIUS };

                            pickUV_tool ( &pt, sce, cam, urm, eflags, event );

                            /*** cancel arrays allocated for undo-redo ***/
                            if ( olduv ) free ( olduv );
                        } else {
                            /*** remember coords for undo-redo ***/
                            g3duv_copyUVFromList ( lseluv, &newuv );

                            g3durm_uvmap_moveUVList ( urm,
                                                      uvmap, 
                                                      lseluv,
                                                      olduv, 
                                                      newuv, 
                                                      REDRAWVIEW |
                                                      REDRAWUVMAPEDITOR );
                        }

                        list_free ( &lseluv, NULL );

                        olduv = newuv = NULL;
                    } return REDRAWVIEW            | 
                             REDRAWCOORDS          | 
                             BUFFEREDSUBDIVISIONOK | 
                             REDRAWCURRENTOBJECT   | 
                             REDRAWUVMAPEDITOR;

                    default :
                    break;
                }
            }
        }
    }

    return FALSE;
}

/******************************************************************************/
static int move_mesh ( G3DMESH      *mes,
                       G3DMOUSETOOL *mou, 
                       G3DSCENE     *sce, 
                       G3DCAMERA    *cam,
                       G3DURMANAGER *urm,
                       uint32_t      eflags, 
                       G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DVECTOR avgpos;

            glMatrixMode ( GL_PROJECTION );
            glPushMatrix ( );
            glLoadIdentity ( );
            g3dcamera_project ( cam, eflags );

            glMatrixMode ( GL_MODELVIEW );
            glPushMatrix ( );
            g3dcamera_view ( cam, HIDEGRID );
            glMultMatrixd ( obj->wmatrix );

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            glPopMatrix ( );
            glMatrixMode ( GL_PROJECTION );
            glPopMatrix ( );

            mouseXpress = bev->x;
            mouseYpress = bev->y;

            if ( eflags & VIEWVERTEX ) {
                lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
            }

            if ( eflags & VIEWEDGE ) {
                lver = g3dmesh_getVertexListFromSelectedEdges ( mes );
            }

            if ( eflags & VIEWFACE ) {
                lver = g3dmesh_getVertexListFromSelectedFaces ( mes );
            }

            g3dvertex_getAveragePositionFromList ( lver, &avgpos );
            g3dvertex_copyPositionFromList       ( lver, &oldpos );
 
            lfac = g3dvertex_getFacesFromList  ( lver );
            ledg = g3dface_getEdgesFromList    ( lfac );

            gluProject ( avgpos.x, avgpos.y, avgpos.z, MVX, PJX, VPX, &winx, &winy, &winz );
            gluUnProject ( ( GLdouble ) bev->x,
                           ( GLdouble ) VPX[0x03] - bev->y,
                           ( GLdouble ) winz,
                           MVX, PJX, VPX,
                           &orix, &oriy, &oriz );

            g3dobject_startUpdateModifiers_r ( mes, eflags );
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( mev->state & G3DButton1Mask ) {
                if ( ( eflags & VIEWVERTEX ) ||
                     ( eflags & VIEWEDGE   ) ||
                     ( eflags & VIEWFACE   ) ) {
                    G3DVECTOR *axis = sce->csr.axis;
                    LIST *ltmpver = lver;
                    double difx, dify, difz;
                    uint32_t nbver = 0x00;

                    gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &newx, &newy, &newz );

                    difx = ( newx - orix );
                    dify = ( newy - oriy );
                    difz = ( newz - oriz );

                    memset ( &mes->avgSelVerPos, 0x00, sizeof ( G3DVECTOR ) );
                    memset ( &mes->avgSelEdgPos, 0x00, sizeof ( G3DVECTOR ) );
                    memset ( &mes->avgSelFacPos, 0x00, sizeof ( G3DVECTOR ) );

                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                        if ( ( eflags & XAXIS ) && axis[0].w ) ver->pos.x += difx;
                        if ( ( eflags & YAXIS ) && axis[1].w ) ver->pos.y += dify;
                        if ( ( eflags & ZAXIS ) && axis[2].w ) ver->pos.z += difz;

                        if ( obj->parent->childvertexchange ) {
                            obj->parent->childvertexchange ( obj->parent,
                                                             obj, ver );
                        }

                        if ( ver->flags & VERTEXSKINNED ) {
                            g3dvertex_computeSkinnedPosition ( ver );
                        }

                        /*** move the cursor as well ***/
                        if ( eflags & VIEWVERTEX ) {
                            mes->avgSelVerPos.x += ver->pos.x;
                            mes->avgSelVerPos.y += ver->pos.y;
                            mes->avgSelVerPos.z += ver->pos.z;
                        }

                        if ( eflags & VIEWEDGE   ) {
                            mes->avgSelEdgPos.x += ver->pos.x;
                            mes->avgSelEdgPos.y += ver->pos.y;
                            mes->avgSelEdgPos.z += ver->pos.z;
                        }

                        if ( eflags & VIEWFACE   ) {
                            mes->avgSelFacPos.x += ver->pos.x;
                            mes->avgSelFacPos.y += ver->pos.y;
                            mes->avgSelFacPos.z += ver->pos.z;
                        }

                        nbver++;

                        ltmpver = ltmpver->next;
                    }

                    if ( nbver ) {
                        if ( eflags & VIEWVERTEX ) {
                            mes->avgSelVerPos.x /= nbver;
                            mes->avgSelVerPos.y /= nbver;
                            mes->avgSelVerPos.z /= nbver;
                        }

                        if ( eflags & VIEWEDGE ) {
                            mes->avgSelEdgPos.x /= nbver;
                            mes->avgSelEdgPos.y /= nbver;
                            mes->avgSelEdgPos.z /= nbver;
                        }

                        if ( eflags & VIEWFACE ) {
                            mes->avgSelFacPos.x /= nbver;
                            mes->avgSelFacPos.y /= nbver;
                            mes->avgSelFacPos.z /= nbver;
                        }
                    }

                    g3dobject_updateModifiers_r ( mes, eflags );

                    if ( mes->onGeometryMove ) {
                        mes->onGeometryMove ( mes, lver, 
                                                   ledg, 
                                                   lfac, 
                                                   eflags );
                    }

                    orix = newx;
                    oriy = newy;
                    oriz = newz;
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** simulate click and release ***/
            if ( ( bev->x == mouseXpress ) && 
                 ( bev->y == mouseYpress ) ) {
                G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                   bev->x, VPX[0x03] - bev->y },
                                        .only_visible = 0x01,
                                        .weight = 0.0f,
                                        .radius = PICKMINRADIUS };

                pick_tool ( &pt, sce, cam, urm, eflags, event );
            }

            g3dvertex_copyPositionFromList ( lver, &newpos );

            g3durm_mesh_moveVertexList ( urm, mes, lver, ledg, lfac, NULL,
                                         oldpos, 
                                         newpos, 
                                         REDRAWVIEW );

            g3dmesh_updateBbox ( mes );

            g3dobject_endUpdateModifiers_r ( mes, eflags );

            list_free ( &lver, NULL );
            list_free ( &lfac, NULL );
            list_free ( &ledg, NULL );

            oldpos = newpos = NULL;
        } return REDRAWVIEW            | 
                 REDRAWCOORDS          | 
                 BUFFEREDSUBDIVISIONOK | 
                 REDRAWCURRENTOBJECT   | 
                 REDRAWUVMAPEDITOR;

        default :
        break;
    }

    return FALSE;
}


/******************************************************************************/
int move_object ( LIST        *lobj,
                  G3DMOUSETOOL *mou, 
                  G3DSCENE     *sce, 
                  G3DCAMERA    *cam,
                  G3DURMANAGER *urm,
                  uint32_t      eflags, 
                  G3DEvent     *event ) {
    static G3DVECTOR vecx = { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f },
                     vecy = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f },
                     vecz = { .x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f };
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR lvecx, lvecy, lvecz, pivot;
    static G3DDOUBLEVECTOR startpos; /*** world original pivot ***/
    static uint32_t nbobj;
    static URMTRANSFORMOBJECT *uto;
    static double PREVWMVX[0x10];

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** Record and undo procedure and record the current state ***/
            uto = g3durm_object_transform ( urm,
                                            lobj,
                                            UTOSAVETRANSLATION, 
                                            REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );
            /***/


            mouseXpress = bev->x;
            mouseYpress = bev->y;

            glMatrixMode ( GL_PROJECTION );
            glPushMatrix ( );
            glLoadIdentity ( );
            g3dcamera_project ( cam, eflags );

            glMatrixMode ( GL_MODELVIEW );
            glPushMatrix ( );
            g3dcamera_view ( cam, HIDEGRID );

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            glPopMatrix ( );
            glMatrixMode ( GL_PROJECTION );
            glPopMatrix ( );

            /*** TOTO: change the call below to use camera.pivot ****/
            g3dscene_getSelectionPosition ( sce, &pivot );

            gluProject ( pivot.x, 
                         pivot.y, 
                         pivot.z, MVX, PJX, VPX, &winx, &winy, &winz );

            gluUnProject ( ( GLdouble ) bev->x,
                           ( GLdouble ) VPX[0x03] - bev->y,
                           ( GLdouble ) winz,
                               MVX, PJX, VPX,
                              &startpos.x, &startpos.y, &startpos.z );

            nbobj = list_count ( lobj );

            if ( nbobj == 0x01 ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) lobj->data;
                /*** PREVWMX is used in VIEWAXIS mode to move vertices back ***/
                memcpy ( PREVWMVX, obj->wmatrix, sizeof ( double ) * 0x10 );
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            LIST *ltmpobj = lobj;

            if ( mev->state & G3DButton1Mask ) {
                G3DDOUBLEVECTOR endpos;

                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                              &endpos.x, &endpos.y, &endpos.z );

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
                    G3DDOUBLEVECTOR dif = { 0.0f, 0.0f, 0.0f }; /** local pivot ***/
                    G3DDOUBLEVECTOR lstartpos, lendpos;
                    G3DVECTOR sca = { 1.0f, 1.0f, 1.0f, 1.0f };

                    if ( nbobj == 0x01 ) {
                        /*** Helps with moving the mouse in the same ***/
                        /*** direcion as axises ***/
                        g3ddoublevector_matrix ( &startpos, obj->iwmatrix, &lstartpos );
                        g3ddoublevector_matrix ( &endpos  , obj->iwmatrix, &lendpos   );

                        g3dvector_matrix ( &vecx, obj->rmatrix, &lvecx );
                        g3dvector_matrix ( &vecy, obj->rmatrix, &lvecy );
                        g3dvector_matrix ( &vecz, obj->rmatrix, &lvecz );

                        /** adjust move amplitude with scaling factor ***/
                        g3dcore_getMatrixScale ( obj->wmatrix, &sca );
                    } else {
                        G3DVECTOR zero = { 0.0f, 0.0f, 0.0f, 1.0f }, lzero;

                        /** World reference for mouse gesture ***/
                        memcpy ( &lstartpos, &startpos, sizeof ( G3DDOUBLEVECTOR ) );
                        memcpy ( &lendpos  , &endpos  , sizeof ( G3DDOUBLEVECTOR ) );

                        /*** build direction vectors ***/
                        g3dvector_matrix ( &zero, obj->parent->iwmatrix, &lzero );
                        g3dvector_matrix ( &vecx, obj->parent->iwmatrix, &lvecx );
                        g3dvector_matrix ( &vecy, obj->parent->iwmatrix, &lvecy );
                        g3dvector_matrix ( &vecz, obj->parent->iwmatrix, &lvecz );

                        lvecx.x -= lzero.x;
                        lvecx.y -= lzero.y;
                        lvecx.z -= lzero.z;

                        lvecy.x -= lzero.x;
                        lvecy.y -= lzero.y;
                        lvecy.z -= lzero.z;

                        lvecz.x -= lzero.x;
                        lvecz.y -= lzero.y;
                        lvecz.z -= lzero.z;

                        /** Commented out. May be useful when object is scaled */
                        /*g3dvector_normalize ( &lvecx, NULL );
                        g3dvector_normalize ( &lvecy, NULL );
                        g3dvector_normalize ( &lvecz, NULL );*/
                    }

                    dif.x = ( lendpos.x - lstartpos.x );
                    dif.y = ( lendpos.y - lstartpos.y );
                    dif.z = ( lendpos.z - lstartpos.z );

                    if ( ( eflags & XAXIS ) && sce->csr.axis[0x00].w ) {
                        obj->pos.x += ( lvecx.x * dif.x * sca.x );
                        obj->pos.y += ( lvecx.y * dif.x * sca.x );
                        obj->pos.z += ( lvecx.z * dif.x * sca.x );
                    }

                    if ( ( eflags & YAXIS ) && sce->csr.axis[0x01].w ) {
                        obj->pos.x += ( lvecy.x * dif.y * sca.y );
                        obj->pos.y += ( lvecy.y * dif.y * sca.y );
                        obj->pos.z += ( lvecy.z * dif.y * sca.y );
                    }

                    if ( ( eflags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                        obj->pos.x += ( lvecz.x * dif.z * sca.z );
                        obj->pos.y += ( lvecz.y * dif.z * sca.z );
                        obj->pos.z += ( lvecz.z * dif.z * sca.z );
                    }

                    g3dobject_updateMatrix_r ( obj, eflags );

                    if ( eflags & VIEWAXIS ) {
                        /*** in case this was in VIEWAXIS mode, we move ***/
                        /*** back the vertices were they were before,   ***/
                        /*** in world coord ***/
                        if ( nbobj == 0x01 ) {
                            if ( obj->type == G3DMESHTYPE ) {
                                G3DMESH *mes = ( G3DMESH * ) obj;

                                g3dmesh_moveAxis ( mes, PREVWMVX, eflags );
                                memcpy ( PREVWMVX, obj->wmatrix, sizeof ( double ) * 0x10 );
                            }

                            if ( obj->type & SPLINE ) {
                                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                                g3dspline_moveAxis ( spl, PREVWMVX, eflags );
                                memcpy ( PREVWMVX, obj->wmatrix, sizeof ( double ) * 0x10 );
                            }
                        }
                    }

                    ltmpobj = ltmpobj->next;
                }

                memcpy ( &startpos, &endpos, sizeof ( G3DDOUBLEVECTOR ) );
            }
        } return REDRAWVIEW | REDRAWCOORDS;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            LIST *ltmpobj = lobj;

            while ( ltmpobj ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;

                    g3dmesh_updateBbox ( mes );
                }

                ltmpobj = ltmpobj->next;
            }

            /*** simulate click and release ***/
            if ( ( bev->x == mouseXpress ) && 
                 ( bev->y == mouseYpress ) ) {
                G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                   bev->x, VPX[0x03] - bev->y },
                                        .only_visible = 0x01,
                                        .weight = 0.0f,
                                        .radius = PICKMINRADIUS };

                /*** FIRST UNDO the TRANSFORM that we saved at buttonPress ***/
                /*** and that was not used at all ***/
                g3durmanager_undo ( urm, eflags );

                pick_tool ( &pt, sce, cam, urm, eflags, event );
            } else {
                urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
            }

        } return REDRAWALL;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int move_tool ( G3DMOUSETOOL *mou, 
                       G3DSCENE *sce, 
                       G3DCAMERA *cam,
                       G3DURMANAGER *urm, 
                       uint32_t flags, 
                       G3DEvent *event ) {
    static GLint VPX[0x04];
    static LIST *lver, *lfac, *lsub, *ledg, *ffdlsub, *lvtx;
    static G3DMESHFAC **msftab; /*** list of faces to update from skinning ***/
    static uint32_t nbmsf;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                               bev->x, VPX[0x03] - bev->y },
                                    .only_visible = 0x00,
                                    .weight = 0.0f,
                                    .radius = PICKMINRADIUS };

            pick_cursor ( &pt, sce, cam, flags );
        } break;

        default :
        break;
    }

    if ( flags & VIEWOBJECT ) {
        return move_object ( sce->lsel, mou, sce, cam, urm, flags, event );
    }

    if ( obj ) {
        if ( flags & VIEWAXIS ) {
            LIST ldummy = { .next = NULL, 
                            .data = obj,
                            .prev = NULL };

            return move_object ( &ldummy, mou, sce, cam, urm, flags, event );
        }

        if ( flags & VIEWUVWMAP ) {
            if ( obj->type & MESH ) {
                G3DMESH  *mes = ( G3DMESH * ) obj;
                G3DUVMAP * map = g3dmesh_getSelectedUVMap ( mes );

                if ( map && ( g3duvmap_isFixed ( map ) == 0x00 ) ) {
                    LIST ldummy = { .next = NULL, 
                                    .data = map,
                                    .prev = NULL };

                    return move_object ( &ldummy, mou, sce, cam, urm, flags, event );
                }
            }
        }

        if ( ( flags & VIEWVERTEX ) ||
             ( flags & VIEWEDGE   ) ||
             ( flags & VIEWFACE   ) ) {
            if ( obj->type & SPLINE ) {
                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                return move_spline ( spl, mou, sce, cam, urm, flags, event );
            }

            if ( ( obj->type & MESH ) && ( obj->type & EDITABLE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                return move_mesh ( mes, mou, sce, cam, urm, flags, event );
            }
        }
    }

    return FALSE;
}
