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
#include <g3d.h>

#ifdef UNUSED
    if ( mes->subdiv && ( objmes->flags & BUFFEREDSUBDIVISION ) ) {
        /*** only update subdivided polygons from the just picked vertices ***/
        LIST *lsub = g3dvertex_getFacesFromList ( lselver );

        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              lsub,
                              COMPUTESUBDIVISION, engine_flags );


        list_free ( &lsub, NULL );
    }
#endif

/******************************************************************************/
void g3dsubdivider_setSyncSubdivision ( G3DSUBDIVIDER *sdr ) {
    ((G3DOBJECT*) sdr)->flags |= SYNCSUBDIVISION;

    sdr->subdiv_render = sdr->subdiv_preview;
}

/******************************************************************************/
void g3dsubdivider_unsetSyncSubdivision ( G3DSUBDIVIDER *sdr ) {
    ((G3DOBJECT*) sdr)->flags &= (~SYNCSUBDIVISION);
}


/*
                if ( ( obj->flags & MESHUSEISOLINES ) ) {
                    g3dmesh_drawSubdividedObject ( mes, engine_flags | NODRAWPOLYGON );
                }
*/

/*
                    if ( viewSkin == 0x00 ) {
                        fac->flags |= FACESUBDIVIDED;

                        if ( obj->flags & OBJECTSELECTED ) {
                            g3dface_bindMaterials ( fac, mes->ltex, NULL, engine_flags );
                        } else {
                            g3dface_bindMaterials ( fac, mes->ltex, NULL, engine_flags & (~VIEWFACE) );
                        }
                    }/*

/******************************************************************************/
G3DMESH *g3dsubdivider_commit ( G3DSUBDIVIDER *sdr, 
                                uint32_t       commitMeshID,
                                unsigned char *commitMeshName,
                                uint32_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );
    G3DMESH *commitMesh = g3dmesh_new ( commitMeshID,
                                        commitMeshName, 
                                        engine_flags );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        G3DOBJECT *obj = ( G3DOBJECT * ) mes;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
        LIST *ltmpfac = mes->lfac;
        uint32_t nbCommitFac = mes->nbfac * pow(4,sdr->subdiv_preview); /*** need arrays big enough even if bigger ***/
        uint32_t nbCommitEdg = nbCommitFac * 0x04; /*** need arrays big enough even if bigger ***/
        uint32_t nbCommitVer = nbCommitFac * 0x04; /*** need arrays big enough even if bigger ***/
        G3DVERTEX **commitVertices;
        G3DEDGE   **commitEdges;
        G3DFACE   **commitFaces;
        uint32_t nbrtfac;
        uint32_t i;
        uint32_t quaFaces, quaEdges, quaVertices;


        g3dquad_evalSubdivision ( sdr->subdiv_preview, &quaFaces, 
                                                       &quaEdges,
                                                       &quaVertices );

        g3dmesh_renumberVertices ( mes );
        g3dmesh_renumberEdges    ( mes );
        g3dmesh_renumberFaces    ( mes );

        commitVertices = ( G3DVERTEX ** ) calloc ( nbCommitVer, sizeof ( G3DVERTEX * ) );
        commitEdges    = ( G3DEDGE   ** ) calloc ( nbCommitEdg, sizeof ( G3DEDGE   * ) );
        commitFaces    = ( G3DFACE   ** ) calloc ( nbCommitFac, sizeof ( G3DFACE   * ) );

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, mes,
                                                        fac,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        NULL,
                                                        commitVertices,
                                                        commitEdges,
                                                        commitFaces,
                                                        mes->ltex,
                                                        g3dsubindex_get ( 0x04, sdr->subdiv_preview ),
                                                        g3dsubindex_get ( 0x03, sdr->subdiv_preview ),
                                                        sdr->subdiv_preview,
                                                        SUBDIVISIONCOMMIT,
                                                        engine_flags );

            ltmpfac = ltmpfac->next;
        }

        for ( i = 0x00; i < nbCommitVer; i++ ) {
            if ( commitVertices[i] ) {
                g3dmesh_addVertex ( commitMesh, commitVertices[i] );
            }
        }

        for ( i = 0x00; i < nbCommitFac; i++ ) {
            if ( commitFaces[i] ) {
                g3dmesh_addFace ( commitMesh, commitFaces[i] );
            }
        }

        for ( i = 0x00; i < nbCommitEdg; i++ ) {
            if ( commitEdges[i] ) {
                g3dmesh_addEdge ( commitMesh, commitEdges[i] );
            }
        }

        g3dobject_importTransformations ( commitMesh, mes );

        g3dmesh_updateBbox ( commitMesh );
 
        g3dmesh_update ( commitMesh, NULL, /*** Recompute vertices    ***/
                                     NULL, /*** Recompute faces       ***/
                                     UPDATEFACEPOSITION |
                                     UPDATEFACENORMAL   |
                                     UPDATEVERTEXNORMAL |
                                     COMPUTEUVMAPPING,
                                     engine_flags );

        free( commitVertices );
        free( commitEdges    );
        free( commitFaces    );
    }

    return commitMesh;
}

