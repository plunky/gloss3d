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

extern uint32_t quad_indexes[][0x04];

/******************************************************************************/
/***
Vertex ID

         0____1        0
         |    |       /\
         |    |      /  \
         |____|     /____\
         3    2    2      1

Edge ID
          __0__ 
         |     |     /\
         3     1    2  0
         |__2__|   /__1__\
***/

/*****************************************************************************/
/*void g3dqface_normal ( G3DFACE *fac ) {
    G3DVECTOR v0v1, v0v2, v0v3, none, ntwo;
    G3DVERTEX *v0 = fac->ver[0x00],
              *v1 = fac->ver[0x01],
              *v2 = fac->ver[0x02],
              *v3 = fac->ver[0x03];

    g3dvector_init ( &v0v1, ( v1->pos.x - v0->pos.x ),
                            ( v1->pos.y - v0->pos.y ),
                            ( v1->pos.z - v0->pos.z ), 1.0f );
    g3dvector_normalize ( &v0v1 );

    g3dvector_init ( &v0v2, ( v2->pos.x - v0->pos.x ),
                            ( v2->pos.y - v0->pos.y ),
                            ( v2->pos.z - v0->pos.z ), 1.0f );
    g3dvector_normalize ( &v0v2 );

    g3dvector_init ( &v0v3, ( v3->pos.x - v0->pos.x ),
                            ( v3->pos.y - v0->pos.y ),
                            ( v3->pos.z - v0->pos.z ), 1.0f );
    g3dvector_normalize ( &v0v3 );

    g3dvector_cross ( &v0v1, &v0v2, &none );
    g3dvector_cross ( &v0v1, &v0v3, &ntwo );

    g3dvector_average ( &none, &ntwo, &fac->cen.nor );

    g3dvector_normalize ( &fac->cen.nor );
}*/

/*****************************************************************************/
void g3dtriangle_evalSubdivision ( uint32_t level, uint32_t *totalFaces,
                                                   uint32_t *totalEdges,
                                                   uint32_t *totalVertices ) {
    (*totalFaces)    = 0x00;
    (*totalEdges)    = 0x00;
    (*totalVertices) = 0x00;

    if ( level ) {
        /*** After the first subdivision, we would have ***/
        uint32_t nbFaces    = 0x03; /*** 3 faces    ***/
        uint32_t nbEdges    = 0x09; /*** 9 edges    ***/
        uint32_t nbVertices = 0x07; /*** 7 vertices ***/
        uint32_t i;

        /*** Other steps ***/
        for ( i = 0x00; i < level - 1; i++ ) {
            nbVertices = (nbVertices + nbEdges + nbFaces);
            nbEdges    = (nbEdges * 0x02 ) + (nbFaces * 0x04);
            nbFaces    = (nbFaces * 0x04 );
        }

        (*totalFaces)    = nbFaces;
        (*totalEdges)    = nbEdges;
        (*totalVertices) = nbVertices;
    }
}

/*****************************************************************************/
void g3dquad_evalSubdivision ( uint32_t level, uint32_t *totalFaces,
                                               uint32_t *totalEdges,
                                               uint32_t *totalVertices ) {
    (*totalFaces)    = 0x00;
    (*totalEdges)    = 0x00;
    (*totalVertices) = 0x00;

    if ( level ) {
        /*** After the first subdivision, we would have ***/
        uint32_t nbFaces    = 0x04; /***  4 faces    ***/
        uint32_t nbEdges    = 0x0C; /*** 12 edges    ***/
        uint32_t nbVertices = 0x09; /***  9 vertices ***/
        uint32_t i;

        /*** Other steps ***/
        for ( i = 0x00; i < level - 1; i++ ) {
            nbVertices = (nbVertices + nbEdges + nbFaces);
            nbEdges    = (nbEdges * 0x02 ) + (nbFaces * 0x04);
            nbFaces    = (nbFaces * 0x04 );
        }

        (*totalFaces)    = nbFaces;
        (*totalEdges)    = nbEdges;
        (*totalVertices) = nbVertices;
    }
}

/*****************************************************************************/
void g3dface_evalSubdivision ( G3DFACE *fac, uint32_t level,
                                             uint32_t *totalFaces,
                                             uint32_t *totalEdges,
                                             uint32_t *totalVertices ) {
    if ( fac->nbver == 0x03 ) g3dtriangle_evalSubdivision ( level, totalFaces, totalEdges, totalVertices );
    if ( fac->nbver == 0x04 ) g3dquad_evalSubdivision     ( level, totalFaces, totalEdges, totalVertices );
}

/*****************************************************************************/
void g3dface_addHeightMap ( G3DFACE *fac, G3DHEIGHTMAP *heightmap ) {
    fac->heightmap = heightmap;
}

/*****************************************************************************/
uint32_t g3dface_isFullyMirrored ( G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( ( ( fac->ver[i]->flags & VERTEXSYMYZ ) |
               ( fac->ver[i]->flags & VERTEXSYMXY ) |
               ( fac->ver[i]->flags & VERTEXSYMZX ) ) == 0x00 ) {
            return 0x00;
        }
    }

    return 0x01;
}

/*****************************************************************************/
void g3dface_initSubface ( G3DFACE *fac, G3DSUBFACE   *subfac,
                                         G3DHEIGHTMAP *mainheightmap,
                                         G3DVERTEX    *oriver,
                                         G3DVERTEX    *orivercpy,
                                         G3DSUBUVSET  *subuvs,
                                         uint32_t    (*qua_indexes)[0x04],
                                         uint32_t    (*tri_indexes)[0x04],
                                         uint32_t      iteration,
                                         uint32_t      curdiv,
                                         uint32_t      object_flags,
                                         uint32_t      subdiv_flags,
                                         uint32_t      engine_flags ) {
    uint32_t i, j;

    subfac->ancestorFace   = ((G3DSUBFACE*)fac)->ancestorFace;

    subfac->fac.flags = fac->flags;
    subfac->fac.nbver = 0x04;
    subfac->fac.luvs  = NULL;
    subfac->fac.nbuvs = 0;

    /* to uncomment */
    /*subfac->rtuvmem = fac->rtuvmem;*/

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->ver[i] == oriver ) {
            uint32_t p = ( i + fac->nbver - 0x01 ) % fac->nbver;

            subfac->fac.id = fac->id | ( i << (iteration*2) );

            subfac->fac.ver[0x00] = orivercpy;
            subfac->fac.ver[0x01] = ((G3DSUBEDGE*)fac->edg[i])->subver;
            subfac->fac.ver[0x02] = ((G3DSUBFACE*)fac)->subver;
            subfac->fac.ver[0x03] = ((G3DSUBEDGE*)fac->edg[p])->subver;

            subfac->fac.heightmap = fac->heightmap;

            if ( ( curdiv > 1 ) ||
                 ( subdiv_flags & SUBDIVISIONCOMMIT ) ||
                 ( object_flags & MESHUSEISOLINES   ) ) {
                subfac->fac.edg[0x00] = (G3DEDGE*)g3dsubedge_getSubEdge ( (G3DSUBEDGE*)fac->edg[i], (G3DVERTEX*)orivercpy, (G3DVERTEX*)((G3DSUBEDGE*)fac->edg[i])->subver );
                subfac->fac.edg[0x01] = ((G3DSUBFACE*)fac)->innedg[i];
                subfac->fac.edg[0x02] = ((G3DSUBFACE*)fac)->innedg[p];
                subfac->fac.edg[0x03] = (G3DEDGE*)g3dsubedge_getSubEdge ( (G3DSUBEDGE*)fac->edg[p], (G3DVERTEX*)orivercpy, (G3DVERTEX*)((G3DSUBEDGE*)fac->edg[p])->subver );
            }

            /*** we need normal vector only on last subdivision ***/
            if ( ( curdiv == 0x01 ) || 
            /*** we need to compute edge normals from face normals ***/
            /*** when doing displacement, at each level. Same for sculpting ***/
                 ( ( engine_flags & NODISPLACEMENT ) == 0x00 ) || 
                 ( fac->heightmap ) ) {
                g3dface_normal ( subfac );
            }

            g3dsubface_importUVSets ( subfac, fac, i, subuvs, curdiv );

            /*g3dsubface_position ( subfac );*/

            return;
        }
    }
}

/*****************************************************************************/
uint32_t g3dface_hasEdge ( G3DFACE *fac, G3DEDGE *edg  ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->edg[i] == edg ) return 0x01;
    }

    return 0x00;
}

