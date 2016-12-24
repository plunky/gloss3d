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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
typedef struct _SMOOTHVERTEX {
    uint32_t      verid;
    G3DRTVERTEX  *rtver;
    G3DSCULPTMAP *scmap;
} SMOOTHVERTEX;

/******************************************************************************/
G3DSMOOTHTOOL *smoothTool_new ( ) {
    uint32_t structsize = sizeof ( G3DSMOOTHTOOL );

    G3DSMOOTHTOOL *st =  ( G3DSMOOTHTOOL * ) calloc ( 0x01, structsize );

    if ( st == NULL ) {
        fprintf ( stderr, "smoothTool_new: Memory allocation failed\n" );

        return NULL;
    }

    st->only_visible = 0x01;
    st->pressure     = 0.5f;
    st->radius       = 0x20;

    return st;
}

#define BUFFERSIZE 0x10000

/******************************************************************************/
void smooth_pick ( G3DSMOOTHTOOL *st, G3DMESH *mes,
                                      G3DCAMERA *cam,
                                      G3DVECTOR *dir,
                                      uint32_t engine_flags ) {
    int MX = st->coord[0x00] + ( ( st->coord[0x02] - st->coord[0x00] ) >> 0x01 ),
        MY = st->coord[0x01] + ( ( st->coord[0x03] - st->coord[0x01] ) >> 0x01 ),
        MW = ( st->coord[0x02] - st->coord[0x00] ), /*** width ***/
        MH = ( st->coord[0x03] - st->coord[0x01] ); /*** height ***/
    G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };
    uint32_t nbFaces, nbEdges, nbVertices;
    static GLint VPX[0x04];
    static double PJX[0x10];
    int middle = 0x00;
    GLuint buffer[BUFFERSIZE], *ptr = buffer;
    GLint min, max;
    uint32_t i, j;
    GLint hits;
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    /*** Get the temporary subdivision arrays for CPU #0 ***/
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
    G3DDOUBLEVECTOR near;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t triFaces, triEdges, triVertices;
    uint32_t quaFaces, quaEdges, quaVertices;
    uint32_t subdiv = mes->subdiv;
    LIST *ltmpfac = mes->lfac;
    LIST *lfac = NULL, *lextfac = NULL;
    G3DPLANE plane;
    uint32_t nbver = 0x00;
    LIST *lsmver = NULL;

    memset ( &plane, 0x00, sizeof ( G3DPLANE ) );

    g3dtriangle_evalSubdivision ( subdiv, &triFaces, &triEdges, &triVertices );
    g3dquad_evalSubdivision     ( subdiv, &quaFaces, &quaEdges, &quaVertices );

    /*if ( MW == 0x00 ) { MW = 0x30; middle = 0x01; };
    if ( MH == 0x00 ) { MH = 0x30; middle = 0x01; };*/

    glSelectBuffer ( BUFFERSIZE, buffer );

    glRenderMode ( GL_SELECT );

    glInitNames ( );
    glPushName ( 0xFFFFFFFF );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    gluPickMatrix ( MX, MY, st->radius, st->radius, VPX );
    g3dcamera_project ( cam, engine_flags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    glMultMatrixd ( ((G3DOBJECT*)mes)->wmatrix );

    if ( ( obj->flags & BUFFEREDSUBDIVISION ) ) {
        if ( mes->subdiv ) {
            ltmpfac = mes->lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                uint32_t nbrtver = ( fac->nbver == 0x04 ) ? quaVertices : triVertices;

                for ( i = 0x00; i < nbrtver; i++ ) {
                    glLoadName ( ( fac->id << 0x10 ) | i );
                    glBegin ( GL_POINTS );
                    glVertex3fv ( &fac->rtvermem[i].pos );
                    glEnd ( );
                }

                ltmpfac = ltmpfac->next;
            }
        }
    } else {
        g3dmesh_drawSubdividedObject ( mes, engine_flags | SELECTMODE );
    }

    hits = glRenderMode ( GL_RENDER );
    /*printf ("hits:%d\n", hits );*/

    for ( i = 0x00; i < hits; i++ ) {
        GLuint nbname = (*ptr++);

        min = *(ptr++); /** D0 NOT  COMMENT THIS ***/
        max = *(ptr++); /** D0 NOT  COMMENT THIS ***/
    /*printf ("nbname:%d\n", nbname );*/
        for ( j = 0x00; j < nbname; j++ ) {
            GLuint name = (*ptr++);

          if ( name != 0xFFFFFFFF ) {
            uint32_t facID = name >> 0x10;
            uint32_t verID = name & 0x000FFFF;
            G3DFACE *fac = mes->faceindex[facID];
            SMOOTHVERTEX *smver = calloc ( 0x01, sizeof ( SMOOTHVERTEX ) );

            /*** prepare redrawing ***/
            if ( list_seek ( lfac, fac ) == NULL ) {
                list_insert ( &lfac, fac );
            }

            /*g3dface_evalSubdivision ( fac, mes->subdiv, &nbFaces, &nbEdges, &nbVertices );*/

            smver->verid =  verID;
            smver->rtver = &fac->rtvermem[verID];
            smver->scmap =  fac->sculptmap;

            list_insert ( &lsmver, smver );

            plane.pos.x += fac->rtvermem[verID].pos.x;
            plane.pos.y += fac->rtvermem[verID].pos.y;
            plane.pos.z += fac->rtvermem[verID].pos.z;

            plane.nor.x += fac->rtvermem[verID].nor.x;
            plane.nor.y += fac->rtvermem[verID].nor.y;
            plane.nor.z += fac->rtvermem[verID].nor.z;

            nbver++;

            /*list_insert ( &lis, ( void * ) cname );*/
            /*printf ("facID:%d verID:%d\n", facID, verID );*/
        }
      }
    }

    if ( nbver > 0x02 ) {
        LIST *ltmpsmver = lsmver;

        plane.pos.x /= nbver;
        plane.pos.y /= nbver;
        plane.pos.z /= nbver;

        plane.nor.x /= nbver;
        plane.nor.y /= nbver;
        plane.nor.z /= nbver;

        g3dvector_normalize ( &plane.nor, NULL );

        plane.d = - ( plane.nor.x * plane.pos.x ) + 
                    ( plane.nor.y * plane.pos.y ) +
                    ( plane.nor.z * plane.pos.z );

        /*G3DVERTEX *ver = g3dvertex_new ( plane.pos.x, plane.pos.y, plane.pos.z );

        ver->nor.x = plane.nor.x;
        ver->nor.y = plane.nor.y;
        ver->nor.z = plane.nor.z;
        ver->nor.w = 1.0f;
        ver->surface = 1.0f;

        g3dmesh_addVertex ( mes, ver  );*/

        while ( ltmpsmver ) {
            SMOOTHVERTEX *smver = ( SMOOTHVERTEX * ) ltmpsmver->data;
            float distance = ( plane.nor.x * smver->rtver->pos.x ) + 
                             ( plane.nor.y * smver->rtver->pos.y ) + 
                             ( plane.nor.z * smver->rtver->pos.z ) + plane.d;
/*printf("%f\n", distance );*/
            smver->scmap->points[smver->verid].x -= (plane.nor.x * distance * 0.01f);
            smver->scmap->points[smver->verid].y -= (plane.nor.y * distance * 0.01f);
            smver->scmap->points[smver->verid].z -= (plane.nor.z * distance * 0.01f);

            ltmpsmver = ltmpsmver->next;
        }
    }

    lextfac = g3dface_getNeighbourFacesFromList ( lfac );

    /*g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          COMPUTESUBDIVISION, engine_flags );*/


    ltmpfac = lextfac;
    /*ltmpfac = lfac;*/


    g3dmesh_fillSubdividedFaces ( mes, lextfac, engine_flags );

    /*while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t (*subindex)[0x04] = g3dsubindex_get ( fac->nbver, mes->subdiv );

        g3dsubdivisionV3_subdivide ( sdv, fac,
                                          fac->rtfacmem,
                                          fac->rtedgmem,
                                          fac->rtvermem,
                                          subindex,
                                          mes->subdiv,
                                          ((G3DOBJECT*)mes)->flags,
                                          engine_flags );

        ltmpfac = ltmpfac->next;
    }*/

    list_free ( &lextfac, NULL );
    list_free ( &lfac, NULL );

    list_free ( &lsmver, free );
}