/******************************************************************************/
void g3dsubdivider_fillBuffers ( G3DSUBDIVIDER *sdr, LIST *lfac,
                                                     uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        #define MAX_SUBDIVISION_THREADS 0x20
        G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
        LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        G3DSUBDIVISIONTHREAD std[MAX_SUBDIVISION_THREADS];
        pthread_t tid[MAX_SUBDIVISION_THREADS]; /*** let's say, max 32 threads ***/
        pthread_attr_t attr;
        uint32_t i;
        uint32_t engine_flags = flags;

        if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
            /*** Force the flag in case our mesh does not need displacement ***/
            engine_flags |= NODISPLACEMENT;
        }

        /*** init face list ***/
        g3dmesh_getNextFace ( mes, ltmpfac );

        if ( sif->nbcpu < 0x02 ) {
            uint32_t cpuID = 0x00;
            g3dsubdivisionthread_init ( &std[0x00], mes,
                                                    sdr->rtvermem,
                                                    sdr->nbrtver,
                                                    sdr->rtedgmem,
                                                    sdr->nbrtedg,
                                                    sdr->rtquamem,
                                                    sdr->nbrtfac,
                                                    sdr->rtuvmem,
                                                    sdr->nbrtuv,
                                                    sdr->nbVerticesPerTriangle,
                                                    sdr->nbVerticesPerQuad,
                                                    sdr->nbEdgesPerTriangle,
                                                    sdr->nbEdgesPerQuad,
                                                    sdr->nbFacesPerTriangle,
                                                    sdr->nbFacesPerQuad,
                                                    cpuID,
                                                    sdr->subdiv_preview,
                                                    engine_flags );

            g3dsubdivisionV3_subdivide_t ( &std[0x00] );
        } else {
            pthread_attr_init ( &attr );

            /*** start threads on all CPUs ***/
            pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

            for ( i = 0x00; i < sif->nbcpu; i++ ) {
                uint32_t cpuID = i;
                g3dsubdivisionthread_init ( &std[i], mes,
                                                     sdr->rtvermem,
                                                     sdr->nbrtver,
                                                     sdr->rtedgmem,
                                                     sdr->nbrtedg,
                                                     sdr->rtquamem,
                                                     sdr->nbrtfac,
                                                     sdr->rtuvmem,
                                                     sdr->nbrtuv,
                                                     sdr->nbVerticesPerTriangle,
                                                     sdr->nbVerticesPerQuad,
                                                     sdr->nbEdgesPerTriangle,
                                                     sdr->nbEdgesPerQuad,
                                                     sdr->nbFacesPerTriangle,
                                                     sdr->nbFacesPerQuad,
                                                     cpuID,
                                                     sdr->subdiv_preview,
                                                     engine_flags | 
                                                     G3DMULTITHREADING  );

                pthread_create ( &tid[i], 
                                 &attr, 
                                 (void * (*)(void *))g3dsubdivisionV3_subdivide_t,
                                 &std[i] );
            }

            for ( i = 0x00; i < sif->nbcpu; i++ ) {
                pthread_join   ( tid[i], NULL );
            }
        }
    }
}