/*****************************************************************************/
uint32_t g3dquad_isRegular ( G3DFACE *fac ) {
    if ( ( fac->ver[0]->nbedg == 0x04 ) &&
         ( fac->ver[1]->nbedg == 0x04 ) &&
         ( fac->ver[2]->nbedg == 0x04 ) &&
         ( fac->ver[3]->nbedg == 0x04 ) &&
         ( fac->ver[0]->nbfac == 0x04 ) &&
         ( fac->ver[1]->nbfac == 0x04 ) &&
         ( fac->ver[2]->nbfac == 0x04 ) &&
         ( fac->ver[3]->nbfac == 0x04 ) &&
         ( fac->edg[0]->nbfac == 0x02 ) &&
         ( fac->edg[1]->nbfac == 0x02 ) &&
         ( fac->edg[2]->nbfac == 0x02 ) &&
         ( fac->edg[3]->nbfac == 0x02 ) ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
void g3dface_subdivideUVSets ( G3DFACE *fac ) {
    LIST *ltmpuvs = fac->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

        g3duvset_subdivide ( uvs, fac );

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
void g3dface_allocSubdividedUVSets ( G3DFACE *fac, uint32_t nbrtfac ) {
    uint32_t structsize = sizeof ( G3DRTUV );

    /*** TODO - here it should be fac->nbtex instead of fac->nbuvs ***/
    /*fac->rtuvmem = realloc ( fac->rtuvmem, fac->nbuvs * nbrtfac * structsize );*/

    /*if ( fac->rtuvmem == NULL ) {*/
        fprintf ( stderr, "g3dface_allocSubdividedUVSets: realloc failed\n" );
    /*}*/
}

/******************************************************************************/
void g3dface_importUVSets ( G3DFACE *fac, G3DFACE *ref ) {
    LIST *ltmpuvs = ref->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *refuvs = ( G3DUVSET * ) ltmpuvs->data;
        G3DUVSET *newuvs = g3dface_getUVSet ( fac, refuvs->map );

        memcpy ( newuvs, refuvs, sizeof ( G3DUVSET ) );

        /*g3dface_addUVSet ( fac, newuvs );*/

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
G3DUVSET *g3dface_getUVSet ( G3DFACE *fac, G3DUVMAP *map ) {
    LIST *ltmpuvs = fac->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

        if ( uvs->map == map ) return uvs;

        ltmpuvs = ltmpuvs->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dface_countFixedUVSets ( G3DFACE *fac ) {
    LIST *ltmpuvs = fac->luvs;
    uint32_t nbuvs = 0x00;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

        if ( ((G3DOBJECT*)uvs->map)->flags & UVMAPFIXED ) nbuvs++;

        ltmpuvs = ltmpuvs->next;
    }

    return nbuvs;
}

/******************************************************************************/
uint32_t g3dface_countUVSetsFromList ( LIST *lfac, uint32_t fixedOnly ) {
    uint32_t nbuvs = 0x00;
    LIST *ltmpfac = lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fixedOnly ) {
            nbuvs += g3dface_countFixedUVSets ( fac );
        } else {
            nbuvs += fac->nbuvs;
        }

        ltmpfac = ltmpfac->next;
    }

    return nbuvs;
}

/******************************************************************************/
void g3dface_addUVSet ( G3DFACE *fac, G3DUVSET *uvs ) {
    uint32_t i;

    list_insert ( &fac->luvs, uvs );

    fac->nbuvs++;
}

/******************************************************************************/
void g3dface_removeAllUVSets ( G3DFACE *fac ) {
    LIST *ltmpuvs = fac->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        /*** g3dface_removeUVSet is going to remove the current LIST node, ***/
        /*** so we need to backup the pointer in a temporary variable.     ***/
        LIST *lnext = ltmpuvs->next;

        g3dface_removeUVSet ( fac, uvs );

        ltmpuvs = lnext;
    }
}

/******************************************************************************/
void g3dface_removeUVSet ( G3DFACE *fac, G3DUVSET *uvs ) {
    uint32_t i;

    list_remove ( &fac->luvs, uvs );

    fac->nbuvs--;
}

/******************************************************************************/
G3DEDGE *g3dface_getEdgeByID ( G3DFACE *fac, uint32_t id ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->edg[i]->id == id ) {
            return fac->edg[i];
        }
    }

    return NULL;
}

/******************************************************************************/
G3DVERTEX *g3dface_getVertexByID ( G3DFACE *fac, uint32_t id ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->ver[i]->id == id ) {
            return fac->ver[i];
        }
    }

    return NULL;
}

/******************************************************************************/
LIST *g3dface_getVerticesFromList ( LIST *lfac ) {
    LIST *ltmp = lfac;
    LIST *lver = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        int i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            if ( list_seek ( lver, fac->ver[i] ) == NULL ) {
                list_insert ( &lver, fac->ver[i] );
            }
        }

        ltmp = ltmp->next;
    }

    return lver;
}

/******************************************************************************/
LIST *g3dface_getExtendedFacesFromList ( LIST *lfac ) {
    LIST *lextfac = list_copy ( lfac );
    LIST *ltmpfac = lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DEDGE *edg = ( G3DEDGE * ) fac->edg[i];
            LIST *ltmpedgfac = edg->lfac;

            while ( ltmpedgfac ) {
                G3DFACE *extfac = ( G3DFACE * ) ltmpedgfac->data;

                if ( list_seek ( lextfac, extfac ) == NULL ) {
                    list_insert ( &lextfac, extfac );
                }

                ltmpedgfac = ltmpedgfac->next;
            }
        } 

        ltmpfac = ltmpfac->next;
    }

    return lextfac;
}

/******************************************************************************/
LIST *g3dface_getNeighbourFacesFromList ( LIST *lfac ) {
    LIST *lextfac = list_copy ( lfac );
    LIST *ltmpfac = lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DVERTEX *ver = fac->ver[i];
            LIST *ltmpverfac = ver->lfac;

            while ( ltmpverfac ) {
                G3DFACE *extfac = ( G3DFACE * ) ltmpverfac->data;

                if ( list_seek ( lextfac, extfac ) == NULL ) {
                    list_insert ( &lextfac, extfac );
                }

                ltmpverfac = ltmpverfac->next;
            }
        } 

        ltmpfac = ltmpfac->next;
    }

    return lextfac;
}

/******************************************************************************/
void g3dface_unbindMaterials ( G3DFACE *fac, LIST    *ltex,
                                             uint32_t engine_flags ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmpuvs  = fac->luvs;
    uint32_t nbtex = 0x00;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        LIST *ltmptex = ltex;

        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data; 
            G3DMATERIAL *mat = tex->mat;
            G3DIMAGE *difimg = NULL;

            if ( tex->map == uvs->map ) {
                if ( nbtex < GL_MAX_TEXTURE_UNITS_ARB ) {
                    if ( mat->flags & DIFFUSE_ENABLED ) {
                        if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                            difimg = mat->diffuse.image;
                        }

                        if ( mat->diffuse.flags & USEPROCEDURAL ) {
                            if ( mat->diffuse.proc ) {
                                difimg = &mat->diffuse.proc->image;
                            }
                        }
                    }

                    if ( difimg ) {
                        #ifdef __linux__
                        glActiveTextureARB ( arbid );
                        #endif
                        #ifdef __MINGW32__
                        if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                        #endif
                        
                        glDisable ( GL_TEXTURE_2D );

                        arbid++;
                    }

                    if ( mat->bump.image ) {
                        #ifdef __linux__
                        glActiveTextureARB ( arbid );
                        #endif
                        #ifdef __MINGW32__
                        if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                        #endif

                        glDisable ( GL_TEXTURE_2D );

                        arbid++;
                    }
                }
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
uint32_t g3dface_bindMaterials ( G3DFACE *fac, LIST           *ltex, 
                                               G3DARBTEXCOORD *texcoord,
                                               uint32_t        engine_flags ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmpuvs  = fac->luvs;
    uint32_t nbtex = 0x00;
    static GLfloat blackDiffuse[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   blackAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f },
                   blackSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   blackShininess  = 0.0f;
    uint32_t drawTextures = ( engine_flags & NOTEXTURE ) ? 0x00 : 0x01;
    static GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f },
                   whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteShininess  = 0.0f;
    static GLfloat selectDiffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
    static GLfloat grayDiffuse[]   = { MESHCOLORF, 
                                       MESHCOLORF, 
                                       MESHCOLORF, 1.0f };

    glDisable ( GL_COLOR_MATERIAL );

    if ( ( fac->flags & FACESELECTED ) &&
         ( engine_flags & VIEWFACE ) ) {
        glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) selectDiffuse );
    } else {
        glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) grayDiffuse );
    }

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        LIST *ltmptex = ltex;

        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data; 
            G3DMATERIAL *mat = tex->mat;
            G3DCOLOR specular = { mat->specular.solid.r * mat->specular_level,
                                  mat->specular.solid.g * mat->specular_level,
                                  mat->specular.solid.b * mat->specular_level,
                                  mat->specular.solid.a * mat->specular_level };
            G3DIMAGE *difimg = NULL;

            if ( tex->map == uvs->map ) {
                if ( nbtex < GL_MAX_TEXTURE_UNITS_ARB ) {
                    glDisable ( GL_COLOR_MATERIAL );

                    if ( mat->flags & DIFFUSE_ENABLED ) {
                        if ( mat->diffuse.flags & USESOLIDCOLOR ) {
                            glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) &mat->diffuse.solid );
                        } else {
                            glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) whiteDiffuse );
                        }

                        if ( fac->flags & FACESELECTED ) {
                            glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) selectDiffuse );
                        }

                        if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                            difimg = mat->diffuse.image;
                        }

                        if ( mat->diffuse.flags & USEPROCEDURAL ) {
                            if ( mat->diffuse.proc ) {
                                difimg = &mat->diffuse.proc->image;
                            }
                        }
                    }

                    /*if ( difimg ) {
                        glBindTexture ( GL_TEXTURE_2D, difimg->id );
                        glEnable      ( GL_TEXTURE_2D );

                        glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
                                                    GL_COMBINE_EXT );
                    }*/

                    glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT  , ( GLfloat * ) whiteAmbient );
                    glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR , ( GLfloat * ) &specular );
                    glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, ( GLfloat * ) &mat->shininess );

