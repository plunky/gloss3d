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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/
/******************************************************************************/

void (*myGLActiveTexture)( GLenum texture ) = NULL;

static int pen_tool ( G3DMOUSETOOL *mou, 
                      G3DSCENE     *sce, 
                      G3DCAMERA    *cam,
                      G3DURMANAGER *urm,
                      uint64_t engine_flags, 
                      G3DEvent     *event );
static int eraser_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm,
                         uint64_t engine_flags, 
                         G3DEvent     *event );
static void selector_draw ( G3DMOUSETOOL *gtool, 
                            G3DSCENE     *sce, 
                            uint64_t engine_flags );

/******************************************************************************/
M3DMOUSETOOLBUCKET *m3dmousetoolbucket_new ( ) {
    uint32_t structsize = sizeof ( M3DMOUSETOOLBUCKET );
    void *memarea = calloc ( 0x01, structsize );
    M3DMOUSETOOLBUCKET *mtb =  ( M3DMOUSETOOLBUCKET * ) memarea;
    M3DMOUSETOOL *ltool = ( M3DMOUSETOOL * ) mtb;

    if ( mtb == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    m3dmousetool_init ( ltool,
                        BUCKETTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        pen_tool,
                        MOUSETOOLREADWRITE );

    ltool->obj = ( M3DOBJECT * ) m3dbucket_new ( );

    return mtb;
}

/******************************************************************************/
M3DMOUSETOOLSELECTOR *m3dmousetoolselect_new ( ) {
    uint32_t structsize = sizeof ( M3DMOUSETOOLSELECTOR );
    void *memarea = calloc ( 0x01, structsize );
    M3DMOUSETOOLSELECTOR *sr =  ( M3DMOUSETOOLSELECTOR * ) memarea;
    M3DMOUSETOOL *ltool = ( M3DMOUSETOOL * ) sr;

    if ( sr == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    m3dmousetool_init ( ltool,
                        SELECTTOOL,
                        's',
                        NULL,
                        NULL,
                        selector_draw,
                        pen_tool,
                        MOUSETOOLREADONLY );

    ltool->obj = ( M3DOBJECT * ) m3dselector_new ( );

    return sr;
}

/******************************************************************************/
M3DMOUSETOOLPEN *m3dmousetoolpen_new ( ) {
    uint32_t structsize = sizeof ( M3DMOUSETOOLPEN );
    void *memarea = calloc ( 0x01, structsize );
    M3DMOUSETOOLPEN *pn =  ( M3DMOUSETOOLPEN * ) memarea;
    M3DMOUSETOOL *ltool = ( M3DMOUSETOOL * ) pn;

    if ( pn == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    m3dmousetool_init ( ltool,
                        PENTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        pen_tool,
                        MOUSETOOLREADWRITE );

    ltool->obj = ( M3DOBJECT * ) m3dbasepen_new ( );

    return pn;
}

/******************************************************************************/
M3DMOUSETOOLERASER *m3dmousetooleraser_new ( ) {
    uint32_t structsize = sizeof ( M3DMOUSETOOLERASER );
    void *memarea = calloc ( 0x01, structsize );
    M3DMOUSETOOLERASER *er =  ( M3DMOUSETOOLERASER * ) memarea;
    M3DMOUSETOOL *ltool = ( M3DMOUSETOOL * ) er;

    if ( er == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    m3dmousetool_init ( ltool,
                        ERASERTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        eraser_tool,
                        MOUSETOOLREADWRITE );

    ltool->obj = ( M3DOBJECT * ) m3dbasepen_new ( );


    return er;
}

/******************************************************************************/
static void selector_draw ( G3DMOUSETOOL *gtool, 
                            G3DSCENE     *sce, 
                            uint64_t engine_flags ) {
    M3DMOUSETOOLSELECTOR *sr =  ( M3DMOUSETOOLSELECTOR * ) gtool;
    M3DMOUSETOOL *ltool = ( M3DMOUSETOOL * ) sr;
    M3DSELECTOR *sel = ( M3DSELECTOR * ) ltool->obj;
    LIST *ltmplin = sel->llines;
    double sx[0x04], sy[0x04], sz[0x04];

    glPushAttrib ( GL_ENABLE_BIT ); 

    glDisable ( GL_DEPTH_TEST );
    glColor3ub ( 0xFF, 0xFF, 0xFF );
    glLineStipple ( 1, 0x5555 );
    glEnable ( GL_LINE_STIPPLE );
    glBegin ( GL_LINES );
    while ( ltmplin ) {
        M3DSELECTORLINE *lin = ( M3DSELECTORLINE * ) ltmplin->data;

        glVertex3f ( lin->srcpt->u, lin->srcpt->v, 0.0f );
        glVertex3f ( lin->dstpt->u, lin->dstpt->v, 0.0f );

        ltmplin = ltmplin->next;
    }
    glEnd ( );

    ltmplin = sel->llines;

    glColor3ub ( 0x00, 0x00, 0x00 );
    glLineStipple ( 1, 0x6666 );
    glEnable ( GL_LINE_STIPPLE );
    glBegin ( GL_LINES );
    while ( ltmplin ) {
        M3DSELECTORLINE *lin = ( M3DSELECTORLINE * ) ltmplin->data;

        glVertex3f ( lin->srcpt->u, lin->srcpt->v, 0.0f );
        glVertex3f ( lin->dstpt->u, lin->dstpt->v, 0.0f );

        ltmplin = ltmplin->next;
    }
    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
int basepen_tool ( G3DMOUSETOOL *mou, 
                   G3DSCENE     *sce, 
                   G3DCAMERA    *cam,
                   G3DURMANAGER *urm,
                   M3DPATTERN   *pattern,
                   uint32_t      fgcolor,
                   uint32_t      bgcolor,
                   uint64_t engine_flags, 
                   G3DEvent     *event ) {
    /*** selection rectangle coords ***/
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static G3DOBJECT *obj;
    static G3DMESH *mes;
    M3DMOUSETOOL *ltool = ( M3DMOUSETOOL * ) mou;
    M3DMOUSETOOLPEN *pen = ( M3DMOUSETOOLPEN * ) mou;
    double mx, my, mz;
    int32_t updcoord[0x04];
    static double oldx, oldy;
    static G3DIMAGE *bckimg; /** image for undo redo ***/
    static  int32_t x1, y1, x2, y2;

    if ( myGLActiveTexture == NULL ) {
#ifdef __MINGW32__
        myGLActiveTexture = wglGetProcAddress( "glActiveTexture" );
#endif
#ifdef __linux__
	    myGLActiveTexture = glActiveTexture;
#endif
    }

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        uint32_t channelID = GETCHANNEL(engine_flags);
                        G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                        if ( chn ) {
                            if ( chn->flags & USEIMAGECOLOR ) {
                                if ( chn->image ) {
                                    uint32_t retval;
                                    gluUnProject ( bev->x, 
                                                   VPX[0x03] - bev->y, 
                                                   0.0f,
                                                   MVX,
                                                   PJX,
                                                   VPX,
                                                  &mx,
                                                  &my,
                                                  &mz );

                                    memset ( mou->zbuffer, 
                                             0x00, 
                                             chn->image->width * 
                                             chn->image->height);

                                    bckimg = g3dimage_copy ( chn->image );

                                    retval = ltool->obj->press ( ltool->obj,
                                                                 pattern,
                                                                 fgcolor,
                                                                 bgcolor,
                                                                 mx * chn->image->width,
                                                                 my * chn->image->height,
                                                                 chn->image->data,
                                                                 chn->image->width,
                                                                 chn->image->height,
                                                                 chn->image->bytesPerPixel * 0x08,
                                                                 mou->mask,
                                                                 mou->zbuffer,
                                                                 updcoord,
                                                                 0x00 );

                                    x1 = updcoord[0x00];
                                    y1 = updcoord[0x01];
                                    x2 = updcoord[0x02];
                                    y2 = updcoord[0x03];

                                    if ( ( retval & M3DUPDATESUBIMAGE ) || 
                                         ( retval & M3DUPDATEIMAGE    ) ) {
                                        g3dimage_bind ( chn->image );

                                        chn->image->flags |= ALTEREDIMAGE;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            oldx = bev->x;
            oldy = bev->y;
        } return REDRAWVIEW | REDRAWUVMAPEDITOR;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        uint32_t channelID = GETCHANNEL(engine_flags);
                        G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                        if ( chn ) {
                            if ( chn->flags & USEIMAGECOLOR ) {
                                if ( chn->image ) {
                                    G3DIMAGE *image = chn->image;
                                    uint32_t retval = 0x00;
                                    uint32_t m3dFlags = 0x00;

                                    m3dFlags |= ( mev->state & G3DButton1Mask ) ? M3DBUTTON1PRESSED : 0x00;

                                    gluUnProject ( mev->x, 
                                                   VPX[0x03] - mev->y, 
                                                   0.0f,
                                                   MVX,
                                                   PJX,
                                                   VPX,
                                                  &mx,
                                                  &my,
                                                  &mz );

                                    if ( pen->incremental ) {
                                        if ( mev->state & G3DButton1Mask ) {
                                            static double oldVector[0x02];
                                                   double newVector[0x02];

                                            newVector[0x00] = mev->x - oldx;
                                            newVector[0x01] = mev->y - oldy;

                                            /*** If we are going in the opposite direction, then reset the zbuffer. ***/
                                            /*** This allows for a second pass of the pencil ***/
                                            if ( ( ( newVector[0x00] * 
                                                     oldVector[0x00] ) +
                                                   ( newVector[0x01] * 
                                                     oldVector[0x01] ) ) < 0x00 ) {
                                                memset ( mou->zbuffer, 
                                                         0x00, 
                                                         image->width * image->height );
                                            }

                                            oldVector[0x00] = newVector[0x00];
                                            oldVector[0x01] = newVector[0x01];
                                        }
                                    }

                                    retval = ltool->obj->move ( ltool->obj,
                                                                pattern,
                                                                fgcolor,
                                                                bgcolor,
                                                                mx * image->width,
                                                                my * image->height,
                                                                image->data,
                                                                image->width,
                                                                image->height,
                                                                image->bytesPerPixel * 0x08,
                                                                mou->mask,
                                                                mou->zbuffer,
                                                                updcoord,
                                                                m3dFlags );

                                    if ( updcoord[0x00] < x1 ) x1 = updcoord[0x00];
                                    if ( updcoord[0x01] < y1 ) y1 = updcoord[0x01];
                                    if ( updcoord[0x02] > x2 ) x2 = updcoord[0x02];
                                    if ( updcoord[0x03] > y2 ) y2 = updcoord[0x03];

                                    if ( retval & M3DUPDATESUBIMAGE ) {
                                        uint32_t updw = updcoord[0x02] - 
                                                        updcoord[0x00] + 0x01,
                                                 updh = updcoord[0x03] - 
                                                        updcoord[0x01] + 0x01;


                                        glEnable ( GL_TEXTURE_2D );
                                        glBindTexture ( GL_TEXTURE_2D, image->id );
                                        myGLActiveTexture( image->id ); 
                                        glPixelStorei ( GL_UNPACK_ROW_LENGTH, image->width );
                                        glTexSubImage2D ( GL_TEXTURE_2D, 
                                                          0x00,
                                                          updcoord[0x00],
                                                          updcoord[0x01],
                                                          updw,
                                                          updh,
                                                          GL_RGB,
                                                          GL_UNSIGNED_BYTE,
                                                          image->data + ( updcoord[0x01] * image->bytesPerLine ) + ( updcoord[0x00] * image->bytesPerPixel ) );
                                        glBindTexture ( GL_TEXTURE_2D, 0x00 );
                                        glDisable ( GL_TEXTURE_2D );
                                    }

                                    if ( ( retval & M3DUPDATESUBIMAGE ) || 
                                         ( retval & M3DUPDATEIMAGE    ) ) {
                                        chn->image->flags |= ALTEREDIMAGE;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            oldx = mev->x;
            oldy = mev->y;
        } return REDRAWUVMAPEDITOR | REDRAWVIEW;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        uint32_t channelID = GETCHANNEL(engine_flags);
                        G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                        if ( chn ) {
                            if ( chn->flags & USEIMAGECOLOR ) {
                                if ( chn->image ) {
                                    uint32_t retval;

                                    gluUnProject ( bev->x, 
                                                   VPX[0x03] - bev->y, 
                                                   0.0f,
                                                   MVX,
                                                   PJX,
                                                   VPX,
                                                  &mx,
                                                  &my,
                                                  &mz );

                                    retval = ltool->obj->release ( ltool->obj,
                                                                   pattern,
                                                                   fgcolor,
                                                                   bgcolor,
                                                                   mx * chn->image->width,
                                                                   my * chn->image->height,
                                                                   chn->image->data,
                                                                   chn->image->width,
                                                                   chn->image->height,
                                                                   chn->image->bytesPerPixel * 0x08,
                                                                   mou->mask,
                                                                   mou->zbuffer,
                                                                   updcoord,
                                                                   0x00 );

                                    if ( ( retval & M3DUPDATESUBIMAGE ) || 
                                         ( retval & M3DUPDATEIMAGE    ) ) {
                                        g3dimage_bind ( chn->image );

                                        chn->image->flags |= ALTEREDIMAGE;

                                        g3durm_image_paint ( urm, 
                                                             chn->image,
                                                             bckimg,
                                                             x1,
                                                             y1,
                                                             x2,
                                                             y2,
                                                             REDRAWVIEW );

                                        g3dimage_free ( bckimg );
                                    }
                                }
                            }
                        }

                        if ( channelID == DISPLACEMENTCHANNELID ) {
                        /** TODO: do this only for subdivided meshes ***/
                            g3dmaterial_updateMeshes ( mat, 
                                                       sce, 
                                                       engine_flags );
                        }
                    }
                }
            }

            bckimg = NULL;

            /*obj = NULL; */
        } return REDRAWVIEW | REDRAWUVMAPEDITOR;

        default :
        break;
    }

    return 0x00;
}

/******************************************************************************/
static int pen_tool ( G3DMOUSETOOL *mou, 
                      G3DSCENE     *sce, 
                      G3DCAMERA    *cam,
                      G3DURMANAGER *urm,
                      uint64_t      engine_flags, 
                      G3DEvent     *event ) {
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    return basepen_tool ( mou, 
                          sce,
                          cam,
                          urm,
                          sysinfo->pattern,
                          sysinfo->fgcolor,
                          sysinfo->bgcolor, 
                          engine_flags, 
                          event );
}

/******************************************************************************/
static int eraser_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm,
                         uint64_t engine_flags, 
                         G3DEvent     *event ) {
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    return basepen_tool ( mou, 
                          sce,
                          cam,
                          urm,
                          sysinfo->pattern,
                          0xFFFFFFFF,
                          0x00000000, 
                          engine_flags, 
                          event );
}