/******************************************************************************/
void g3dsubdivider_allocBuffers ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        g3dtriangle_evalSubdivision ( sdr->subdiv_preview, 
                                      &sdr->nbFacesPerTriangle, 
                                      &sdr->nbEdgesPerTriangle,
                                      &sdr->nbVerticesPerTriangle );
        g3dquad_evalSubdivision     ( sdr->subdiv_preview,
                                      &sdr->nbFacesPerQuad, 
                                      &sdr->nbEdgesPerQuad,
                                      &sdr->nbVerticesPerQuad );

        sdr->nbrtfac = ( mes->nbtri * sdr->nbFacesPerTriangle    ) +
                       ( mes->nbqua * sdr->nbFacesPerQuad        );
        sdr->nbrtedg = ( mes->nbtri * sdr->nbEdgesPerTriangle    ) +
                       ( mes->nbqua * sdr->nbEdgesPerQuad        );
        sdr->nbrtver = ( mes->nbtri * sdr->nbVerticesPerTriangle ) +
                       ( mes->nbqua * sdr->nbVerticesPerQuad     );
        sdr->nbrtuv  = ( mes->nbtri * sdr->nbVerticesPerTriangle ) +
                       ( mes->nbqua * sdr->nbVerticesPerQuad     )  * mes->nbuvmap;

        sdr->rtquamem = realloc ( sdr->rtquamem, ( sdr->nbrtfac * sizeof ( G3DRTQUAD   ) ) );
        sdr->rtedgmem = realloc ( sdr->rtedgmem, ( sdr->nbrtedg * sizeof ( G3DRTEDGE   ) ) );
        sdr->rtvermem = realloc ( sdr->rtvermem, ( sdr->nbrtver * sizeof ( G3DRTVERTEX ) ) );
        sdr->rtuvmem  = realloc ( sdr->rtuvmem , ( sdr->nbrtuv  * sizeof ( G3DRTUV     ) ) );

    /*while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->rtquamem = rtquamem;
        fac->rtedgmem = rtedgmem;
        fac->rtvermem = rtvermem;

        if ( fac->nbver == 0x04 ) {
            rtquamem += quaFaces;
            rtedgmem += quaEdges;
            rtvermem += quaVertices;

            if ( fac->nbuvs ) fac->rtuvmem = ( G3DRTUV * ) realloc ( fac->rtuvmem, quaVertices * fac->nbuvs * sizeof ( G3DRTUV ) );
        } else {
            rtquamem += triFaces;
            rtedgmem += triEdges;
            rtvermem += triVertices;

            if ( fac->nbuvs ) fac->rtuvmem = ( G3DRTUV * ) realloc ( fac->rtuvmem, triVertices * fac->nbuvs * sizeof ( G3DRTUV ) );
        }

        ltmpfac = ltmpfac->next;
    }*/
    }
}

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_copy ( G3DSUBDIVIDER *sdr,
                                    uint32_t       engine_flags ) {
    G3DOBJECT *objsdr = ( G3DOBJECT * ) sdr;

    return g3dsubdivider_new ( objsdr->id, objsdr->name, engine_flags );
}

/******************************************************************************/
void g3dsubdivider_startUpdate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) sdr;
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        LIST *lver = NULL;

        if ( engine_flags & VIEWVERTEX ) {
            lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
        }

        if ( engine_flags & VIEWEDGE ) {
            lver = g3dmesh_getVertexListFromSelectedEdges ( mes );
        }

        if ( engine_flags & VIEWFACE ) {
            lver = g3dmesh_getVertexListFromSelectedFaces ( mes );
        }

        sdr->lsubfac = g3dvertex_getAreaFacesFromList ( lver );
    }
}

/******************************************************************************/
void g3dsubdivider_update ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    g3dsubdivider_fillBuffers ( sdr, sdr->lsubfac, engine_flags );
}

/******************************************************************************/
void g3dsubdivider_endUpdate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    list_free ( &sdr->lsubfac, NULL );
}

/******************************************************************************/
uint32_t g3dsubdivider_modify ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    g3dsubdivider_allocBuffers ( sdr, engine_flags );
    g3dsubdivider_fillBuffers  ( sdr, NULL, engine_flags );
}

/******************************************************************************/
void g3dsubdivider_activate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        g3dmesh_modify_r ( parent, engine_flags );
    }

    /*g3dmesh_modify ( obj, engine_flags );*/
}

/******************************************************************************/
void g3dsubdivider_deactivate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {

}