#ifdef unused
                        if ( ( fac->flags   & FACESELECTED ) && 
                             /*( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) &&*/
                             ( engine_flags & VIEWFACE     ) ) {
                            glEnable   ( GL_COLOR_MATERIAL );
                            glColor3ub ( 0xFF, 0x7F, 0x00  );
                        } else {
                            glDisable ( GL_COLOR_MATERIAL );

                            glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE,   ( GLfloat * ) &mat->diffuse.solid  );

                        }
                    }
#endif
                    #ifdef __linux__
                    glActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                    #endif

                    if ( difimg ) {
                        glBindTexture ( GL_TEXTURE_2D, difimg->id );
                        glEnable      ( GL_TEXTURE_2D );

                        if ( tex->map->projection == UVMAPBACKGROUND ) {
                            g3duvset_mapFaceWithBackgroundProjection ( uvs, 
                                                                       fac,
                                                                       engine_flags );
                        }

                        glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
                                                    GL_COMBINE_EXT );

                        texcoord[nbtex].u[0x00] = uvs->veruv[0x00].u;
                        texcoord[nbtex].v[0x00] = uvs->veruv[0x00].v;
                        /*texcoord[nbtex].q[0x00] = uvs->veruv[0x00].q;*/

                        texcoord[nbtex].u[0x01] = uvs->veruv[0x01].u;
                        texcoord[nbtex].v[0x01] = uvs->veruv[0x01].v;
                        /*texcoord[nbtex].q[0x01] = uvs->veruv[0x01].q;*/

                        texcoord[nbtex].u[0x02] = uvs->veruv[0x02].u;
                        texcoord[nbtex].v[0x02] = uvs->veruv[0x02].v;
                        /*texcoord[nbtex].q[0x02] = uvs->veruv[0x02].q;*/

                        texcoord[nbtex].u[0x03] = uvs->veruv[0x03].u;
                        texcoord[nbtex].v[0x03] = uvs->veruv[0x03].v;
                        /*texcoord[nbtex].q[0x03] = uvs->veruv[0x03].q;*/
                        texcoord[nbtex].tid = arbid;
                    }

                    /*glEnable ( GL_COLOR_MATERIAL );*/

                    arbid++; nbtex++;
                }
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }

    return nbtex;
}


/******************************************************************************/
void g3dface_drawSimple  ( G3DFACE *fac, uint32_t object_flags,
                                         uint32_t engine_flags ) {
    G3DVERTEX *v0 = fac->ver[0x00],
              *v1 = fac->ver[0x01],
              *v2 = fac->ver[0x02],
              *v3 = fac->ver[0x03];

    if ( fac->nbver == 0x03 ) {
        G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
                  *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos,
                  *p2 = ( v2->flags & VERTEXSKINNED ) ? &v2->skn : &v2->pos;

        glBegin ( GL_TRIANGLES );

        glVertex3fv ( ( GLfloat * ) p0 );
        glVertex3fv ( ( GLfloat * ) p1 );
        glVertex3fv ( ( GLfloat * ) p2 );

       glEnd ( );
    } else {
        G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
                  *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos,
                  *p2 = ( v2->flags & VERTEXSKINNED ) ? &v2->skn : &v2->pos,
                  *p3 = ( v3->flags & VERTEXSKINNED ) ? &v3->skn : &v3->pos;

        glBegin ( GL_QUADS );

        glVertex3fv ( ( GLfloat * ) p0 );
        glVertex3fv ( ( GLfloat * ) p1 );
        glVertex3fv ( ( GLfloat * ) p2 );
        glVertex3fv ( ( GLfloat * ) p3 );

        glEnd ( );
    }
}

/******************************************************************************/
void g3dface_draw  ( G3DFACE *fac, float    gouraudScalarLimit,
                                   LIST    *ltex, 
                                   uint32_t object_flags,
                                   uint32_t engine_flags ) {
    static G3DARBTEXCOORD texcoord[GL_MAX_TEXTURE_UNITS_ARB];
    uint32_t   nbtex = 0x00;
    G3DVECTOR *pos[0x04];
    G3DVECTOR  dis[0x04]; /*** displaced position ***/
    uint32_t   i, j;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        /* later ...*/
        /*if ( ( engine_flags & NODISPLACEMENT ) == 0x00 ) {
            g3dvertex_displace ( fac->ver[i], ltex, &dis[i] );
            pos[i] = &dis[i];
        } else {*/
            pos[i] = ( fac->ver[i]->flags & VERTEXSKINNED ) ? &fac->ver[i]->skn :
                                                              &fac->ver[i]->pos;
        /*}*/
    }

    if ( ltex && fac->luvs && ( ( engine_flags & SELECTMODE ) == 0x00 ) ) {
       nbtex = g3dface_bindMaterials ( fac, ltex, texcoord, engine_flags );
    }

    ( fac->nbver == 0x04 ) ? glBegin ( GL_QUADS     ) : 
                             glBegin ( GL_TRIANGLES ); 
    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DVERTEX *ver = fac->ver[i];

        if ( engine_flags & VIEWSKIN     ) {
            glColor3f ( 1.0f, ver->weight, 0.0f );
        }

        if ( (   fac->flags & FACESELECTED   ) && 
             (   engine_flags & VIEWFACE     ) &&
             ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );
        }

        if ( engine_flags & VIEWSKIN ) glColor3f ( 1.0f, ver->weight, 0.0f );

        for ( j = 0x00; j < nbtex; j++ ) {
            #ifdef __linux__
            glMultiTexCoord2fARB ( texcoord[j].tid,
                                   texcoord[j].u[i], 
                                   texcoord[j].v[i]/*,
                                   0.0f,
                                   texcoord[j].q[i]*/ );
            #endif
            #ifdef __MINGW32__
            if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[j].tid,
                                                                       texcoord[j].u[i], 
                                                                       texcoord[j].v[i]/*,
                                                                       0.0f,
                                                                       texcoord[j].q[i]*/ ); 
            #endif
        }

/*printf("%f\n", fabs ( g3dvector_scalar ( &ver->nor, &fac->nor ) ) );*/
        if ( fabs ( g3dvector_scalar ( &ver->nor, 
                                       &fac->nor ) ) < gouraudScalarLimit ) {
            glNormal3fv ( ( float * ) &fac->nor );
        } else {
            glNormal3fv ( ( float * ) &ver->nor );
        }

        glVertex3fv ( ( float * )  pos[i] );
    }
    glEnd ( );

    if ( ltex && fac->luvs && ( ( engine_flags & SELECTMODE ) == 0x00 ) ) {
        g3dface_unbindMaterials ( fac, ltex, engine_flags );
    }
}

/******************************************************************************/
void g3dface_drawQuad ( G3DFACE *fac,
                        float    gouraudScalarLimit,
                        LIST    *ltex, 
                        uint32_t object_flags,
                        uint32_t engine_flags ) {
    g3dface_draw ( fac, gouraudScalarLimit, ltex, object_flags, engine_flags );
}