/******************************************************************************/
uint32_t smoothTool_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                              G3DCAMERA *cam,
                                              G3DURMANAGER *urm, 
                                              uint32_t engine_flags ) {
    if ( mou->data ) {
        G3DSMOOTHTOOL *st = mou->data;
    } else {
        mou->data = smoothTool_new ( );
    }

    return 0x00;
}

/******************************************************************************/
static void smooth_free ( void *data ) {

}

/******************************************************************************/
int smooth_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                  G3DURMANAGER *urm, uint32_t engine_flags, G3DEvent *event ) {
    G3DSMOOTHTOOL *st = mou->data;
    static GLint VPX[0x04];
    static G3DOBJECT *obj;
    static GLdouble MVX[0x10], PJX[0x10];

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            obj = g3dscene_getLastSelected ( sce );

            /*** Smoothing is only available to buffer subdivided objects because ***/
            /*** we can only retrieve the Z values thanks to the coordinates stored***/
            /*** in the rtvermem buffer. ***/
            if ( ( obj->flags & BUFFEREDSUBDIVISION ) == 0x00 ) {
                printf ( stderr, "smooth_tool: Smoothing is only available to "
                                 "buffer subdivided objects" );

                obj = NULL;

                return 0x00;
            }

            /*glGetIntegerv ( GL_VIEWPORT, VPX );*/
        } break;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            G3DDOUBLEVECTOR nearvec, farvec;
            G3DVECTOR dir;

            st->ctrl_key = ( mev->state & G3DControlMask ) ? 0x01 : 0x00;

            if ( obj && ( obj->type & MESH ) ) {
                glPushMatrix ( );
                glMultMatrixd ( obj->wmatrix );
                glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX  );
                glPopMatrix ( );

                glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                glGetIntegerv ( GL_VIEWPORT, VPX );

                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) 0,
                               MVX, PJX, VPX,
                               &nearvec.x, &nearvec.y, &nearvec.z );

                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) 1,
                               MVX, PJX, VPX,
                               &farvec.x, &farvec.y, &farvec.z );

                dir.x = farvec.x - nearvec.x;
                dir.y = farvec.y - nearvec.y;
                dir.z = farvec.z - nearvec.z;

                g3dvector_normalize ( &dir, NULL );

                if ( engine_flags & VIEWSCULPT ) {
                    st->coord[0x00] = st->coord[0x02] = mev->x;
                    st->coord[0x01] = st->coord[0x03] = VPX[0x03] - mev->y;

                    G3DMESH *mes = ( G3DMESH * ) obj;

                    smooth_pick ( st, mes, cam, &dir, engine_flags );
                }
            }
        } break;

        case G3DButtonRelease : {
            obj = NULL;
        } break;

        default:
        break;
    }

    return REDRAWVIEW;
}