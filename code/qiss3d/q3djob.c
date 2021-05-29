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
#include <qiss3d/q3d.h>

/******************************************************************************/
Q3DFILTER *q3djob_getFilter ( Q3DJOB *qjob, const char *filtername ) {
    return NULL;
}

/******************************************************************************/
void q3djob_addJob ( Q3DJOB *qjob, 
                     Q3DJOB *subqjob ) {
    list_insert ( &qjob->lqjob, subqjob );
}

/******************************************************************************/
void q3djob_removeJob ( Q3DJOB *qjob,
                        Q3DJOB *subqjob ) {
    list_remove ( &qjob->lqjob, subqjob );
}

/******************************************************************************/
void q3djob_end ( Q3DJOB *qjob ) {
    qjob->running   = 0x00;
    qjob->cancelled = 0x01;
}

/******************************************************************************/
void q3djob_cancel ( Q3DJOB *qjob ) {
    LIST *ltmpqjob = qjob->lqjob;

    printf ( "Stopping rendering threads\n" );

    while ( ltmpqjob ) {
        Q3DJOB *subqjob = ( Q3DJOB * ) ltmpqjob->data;

        /*** wait for raytracing scanlines to finish ***/
        q3djob_end ( subqjob );

        ltmpqjob = ltmpqjob->next;
    }

    q3djob_end ( qjob );

    /*Rendering might not be started form a thread, so we have to handle that*/
    if ( qjob->threaded ) {
        pthread_join ( qjob->tid, NULL );
    }
}