/******************************************************************************/
void g3dface_drawTriangle  ( G3DFACE *fac,
                             float    gouraudScalarLimit,
                             LIST    *ltex, 
                             uint32_t object_flags,
                             uint32_t engine_flags ) {
    g3dface_draw ( fac, gouraudScalarLimit, ltex, object_flags, engine_flags );
}

#ifdef unused
/******************************************************************************/
void g3dface_drawQuad  ( G3DFACE *fac, LIST *ltex, uint32_t object_flags,
                                                   uint32_t engine_flags ) {
    G3DVERTEX *v0 = fac->ver[0x00],
              *v1 = fac->ver[0x01],
              *v2 = fac->ver[0x02],
              *v3 = fac->ver[0x03];
    G3DVERTEX **ver = fac->ver;
    G3DUVSET *uvs = ( fac->luvs ) ? ( G3DUVSET * ) fac->luvs->data : NULL;
    uint32_t i;

    if ( object_flags & MESHGEOMETRYONLY ) {
        glBegin ( GL_QUADS );
        for ( i = 0x00; i < fac->nbver; i++ ) {
            if ( ver[i]->flags & VERTEXSKINNED ) {
                glVertex3fv ( ( float * ) &ver[i]->skn );
            } else {
                glVertex3fv ( ( float * ) &ver[i]->pos );
            }
        }
        glEnd ( );


        return;
    }


if ( ( ( engine_flags & NODRAWPOLYGON ) == 0x00 ) &&
     ( ( engine_flags & SELECTMODE    ) == 0x00 ) ) {
    if ( engine_flags & VIEWSKIN ) {
        glBegin ( GL_QUADS );

        for ( i = 0x00; i < fac->nbver; i++ ) {
            glColor3f ( 1.0f, ver[i]->weight, 0.0f );

            glNormal3fv ( ( float * ) &ver[i]->nor );

            if ( ver[i]->flags & VERTEXSKINNED ) {
                glVertex3fv ( ( float * ) &ver[i]->skn );
            } else {
                glVertex3fv ( ( float * ) &ver[i]->pos );
            }
        }

        glEnd ( );
    } else {
        G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos;
        G3DVECTOR *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;
        G3DVECTOR *p2 = ( v2->flags & VERTEXSKINNED ) ? &v2->skn : &v2->pos;
        G3DVECTOR *p3 = ( v3->flags & VERTEXSKINNED ) ? &v3->skn : &v3->pos;
        static G3DARBTEXCOORD texcoord[GL_MAX_TEXTURE_UNITS_ARB];

        if ( ltex && fac->luvs && ( ( engine_flags & SELECTMODE ) == 0x00 ) ) {
            uint32_t nbtex = g3dface_bindMaterials ( fac, ltex, texcoord, engine_flags );

            glBegin  ( GL_QUADS );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x00], 
                                       texcoord[i].v[0x00] ); 
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x00], 
                                                                           texcoord[i].v[0x00] ); 
                #endif
            }
            glNormal3fv  ( ( float * ) &v0->nor );
            glVertex3fv  ( ( float * ) p0 );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x01], 
                                       texcoord[i].v[0x01] );
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x01], 
                                                                           texcoord[i].v[0x01] );
                #endif
            }
            glNormal3fv  ( ( float * ) &v1->nor );
            glVertex3fv  ( ( float * ) p1 );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x02], 
                                       texcoord[i].v[0x02] );
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x02], 
                                                                           texcoord[i].v[0x02] );
                #endif
            }
            glNormal3fv  ( ( float * ) &v2->nor );
            glVertex3fv  ( ( float * ) p2 );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x03], 
                                       texcoord[i].v[0x03] ); 
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x03], 
                                                                           texcoord[i].v[0x03] ); 
                #endif
            }
            glNormal3fv  ( ( float * ) &v3->nor );
            glVertex3fv  ( ( float * ) p3 );

            glEnd ( );

            g3dface_unbindMaterials ( fac, ltex, engine_flags );
        } else {
            if ( ( fac->flags & FACESELECTED ) && 
                 ( engine_flags & VIEWFACE   ) &&
                 ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) ) {
                glColor3ub ( 0xFF, 0x7F, 0x00 );
            } else {
                glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );
            }

            glBegin ( GL_QUADS );

            glNormal3fv  ( ( float * ) &v0->nor );
            glVertex3fv  ( ( float * ) p0 );

            glNormal3fv  ( ( float * ) &v1->nor );
            glVertex3fv  ( ( float * ) p1 );

            glNormal3fv  ( ( float * ) &v2->nor );
            glVertex3fv  ( ( float * ) p2 );

            glNormal3fv  ( ( float * ) &v3->nor );
            glVertex3fv  ( ( float * ) p3 );

            glEnd ( );
        }
    }
}

    /*** Draw edges in subdivided mode ***/
    if ( ( ( engine_flags & SELECTMODE   ) == 0x00 ) &&
         ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) &&
           ( fac->flags & FACEDRAWVERTICES ) ) {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glDisable ( GL_LIGHTING );
        glPointSize ( 4.0f );

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DVERTEX *v0 = fac->ver[i];
            G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos;

            if ( fac->ver[i]->flags & VERTEXORIGINAL ) {
                if ( ( fac->ver[i]->flags & VERTEXSELECTED ) &&
                     ( engine_flags       & VIEWVERTEX     ) ) {
                    glColor3ub ( 0xFF, 0x00, 0x00 );
                } else {
                    glColor3ub ( 0x00, 0x00, 0xFF );
                }

                if ( engine_flags & SELECTMODE ) glLoadName ( ( GLint ) v0->id );
                glBegin ( GL_POINTS );
                glVertex3fv ( ( float * ) p0 );
                glEnd ( );
            }
        }

        glPopAttrib ( );
    }

    /*** NOT NEED FOR TRIANGLES, THUS ABSENT FROM g3dface_drawTriangle() ***/

    /*** Draw edges in subdivided mode ***/
    if ( ( ( engine_flags & SELECTMODE   ) == 0x00 ) &&
         ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) &&
           ( fac->flags & FACEDRAWEDGES ) ) {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glDisable ( GL_LIGHTING );

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DEDGE *edg = fac->edg[i];
            G3DVERTEX *v0 = edg->ver[0x00],
                      *v1 = edg->ver[0x01];
            G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
                      *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;

            if ( edg->flags & EDGEORIGINAL ) {
                if ( ( fac->edg[i]->flags & EDGESELECTED ) &&
                     ( engine_flags       & VIEWEDGE     ) ) {
                    glColor3ub ( 0xFF, 0x7F, 0x00 );
                } else {
                    glColor3ub ( 0x00, 0x7F, 0xFF );
                }

                if ( engine_flags & SELECTMODE ) glLoadName ( ( GLint ) edg->id );

                glBegin ( GL_LINES );
                glVertex3fv ( ( float * ) p0 );
                glVertex3fv ( ( float * ) p1 );
                glEnd ( );
            }
        }

        glPopAttrib ( );
    }
}