/******************************************************************************/
static void bindMaterials ( G3DMESH *mes, G3DFACE *fac, 
                                          G3DRTUV *rtuvmem ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->ltex;
    LIST *ltmpuvs = fac->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data; 
            G3DMATERIAL *mat = tex->mat;
            G3DIMAGE    *difimg = NULL;

            if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
                difimg = mat->diffuse.image;
            }

            if ( mat->flags & DIFFUSE_USEPROCEDURAL ) {
                if ( mat->diffuse.proc ) {
                    difimg = &mat->diffuse.proc->image;
                }
            }

            #ifdef __linux__
            glActiveTextureARB ( arbid );
            #endif
            #ifdef __MINGW32__
            if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
            #endif

            if ( difimg ) glBindTexture ( GL_TEXTURE_2D, difimg->id );
            glEnable      ( GL_TEXTURE_2D );

            glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT );

            if ( tex->map == uvs->map ) {
                #ifdef __linux__
                glClientActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                if ( ext_glClientActiveTextureARB ) ext_glClientActiveTextureARB ( arbid );
                #endif
                glEnableClientState ( GL_TEXTURE_COORD_ARRAY );

                glTexCoordPointer ( 0x02, GL_FLOAT, 0x00, rtuvmem );

                arbid++;
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
static void unbindMaterials ( G3DMESH *mes, G3DFACE *fac, 
                                            G3DRTUV *rtuvmem ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->ltex;
    LIST *ltmpuvs = fac->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data; 
            G3DMATERIAL *mat = tex->mat;

            #ifdef __linux__
            glActiveTextureARB ( arbid );
            #endif
            #ifdef __MINGW32__
            if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
            #endif

            glDisable ( GL_TEXTURE_2D );

            if ( tex->map == uvs->map ) {
                #ifdef __linux__
                glClientActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                if ( ext_glClientActiveTextureARB ) ext_glClientActiveTextureARB ( arbid );
                #endif
                glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

                arbid++;
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
uint32_t g3dsubdivider_draw ( G3DSUBDIVIDER *sdr, G3DCAMERA *cam,
                                                  uint32_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( obj->flags & OBJECTINACTIVE ) return 0x00;

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE     *fac      = ( G3DFACE * ) ltmpfac->data;
            G3DRTVERTEX *rtvermem = NULL;
            G3DRTEDGE   *rtedgmem = NULL;
            G3DRTQUAD   *rtquamem = NULL;
            G3DRTUV     *rtuvmem  = NULL;
            uint32_t     nbrtfac;
            GLint arbid = GL_TEXTURE0_ARB;
            LIST *ltmptex = mes->ltex;
            LIST *ltmpuvs = fac->luvs;

            if ( fac->nbver == 0x03 ) {
                nbrtfac  = sdr->nbFacesPerTriangle;
                rtvermem = sdr->rtvermem + ( fac->typeID * sdr->nbVerticesPerTriangle );
                rtquamem = sdr->rtquamem + ( fac->typeID * sdr->nbFacesPerTriangle );
                rtuvmem  = sdr->rtuvmem  + ( fac->typeID * sdr->nbVerticesPerTriangle * mes->nbuvmap );
            } else {
                nbrtfac  = sdr->nbFacesPerQuad;
                rtvermem = sdr->rtvermem + ( mes->nbtri  * sdr->nbVerticesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbVerticesPerQuad );
                rtquamem = sdr->rtquamem + ( mes->nbtri  * sdr->nbFacesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbFacesPerQuad );
                rtuvmem  = sdr->rtuvmem  + ( mes->nbtri  * sdr->nbVerticesPerTriangle * mes->nbuvmap ) +
                                           ( fac->typeID * sdr->nbVerticesPerQuad * mes->nbuvmap );
            }

            bindMaterials ( mes, fac, rtuvmem );

            glEnableClientState ( GL_VERTEX_ARRAY );
            glEnableClientState ( GL_NORMAL_ARRAY );
            glEnableClientState ( GL_COLOR_ARRAY  );
            glColorPointer  ( 4, GL_FLOAT, sizeof ( G3DRTVERTEX ), ((char*)rtvermem)      );
            glVertexPointer ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), ((char*)rtvermem) + 28 );
            glNormalPointer (    GL_FLOAT, sizeof ( G3DRTVERTEX ), ((char*)rtvermem) + 16 );
            glDrawElements ( GL_QUADS, nbrtfac * 4, GL_UNSIGNED_INT, rtquamem );
            glDisableClientState ( GL_COLOR_ARRAY  );
            glDisableClientState ( GL_NORMAL_ARRAY );
            glDisableClientState ( GL_VERTEX_ARRAY );

            unbindMaterials ( mes, fac, rtuvmem );

            ltmpfac = ltmpfac->next;
        }
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
void g3dsubdivider_init ( G3DSUBDIVIDER *sdr, 
                          uint32_t       id, 
                          char          *name, 
                          uint32_t       engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) sdr;

    sdr->subdiv_preview = sdr->subdiv_render = 0x02;

    g3dmodifier_init ( mod, G3DSUBDIVIDERTYPE, id, name, DRAWBEFORECHILDREN,
                                                         g3dsubdivider_draw,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         g3dsubdivider_copy,
                                                         g3dsubdivider_activate,
                                                         g3dsubdivider_deactivate,
                                                         g3dsubdivider_commit,
                                                         NULL,
                                                         NULL,
                                                         g3dsubdivider_modify,
                                                         NULL,
                                                         g3dsubdivider_update,
                                                         NULL );
}

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_new ( uint32_t id, 
                                   char    *name, 
                                   uint32_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DSUBDIVIDER );
    G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) calloc ( 0x01, structSize );

    if ( sdr == NULL ) {
        fprintf ( stderr, "g3dsubdivider_new: calloc failed\n" );

        return NULL;
    }

    g3dsubdivider_init ( sdr, id, name, engine_flags );


    return sdr;
}