/******************************************************************************/
/******** Wait the end of the render process, ie the end of each thread *******/
void q3djob_wait ( Q3DJOB *qjob ) {
    LIST *ltmp = qjob->lthread;

    while ( ltmp ) {
        pthread_t tid = ( pthread_t ) ltmp->data;

        pthread_join ( tid, NULL );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void q3djob_free ( Q3DJOB *qjob ) {
    /*** free all rendering threads - one per CPU ***/
    list_free ( &qjob->lthread, NULL );

    q3dobject_free_r ( ( Q3DOBJECT * ) qjob->qsce );

    if ( qjob->img ) free ( qjob->img );

    q3darea_reset ( &qjob->qarea );

    q3dobject_free   ( ( Q3DOBJECT * ) qjob->qcam );

    printf ("Q3DJOB Freed\n" );
}

/******************************************************************************/
static uint32_t q3djob_getNextLine ( Q3DJOB           *qjob,
                                     Q3DINTERPOLATION *pone,
                                     Q3DINTERPOLATION *ptwo ) {
    Q3DAREA *qarea = &qjob->qarea;
    uint32_t scanline;

    pthread_mutex_lock ( &qarea->lock );

    scanline = qarea->scanline++;

    memcpy ( &pone->src, &qarea->pol[0x00].src, sizeof ( Q3DVECTOR3F ) );
    memcpy ( &pone->dst, &qarea->pol[0x00].dst, sizeof ( Q3DVECTOR3F ) );

    memcpy ( &ptwo->src, &qarea->pol[0x01].src, sizeof ( Q3DVECTOR3F ) );
    memcpy ( &ptwo->dst, &qarea->pol[0x01].dst, sizeof ( Q3DVECTOR3F ) );

    /*** prepare the next call ***/
    q3dinterpolation_step ( &qarea->pol[0x00] );
    q3dinterpolation_step ( &qarea->pol[0x01] );

    pthread_mutex_unlock ( &qarea->lock  );

    return scanline;
}

/******************************************************************************/
void q3djob_filterline ( Q3DJOB *qjob, 
                                uint32_t from, 
                                uint32_t to,
                                uint32_t depth, 
                                uint32_t width ) {
    char *img = qjob->img;
    uint32_t i;

    if ( qjob->running == 0x00 ) return;

    for ( i = 0x00; i < MAXFILTERS; i++ ) {
        Q3DFILTER *fil = qjob->flin[i];

        if ( fil ) {
            if ( ( fil->flags & ENABLEFILTER ) && 
                 ( fil->type  & FILTERLINE   ) ) {

                if ( fil->draw ( fil, 
                                 qjob, 
                                 qjob->curframe, 
                                 img, 
                                 from, 
                                 to, 
                                 depth, 
                                 width ) ) {

                    /*** stop processing filters if 1 is returned ***/
                    return;
                }
            }
        }
    }
}

/******************************************************************************/
static void q3djob_filterimage ( Q3DJOB  *qjob, 
                                 uint32_t from, 
                                 uint32_t to,
                                 uint32_t depth, 
                                 uint32_t width ) {
    char *img = qjob->img;
    uint32_t i;

    if ( qjob->running == 0x00 ) return;

    for ( i = 0x00; i < MAXFILTERS; i++ ) {
        Q3DFILTER *fil = qjob->fimg[i];

        if ( fil ) {
            if ( ( fil->flags & ENABLEFILTER ) && 
                 ( fil->type  & FILTERIMAGE   ) ) {
                if ( fil->draw ( fil, 
                                 qjob, 
                                 qjob->curframe, 
                                 img, 
                                 from, 
                                 to, 
                                 depth, 
                                 width ) ) {
                    /*** stop processing filters if 1 is returned ***/
                    return;
                }
            }
        }
    }
}

/******************************************************************************/
static uint32_t q3djob_filterbefore ( Q3DJOB  *qjob, 
                                      uint32_t from, 
                                      uint32_t to,
                                      uint32_t depth, 
                                      uint32_t width ) {
    char *img = qjob->img;
    uint32_t i;

    if ( qjob->running == 0x00 ) return 0x00;

    for ( i = 0x00; i < MAXFILTERS; i++ ) {
        Q3DFILTER *fil = qjob->fbef[i];

        if ( fil ) {
            if ( ( fil->flags & ENABLEFILTER ) && 
                 ( fil->type  & FILTERBEFORE ) ) {
                uint32_t ret;

                if ( ret = fil->draw ( fil,
                                       qjob, 
                                       qjob->curframe, 
                                       img, 
                                       from, 
                                       to, 
                                       depth, 
                                       width ) ) {
                    /*** stop processing filters if 1 is returned ***/
                    return ret;
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void *q3djob_raytrace ( void *ptr ) {
    Q3DJOB *qjob = ( Q3DJOB * ) ptr;
    Q3DAREA *qarea = &qjob->qarea;
    Q3DINTERPOLATION pone, ptwo;
    unsigned char *img = qjob->img;
    uint32_t scanline;
    uint32_t steps  = ( qarea->x2 - qarea->x1 );
    uint32_t width  = ( qarea->x2 - qarea->x1 ) + 0x01;
    uint32_t height = ( qarea->y2 - qarea->y1 ) + 0x01;
    uint32_t bytesperline = ( width * 0x03 );
    uint32_t outlineFlag = ( qjob->qrsg->flags & RENDERWIREFRAME ) ? RAYQUERYOUTLINE : 0x00;
    int i;

    /*** return immediately when canceled ***/
    /*pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );*/

    while ( ( ( scanline = q3djob_getNextLine ( qjob,
                                               &pone,
                                               &ptwo ) ) <= qarea->y2 ) && qjob->running ) {
        unsigned char *imgptr = &img[(scanline*bytesperline)];

        q3dinterpolation_build ( &pone, &ptwo, steps );

        for ( i = qarea->x1; ( i <= qarea->x2 ) && qjob->running; i++ ) {
            uint32_t color;
            Q3DVECTOR3F intersectionPointToSource;
            Q3DRAY qray;
            float viewingDistance;
            uint32_t offset = ( scanline * qarea->width ) + i;

            /*** Ray building ***/
            /*** First, reset the ray ***/
            memset ( &qray, 0x00, sizeof ( Q3DRAY ) );

            qray.x = i;
            qray.y = scanline;

            /*** then set its source position ***/
            /*memcpy ( &ray.ori, &pone.src, sizeof ( R3DVECTOR ) );*/

            qray.src.x = pone.src.x;
            qray.src.y = pone.src.y;
            qray.src.z = pone.src.z;

            /*** and its destination vector ***/
            qray.dir.x = ( pone.dst.x - pone.src.x );
            qray.dir.y = ( pone.dst.y - pone.src.y );
            qray.dir.z = ( pone.dst.z - pone.src.z );

            /*** This value will be compared to the depth value of the hit  ***/
            /*** point. This allows us to pick the closest hit to the eye.  ***/
            qray.distance = INFINITY;
            qray.flags    = Q3DRAY_PRIMARY_BIT;

            /*** but don't forget to normalize the latter **/
            q3dvector3f_normalize ( &qray.dir, &viewingDistance );

            /*** shoot the ray ***/
            color = q3dray_shoot_r ( &qray,
                                      qjob,
                                      NULL,
                                      qjob->curframe,
                                      0x04,
                                      RAYQUERYHIT |
                                      RAYQUERYSURFACECOLOR   | 
                                      RAYQUERYLIGHTING       |
                                      RAYQUERYREFLECTION     |
                                      RAYQUERYREFRACTION     |
                                      RAYQUERYIGNOREBACKFACE/* |
                                      outlineFlag*/ );

            imgptr[0x00] = ( color & 0x00FF0000 ) >> 0x10;
            imgptr[0x01] = ( color & 0x0000FF00 ) >> 0x08;
            imgptr[0x02] = ( color & 0x000000FF );

#ifdef unused
            if ( qjob->qrsg->flags & RENDERFOG ) {
                uint32_t fogR = ( ( qjob->qrsg->fog.color & 0x00FF0000 ) >> 0x10 ),
                         fogG = ( ( qjob->qrsg->fog.color & 0x0000FF00 ) >> 0x08 ),
                         fogB =   ( qjob->qrsg->fog.color & 0x000000FF );

                float fogDistance = qarea->zBuffer[offset];
                float fogNear = qjob->qrsg->fog.fnear;
                float fogFar = qjob->qrsg->fog.ffar;
                float deltaFog = ( fogFar - fogNear );
                float incFogRatio = ( deltaFog ) ? ( fogDistance - 
                                                     fogNear ) / deltaFog : 0.0f;
                /*** note: fog strength is between 0.0f and 1.0f ***/
                float decFogRatio = qjob->qrsg->fog.strength - incFogRatio;

                if ( fogDistance < fogNear ) {
                    incFogRatio = 0.0f;
                    decFogRatio = 1.0f;
                }

                if ( fogDistance > fogNear ) {
                    if ( incFogRatio > 1.0f ) incFogRatio = 1.0f;

                    incFogRatio = incFogRatio * qjob->qrsg->fog.strength;
                    decFogRatio = 1.0f - incFogRatio;
                }

                uint32_t R = ( imgptr[0x00] * decFogRatio ) + ( fogR * incFogRatio );
                uint32_t G = ( imgptr[0x01] * decFogRatio ) + ( fogG * incFogRatio );
                uint32_t B = ( imgptr[0x02] * decFogRatio ) + ( fogB * incFogRatio );

                if ( R > 0xFF ) R = 0xFF;
                if ( G > 0xFF ) G = 0xFF;
                if ( B > 0xFF ) B = 0xFF;

                imgptr[0x00] = R;
                imgptr[0x01] = G;
                imgptr[0x02] = B;
            }
#endif

            imgptr += 0x03;

            /*** be ready for the next ray ***/
            q3dinterpolation_step ( &pone );
        }

        q3djob_filterline ( qjob, scanline, scanline + 0x01, 0x18, width );
    }

    /*** this is needed for memory release ***/
    /*pthread_exit ( NULL );*/


    return NULL;
}

/******************************************************************************/
void q3djob_createRenderThread ( Q3DJOB *qjob ) {
    pthread_attr_t attr;
    pthread_t tid;

    pthread_attr_init ( &attr );

    /*** start thread son all CPUs ***/
    pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

    /*** launch rays ***/
    pthread_create ( &tid, &attr, q3djob_raytrace, qjob );

    /*** register our thread for q3djob_wait() can use pthread_join() ***/
    list_insert ( &qjob->lthread, ( void * ) tid );
}

/******************************************************************************/
static void q3djob_initFilters ( Q3DJOB *qjob ) {
    Q3DSETTINGS *qrsg   = qjob->qrsg;
    Q3DFILTER *toimg    = NULL;
    Q3DFILTER *toffmpeg = NULL;
    Q3DFILTER *towindow = qrsg->input.towindow;
    Q3DFILTER *simpleAA = q3dfilter_simpleaa_new ( );

    if ( qrsg->flags & RENDERSAVE ) {
        if ( qrsg->output.format == RENDERTOVIDEO ) {
            G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
            char buf[0x1000];

            snprintf ( buf, 0x1000, "%s.avi", qrsg->output.outfile );

            toffmpeg = q3dfilter_toFfmpeg_new ( 0x00, 
                                                qrsg->output.width,
                                                qrsg->output.height,
                                                qrsg->output.depth,
                                                qrsg->output.fps,
                                                qrsg->output.endframe - 
                                                qrsg->output.startframe,
    #ifdef __MINGW32__
                                               &gui->cvars,
    #endif
                                                buf,
                                                sysinfo->ffmpegPath,
                                                sysinfo->ffplayPath );

            if ( toffmpeg == NULL ) fprintf ( stderr, "FFMpeg not found!\n");
        }

        if ( qrsg->output.format == RENDERTOIMAGE ) {
            static char buf[0x1000];

            snprintf ( buf, 0x1000, "%s.jpg", qrsg->output.outfile );

            if ( qrsg->output.startframe != qrsg->output.endframe ) {
                toimg = q3dfilter_writeImage_new ( buf, 0x01 );
            } else {
                toimg = q3dfilter_writeImage_new ( buf, 0x00 );
            }
        }
    }

    /*** line filters ***/
    qjob->flin[MAXFILTERS - 0x01] = qrsg->input.towindow;

    /*** image filters ***/
    qjob->fimg[MAXFILTERS - 0x05] = simpleAA; /*** simple anti aliasing ***/
    qjob->fimg[MAXFILTERS - 0x04] = towindow; /*** output to widget    ***/
    qjob->fimg[MAXFILTERS - 0x03] = toffmpeg; /*** output to ffmpeg    ***/
    qjob->fimg[MAXFILTERS - 0x02] = toimg;    /*** write image to disk ***/
    qjob->fimg[MAXFILTERS - 0x01] = qrsg->input.clean;

    /*** before filters ***/

}

/******************************************************************************/
Q3DJOB *q3djob_new ( Q3DSETTINGS *qrsg, 
                     G3DSCENE    *sce,
                     G3DCAMERA   *cam,
                     uint64_t     flags ) {
    uint32_t structsize = sizeof ( Q3DJOB );
    uint32_t bytesperline = ( qrsg->output.width * 0x03 );
    Q3DJOB *qjob;
    int i;

    if ( ( qjob = ( Q3DJOB * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    qjob->qrsg = qrsg;

    /*** This 24bpp buffer will receive the raytraced pixel values ***/
    if ( ( qjob->img = calloc ( qrsg->output.height, bytesperline ) ) == NULL ) {
        fprintf ( stderr, "%s: image memory allocation failed\n", __func__ );

        free ( qjob );

        return NULL;
    }

    qjob->curframe = qrsg->output.startframe;

    qjob->running = 0x01;

    qjob->qsce = q3dscene_import ( sce, qjob->curframe );

    qjob->qcam = q3dcamera_new ( cam, qrsg->output.width, 
                                      qrsg->output.height );

    q3darea_init ( &qjob->qarea,
                    qjob->qsce,
                    qjob->qcam,
                    qrsg->output.x1,
                    qrsg->output.y1,
                    qrsg->output.x2,
                    qrsg->output.y2,
                    qrsg->output.width,
                    qrsg->output.height,
                    0x18,
                    qjob->curframe );

    q3djob_initFilters ( qjob );


    return qjob;
}

/******************************************************************************/
void q3djob_render_t_free ( Q3DJOB *qjob ) {
    /*** free filters after rendering ***/
    if ( ( qjob->flags & NOFREEFILTERS ) == 0x00 ) {
/*
        list_free ( &qjob->qrsg->input.lfilters, (void(*)(void*))q3dfilter_free );
*/
    }
}

/******************************************************************************/
void *q3djob_render_sequence_t ( Q3DJOB *qjob ) {
    G3DSCENE *sce  = ( G3DSCENE *  ) q3dobject_getObject ( ( Q3DOBJECT * ) qjob->qsce );
    G3DCAMERA *cam = ( G3DCAMERA * ) q3dobject_getObject ( ( Q3DOBJECT * ) qjob->qcam );
    uint32_t x1 = qjob->qarea.x1, 
             y1 = qjob->qarea.y1,
             x2 = qjob->qarea.x2,
             y2 = qjob->qarea.y2;
    uint32_t width  = qjob->qarea.width,
             height = qjob->qarea.height;
    int32_t startframe = qjob->qrsg->output.startframe,
            endframe = qjob->qrsg->output.endframe;
    int32_t i, j;

    qjob->threaded = 0x01;

    /*** Render the first frame ***/
    q3djob_render ( qjob );

    for ( i = startframe + 1; i <= endframe; i++ ) {
        if ( qjob->running ) {
            Q3DJOB *nextqjob;

            nextqjob = q3djob_new ( qjob->qrsg, sce, cam, NOFREEFILTERS );

            nextqjob->curframe = i;

            /*** register this child rendeqjobne in case we need to cancel it ***/
            q3djob_addJob ( qjob, nextqjob );

            /*** Render the current frame ***/
            q3djob_render ( nextqjob );

            /*** unregister this child rendeqjobne. No need to cancel it ***/
            /*** anymore after this step, all threads are over after ***/
            /*** q3djob_render().  ***/
            q3djob_removeJob ( qjob, nextqjob );

            /*** Free the current frame ***/
            q3dobject_free  ( ( Q3DOBJECT * ) nextqjob );
        }
    }

    qjob->running = 0x00;

    /*** this is needed for memory release ***/
    pthread_exit ( NULL );


    return NULL;
}

/******************************************************************************/
void *q3djob_render_frame_t ( Q3DJOB *qjob ) {
    qjob->threaded = 0x01;

    /*** RENDER ! ***/
    q3djob_render ( qjob );

    qjob->running = 0x00;




    return NULL;
}

/******************************************************************************/
void q3djob_render ( Q3DJOB *qjob ) {
    uint32_t nbcpu = g3dcore_getNumberOfCPUs ( );
    clock_t t = clock ( );  
    int i;
    uint32_t doRender;

    /*** http://www.thegeekstuff.com/2012/05/c-mutex-examples/ ***/
    /*** This mutex is used when each rendering thread ***/
    /*** is querying for the next scanline to render   ***/
    if ( pthread_mutex_init ( &qjob->qarea.lock, NULL ) != 0x00 ) {
        fprintf ( stderr, "mutex init failed\n" );

        return;
    }

    doRender = q3djob_filterbefore ( qjob, 
                                     0x00, 
                                     0x00,
                                     qjob->qarea.depth, 
                                     qjob->qarea.width );

    if ( doRender != 0x02 ) {
        /*** Start as many threads as CPUs. Each thread queries the parent ***/
        /*** process in order to retrieve the scanline number it must render. ***/
        for ( i = 0x00; i < nbcpu; i++ ) {
            q3djob_createRenderThread ( qjob );
        }

        /*** wait all render threads to finish ***/ 
        q3djob_wait ( qjob );
    }

    q3djob_filterimage ( qjob, 
                         qjob->qarea.y1, 
                         qjob->qarea.y2,
                         qjob->qarea.depth, 
                         qjob->qarea.width );

    t = clock() - t;

    printf ("Render took %f seconds.\n", ( float ) t / ( CLOCKS_PER_SEC * nbcpu ) );
}