/******************************************************************************/
void g3dface_drawTriangle ( G3DFACE *fac, LIST *ltex, uint32_t object_flags,
                                                      uint32_t engine_flags ) {
    G3DVERTEX *v0 = fac->ver[0x00],
              *v1 = fac->ver[0x01],
              *v2 = fac->ver[0x02];
    G3DVERTEX **ver = fac->ver;
    /*G3DMATERIAL *mat = fac->mat;*/
    G3DUVSET *uvs = ( fac->luvs ) ? ( G3DUVSET * ) fac->luvs->data : NULL;
    uint32_t i;

    if ( object_flags & MESHGEOMETRYONLY ) {
        glBegin ( GL_TRIANGLES );
        for ( i = 0x00; i < fac->nbver; i++ ) {
            if ( ver[i]->flags & VERTEXSKINNED ) {
                glVertex3fv ( ( float * ) &ver[i]->skn );
            } else {
                glVertex3fv ( ( float * ) &ver[i]->pos );
            }
        }
        glEnd ( );


        return;
    }

if ( ( ( engine_flags & NODRAWPOLYGON ) == 0x00 ) &&
     ( ( engine_flags & SELECTMODE    ) == 0x00 ) ) {
    if ( engine_flags & VIEWSKIN ) {
        glBegin ( GL_TRIANGLES );

        for ( i = 0x00; i < fac->nbver; i++ ) {
            if ( ver[i]->flags & VERTEXPAINTED ) {
                glColor3f ( 1.0f, ver[i]->weight, 0.0f );
            } else {
                glColor3f ( 1.0f, 0.0f          , 0.0f );
            }

            glNormal3fv ( ( float * ) &ver[i]->nor );

            if ( ver[i]->flags & VERTEXSKINNED ) {
                glVertex3fv ( ( float * ) &ver[i]->skn );
            } else {
                glVertex3fv ( ( float * ) &ver[i]->pos );
            }
        }

        glEnd ( );
    } else {
        G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos;
        G3DVECTOR *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;
        G3DVECTOR *p2 = ( v2->flags & VERTEXSKINNED ) ? &v2->skn : &v2->pos;
        static G3DARBTEXCOORD texcoord[GL_MAX_TEXTURE_UNITS_ARB];

        if ( ltex && fac->luvs && ( ( engine_flags & SELECTMODE ) == 0x00 ) ) {
            uint32_t nbtex = g3dface_bindMaterials ( fac, ltex,
                                                          texcoord,
                                                          engine_flags );

            glBegin ( GL_TRIANGLES );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x00], 
                                       texcoord[i].v[0x00] ); 
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x00], 
                                                                           texcoord[i].v[0x00] ); 
                #endif
            }
            glNormal3fv  ( ( float * ) &v0->nor );
            glVertex3fv  ( ( float * ) p0 );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x01], 
                                       texcoord[i].v[0x01] );
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x01], 
                                                                           texcoord[i].v[0x01] );
                #endif
            }
            glNormal3fv  ( ( float * ) &v1->nor );
            glVertex3fv  ( ( float * ) p1 );

            for ( i = 0x00; i < nbtex; i++ ) {
                #ifdef __linux__
                glMultiTexCoord2fARB ( texcoord[i].tid,
                                       texcoord[i].u[0x02], 
                                       texcoord[i].v[0x02] );
                #endif
                #ifdef __MINGW32__
                if ( ext_glMultiTexCoord2fARB ) ext_glMultiTexCoord2fARB ( texcoord[i].tid,
                                                                           texcoord[i].u[0x02], 
                                                                           texcoord[i].v[0x02] );
                #endif
            }
            glNormal3fv  ( ( float * ) &v2->nor );
            glVertex3fv  ( ( float * ) p2 );

            glEnd ( );

            g3dface_unbindMaterials ( fac, ltex, engine_flags );
        } else {
            if ( ( fac->flags & FACESELECTED ) && 
                 ( engine_flags & VIEWFACE   ) &&
                 ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) ) {
                glColor3ub ( 0xFF, 0x7F, 0x00 );
            } else {
                glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );
            }

            glBegin ( GL_TRIANGLES );

            glNormal3fv  ( ( float * ) &v0->nor );
            glVertex3fv  ( ( float * ) p0 );

            glNormal3fv  ( ( float * ) &v1->nor );
            glVertex3fv  ( ( float * ) p1 );

            glNormal3fv  ( ( float * ) &v2->nor );
            glVertex3fv  ( ( float * ) p2 );

            glEnd ( );
        }
    }
}

}
#endif

/******************************************************************************/
void g3dface_drawQuadList ( LIST    *lqua,
                            float    gouraudScalarLimit, 
                            LIST    *ltex, 
                            uint32_t object_flags,
                            uint32_t engine_flags ) {

    while ( lqua ) {
        G3DFACE *fac = ( G3DFACE * ) lqua->data;

        g3dface_drawQuad ( fac, gouraudScalarLimit, ltex, object_flags, engine_flags );


        lqua = lqua->next;
    }
}

/******************************************************************************/
void g3dface_drawTriangleList ( LIST    *ltri,
                                float    gouraudScalarLimit, 
                                LIST    *ltex, 
                                uint32_t object_flags,
                                uint32_t engine_flags ) {

    while ( ltri ) {
        G3DFACE *fac = ( G3DFACE * ) ltri->data;

        g3dface_drawTriangle ( fac, gouraudScalarLimit, ltex, object_flags, engine_flags );


        ltri = ltri->next;
    }
}

/******************************************************************************/
LIST *g3dface_getEdgesFromList ( LIST *lfac ) {
    LIST *ledg = NULL;

    while ( lfac ) {
        G3DFACE *fac = ( G3DFACE * ) lfac->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DEDGE *edg = fac->edg[i];

            if ( list_seek ( ledg, edg ) == NULL ) {
                list_insert ( &ledg, edg );
            }
        }

        lfac = lfac->next;
    } 

    return ledg;
}

