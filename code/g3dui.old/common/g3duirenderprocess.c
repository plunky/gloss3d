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
#include <g3dui.h>

/******************************************************************************/
void g3duirenderprocess_savejpg ( G3DUIRENDERPROCESS *rps, char *filename ) {
    g3dcore_writeJpeg ( filename,
                        rps->qjob->qarea.width,
                        rps->qjob->qarea.height,
                        rps->qjob->qarea.depth,
                        rps->qjob->img );
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3duirenderprocess_new ( uint64_t  id ) {
    uint32_t size = sizeof ( G3DUIRENDERPROCESS );
    G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) calloc ( 0x01, size );

    if ( rps == NULL ) {
        fprintf ( stderr, "g3duirenderprocess_new: mem allocation failed\n" );

        return NULL;
    }

    rps->id   = id;

    return rps;
}

/******************************************************************************/
void g3duirenderprocess_init ( G3DUIRENDERPROCESS *rps,
                               G3DUI              *gui,
                               Q3DJOB             *qjob,
                               Q3DFILTER          *towindow,
                               Q3DFILTER          *toframe ) {
    rps->qjob = qjob;

    rps->gui = gui;

    rps->towindow = towindow;
    rps->toframe = toframe;
}

/******************************************************************************/
void g3duirenderprocess_free ( G3DUIRENDERPROCESS *rps ) {
    /*** COMMENTED OUT: this is freed by Q3DJOB ***/
    /*q3dfilter_free ( rps->towindow );*/

    /*** Commented out: must not be freed or expect a crash when gotoframe ***/
    /*** is called due to the RPS structure being freed before the call ***/
    /*** cf g3duicom.c ***/
    /*q3dfilter_free ( rps->toframe );*/ 

    printf ( "freeing render process #%d\n", rps->id );

    list_remove ( &rps->gui->lrps, rps );

    if ( rps->filename ) free ( rps->filename );

    free ( rps );
}

/******************************************************************************/
void *g3duirenderprocess_render_frame_t ( G3DUIRENDERPROCESS *rps ) {
    uint32_t qjob_flags = rps->qjob->flags;

    rps->gui->engine_flags |= LOADFULLRESIMAGES;

    q3djob_render_frame ( rps->qjob );

    rps->gui->engine_flags &= (~LOADFULLRESIMAGES);

    if ( qjob_flags & JOBFREEONCOMPLETION ) {
        g3duirenderprocess_free ( rps );
    }

    /*** this is needed for memory release ***/
    pthread_exit ( NULL );

    return NULL;
}

/******************************************************************************/
void *g3duirenderprocess_render_sequence_t ( G3DUIRENDERPROCESS *rps ) {
    uint32_t qjob_flags = rps->qjob->flags;

    rps->gui->engine_flags |= LOADFULLRESIMAGES;

    q3djob_render_sequence ( rps->qjob );

    rps->gui->engine_flags &= (~LOADFULLRESIMAGES);

    if ( qjob_flags & JOBFREEONCOMPLETION ) {
        g3duirenderprocess_free ( rps );
    }

    /*** this is needed for memory release ***/
    pthread_exit ( NULL );

    return NULL;
}

/******************************************************************************/
void g3duirenderprocess_filename ( G3DUIRENDERPROCESS *rps, char *filename ) {
    uint32_t len = strlen ( filename ) + 0x01;

    rps->filename = realloc ( rps->filename, len );

    memset ( rps->filename, 0x00, len );

    memcpy ( rps->filename, filename, len );
}