/******************************************************************************/
uint32_t g3dface_isAligned ( G3DFACE *fac, uint32_t edgeid, G3DFACE *ali ) {
    uint32_t i;

    for ( i = 0x00; i < ali->nbver; i++ ) {
        G3DEDGE *edg = ( G3DEDGE * ) ali->edg[i];
        int n = ( i + 0x01 ) % ali->nbver;

        if ( edg == fac->edg[edgeid] ) {
            if ( ali->ver[i] == fac->ver[edgeid] ) {
                return 0x00;
            } else {
                return 0x01;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dface_replaceVertex ( G3DFACE *fac, G3DVERTEX *ref,
                                           G3DVERTEX *sub ) {
    int i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        int p = ( i + fac->nbver - 0x01 ) % fac->nbver;

        if ( fac->ver[i] == ref ) {
            fac->ver[i] = sub;

            g3dvertex_removeFace ( ref, fac );
            g3dvertex_addFace    ( sub, fac );

            if ( fac->edg[i] ) g3dedge_removeFace ( fac->edg[i], fac );
            if ( fac->edg[p] ) g3dedge_removeFace ( fac->edg[p], fac );

            fac->edg[i] = NULL;
            fac->edg[p] = NULL;


            return;
        }
    }
}

/******************************************************************************/
void g3dface_alignUnselected ( G3DFACE *fac ) {
    int i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DEDGE *edgi = ( G3DEDGE * ) fac->edg[i];
        int n = ( i + 0x01 ) % fac->nbver;
        LIST *ltmp = edgi->lfac;

        while ( ltmp ) {
            G3DFACE *nei = ( G3DFACE * ) ltmp->data;

            if ( ( nei != fac ) && ( ( nei->flags & FACEALIGNED  ) == 0x00 )
                                && ( ( nei->flags & FACESELECTED ) == 0x00 ) ) {
                int j;

                if ( g3dface_isAligned ( fac, i, nei) == 0x00 ) {
                    g3dface_invertNormal ( nei );

                    nei->flags |= FACEALIGNED;
                }
            }

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
void g3dface_recurseAlign ( G3DFACE *fac ) {
    int i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DEDGE *edgi = ( G3DEDGE * ) fac->edg[i];
        int n = ( i + 0x01 ) % fac->nbver;
        LIST *ltmp = edgi->lfac;

        while ( ltmp ) {
            G3DFACE *nei = ( G3DFACE * ) ltmp->data;

            if ( ( nei != fac ) && ( ( nei->flags & FACEALIGNED ) == 0x00 ) ) {
                int j;

                if ( g3dface_isAligned ( fac, i, nei) == 0x00 ) {
                    g3dface_invertNormal ( nei );

                    nei->flags |= FACEALIGNED;

                    g3dface_recurseAlign ( nei );
                }
            }

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
/*** return NULL = delete the face ***/
G3DFACE *g3dface_weld ( G3DFACE *fac, LIST *lselver, G3DVERTEX *wel ) {
    G3DVERTEX *ver[0x04] = { NULL, NULL, NULL, NULL };
    uint32_t nbhit = 0x00;
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( list_seek ( lselver, fac->ver[i] ) ) {
            ver[i] = fac->ver[i];

            nbhit++;
        }
    }

    /*** face will disappear ***/
    if ( nbhit == 0x03 ) return NULL;
    if ( nbhit == 0x04 ) return NULL;

    /*** if the face is a triangle, welding 2 vertices makes it disappear ! ***/
    if ( ( nbhit == 0x02 ) && ( fac->nbver == 0x03 ) ) return NULL;

    /*** if the face is a quad, welding 2 points make it disappear if these ***/
    /*** two vertices are not neighbours. Otherwise, a triangle is created. ***/
    if ( ( nbhit == 0x02 ) && ( fac->nbver == 0x04 ) ) {
        if ( ( ver[0x00] && ver[0x02] ) ||
             ( ver[0x01] && ver[0x03] ) ) {

            return NULL;
        } else {
            G3DVERTEX *tri[0x03] = { NULL, NULL, wel };
            uint32_t n = 0x00;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                if ( ver[i] == NULL ) tri[n++] = fac->ver[i];
            }

            return g3dface_new ( tri, 0x03 );
        }
    }

    if ( ( nbhit == 0x01 ) ) {
        G3DVERTEX *pnt[0x04] = { NULL, NULL, NULL, NULL };

        for ( i = 0x00; i < fac->nbver; i++ ) {
            if ( ver[i] == NULL ) pnt[i] = fac->ver[i];
            else                  pnt[i] = wel;
        }

        return g3dface_new ( pnt, fac->nbver );
    }

    return NULL;
}

/******************************************************************************/
void g3dface_drawEdges ( G3DFACE *fac, uint32_t flags ) {
    int i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->edg[i] ) {
            g3dedge_draw ( fac->edg[i], flags );
        }
    }
}

/******************************************************************************/
void g3dface_drawCenter ( G3DFACE *fac, uint32_t flags ) {
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );
    glPointSize ( 3.0f );
    glBegin ( GL_POINTS );
    glVertex3fv ( ( GLfloat * ) &fac->pos );
    glEnd ( );
    glEnable ( GL_LIGHTING );
    glPopAttrib( );
}

/******************************************************************************/

uint32_t g3dface_convert ( G3DFACE *fac, G3DFACE        *ancestor,
                                         G3DRTTRIANGLE **rttriptr,
                                         G3DRTQUAD     **rtquaptr,
                                         G3DRTUVSET    **rtuvsptr,
                                         uint32_t        object_flags,
                                         uint32_t        engine_flags ) {
#ifdef unused
    uint32_t i, j;

    if ( rttriptr ) {
        static uint32_t symid[0x06] = { 0x02, 0x01, 0x00, 0x00, 0x03, 0x02 };
        static uint32_t nrmid[0x06] = { 0x00, 0x01, 0x02, 0x02, 0x03, 0x00 };
        uint32_t *n, *ptid, nb = 0x00;

        if ( engine_flags & SYMMETRYVIEW ) ptid = symid;
        else                               ptid = nrmid;

        for ( j = 0x00, n = ptid; j < fac->nbver; j += 0x03 ) {
            LIST *ltmpuvs = fac->luvs;
            G3DTINYVECTOR *pos, *nor;

            (*rttriptr)->tripos.x = (*rttriptr)->trinor.x = 0.0f;
            (*rttriptr)->tripos.y = (*rttriptr)->trinor.y = 0.0f;
            (*rttriptr)->tripos.z = (*rttriptr)->trinor.z = 0.0f;

            while ( ltmpuvs && rtuvsptr && (*rtuvsptr) ) {
                G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

                (*rtuvsptr)->u0 = uvs->veruv[*(n+0x00)].u;
                (*rtuvsptr)->v0 = uvs->veruv[*(n+0x00)].v;
                (*rtuvsptr)->u1 = uvs->veruv[*(n+0x01)].u;
                (*rtuvsptr)->v1 = uvs->veruv[*(n+0x01)].v;
                (*rtuvsptr)->u2 = uvs->veruv[*(n+0x02)].u;
                (*rtuvsptr)->v2 = uvs->veruv[*(n+0x02)].v;
                /*(*rtuvsptr)->u3 = uvs->veruv[xxxxx].u; *//*** unused ***/
                /*(*rtuvsptr)->v3 = uvs->veruv[xxxxx].v; *//*** unused ***/

                ltmpuvs = ltmpuvs->next;
            }

            for ( i = 0x00; i < 0x03; i++, n++ ) {
                pos = &(*rttriptr)->verpos[i];
                nor = &(*rttriptr)->vernor[i];

                memcpy ( nor, &fac->ver[(*n)]->nor, sizeof ( G3DTINYVECTOR ) );

                if ( fac->ver[(*n)]->flags & VERTEXSKINNED ) {
                    memcpy ( pos, &fac->ver[(*n)]->skn, sizeof ( G3DTINYVECTOR ) );
                } else {
                    memcpy ( pos, &fac->ver[(*n)]->pos, sizeof ( G3DTINYVECTOR ) );
                }

                (*rttriptr)->tripos.x += pos->x;
                (*rttriptr)->tripos.y += pos->y;
                (*rttriptr)->tripos.z += pos->z;

    /*** We dont recompute the normal vector because this might cause   ***/
    /*** orientation issues with symmetrical objects. We just average it ***/
    /*** from the triangle vertices normal vectors ***/
                /*(*rttriptr)->trinor.x += nor->x;
                (*rttriptr)->trinor.y += nor->y;
                (*rttriptr)->trinor.z += nor->z;*/
            }

            (*rttriptr)->tripos.x *= ONETHIRD;
            (*rttriptr)->tripos.y *= ONETHIRD;
            (*rttriptr)->tripos.z *= ONETHIRD;

            /*(*rttriptr)->trinor.x *= ONETHIRD;
            (*rttriptr)->trinor.y *= ONETHIRD;
            (*rttriptr)->trinor.z *= ONETHIRD;*/

            (*rttriptr)++;

            if ( rtuvsptr && (*rtuvsptr) ) (*rtuvsptr)++;

            nb++;
        }

        return nb;
    }

    if ( rtquaptr ) {
        G3DRTQUAD *rtqua = (*rtquaptr);
        LIST *ltmpuvs = fac->luvs;

        for ( i = 0x00; i < 0x04; i++ ) {
            G3DRTVERTEX *rtver = &rtqua->rtver[i];
            G3DVERTEX     *ver = fac->ver[i];

            if (   ( fac->flags & FACESELECTED   ) &&
                   ( engine_flags & VIEWFACE     ) &&
                 ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) ) {
                g3drtvertex_init ( rtver, ver, 0x01, engine_flags );
            } else {
                g3drtvertex_init ( rtver, ver, 0x00, engine_flags );
            }

            /*if ( engine_flags & VIEWSKIN ) {
                rtver->r = rtver->a = 1.0f;
                rtver->g = ( ver->flags & VERTEXPAINTED ) ? ver->weight : 0.0f;
                rtver->b = 0.0f;
            } else {
                if ( ( fac->flags & FACESELECTED ) &&
                     ( engine_flags & VIEWFACE          ) &&
                     ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) ) {
                    rtver->r = 1.0f;
                    rtver->g = 0.5f;
                    rtver->b = rtver->a = 0.0f;
                } else {
                    rtver->r = rtver->g = rtver->b = rtver->a = MESHCOLORF;
                }
            }

            memcpy ( &rtver->nor, &ver->nor, sizeof ( G3DTINYVECTOR ) );
            memcpy ( &rtver->pos, &ver->pos, sizeof ( G3DTINYVECTOR ) );*/
        }

        /*** Draw vertices in subdivided mode ***/
        if ( ( fac->flags   & FACEDRAWVERTICES  ) &&
             ( object_flags & MESHUSEISOLINES ) ) {
            #ifdef __linux__
            static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
            #endif
            #ifdef __MINGW32__
            static HANDLE lock;

            if ( lock == NULL ) lock = CreateMutex ( NULL, FALSE, NULL );
            #endif

            for ( i = 0x00; i < fac->nbver; i++ ) {
                G3DVERTEX *ancestorVer = g3dface_getVertexByID ( ancestor, fac->ver[i]->id );
                G3DVERTEX *v0 = fac->ver[i];
                G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : 
                                                                &v0->pos;
                if ( ancestorVer ) {
                    G3DRTVERTEX *rtver = ancestorVer->rtvermem;

                    /*** Multiple threads may access the resource ***/
                    #ifdef __linux__
                    pthread_mutex_lock ( &lock );
                    #endif

                    #ifdef __MINGW32__
                    WaitForSingleObject( lock, INFINITE );
                    #endif

                    if ( ancestorVer->flags & VERTEXSELECTED ) {
                        rtver->r = 1.0f;
                        rtver->g = 0.0f;
                        rtver->b = 0.0f;
                        rtver->a = 0.0f;
                    } else {
                        rtver->r = 0.0f;
                        rtver->g = 0.0f;
                        rtver->b = 1.0f;
                        rtver->a = 0.0f;
                    }

                    memcpy ( &rtver->pos, p0, sizeof ( G3DTINYVECTOR ) );

                    #ifdef __linux__
                    pthread_mutex_unlock ( &lock );
                    #endif

                    #ifdef __MINGW32__
                    ReleaseMutex( lock );
                    #endif
                }
            }
        }

        /*** Draw edges in subdivided mode ***/
        if ( ( fac->flags   & FACEDRAWEDGES   ) &&
             ( object_flags & MESHUSEISOLINES ) ) {
            #ifdef __linux__
            static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
            #endif
            #ifdef __MINGW32__
            static HANDLE lock;

            if ( lock == NULL ) lock = CreateMutex ( NULL, FALSE, NULL );
            #endif

            for ( i = 0x00; i < fac->nbver; i++ ) {
                G3DEDGE *edg = fac->edg[i];
                G3DVERTEX *v0 = edg->ver[0x00],
                          *v1 = edg->ver[0x01];
                G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
                          *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;

                if ( edg->flags & EDGEORIGINAL ) {
                    G3DRTEDGE *rtedg = edg->rtedgmem;

                    /*** Multiple threads may access the resource ***/
                    #ifdef __linux__
                    pthread_mutex_lock ( &lock );
                    #endif

                    #ifdef __MINGW32__
                    WaitForSingleObject( lock, INFINITE );
                    #endif

                    if ( ( edg->flags    & EDGESELECTED ) &&
                         ( engine_flags  & VIEWEDGE     ) ) {
                        rtedg->rtver[0x00].r = rtedg->rtver[0x01].r = 1.0f;
                        rtedg->rtver[0x00].g = rtedg->rtver[0x01].g = 0.5f;
                        rtedg->rtver[0x00].b = rtedg->rtver[0x01].b = 0.0f;
                        rtedg->rtver[0x00].a = rtedg->rtver[0x01].a = 0.0f;
                    } else {
                        rtedg->rtver[0x00].r = rtedg->rtver[0x01].r = 0.0f;
                        rtedg->rtver[0x00].g = rtedg->rtver[0x01].g = 0.125f;
                        rtedg->rtver[0x00].b = rtedg->rtver[0x01].b = 0.250f;
                        rtedg->rtver[0x00].a = rtedg->rtver[0x01].a = 0.0f;
                    }

                    memcpy ( &rtedg->rtver[0x00].pos, p0, sizeof ( G3DTINYVECTOR ) );
                    memcpy ( &rtedg->rtver[0x01].pos, p1, sizeof ( G3DTINYVECTOR ) );

                    #ifdef __linux__
                    pthread_mutex_unlock ( &lock );
                    #endif

                    #ifdef __MINGW32__
                    ReleaseMutex( lock );
                    #endif
                }
            }
        }

        while ( ltmpuvs  && rtuvsptr && (*rtuvsptr) ) {
            G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

            (*rtuvsptr)->u0 = uvs->veruv[0x00].u;
            (*rtuvsptr)->v0 = uvs->veruv[0x00].v;
            (*rtuvsptr)->u1 = uvs->veruv[0x01].u;
            (*rtuvsptr)->v1 = uvs->veruv[0x01].v;
            (*rtuvsptr)->u2 = uvs->veruv[0x02].u;
            (*rtuvsptr)->v2 = uvs->veruv[0x02].v;
            (*rtuvsptr)->u3 = uvs->veruv[0x03].u;
            (*rtuvsptr)->v3 = uvs->veruv[0x03].v;

            ltmpuvs = ltmpuvs->next;
        }

        (*rtquaptr)++;

        if ( rtuvsptr && (*rtuvsptr) ) (*rtuvsptr)++;


        return 0x01;
    }
#endif
    return 0x00;
}


/******************************************************************************/
void g3dface_invertNormal ( G3DFACE *fac ) {
    G3DVERTEX *tmpver;
    G3DEDGE   *tmpedg;
    uint32_t i;

    if ( fac->nbver == 0x04 ) {
        tmpver = fac->ver[0x00];
        fac->ver[0x00] = fac->ver[0x01];
        fac->ver[0x01] = tmpver;

        tmpver = fac->ver[0x02];
        fac->ver[0x02] = fac->ver[0x03];
        fac->ver[0x03] = tmpver;

        tmpedg = fac->edg[0x01];
        fac->edg[0x01] = fac->edg[0x03];
        fac->edg[0x03] = tmpedg;
    } else {
        tmpver = fac->ver[0x00];
        fac->ver[0x00] = fac->ver[0x01];
        fac->ver[0x01] = tmpver;

        tmpedg = fac->edg[0x01];
        fac->edg[0x01] = fac->edg[0x02];
        fac->edg[0x02] = tmpedg;
    }

    g3dface_update ( fac );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        g3dvertex_normal ( fac->ver[i], 0x00 );
    }
}

/******************************************************************************/
void g3dface_setHidden ( G3DFACE *fac ) {
    fac->flags |= FACEHIDDEN;
}

/******************************************************************************/
void g3dface_unsetHidden ( G3DFACE *fac ) {
    fac->flags &= (~FACEHIDDEN);
}

/******************************************************************************/
void g3dface_setSelected ( G3DFACE *fac ) {
    fac->flags |= FACESELECTED;
}

/******************************************************************************/
void g3dface_unsetSelected ( G3DFACE *fac ) {
    fac->flags &= (~FACESELECTED);
}

/******************************************************************************/
int g3dface_pointin ( G3DFACE *fac, G3DVECTOR *vp ) {
    if ( fac->nbver == 0x03 ) {
        if ( g3dvector_sameside ( &fac->ver[0x00]->pos, &fac->ver[0x01]->pos, &fac->pos, vp ) &&
             g3dvector_sameside ( &fac->ver[0x01]->pos, &fac->ver[0x02]->pos, &fac->pos, vp ) &&
             g3dvector_sameside ( &fac->ver[0x02]->pos, &fac->ver[0x00]->pos, &fac->pos, vp ) ) {

            return 0x01;
        }
    } else {
        if ( g3dvector_sameside ( &fac->ver[0x00]->pos, &fac->ver[0x01]->pos, &fac->pos, vp ) &&
             g3dvector_sameside ( &fac->ver[0x01]->pos, &fac->ver[0x02]->pos, &fac->pos, vp ) &&
             g3dvector_sameside ( &fac->ver[0x02]->pos, &fac->ver[0x03]->pos, &fac->pos, vp ) && 
             g3dvector_sameside ( &fac->ver[0x03]->pos, &fac->ver[0x00]->pos, &fac->pos, vp ) ) {

            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dface_intersect ( G3DFACE *fac, G3DVECTOR *vo, G3DVECTOR *vf,
                                                          G3DVECTOR *vout ) {
    G3DVECTOR vfvo = { ( vo->x - vf->x ),
                       ( vo->y - vf->y ),
                       ( vo->z - vf->z ), 1.0f },
              vovf = { ( vf->x - vo->x ),
                       ( vf->y - vo->y ),
                       ( vf->z - vo->z ), 1.0f };
    float d = - ( ( fac->nor.x * fac->pos.x ) + 
                  ( fac->nor.y * fac->pos.y ) + 
                  ( fac->nor.z * fac->pos.z ) );
    float nom = ( fac->nor.x * vo->x ) +
                ( fac->nor.y * vo->y ) +
                ( fac->nor.z * vo->z ) + d;
    float den = ( fac->nor.x * vfvo.x ) + 
                ( fac->nor.y * vfvo.y ) +
                ( fac->nor.z * vfvo.z );
    float u;

    /*** may be useful to do the following check with some epsilon value ***/
    if ( den == 0.0f ) return 0x00;

    u = ( nom / den );

    if ( ( u >= 0.0f ) && ( u <= 1.0f ) ) {
        vout->x = vo->x + ( u * vovf.x );
        vout->y = vo->y + ( u * vovf.y );
        vout->z = vo->z + ( u * vovf.z );

        if ( g3dface_pointin ( fac, vout ) ) {

            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dface_position ( G3DFACE *fac ) {
    float    nbverdiv = ( fac->nbver == 0x03 ) ? ONETHIRD : 0.25f;
    uint32_t i;

    fac->pos.x = fac->pos.y = fac->pos.z = 0.0f;
    fac->pos.w = 1.0f;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->ver[i]->flags & VERTEXSKINNED ) {
            fac->pos.x += fac->ver[i]->skn.x;
            fac->pos.y += fac->ver[i]->skn.y;
            fac->pos.z += fac->ver[i]->skn.z;
        } else {
            fac->pos.x += fac->ver[i]->pos.x;
            fac->pos.y += fac->ver[i]->pos.y;
            fac->pos.z += fac->ver[i]->pos.z;
        }
    }

    fac->pos.x = ( fac->pos.x * nbverdiv );
    fac->pos.y = ( fac->pos.y * nbverdiv );
    fac->pos.z = ( fac->pos.z * nbverdiv );
}

/******************************************************************************/
void g3dface_update ( G3DFACE *fac ) {
    uint32_t i;

    /*** Compute face's position ***/
    g3dface_position ( fac );

    /*** Compute face's normal vector ***/
    g3dface_normal ( fac );
}

/******************************************************************************/
void g3dface_initWithEdges ( G3DFACE *fac, G3DVERTEX **ver, 
                                           G3DEDGE   **edg,
                                           uint32_t    nbver ) {
    uint32_t i;

    g3dface_init ( fac, ver, nbver );

    for ( i = 0x00; i < nbver; i++ ) {
        fac->edg[i] = edg[i];

        g3dedge_addFace ( edg[i], fac );
    }
}

/******************************************************************************/
void g3dface_init ( G3DFACE *fac, G3DVERTEX **ver, uint32_t nbver ) {
    uint32_t i;

    fac->nbver = nbver;

    /*** add this face to the vertex's faces list ***/
    for ( i = 0x00; i < nbver; i++ ) {
        fac->ver[i] = ver[i];

        g3dvertex_addFace ( ver[i], fac );
    }

    /*** update normal vector and face position ***/
    g3dface_update ( fac );
}

/******************************************************************************/
G3DFACE *g3dface_newWithEdges ( G3DVERTEX **ver,
                                G3DEDGE   **edg, uint32_t nbver ) {
    G3DFACE *fac;
    uint32_t i;

    fac = ( G3DFACE * ) calloc ( 0x01, sizeof ( G3DFACE ) );
    if ( fac == NULL ) {
        fprintf ( stderr, "g3dface_new: calloc failed\n" );

        return NULL;
    }

    g3dface_initWithEdges ( fac, ver, edg, nbver );

    return fac;
}

/******************************************************************************/
G3DFACE *g3dface_new ( G3DVERTEX **ver, uint32_t nbver ) {
    G3DFACE *fac;
    uint32_t i;

    fac = ( G3DFACE * ) calloc ( 0x01, sizeof ( G3DFACE ) );
    if ( fac == NULL ) {
        fprintf ( stderr, "g3dface_new: calloc failed\n" );

        return NULL;
    }

    g3dface_init ( fac, ver, nbver );

    return fac;
}

/******************************************************************************/
G3DFACE *g3dquad_new ( G3DVERTEX *v0, G3DVERTEX *v1,
                       G3DVERTEX *v2, G3DVERTEX *v3 ) {
    G3DVERTEX *ver[0x04] = { v0, v1, v2, v3 };

    return g3dface_new ( ver, 0x04 );
}

/******************************************************************************/
G3DFACE *g3dtriangle_new ( G3DVERTEX *v0, G3DVERTEX *v1,
                           G3DVERTEX *v2 ) {
    G3DVERTEX *ver[0x03] = { v0, v1, v2 };

    return g3dface_new ( ver, 0x03 );
}

/******************************************************************************/
void g3dface_linkVertices ( G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        g3dvertex_addFace ( fac->ver[i], fac );
    }
}

/*****************************************************************************/
/******* This function needs some extra treatment for > 4 Vertices faces *****/
void g3dface_normal ( G3DFACE *fac ) {
    G3DVERTEX *v0 = fac->ver[0x00],
              *v1 = fac->ver[0x01],
              *v2 = fac->ver[0x02];
    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos,
              *p2 = ( v2->flags & VERTEXSKINNED ) ? &v2->skn : &v2->pos;
    G3DVECTOR p0p1 = { ( p1->x - p0->x ),
                       ( p1->y - p0->y ),
                       ( p1->z - p0->z ), 1.0f },
              p0p2 = { ( p2->x - p0->x ),
                       ( p2->y - p0->y ),
                       ( p2->z - p0->z ), 1.0f };

    g3dvector_cross ( &p0p1, &p0p2, &fac->nor );

    if ( fac->nbver == 0x04 ) {
        G3DVERTEX *v3 = fac->ver[0x03];
        G3DVECTOR *p3 = ( v3->flags & VERTEXSKINNED ) ? &v3->skn : &v3->pos;

        G3DVECTOR p2p3 = { ( p3->x - p2->x ),
                           ( p3->y - p2->y ),
                           ( p3->z - p2->z ), 1.0f },
                  p2p0 = { ( p0->x - p2->x ),
                           ( p0->y - p2->y ),
                           ( p0->z - p2->z ), 1.0f };
        G3DVECTOR nor;

        g3dvector_cross ( &p2p3, &p2p0, &nor );

        g3dvector_normalize ( &nor, NULL );

        fac->nor.x = ( fac->nor.x + nor.x ) * 0.5f;
        fac->nor.y = ( fac->nor.y + nor.y ) * 0.5f;
        fac->nor.z = ( fac->nor.z + nor.z ) * 0.5f;
    }

    g3dvector_normalize ( &fac->nor, &fac->surface );
}

/******************************************************************************/
/*** Returns the vertex that does not belong to the edge passed as parameter **/
/*** Works only for triangles of course. ***/
G3DVERTEX *g3dface_getLoneVertex ( G3DFACE *fac, G3DEDGE *edg ) {
    if ( fac->nbver == 0x03 ) {
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            if ( ( fac->ver[i] != edg->ver[0x00] )&&
                 ( fac->ver[i] != edg->ver[0x01] ) ) {
                return fac->ver[i];
            }
        }
    }

    return NULL;
}

/******************************************************************************/
/**** This must be called after the face was added to a mesh or else expect ***/
/**** its edges to be NULL ***/
/*** Checks the orientation with the first neighbour found ***/
/*** Returns 0 = orientation is OK ***/
/*** Returns 1 = orientation should be inverted ***/
uint32_t g3dface_checkOrientation ( G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
                    /*** p = previous, n = next ***/
        int pi = ( i + fac->nbver - 0x01 ) % fac->nbver,
            ni = ( i + 0x01 ) % fac->nbver;
        G3DEDGE *edg = fac->edg[i];
        LIST *ltmpfac = edg->lfac;

        while ( ltmpfac ) {
            G3DFACE *nei = ( G3DFACE * ) ltmpfac->data;
            int j;

            if ( nei != fac ) {   
                for ( j = 0x00; j < nei->nbver; j++ ) {
                    /*** p = previous, n = next ***/
                    int pj = ( j + nei->nbver - 0x01 ) % nei->nbver,
                        nj = ( j + 0x01 ) % nei->nbver;

                    if ( nei->ver[j] == fac->ver[i] ) {
                        if ( nei->ver[pj] == fac->ver[pi] ) return 0x01;
                        if ( nei->ver[pj] == fac->ver[ni] ) return 0x00;
                        if ( nei->ver[nj] == fac->ver[pi] ) return 0x00;
                        if ( nei->ver[nj] == fac->ver[ni] ) return 0x01;
                    }
                }
            }

            ltmpfac = ltmpfac->next;
        }
    }

    return 0x00;
}

/******************************************************************************/
/*** Connect triangles by the edge passed as parameter. The triangles should **/
/*** belong to the same mesh, otherwise, expect strange behavior            ***/
/******************************************************************************/
G3DFACE *g3dface_merge ( G3DFACE *fac, G3DFACE *bud, G3DEDGE *edg ) {
    G3DVERTEX *ver[0x04] = { g3dface_getLoneVertex ( fac, edg ),
                             edg->ver[0x00],
                             g3dface_getLoneVertex ( bud, edg ),
                             edg->ver[0x01] };

    if ( ver[0x00] && ver[0x01] && ver[0x02] && ver[0x03] ) {
        return g3dface_new ( ver, 0x04 );
    }

    return NULL;
}

/******************************************************************************/
/**** Triagulate a QUAD. Dont use it on triangles ***/
void g3dface_triangulate ( G3DFACE *fac, G3DFACE *tri[0x02], int clockwise ) {
    if ( clockwise ) {
        tri[0x00] = g3dtriangle_new ( fac->ver[0x00],
                                      fac->ver[0x01], fac->ver[0x02] );
        tri[0x01] = g3dtriangle_new ( fac->ver[0x02],
                                      fac->ver[0x03], fac->ver[0x00] );
    } else {
        tri[0x00] = g3dtriangle_new ( fac->ver[0x00],
                                      fac->ver[0x01], fac->ver[0x03] );
        tri[0x01] = g3dtriangle_new ( fac->ver[0x01],
                                      fac->ver[0x02], fac->ver[0x03] );
    }
}

/******************************************************************************/
/********************** Returns the first shared edge. ************************/
/******************************************************************************/
G3DEDGE *g3dface_compare ( G3DFACE *fac, G3DFACE *bud ) {
    uint32_t i/*, nb = 0x00*/;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t j;

        for ( j = 0x00; j < bud->nbver; j++ ) {
            if ( fac->edg[i] == bud->edg[j] ) {
                return fac->edg[i];
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void g3dface_free ( G3DFACE *fac ) {
    /*printf ( "freeing memory for face\n" );*/

    free ( fac );
}