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

#ifndef _G3DENGINE_H_
#define _G3DENGINE_H_

#define GL_GLEXT_PROTOTYPES /*** for glBuildMipmap ***/

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#include <math.h>

/******************************************************************************/
#ifdef __linux__
#include <X11/X.h>
#ifdef WITH_MOTIF
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#endif
#endif

#ifdef __MINGW32__
#include <windows.h>
#endif

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __linux__
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

#ifdef __MINGW32__
#include <GL/glext.h>
PFNGLACTIVETEXTUREARBPROC       ext_glActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC     ext_glMultiTexCoord2fARB;
PFNGLCLIENTACTIVETEXTUREARBPROC ext_glClientActiveTextureARB;
void                          (*ext_glGenerateMipmap) (GLenum target);
#endif

/******************************************************************************/
#include <pthread.h>

/***#include <setjmp.h>***/
/******************************************************************************/
#ifdef __MINGW32__
#undef HAVE_STDLIB_H
#endif
#include <jpeglib.h>

/******************************** FreeType ************************************/
#include <ft2build.h>
#include FT_FREETYPE_H

/******************************************************************************/
#include <list.h>

/******************************************************************************/
#define ONETHIRD 0.3333333f

#define EPSILON 0.0001f
#define NORMMAX 1.001f
#define NORMMIN 0.999f

#define SETPOSITION      ( 1      )
#define SETNORMAL        ( 1 << 1 )
#define SETEDGELIST      ( 1 << 2 )
#define SETFACELIST      ( 1 << 3 )

/******************************************************************************/
#define BBOXLEN 0.2f

/********************************** Error codes *******************************/
#define G3DERROR_SUCCESS         0x00000000
#define G3DERROR_OBJECT_REFERRED  0x00000010

/******************************* Engine Flags *********************************/


#define VIEWOBJECT         ((uint64_t)1        )
#define VIEWVERTEX         ((uint64_t)1  <<  1 )
#define VIEWEDGE           ((uint64_t)1  <<  2 )
#define VIEWFACE           ((uint64_t)1  <<  3 )
#define VIEWFACENORMAL     ((uint64_t)1  <<  4 )
#define VIEWVERTEXNORMAL   ((uint64_t)1  <<  5 )
#define VIEWNORMALS        ((uint64_t)VIEWFACENORMAL | VIEWVERTEXNORMAL )
#define VIEWSKIN           ((uint64_t)1  <<  6 )
#define VIEWUVWMAP         ((uint64_t)1  <<  7 )

#define VIEWAXIS           ((uint64_t)1  <<  9 )
#define VIEWSCULPT         ((uint64_t)1  << 10 )
#define VIEWPATH           ((uint64_t)1  << 11 )
#define VIEWDETAILS        ((uint64_t)VIEWUVWMAP | VIEWSKIN | \
                                      VIEWVERTEX | VIEWEDGE | VIEWFACE | \
                                      VIEWFACENORMAL | VIEWVERTEXNORMAL )
#define MODEMASK           ((uint64_t)VIEWOBJECT | VIEWUVWMAP | VIEWSKIN | \
                                      VIEWVERTEX | VIEWEDGE   | VIEWFACE | \
                                      VIEWPATH   | VIEWAXIS   | VIEWSCULPT )
#define SELECTMODE         ((uint64_t)1  << 12 )
#define XAXIS              ((uint64_t)1  << 13 )
#define YAXIS              ((uint64_t)1  << 14 ) 
#define ZAXIS              ((uint64_t)1  << 15 )
#define G3DMULTITHREADING  ((uint64_t)1  << 16 )
#define KEEPVISIBLEONLY    ((uint64_t)1  << 17 )
#define SYMMETRYVIEW       ((uint64_t)1  << 18 )
#define ONGOINGANIMATION   ((uint64_t)1  << 19 ) /*** This helps us to ***/
                                       /*** forbid buffered subdivision ***/
#define HIDEBONES          ((uint64_t)1  << 20 )
#define HIDEGRID           ((uint64_t)1  << 21 )
#define NOLIGHTING         ((uint64_t)1  << 22 )
#define NODISPLACEMENT     ((uint64_t)1  << 23 )
#define NOTEXTURE          ((uint64_t)1  << 24 )
#define LOADFULLRESIMAGES  ((uint64_t)1  << 25 ) /* used by the renderer especially for animated textures */
#define NODRAWPOLYGON      ((uint64_t)1  << 26 )
#define NOBACKGROUNDIMAGE  ((uint64_t)1  << 27 )
#define MODIFIERTOOKOVER   ((uint64_t)1  << 28 )
#define ONGOINGRENDERING   ((uint64_t)1  << 29 )

/************************ G3DOBJECT's draw callback return flags **************/
#define DRAW_LIGHTON          ( 1 << 0 )


/******************************* UVMAp Editor flags ***************************/
#define VIEWVERTEXUV       ((uint64_t)1  << 29 )
#define VIEWFACEUV         ((uint64_t)1  << 30 )
#define UVMODEMASK         ((uint64_t)VIEWVERTEXUV | VIEWFACEUV )


#define DIFFUSECHANNELID        0x00
#define SPECULARCHANNELID       0x01
#define DISPLACEMENTCHANNELID   0x02
#define ALPHACHANNELID          0x03
#define BUMPCHANNELID           0x04
#define AMBIENTCHANNELID        0x05
#define REFLECTIONCHANNELID     0x06
#define REFRACTIONCHANNELID     0x07
#define UVCHANNELMASK      ( ((uint64_t)1  << 30 ) | \
                             ((uint64_t)1  << 31 ) | \
                             ((uint64_t)1  << 32 ) | \
                             ((uint64_t)1  << 33 ) )
#define SHOWCHANNEL(f,a)  (f=(f&(~UVCHANNELMASK)|((uint64_t)a<<30)))
#define GETCHANNEL(f)    ((f&UVCHANNELMASK)>>30)

/******************************* Object Types *********************************/
#define OBJECT          ( ( uint64_t )  1       )
#define PRIMITIVE       ( ( uint64_t )  1 << 1  )
#define SPHERE          ( ( uint64_t )  1 << 2  )
#define TORUS           ( ( uint64_t )  1 << 3  )
#define MESH            ( ( uint64_t )  1 << 4  )
#define CAMERA          ( ( uint64_t )  1 << 5  )
#define SCENE           ( ( uint64_t )  1 << 6  )
#define BONE            ( ( uint64_t )  1 << 7  )
#define LIGHT           ( ( uint64_t )  1 << 8  )
#define SPOT            ( ( uint64_t )  1 << 9  )
#define CUBE            ( ( uint64_t )  1 << 10 )
#define SYMMETRY        ( ( uint64_t )  1 << 11 )
#define CYLINDER        ( ( uint64_t )  1 << 12 )
#define MODIFIER        ( ( uint64_t )  1 << 13 )
#define FFD             ( ( uint64_t )  1 << 14 )
#define PLANE           ( ( uint64_t )  1 << 15 )
#define CONE            ( ( uint64_t )  1 << 16 )
#define UVMAP           ( ( uint64_t )  1 << 17 )
#define PIVOT           ( ( uint64_t )  1 << 18 )
#define SUBDIVIDER      ( ( uint64_t )  1 << 19 )
#define WIREFRAME       ( ( uint64_t )  1 << 20 )
#define MULTIPLIER      ( ( uint64_t )  1 << 21 )
#define EDITABLE        ( ( uint64_t )  1 << 22 )
#define SPLINE          ( ( uint64_t )  1 << 23 )
#define SPLINEREVOLVER  ( ( uint64_t )  1 << 24 )
#define TEXT            ( ( uint64_t )  1 << 25 )
#define TUBE            ( ( uint64_t )  1 << 26 )
#define MORPHER         ( ( uint64_t )  1 << 27 )
#define SKIN            ( ( uint64_t )  1 << 28 )
#define INSTANCE        ( ( uint64_t )  1 << 29 )
#define PARTICLEEMITTER ( ( uint64_t )  1 << 30 )

#define G3DOBJECTTYPE     ( OBJECT )
#define G3DNULLTYPE       ( G3DOBJECTTYPE )
#define G3DMESHTYPE       ( OBJECT | MESH | EDITABLE )
#define G3DPRIMITIVETYPE  ( OBJECT | MESH | EDITABLE | PRIMITIVE )
#define G3DSPHERETYPE     ( OBJECT | MESH | EDITABLE | PRIMITIVE | SPHERE   )
#define G3DPLANETYPE      ( OBJECT | MESH | EDITABLE | PRIMITIVE | PLANE    )
#define G3DTORUSTYPE      ( OBJECT | MESH | EDITABLE | PRIMITIVE | TORUS    )
#define G3DCUBETYPE       ( OBJECT | MESH | EDITABLE | PRIMITIVE | CUBE     )
#define G3DCYLINDERTYPE   ( OBJECT | MESH | EDITABLE | PRIMITIVE | CYLINDER )
#define G3DTUBETYPE       ( OBJECT | MESH | EDITABLE | PRIMITIVE | TUBE     )
#define G3DCONETYPE       ( OBJECT | MESH | EDITABLE | PRIMITIVE | CONE     )
#define G3DSYMMETRYTYPE   ( OBJECT | MULTIPLIER | SYMMETRY )
#define G3DCAMERATYPE     ( OBJECT | CAMERA )
#define G3DSCENETYPE      ( OBJECT | SCENE )
#define G3DBONETYPE       ( OBJECT | BONE )
                    /* ffd not flagged as mesh but still inherits from mesh */
#define G3DFFDTYPE            ( OBJECT        | MODIFIER | FFD | EDITABLE )
#define G3DWIREFRAMETYPE      ( OBJECT | MESH | MODIFIER | WIREFRAME )
#define G3DSUBDIVIDERTYPE     ( OBJECT        | MODIFIER | SUBDIVIDER )
#define G3DSPLINEREVOLVERTYPE ( OBJECT | MESH | MODIFIER | SPLINEREVOLVER )
#define G3DLIGHTTYPE          ( OBJECT | LIGHT )
#define G3DUVMAPTYPE          ( OBJECT | UVMAP )
#define G3DPIVOTTYPE          ( OBJECT | PIVOT )
#define G3DSPLINETYPE         ( OBJECT | SPLINE )
#define G3DTEXTTYPE           ( OBJECT | MESH | TEXT )
/*** we dont need the MESH bit for the morpher modifier, that way it is ***/
/*** ignore by g3dobject_getByType(), and mororver, it doesn't use vertex ***/
/*** list or face list or edge list ***/
#define G3DMORPHERTYPE         ( OBJECT        | MODIFIER | MORPHER )
#define G3DSKINTYPE            ( OBJECT        | MODIFIER | SKIN )
#define G3DINSTANCETYPE        ( OBJECT        | INSTANCE )
#define G3DPARTICLEEMITTERTYPE ( OBJECT        | PARTICLEEMITTER )

/******************************************************************************/
/** symmetry orientation ***/
/**** Vertex flags ***/
#define VERTEXSELECTED     (  1       )
#define VERTEXSYMYZ        (  1 << 1  )
#define VERTEXSYMXY        (  1 << 2  )
#define VERTEXSYMZX        (  1 << 3  )
#define VERTEXSYMALL       ( VERTEXSYMYZ | VERTEXSYMXY | VERTEXSYMZX )
#define VERTEXTOPOLOGY     (  1 << 4  ) /*** Subdivision: set topology ? ***/
/*#define VERTEXFACETOPOLOGY (  1 << 5  ) *//*** Subdivision: set topology ? ***/
/*#define VERTEXEDGETOPOLOGY (  1 << 6  ) *//*** Subdivision: set topology ? ***/
#define VERTEXMALLOCFACES  (  1 << 7  ) /*** Subdivision: malloc faces ? ***/
#define VERTEXMALLOCEDGES  (  1 << 8  ) /*** Subdivision: malloc edges ? ***/
#define VERTEXMALLOCUVS    (  1 << 9  ) /*** Subdivision: malloc UVs   ? ***/
#define VERTEXSPLIT        (  1 << 10 ) /*** for extrusion ***/
#define VERTEXPAINTED      (  1 << 11 ) /*** Skin view mode ***/
#define VERTEXSKINNED      (  1 << 12 ) /*** Use Skin position ***/
#define VERTEXUSEADAPTIVE  (  1 << 13 ) /*** Adaptive subdivision flag for face subver ***/
#define VERTEXLOCKADAPTIVE (  1 << 14 ) /*** Adaptive subdivision flag for face subver ***/
#define VERTEXORIGINAL     (  1 << 15  )
#define VERTEXSUBDIVIDED   (  1 << 16  )
#define VERTEXINNER        (  1 << 17  )
#define VERTEXOUTER        (  1 << 18  )
#define VERTEXELEVATED     (  1 << 19  )

#define VERTEXONEDGE       (  1 << 20  )
#define VERTEXSTITCHABLE   (  1 << 21  )
#define VERTEXHANDLE       (  1 << 22  )

/******************************* Texture Flags ********************************/
#define TEXTURESELECTED   ( 1      )
#define TEXTUREDISPLACE   ( 1 << 1 )
#define TEXTURERESTRICTED ( 1 << 2 )
#define TEXTUREREPEATED   ( 1 << 3 )

/******************************** Edge Flags **********************************/
#define EDGESELECTED      (  1       )
#define EDGEMALLOCFACES   (  1 << 1  )
#define EDGEUSEADAPTIVE   (  1 << 2  )
#define EDGELOCKADAPTIVE  (  1 << 3  )
#define EDGESUBDIVIDED    (  1 << 4  )
#define EDGEORIGINAL      (  1 << 5  )
#define EDGEPOSITIONNED   (  1 << 6  )
#define EDGEINNER         (  1 << 7  )
#define EDGEOUTER         (  1 << 8  )
#define EDGETOPOLOGY      (  1 << 9  )
#define EDGEREVERT        (  1 << 10 )
/*** Can be used by functions to temporarily mark a face. Call g3dedge_mark ***/
/*** Function MUST clear the flag after processing. Call g3dedge_unmark ***/
#define EDGEMARKED        (  1 << 31 )

/******************************** Face Flags **********************************/
#define FACESELECTED          (  1       )
#define FACEALIGNED           (  1 << 1  )
#define FACECOMPUTECENTER     (  1 << 2  )
#define FACEHIDDEN            (  1 << 3  )
#define FACEMALLOCUVSETS      (  1 << 4  )
#define FACEMALLOCTEXTURES    (  1 << 5  )
#define FACESUBDIVIDED        (  1 << 6  )
#define FACEUSEADAPTIVE       (  1 << 7  )
#define FACEDRAWEDGES         (  1 << 8  )
#define FACEDRAWVERTICES      (  1 << 9  )
#define FACEORIGINAL          (  1 << 10 )
#define FACEINNER             (  1 << 11 )
#define FACEOUTER             (  1 << 12 )
#define FACEFROMQUAD          (  1 << 13 ) /*** subface ancestor is a quad ***/
#define FACEFROMTRIANGLE      (  1 << 14 ) /*** subface ancestor is a triangle ***/
/*** Can be used by functions to temporarily mark a face. Call g3dface_mark ***/
/*** Function MUST clear the flag after processing. Call g3dface_unmark ***/
#define FACEMARKED            (  1 << 31 )
                                         


/********************************** UV Flags **********************************/
#define UVSELECTED            (  1       )

/******************************** UVSets Flags ********************************/
#define UVSETSELECTED         (  1       )

/******************************** height Flags ********************************/
#define HEIGHTSET (  1       )

/***************************** Subdivision Flags ******************************/
#define SUBDIVISIONMALLOCVERTICES (  1       )
#define SUBDIVISIONMALLOCEDGES    (  1 <<  1 )
#define SUBDIVISIONMALLOCFACES    (  1 <<  2 )
#define SUBDIVISIONMALLOCUVSETS   (  1 <<  3 )
#define SUBDIVISIONCLEANVERTICES  (  1 <<  4 )
#define SUBDIVISIONCLEANEDGES     (  1 <<  5 )
#define SUBDIVISIONCLEANFACES     (  1 <<  6 )
#define SUBDIVISIONCLEANORIGINAL  (  1 <<  7 )
#define SUBDIVISIONLASTSTEP       (  1 <<  8 ) /*** Adaptive mode ***/
#define SUBDIVISIONDRAWEDGES      (  1 <<  9 ) 
#define SUBDIVISIONDRAWVERTICES   (  1 << 10 )
#define SUBDIVISIONELEVATE        (  1 << 11 ) /* is heightmapping required */
#define SUBDIVISIONCOMMIT         (  1 << 12 )
#define SUBDIVISIONDUMP           (  1 << 13 ) 
#define SUBDIVISIONCOMPUTE        (  1 << 14 )
#define SUBDIVISIONDISPLAY        (  1 << 15 )
#define SUBDIVISIONINDEX          (  1 << 16 )
#define SUBDIVISIONNOELEVATE      (  1 << 17 )

/************************* Subdivision topology Flags *************************/
#define NEEDEDGETOPOLOGY              (  1       )
#define NEEDFULLTOPOLOGY               0xFFFFFFFF

/***************************** Weightgroup Flags ******************************/
#define WEIGHTGROUPSELECTED   ( 1      )

/***************************** Meshpose Flags ******************************/
#define MESHPOSESELECTED      ( 1      )

/******************************* Keyframe Flags *******************************/
#define KEYSELECTED  ( 1      )
#define KEYLOOP      ( 1 << 1 )
#define KEYPOSITION  ( 1 << 2 )
#define KEYROTATION  ( 1 << 3 )
#define KEYSCALING   ( 1 << 4 )
#define KEYDATA      ( 1 << 5 )

/****************************** Object flags **********************************/
/*
 * first 16 bits are shared
 * next 12 bits are object defined
 * Last 4 bits are user defined
 */
#define OBJECTSELECTED            (  1       )
#define OBJECTNOSYMMETRY          (  1 <<  1 )
#define OBJECTINVISIBLE           (  1 <<  2 )
#define OBJECTINACTIVE            (  1 <<  3 )
/*** unused                       (  1 <<  4 ) ***/
/*** unused                       (  1 <<  5 ) ***/
#define OBJECTNOTRANSLATION       (  1 <<  6 )
#define OBJECTNOROTATION          (  1 <<  7 )
#define OBJECTNOSCALING           (  1 <<  8 )
#define OBJECTNOSHADING           (  1 <<  9 )
#define MODIFIERNEEDSNORMALUPDATE (  1 << 10 )
#define OBJECTORPHANED            (  1 << 11 )

/*** Private flags ***/
/*** Bone flags ***/
#define BONEFIXED             (  1 << 17 )
#define BONEFROMFILE          (  1 << 18 )
/*** Mesh private flags ***/
#define MESHUSEADAPTIVE       (  1 << 19 )
#define MESHUSEISOLINES       (  1 << 20 )
#define MESHGEOMETRYONLY      (  1 << 21 )
#define MESHGEOMETRYINARRAYS  (  1 << 22 ) /* use arrays instead of lists */

/*** Light flags ***/
#define LIGHTCASTSHADOWS      (  1 << 17 )
#define SPOTLIGHT             (  1 << 18 )
#define KEYLIGHT              (  1 << 19 )
#define LIGHTHARDSHADOWS      (  1 << 20 )
#define LIGHTSOFTSHADOWS      (  1 << 21 )

/*** UVMap flags ***/
#define UVMAPFIXED            (  1 << 17 )
/*** Wireframe flags ***/
#define TRIANGULAR            (  1 << 17 )
/*** Subdivider flags ***/
#define SYNCLEVELS            (  1 << 17 ) /*** edit and render levels synced ***/
/*** Spline flags ***/
#define QUADRATIC             (  1 << 17 )
#define CUBIC                 (  1 << 18 )
/**** CAMERA flags ***/
#define CAMERAORTHOGRAPHIC    (  1 << 17 )
#define CAMERAPERSPECTIVE     (  1 << 18 )
#define CAMERADOF             (  1 << 19 )
#define CAMERAXY              (  1 << 20 )
#define CAMERAYZ              (  1 << 21 )
#define CAMERAZX              (  1 << 22 )
#define CAMERADEFAULT         (  1 << 23 ) /*** not part of the obj hierarchy ***/
/**** SPHERE flags ***/
#define SPHEREISPERFECT       (  1 << 17 ) /* render perfect */

#define INSTANCEMIRRORED      (  1 << 17 )

/**** Particle Emitter flags ***/
#define DISPLAYPARTICLES      (  1 << 17 )
#define KEYPARTICLEEMITTER    (  1 << 18 )

/******************************** Update flags ********************************/
#define COMPUTEFACEPOINT         (  1       )
#define COMPUTEEDGEPOINT         (  1 <<  1 )
#define NOVERTEXNORMAL           (  1 <<  2 )
#define UPDATEVERTEXNORMAL       (  1 <<  3 )
#define UPDATEFACENORMAL         (  1 <<  4 )
#define UPDATEFACEPOSITION       (  1 <<  5 )
#define RESETMODIFIERS           (  1 <<  6 )
#define UPDATEMODIFIERS          (  1 <<  7 )
#define COMPUTEUVMAPPING         (  1 <<  8 )
#define EDGECOMPUTENORMAL        (  1 <<  9 )
#define UPDATESKIN               (  1 << 10 )

/*** Mesh color ***/
#define MESHCOLORUB 0x80
#define MESHCOLORF  0.5f



/******************************* Orientation **********************************/
#define ORIENTATIONZX 0x00
#define ORIENTATIONXY 0x01
#define ORIENTATIONYZ 0x02

#define ZXSTR "ZX"
#define XYSTR "XY"
#define YZSTR "YZ"

#define XSTR "X"
#define YSTR "Y"
#define ZSTR "Z"

/****************************** Symmetry  *************************************/
#define SYMMETRYZX ORIENTATIONZX
#define SYMMETRYXY ORIENTATIONXY
#define SYMMETRYYZ ORIENTATIONYZ

/**************************** Plane primitive *********************************/
#define PLANEZX ORIENTATIONZX
#define PLANEXY ORIENTATIONXY
#define PLANEYZ ORIENTATIONYZ

/**************************** Torus primitive *********************************/
#define TORUSZX ORIENTATIONZX
#define TORUSXY ORIENTATIONXY
#define TORUSYZ ORIENTATIONYZ

/**************************** Instance object *********************************/
#define INSTANCEZX ORIENTATIONZX
#define INSTANCEXY ORIENTATIONXY
#define INSTANCEYZ ORIENTATIONYZ

/******************************************************************************/
/*** for glLoadName ***/
#define CURSORXAXIS 0x00
#define CURSORYAXIS 0x01
#define CURSORZAXIS 0x02

/***************************** UVW Map policies --*****************************/
#define UVMAPFLAT        0x00  /*** default projection ***/
#define UVMAPSPHERICAL   0x01
#define UVMAPCYLINDRICAL 0x02
#define UVMAPBACKGROUND  0x03

/****************************** Material Flags ********************************/
#define DIFFUSE_ENABLED      ( 1       )
#define SPECULAR_ENABLED     ( 1 <<  1 )
#define DISPLACEMENT_ENABLED ( 1 <<  2 )
#define BUMP_ENABLED         ( 1 <<  3 )
#define REFLECTION_ENABLED   ( 1 <<  4 )
#define REFRACTION_ENABLED   ( 1 <<  5 )
#define ALPHA_ENABLED        ( 1 <<  6 )

/******************************* Channel Flags ********************************/
#define USESOLIDCOLOR        ( 1       )
#define USEIMAGECOLOR        ( 1 <<  1 )
#define USEPROCEDURAL        ( 1 <<  2 )
#define USECHANNELMASK       ( USESOLIDCOLOR | USEIMAGECOLOR | USEPROCEDURAL )

/******************************* Procedural types *****************************/
#define PROCEDURALNOISE        0x00
#define PROCEDURALCHESS        0x01
#define PROCEDURALBRICK        0x02
#define PROCEDURALGRADIENT     0x03

/******************************************************************************/
#define _GETVERTEX(mes,ltmpver) \
((((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) ? ( G3DVERTEX * ) ltmpver : \
                                                      ( G3DVERTEX * ) ltmpver->data )

#define _NEXTVERTEX(mes,ltmpver) \
if (((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) { \
    G3DSUBVERTEX *subver = ( G3DSUBVERTEX * ) ltmpver; \
    ltmpver = (LIST*)((G3DSUBVERTEX*)++subver); \
    if ( ltmpver == ( LIST * ) ( ( G3DSUBVERTEX * ) mes->lver + mes->nbver ) ) { \
        ltmpver = NULL; \
    } \
} else { \
    ltmpver = ltmpver->next; \
}

/******************************************************************************/
#define _GETEDGE(mes,ltmpedg) \
((((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) ? ( G3DEDGE * ) ltmpedg : \
                                                      ( G3DEDGE * ) ltmpedg->data )

#define _NEXTEDGE(mes,ltmpedg) \
if (((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) { \
    G3DSUBEDGE *subedg = ( G3DSUBEDGE * ) ltmpedg; \
    ltmpedg = (LIST*)((G3DSUBEDGE*)++subedg); \
    if ( ltmpedg == ( LIST * ) ( ( G3DSUBEDGE * ) mes->ledg + mes->nbedg ) ) { \
        ltmpedg = NULL; \
    } \
} else { \
    ltmpedg = ltmpedg->next; \
}

/******************************************************************************/
#define _GETFACE(mes,ltmpfac) \
((((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) ? ( G3DFACE * ) ltmpfac : \
                                                      ( G3DFACE * ) ltmpfac->data )

#define _NEXTFACE(mes,ltmpfac) \
if (((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) { \
    G3DSUBFACE *subfac = ( G3DSUBFACE * ) ltmpfac; \
    ltmpfac = (LIST*)((G3DSUBFACE*)++subfac); \
    if ( ltmpfac == ( LIST * ) ( ( G3DSUBFACE * ) mes->lfac + mes->nbfac ) ) { \
        ltmpfac = NULL; \
    } \
} else { \
    ltmpfac = ltmpfac->next; \
}

/******************************************************************************/
#define _FASTLENGTH(vec) ( sqrt ( ( vec.x * vec.x ) + \
                                  ( vec.y * vec.y ) + \
                                  ( vec.z * vec.z ) ) )

#define _FASTNORMALIZE(vec)                       \
{                                                 \
        float number = ( (vec)->x * (vec)->x ) +  \
                       ( (vec)->y * (vec)->y ) +  \
                       ( (vec)->z * (vec)->z );   \
	long i;                                   \
	float x2, y;                              \
	const float threehalfs = 1.5f;            \
                                                  \
	x2 = number * 0.5f;                       \
	y  = number;                              \
	i  = * ( long * ) &y;                     \
	i  = 0x5f3759df - ( i >> 1 );             \
	y  = * ( float * ) &i;                    \
	y  = y * ( threehalfs - ( x2 * y * y ) ); \
                                                  \
	(vec)->x = (vec)->x * y;                  \
	(vec)->y = (vec)->y * y;                  \
	(vec)->z = (vec)->z * y;                  \
}

/******************************************************************************/
#include <g3dengine/g3dcolor.h>
#include <g3dengine/g3drgba.h>
#include <g3dengine/g2dvector.h>
#include <g3dengine/g3dtinyvector.h>
#include <g3dengine/g3dvector.h>
#include <g3dengine/g3dpick.h>

/******************************************************************************/
typedef struct G3DSEGMENT {
    G3DVECTOR src;
    G3DVECTOR dst;
} G3DSEGMENT;

/******************************************************************************/
typedef struct G3DGLOBALS {
    GLint lightID;
} G3DGLOBALS;

/******************************************************************************/
typedef struct _G3DVECTORCACHE {
    G3DVECTOR ref;
    G3DVECTOR buf;
} G3DVECTORCACHE;

/******************************************************************************/
/**** This is NOT to be confused with the plane primitive ***/
typedef struct _G3DPLANE {
    G3DVECTOR pos;
    G3DVECTOR nor;
    float d;
} G3DPLANE;

/******************************************************************************/
typedef struct _G3DDOUBLEVECTOR {
    double x, y, z, w;
} G3DDOUBLEVECTOR;

/******************************************************************************/
typedef struct _G3DBBOX {   /*** Bounding box   ***/
    float xmin, ymin, zmin; /*** Minimum coords ***/
    float xmax, ymax, zmax; /*** Maximum coords ***/
} G3DBBOX;

#include <g3dengine/g3dimage.h>


/******************************************************************************/
typedef struct _G3DPROCEDURAL {
    uint32_t type;
    uint32_t flags;
    void (*getColor)( struct _G3DPROCEDURAL *, double, 
                                               double, 
                                               double, G3DCOLOR * );
    G3DIMAGE image;
    unsigned char *preview; /*** holds the generated image data ***/
} G3DPROCEDURAL;

/******************************************************************************/
#define MAXNOISECOLORS 0x04
#define NOISE_INTERPOLATION_LINEAR 0x00
#define NOISE_INTERPOLATION_FADE   0x01
#define NOISE_INTERPOLATION_COSINE 0x02

typedef struct _G3DPROCEDURALNOISE {
    G3DPROCEDURAL procedural;
    G3DCOLOR      colorPair[MAXNOISECOLORS][0x02];
    float         threshold[MAXNOISECOLORS];
    uint32_t      nbColors;
    uint32_t      nbOctaves;
    uint32_t      interpolation;
    G3DVECTOR    *gradients;
    uint32_t      nbGradientX;
    uint32_t      nbGradientY;
} G3DPROCEDURALNOISE;

/******************************************************************************/
typedef struct _G3DPROCEDURALCHESS {
    G3DPROCEDURAL procedural;
    G3DCOLOR color1;
    G3DCOLOR color2;
    uint32_t udiv;
    uint32_t vdiv;
} G3DPROCEDURALCHESS;

/******************************************************************************/
typedef struct _G3DPROCEDURALGRADIENT {
    G3DPROCEDURAL procedural;
    G3DCOLOR color1;
    G3DCOLOR color2;
    uint32_t horizontal;
} G3DPROCEDURALGRADIENT;

/******************************************************************************/
typedef struct _G3DPROCEDURALBRICK {
    G3DPROCEDURAL procedural;
    G3DCOLOR brickColor;
    G3DCOLOR spacingColor;
    uint32_t nbBricksPerLine;
    uint32_t nbLines;
    float uspacing;
    float vspacing;
} G3DPROCEDURALBRICK;

/******************************************************************************/
typedef struct _G3DCHANNEL {
    uint32_t        flags;
    char          *name;
    G3DCOLOR       solid;
    G3DIMAGE      *image;
    G3DPROCEDURAL *proc;
} G3DCHANNEL;

/******************************************************************************/
typedef struct _G3DMATERIAL {
    uint32_t   id;
    uint32_t    flags;
    char      *name;
    G3DCHANNEL diffuse;
    G3DCHANNEL ambient;
    G3DCHANNEL displacement;
    G3DCHANNEL bump;
    G3DCHANNEL specular; 
    G3DCHANNEL reflection; 
    G3DCHANNEL refraction;
    G3DCHANNEL alpha;
    float     specular_level;  /*** specular intensity ***/
    float     shininess;  /*** specular shininess ***/
    float     alphaOpacity;  /*** specular shininess ***/
    LIST     *lobj;
    uint32_t  nbobj;
} G3DMATERIAL;

/******************************************************************************/
typedef struct _G3DRTVERTEX {
    float         r, g, b;
    G3DTINYVECTOR nor;
    G3DTINYVECTOR pos;
    uint32_t      id;
    uint32_t       flags;
} G3DRTVERTEX;

/******************************************************************************/
typedef struct _G3DVERTEX {
    uint32_t      flags;
    uint32_t     id;      /*** Vertex ID - Should never be trusted !        ***/
    uint32_t     geoID;   /*** geomtry ID, all types included               ***/
    G3DVECTOR    pos;     /*** Vertex position                              ***/
    G3DVECTOR    nor;     /*** Vertex normal vector                         ***/
    LIST        *lfac;    /*** list of faces connected to this vertex       ***/
    LIST        *ledg;    /*** list of connected edges                      ***/
    LIST        *lwei;    /*** List of weight                               ***/
    uint32_t     nbfac;   /*** number of connected faces                    ***/
    uint32_t     nbedg;   /*** number of connected edges                    ***/
    uint32_t     nbwei;   /*** number of weights                            ***/
    float        weight;  /*** weight value used when editing weight groups ***/
    G3DRTVERTEX *rtvermem;/*** Vertex buffer in buffered mode               ***/
    LIST        *lext;    /*** list of vertex extensions                    ***/
    struct _G3DSUBVERTEX *subver;
} G3DVERTEX;

/******************************************************************************/
typedef struct _G3DSPLITVERTEX {
    G3DVERTEX *oriver;
    G3DVERTEX *newver;
} G3DSPLITVERTEX;

/******************************************************************************/
typedef struct _G3DCAMERA G3DCAMERA;
typedef struct _G3DMESH   G3DMESH;
typedef struct _G3DSPLINE G3DSPLINE;
typedef struct _G3DKEY    G3DKEY;
typedef struct _G3DCURVE  G3DCURVE;
typedef struct _G3DRIG    G3DRIG;
typedef struct _G3DTAG    G3DTAG;
typedef struct _G3DSCENE  G3DSCENE;

#define GEOMETRYMOVE_CALLBACK(f) ((void(*)      (G3DMESH *, \
                                                 LIST *,\
                                                 LIST *,\
                                                 LIST *,\
                                                 G3DMODIFYOP,\
                                                 uint64_t ))f)

#define TRANSFORM_CALLBACK(f)    ((void(*)      (G3DOBJECT *,  \
                                                 uint64_t))f)

#define COPY_CALLBACK(f)         ((G3DOBJECT*(*)(G3DOBJECT*,   \
                                                 uint32_t,     \
                                                 const char*,  \
                                                 uint64_t))f)    

#define ACTIVATE_CALLBACK(f)     ((void(*)      (G3DOBJECT*,   \
                                                 uint64_t))f)    

#define DEACTIVATE_CALLBACK(f)   ((void(*)      (G3DOBJECT*,   \
                                                 uint64_t))f)    

#define COMMIT_CALLBACK(f)       ((G3DOBJECT*(*)(G3DOBJECT*,   \
                                                 uint32_t,     \
                                                 const char *, \
                                                 uint64_t))f)

#define ADDCHILD_CALLBACK(f)     ((void(*)      (G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 uint64_t))f)    

#define SETPARENT_CALLBACK(f)    ((void(*)      (G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 uint64_t))f)

#define DRAW_CALLBACK(f)         ((uint32_t(*)  (G3DOBJECT*,   \
                                                 G3DCAMERA*,   \
                                                 uint64_t))f)

#define FREE_CALLBACK(f)         ((void(*)      (G3DOBJECT*))f)

#define PICK_CALLBACK(f)         ((uint32_t(*)  (G3DOBJECT*,   \
                                                 G3DCAMERA*,   \
                                                 uint64_t))f)

#define ANIM_CALLBACK(f)         ((void(*)      (G3DOBJECT*,   \
                                                 float,        \
                                                 uint64_t))f)

#define POSE_CALLBACK(f)         ((void(*)      (G3DOBJECT*,   \
                                                 G3DKEY*))f)

#define UPDATE_CALLBACK(f)       ((void(*)      (G3DOBJECT*,   \
                                                 uint64_t))f)

#define OBJECTKEY_FUNC(f) ((void(*)(G3DKEY*,void*))f)
#define OBJECTBROWSE_FUNC(f) ((uint32_t (*)(G3DOBJECT*,void*,uint64_t))f)

/******************************************************************************/
typedef struct _G3DOBJECT {
    uint32_t id;            /*** Object ID               ***/
    uint64_t type;          /*** Flag for object type    ***/
    uint32_t flags;         /*** selected or not etc ... ***/
    uint32_t update_flags;
    char *name;             /*** Object's name           ***/
    G3DVECTOR pos;          /*** Object center position  ***/
    G3DVECTOR rot;          /*** Object center angles    ***/
    G3DVECTOR rotXAxis;
    G3DVECTOR rotYAxis;
    G3DVECTOR rotZAxis;
    G3DVECTOR sca;          /*** Object's center scaling ***/
    double  lmatrix[0x10];   /*** Local matrix, i.e relative to its parent ***/
    double ilmatrix[0x10];  /*** Inverse local matrix ***/  
    double  wmatrix[0x10];   /*** World matrix, i.e absolute ***/
    double iwmatrix[0x10];  /*** Inverse World matrix, i.e absolute ***/
    double  rmatrix[0x10];   /*** rotation matrix ***/
    /*** Drawing function ***/
    uint32_t (*draw)          ( struct _G3DOBJECT *obj, 
                                struct _G3DCAMERA *cam, 
                                        uint64_t   engine_flags );
    /*** Free memory function ***/
    void     (*free)          ( struct _G3DOBJECT *obj );
    /*** Object selection ***/
    uint32_t     (*pick)      ( struct _G3DOBJECT *obj, 
                                struct _G3DCAMERA *cam,
                                        uint64_t   engine_flags );
    void     (*anim)          ( struct  _G3DOBJECT *obj,
                                float    frame, 
                                uint64_t engine_flags );
    void     (*update)        ( struct  _G3DOBJECT *obj,
                                         uint64_t   engine_flags );
    void     (*pose)          ( struct _G3DOBJECT *, G3DKEY * );
    /* Object copy */
    struct _G3DOBJECT*(*copy) ( struct _G3DOBJECT *, uint32_t,
                                                     const char *,
                                                     uint64_t );
    /*** On Matrix change ***/
    void  (*transform)        ( struct _G3DOBJECT *, uint64_t );
    void  (*childvertexchange)( struct _G3DOBJECT *, struct _G3DOBJECT *, 
                                                     G3DVERTEX * ) ;
    void  (*activate)         ( struct _G3DOBJECT *, uint64_t );
    void  (*deactivate)       ( struct _G3DOBJECT *, uint64_t );
    struct _G3DOBJECT *(*commit) ( struct _G3DOBJECT *, uint32_t,
                                                        const char *,
                                                        uint64_t );
    void  (*addChild)         ( struct _G3DOBJECT *obj, 
                                struct _G3DOBJECT *child,
                                         uint64_t  engine_flags );
    void  (*removeChild)      ( struct _G3DOBJECT *obj, 
                                struct _G3DOBJECT *child,
                                         uint64_t  engine_flags );
    void  (*setParent)        ( struct _G3DOBJECT *child, 
                                struct _G3DOBJECT *parent,
                                struct _G3DOBJECT *oldParent, 
                                         uint64_t  engine_flags );
    struct _G3DOBJECT *parent; /*** Parent Object ***/
    LIST *lchildren;        /*** List of children ***/
    G3DBBOX bbox;
    G3DQUATERNION rotation;
    LIST *lkey; /*** keyframe list ***/
    uint32_t nbkey;
    LIST *lselkey;
    G3DCURVE *curve[0x03]; /* transformation dynamics */
    LIST     *lext; /* list of object extensions */
    LIST     *ltag;
    G3DTAG   *seltag;
    uint32_t  tagID;
} G3DOBJECT;

/******************************************************************************/
typedef struct _G3DREFERRED {
    G3DOBJECT *obj;
    uint32_t   available;
} G3DREFERRED;

/******************************************************************************/
typedef struct _G3DUVPLANE {
    float xradius;
    float yradius;
}  G3DUVPLANE;

/******************************************************************************/
typedef struct _G3DUVMAP {
    G3DOBJECT obj;   /*** Symmetry inherits G3DOBJECT ***/
    G3DUVPLANE pln;      /*** for flat projection ***/
    uint32_t projection;
    uint32_t policy;
    uint32_t fixed;
    /*LIST    *lmat;*/  /*** list of attached materials ***/
    /*uint32_t nbmat;*/ /*** Number of attached materials ***/
    uint32_t mapID;
    LIST    *lseluv; /*** list of selected UVs ***/
    LIST    *lseluvset; /*** list of selected UVSets ***/
} G3DUVMAP;

/******************************************************************************/
typedef struct _G3DPIVOT {
    G3DOBJECT obj;
    G3DCAMERA *cam;
} G3DPIVOT;

typedef struct _G3DFACE  G3DFACE;
typedef struct _G3DUVSET G3DUVSET;
typedef struct _G3DUVSET G3DSUBUVSET;
typedef struct _G3DMODIFIER G3DMODIFIER;

/******************************************************************************/
typedef struct _G3DUV {
    G3DUVSET *set;
    float     u;
    float     v;
    uint32_t   flags;
} G3DUV, G3DSUBUV;

/******************************************************************************/
typedef struct _G3DARBTEXCOORD {
    float u[0x04];
    float v[0x04];
    float q[0x04];
    /*G3DUVSET *uvs;*/ /*** Texture coords ***/
    GLint     tid; /*** OpenGL ARB texture ID ***/
} G3DARBTEXCOORD;

/******************************************************************************/
struct _G3DUVSET {
    uint32_t   flags; /*** this wil lbe used for facegroup selection ***/
    G3DFACE  *fac; /* the face it belongs to. Useful for undo-redo */
    G3DUVMAP *map;
    G3DUV     veruv[0x04];
    G3DUV     miduv[0x04];
    G3DUV     cenuv;       /*** face center UV ***/
    uint32_t  nbuv;
};

/******************************************************************************/
typedef struct _G3DTEXTURE {
    uint32_t       flags;
    uint32_t      slotBit; /*** maximum number of textures per mesh = 32 ***/
    G3DMATERIAL  *mat;
    G3DOBJECT    *obj;
    G3DUVMAP     *map;
    LIST         *lfacgrp;
    uint32_t      nbfacgrp;
    /*G3DRTUVSET   *rtuvsmem; *//*** UVSet buffer - for  non-power-of-2 texture ***/
                            /*** only for diffuse channel ***/
} G3DTEXTURE;

/******************************************************************************/
#define FACEGROUPSELECTED ( 1 << 0x00 )

typedef struct G3DFACEGROUP {
    uint32_t  flags;
    uint32_t id;
    char     *name;
    uint32_t textureSlots;
} G3DFACEGROUP;

/******************************************************************************/
typedef struct _G3DSYMMETRY {
    G3DOBJECT obj;   /*** Symmetry inherits G3DOBJECT ***/
    double smatrix[0x10];
    uint32_t orientation;
    uint32_t merge;
    float mergelimit;
} G3DSYMMETRY;

/******************************************************************************/
typedef struct _G3DCURSOR {
    G3DVECTOR pivot;
    G3DVECTOR axis[0x03];
    double    matrix[0x10];
    float     ratio;
} G3DCURSOR;

/******************************************************************************/
typedef struct _G3DRTUV {
    float u;
    float v;
} G3DRTUV;

/******************************************************************************/
typedef struct _G3DRTUVSET {
    G3DRTUV uv[0x04];
} G3DRTUVSET;

/******************************************************************************/
/*** Interleaved vertex arrays for buffered subdivided mesh. ***/
/*** GL_C4F_N3F_V3F ***/
typedef struct _G3DRTEDGE {
    uint32_t rtver[0x02];
} G3DRTEDGE;

/******************************************************************************/
/*** Interleaved vertex arrays for buffered subdivided mesh. ***/
/*** GL_C4F_N3F_V3F ***/
typedef struct _G3DRTQUAD {
    uint32_t rtver[0x04];
} G3DRTQUAD;

/******************************************************************************/
typedef struct _G3DRTTRIANGLE {
    uint32_t rtver[0x03];
} G3DRTTRIANGLE;

/******************************************************************************/
/***************** This is to use with the raytracer actually *****************/
typedef struct _G3DRTTRIANGLEUVW {
    float u[0x03];
    float v[0x03];
    G3DUVMAP *map; /*** this allows us to retrieve the texture/material ***/
} G3DRTTRIANGLEUVW;

/******************************************************************************/
/*typedef struct _G3DRTTRIANGLE {
    G3DTINYVECTOR verpos[0x03];
    G3DTINYVECTOR vernor[0x03];
    G3DTINYVECTOR tripos;
    G3DTINYVECTOR trinor;*/  /*** original normal vector ***/ /*
    float         surface;
    LIST         *luvw;
} G3DRTTRIANGLE; */

/******************************************************************************/
typedef struct _G3DEDGE {
    uint32_t   id;
    uint32_t    flags;
    uint32_t   nbfac;                /*** number of connected faces           ***/
    LIST      *lfac;                 /*** Face list                           ***/
    G3DVERTEX *ver[0x02];    /*** Our edge is made of 2 vertices      ***/
} G3DEDGE, G3DEXTRUDEVERTEX;

/******************************************************************************/
typedef struct _G3DSPLITEDGE {
    G3DEDGE *ref;
    G3DEDGE *sub;
} G3DSPLITEDGE;

/******************************************************************************/
typedef struct _G3DCUTEDGE {
    G3DEDGE   *edg;
    G3DVERTEX *ver;
    float      ratio;
} G3DCUTEDGE;

/******************************************************************************/
/*typedef struct _G3DSCULTMAP {
    G3DVECTOR *points;
    uint32_t maxpoints;
} G3DSCULPTMAP;*/

/******************************************************************************/
typedef struct _G3DFACE {
    uint32_t         id;           /*** face ID                             ***/
    uint32_t         typeID;
    uint32_t          flags;        /*** selected or not                     ***/
    uint32_t         nbver;        /*** number of vertices and edges        ***/
    G3DVERTEX       *ver[0x04];    /*** vertices array                      ***/
    G3DEDGE         *edg[0x04];    /*** edges array                         ***/
    G3DVECTOR        nor;          /*** Face normal vector                  ***/
    G3DVECTOR        pos;          /*** Face position (average position)    ***/
    LIST            *luvs;         /*** List of UVSets                      ***/
    uint32_t         nbuvs;        /*** Number of UVSets                    ***/
    float            surface;/*** used by the raytracer               ***/
    LIST            *lfacgrp; /*** list of facegroups it belong s to ***/
    LIST            *lext;    /*** list of face extensions                  ***/
    G3DRTVERTEX     *rtvermem;
} G3DFACE;

#include <g3dengine/g3dcurve.h>

/******************************************************************************/
typedef struct _G3DSUBEDGE {
    G3DEDGE     edg;
    G3DVERTEX  *subver;       /*** Edge center for mesh subdivision    ***/
    G3DVECTOR   pos;          /*** Precomputed subdivided position     ***/
    G3DVECTOR   nor;          /*** Precomputed subdivided normal vector***/
    struct _G3DSUBEDGE *subedg[0x02]; /*** for mesh subdivision                ***/
    LIST        lfacbuf[0x02]; /*** static list buffer ***/
    LIST        luvsbuf[0x04]; /*** static list buffer ***/
    G3DEDGE    *ancestorEdge;
    G3DFACE    *ancestorFace;
    uint32_t    commitID;
} G3DINNEREDGE, G3DSUBEDGE;

/******************************************************************************/
#define SUBFACEUVSETBUFFER 0x04 /*** max 4 uvwmaps    ***/
#define SUBFACETEXBUFFER   0x04 /*** max 4 textures   ***/
typedef struct _G3DSUBFACE {
    G3DFACE    fac;
    G3DVERTEX *subver;       /*** Face center when subdividing        ***/
    G3DFACE   *subfac[0x04]; /*** for mesh subdivision                ***/
    G3DEDGE   *innedg[0x04]; /*** for mesh subdivision                ***/
    LIST       luvsbuf[SUBFACEUVSETBUFFER];
    LIST       ltexbuf[SUBFACETEXBUFFER];
    G3DFACE   *ancestorFace;
} G3DSUBFACE;

/******************************************************************************/
#define SUBVERTEXMAXUV   0x08 /*** max 8 UVs   ***/
#define SUBVERTEXMAXEDGE 0x04 /*** max 4 edges ***/
#define SUBVERTEXMAXFACE 0x04 /*** max 4 faces ***/
typedef struct _G3DINNERVERTEX {
    G3DVERTEX  ver;
    LIST       ledgbuf[SUBVERTEXMAXEDGE];
    LIST       lfacbuf[SUBVERTEXMAXFACE];
    LIST       luvbuf[SUBVERTEXMAXUV];
    G3DVERTEX *ancestorVertex;
    G3DEDGE   *ancestorEdge;
    G3DFACE   *ancestorFace;
    uint32_t   commitID;
} G3DINNERVERTEX, G3DSUBVERTEX;

/******************************************************************************/
typedef struct _G3DCUTFACE {
    G3DFACE *fac;
    G3DCUTEDGE *ced[0x04];
} G3DCUTFACE;

/******************************************************************************/
#include <g3dengine/g3dlight.h>
#include <g3dengine/g3dtag.h>
#include <g3dengine/g3dinstance.h>
#include <g3dengine/g3dparticleemitter.h>

/******************************************************************************/
typedef struct _G3DCATMULLSCHEME {
    G3DSUBVERTEX vercat[0x04];
    /*G3DFACE *fac;*/
    G3DVECTOR mavg[0x04];
    G3DVECTOR favg[0x04];
    uint32_t nbver;
} G3DCATMULLSCHEME;

/******************************************************************************/
typedef struct _G3DSPOT {
    G3DLIGHT lig;   /*** Spot inherits G3DLIGHT ***/
    float aperture;
} G3DSPOT;

/******************************************************************************/
typedef struct _G3DSUBINDEX {
    uint32_t (*qua)[0x04];
    uint32_t (*tri)[0x04];
} G3DSUBINDEX;

/******************************************************************************/

#include <g3dengine/g3dlookup.h>
#include <g3dengine/g3dsubdivisionV3.h>
#include <g3dengine/g3dsysinfo.h>

/******************************************************************************/
typedef struct _G3DWEIGHTGROUP {
    uint32_t id;    /*** should never be trusted ***/
    uint32_t flags;
    char    *name;
    G3DMESH *mes ; /*** eases file writing ***/
} G3DWEIGHTGROUP;

/******************************************************************************/
typedef struct _G3DWEIGHT {
    float           weight;    /*** weight               - set when painted ***/
    G3DRIG         *rig;
    G3DWEIGHTGROUP *grp;
} G3DWEIGHT;

/******************************************************************************/
typedef enum  {
    G3DMODIFYOP_MODIFY,
    G3DMODIFYOP_STARTUPDATE,
    G3DMODIFYOP_UPDATE,
    G3DMODIFYOP_UPDATESELF,
    G3DMODIFYOP_ENDUPDATE
} G3DMODIFYOP;

#define DUMP_CALLBACK(f) ((uint32_t(*)(G3DMESH *, void (*)( uint32_t,   \
                                                            uint32_t,   \
                                                            uint32_t,   \
                                                            uint32_t,   \
                                                            void * ),   \
                                                  void (*) ( G3DFACE *, \
                                                             G3DVECTOR *, \
                                                             G3DVECTOR *, \
                                                             void * ),  \
                                                  void *,               \
                                                  uint64_t))f)

struct _G3DMESH {
    G3DOBJECT obj; /*** Mesh inherits G3DOBJECT ***/
    LIST *lver;    /*** List of vertices        ***/
    LIST *ledg;    /*** List of vertices        ***/
    LIST *lfac;    /*** List of faces           ***/
    LIST *lqua;    /*** List of Quads           ***/
    LIST *ltri;    /*** List of Triangles       ***/
    LIST *lfacgrp; /*** List of face groups     ***/
    LIST *lweigrp;
    LIST *ltex;    /*** list of textures ***/
    LIST *luvmap;

    LIST *lselver;
    LIST *lseledg;
    LIST *lselfac;
    LIST *lselweigrp;
    LIST *lselfacgrp;
    LIST *lseltex;
    /*LIST *lseluv;*/
    LIST *lseluvmap;
    
    G3DMODIFIER *lastmod;

    LIST *lupdfac;
    LIST *lupdedg;
    LIST *lupdver;

    uint32_t verid;
    uint32_t edgid;
    uint32_t facid;
    uint32_t subalg; /*** subdivision algo   ***/
    uint32_t nbver;
    uint32_t nbedg;
    uint32_t nbfac;
    uint32_t nbtri;
    uint32_t nbqua;
    uint32_t nbweigrp;
    uint32_t nbfacgrp;
    uint32_t nbhtm; /*** Number of heightmaps ***/
    uint32_t nbselver;
    uint32_t nbseledg;
    uint32_t nbselfac;
    uint32_t nbselweigrp;
    uint32_t nbseltex;
    uint32_t nbseluvmap;
    uint32_t nbtex;
    uint32_t nbuvmap;
    uint32_t textureSlots; /*** available texture slots ***/
    float    gouraudScalarLimit;
    G3DTEXTURE *curtex;
    G3DWEIGHTGROUP *curgrp;
    G3DFACE **faceindex; /*** Face index array in sculpt mode ***/
    /*** callbacks ***/
    void (*onGeometryMove) ( struct _G3DMESH *, LIST *,
                                                LIST *,
                                                LIST *,
                                                G3DMODIFYOP,
                                                uint64_t );
    uint32_t (*dump) ( struct _G3DMESH *, void (*Alloc)( uint32_t, /* nbver */
                                                         uint32_t, /* nbtris */
                                                         uint32_t, /* nbquads */
                                                         uint32_t, /* nbuv */
                                                         void * ),
                                          void (*Dump) ( G3DFACE *,
                                                         G3DVECTOR *,
                                                         G3DVECTOR *,
                                                         void * ),
                                          void *data,
                                          uint64_t );
};

#include <g3dengine/g3dsubdivisionthread.h>
#include <g3dengine/g3dspline.h>
#include <g3dengine/g3dtext.h>

/******************************************************************************/
typedef struct _G3DVERTEXEXTENSION {
    uint32_t name;
} G3DVERTEXEXTENSION;

void g3dvertexextension_init ( G3DVERTEXEXTENSION *ext,
                               uint32_t            name );

/******************************************************************************/
typedef struct _G3DFACEEXTENSION {
    uint32_t name;
} G3DFACEEXTENSION;

typedef struct _G3DHEIGHT {
    float s;
    float w;
} G3DHEIGHT;

typedef struct _G3DFACESCULPTEXTENSION {
    G3DFACEEXTENSION ext;
    G3DVECTOR       *pos;
    G3DHEIGHT       *hei;
    uint32_t        *flags;
    uint32_t         nbver;
    uint32_t         level;
} G3DFACESCULPTEXTENSION;

void g3dfaceextension_init ( G3DFACEEXTENSION *ext,
                             uint32_t          name );

void g3dface_addExtension ( G3DFACE          *fac,
                            G3DFACEEXTENSION *ext );

G3DFACEEXTENSION *g3dface_getExtension ( G3DFACE *fac,
                                         uint32_t name );

void g3dface_removeExtension ( G3DFACE          *fac, 
                               G3DFACEEXTENSION *ext );

G3DFACESCULPTEXTENSION *g3dfacesculptextension_new ( uint32_t extensionName,
                                                     G3DFACE *fac,
                                                     uint32_t level,
                                                     uint32_t sculptMode );

void g3dfacesculptextension_copy ( G3DFACESCULPTEXTENSION *src,
                                   G3DFACE                *srcfac,
                                   G3DFACESCULPTEXTENSION *dst,
                                   G3DFACE                *dstfac,
                                   uint32_t               *mapping,
                                   G3DVECTOR              *factor,
                                   uint32_t                sculptMode );

void g3dfacesculptextension_free ( G3DFACESCULPTEXTENSION *fse );

void g3dfacesculptextension_clearFlags ( G3DFACESCULPTEXTENSION *fse );

void g3dfacesculptextension_adjust ( G3DFACESCULPTEXTENSION *fse, 
                                     G3DFACE                *fac,
                                     uint32_t                level,
                                     uint32_t                sculptMode );

/******************************************************************************/
struct _G3DKEY {
    uint32_t id;    /*** should NEVER be trusted ***/
    uint32_t  flags;
    float frame;
    G3DVECTOR pos;
    G3DVECTOR rot;
    G3DVECTOR sca;
    float loopFrame;
    G3DCURVEPOINT curvePoint[0x03]; /*** - 0:TRANS, 1:ROT, 1:SCA ***/
    void (*free) ( struct _G3DKEY * );
    union {
        int64_t  s64;
        uint64_t u64;
        void    *ptr;
    } data;
};

/******************************************************************************/


#define MODIFY_CALLBACK(f)       ((uint32_t(*) (G3DMODIFIER*, \
                                                G3DMODIFYOP,  \
                                                uint64_t))f)
#define MODDRAW_CALLBACK(f)      ((uint32_t(*) (G3DMODIFIER*, \
                                                G3DCAMERA*,  \
                                                uint64_t))f)
#define MODPICK_CALLBACK(f)      ((uint32_t(*) (G3DMODIFIER*, \
                                                G3DCAMERA*,  \
                                                uint64_t))f)

/******************************************************************************/
typedef struct _G3DMODIFIER {
    G3DMESH    mes;
    uint32_t (*modify)     ( struct _G3DMODIFIER *mod,
                                     G3DMODIFYOP  op,
                                     uint64_t     engine_flags );
    uint32_t (*moddraw)    ( struct _G3DMODIFIER *mod,
                                     G3DCAMERA   *curcam,
                                     uint64_t     engine_flags );
    uint32_t (*modpick)    ( struct _G3DMODIFIER *mod,
                                     G3DCAMERA   *curcam,
                                     uint64_t     engine_flags );
    G3DOBJECT *oriobj; /*** original mesh   ***/
    G3DVECTOR *stkpos; /*** stack positions ***/
    G3DVECTOR *stknor; /*** stack normals   ***/
    G3DVECTOR *verpos;
    G3DVECTOR *vernor;
} G3DMODIFIER;

/******************************************************************************/
typedef struct _G3DBONE {
    G3DOBJECT obj;    /*** Bone inherits G3DOBJECT        ***/
    G3DVECTOR fixpos; /*** Position vector value when the bone was fixed ***/
    G3DVECTOR fixrot; /*** Rotation vector value when the bone was fixed ***/
    G3DVECTOR fixsca; /*** Scale vector value when the bone was fixed ***/
    float len;        /*** Bone len                       ***/
    LIST *lrig;       /*** list of rigged weight groups   ***/
} G3DBONE;

/******************************************************************************/
typedef struct _G3DWIREFRAME {
    G3DMODIFIER   mod;  /*** Bone inherits G3DOBJECT        ***/
    float         thickness;
    float         aperture;
    G3DSUBVERTEX *modver; /* we use the sub structures because of their */
    G3DSUBEDGE   *modedg; /* ability to provide static topology */
    G3DSUBFACE   *modfac;
    uint32_t      nbmodver;
    uint32_t      nbmodedg;
    uint32_t      nbmodfac;
    LIST         *lupdver; /* lit of vertices to update on mesh update */
} G3DWIREFRAME;

#include <g3dengine/g3dsplinerevolver.h>
#include <g3dengine/g3dmorpher.h>
#include <g3dengine/g3dskin.h>

#define SCULPTMODE_HEIGHT 0x00
#define SCULPTMODE_SCULPT 0x01

/******************************************************************************/
typedef struct _G3DSUBDIVIDER {
    G3DMODIFIER  mod;
    G3DRTQUAD   *rtquamem;
    uint32_t     nbrtfac;
    G3DRTEDGE   *rtedgmem;
    uint32_t     nbrtedg;
    G3DRTVERTEX *rtvermem;
    uint32_t     nbrtver;
    G3DRTUV     *rtluim;
    uint32_t     nbrtuv;
    uint32_t     nbVerticesPerTriangle;
    uint32_t     nbVerticesPerQuad;
    uint32_t     nbEdgesPerTriangle;
    uint32_t     nbEdgesPerQuad;
    uint32_t     nbFacesPerTriangle;
    uint32_t     nbFacesPerQuad;
    uint32_t     subdiv_preview;
    uint32_t     subdiv_render;
    uint32_t     sculptResolution;
    uint32_t     sculptMode;
    LIST        *lsubfac;
    G3DFACE    **factab;
} G3DSUBDIVIDER;

/******************************************************************************/
typedef struct _G3DRIG {
    double   isknmatrix[0x10];
    double   defmatrix[0x10];
    G3DSKIN *skn;
    LIST    *lweightgroup;
} G3DRIG;

/******************************************************************************/
typedef struct _G3DFFD {
    G3DMODIFIER mod;
    G3DVERTEX  *pnt;
    uint32_t    nbx;
    uint32_t    nby;
    uint32_t    nbz;
    float       radx;
    float       rady;
    float       radz;
    G3DVECTOR   locmin;
    G3DVECTOR   locmax;
    G3DVECTOR   parmin;
    G3DVECTOR   parmax;
    G3DVECTOR  *pos;
    G3DVECTOR  *uvw;
} G3DFFD;

/******************************************************************************/
typedef struct _G3DPRIMITIVE {
    G3DMESH mes;      /*** Primitive inherits G3DMESH ***/
    void *data;       /*** Primitive private data ***/
    uint32_t datalen; /*** Size of private data. Eases the copy ***/
} G3DPRIMITIVE;

/******************************************************************************/
typedef struct _SPHEREDATASTRUCT {
    int slice, cap;
    float radius;
} SPHEREDATASTRUCT;

/******************************************************************************/
typedef struct _PLANEDATASTRUCT {
    float radu;
    float radv;
    uint32_t nbu;
    uint32_t nbv;
    uint32_t orientation;
} PLANEDATASTRUCT;

/******************************************************************************/
typedef struct _TORUSDATASTRUCT {
    uint32_t slice;
    uint32_t cap;
    float intrad;
    float extrad;
    uint32_t orientation;
} TORUSDATASTRUCT;

/******************************************************************************/
typedef struct _G3DCUBEEDGE {
    G3DVERTEX **ver;
    uint32_t nbver;
} G3DCUBEEDGE, G3DCUBELINE;

/******************************************************************************/
typedef struct _G3DCUBEFACE {
    G3DCUBELINE *lin;
    G3DFACE **fac;   /*** 2 dimensions dynamic G3DFACE array. ***/
    uint32_t nbfac;
} G3DCUBEFACE;

/******************************************************************************/
typedef struct _CUBEDATASTRUCT {
    uint32_t nbx;
    uint32_t nby;
    uint32_t nbz;
    float radius;
} CUBEDATASTRUCT;

/******************************************************************************/
typedef struct _CYLINDERDATASTRUCT {
    uint32_t slice, capx, capy;
    float radius;
    float length;
    uint32_t closed;
    uint32_t orientation;
} CYLINDERDATASTRUCT;

/******************************************************************************/
typedef struct _TUBEDATASTRUCT {
    uint32_t slice, capx, capy;
    float radius;
    float thickness;
    float length;
    uint32_t orientation;
} TUBEDATASTRUCT;

/******************************************************************************/
typedef struct _G3DSCENE {
    G3DOBJECT obj;    /*** Scene inherits G3DOBJECT    ***/
    LIST *lsel;       /*** Selected objects            ***/
    LIST *lmat;       /*** list of materials           ***/
    LIST *limg;       /*** list of images              ***/
    uint32_t nbmat;
    uint32_t childid; /*** Children object IDs counter ***/
    G3DCURSOR csr;
    LIST *lref;
    uint32_t fps;
} G3DSCENE;

/********************* a "portable" version of XRectangle *********************/
typedef struct _G2DRECTANGLE {
    short x, y;
    unsigned short width, height; 
} G2DRECTANGLE; 

/******************************************************************************/
typedef struct _G3DCAMERADOF {
    float nearBlur;
    float noBlur;
    float farBlur;
    uint32_t radius;
} G3DCAMERADOF;

/******************************************************************************/
struct _G3DCAMERA {
    G3DOBJECT obj;                   /*** Camera inherits G3DOBJECT ***/
    G3DOBJECT *target;               /*** Camera's target           ***/
    G3DVECTOR viewpos;
    G3DVECTOR viewrot;
    float focal, ratio, znear, zfar; /*** Camera's lense settings   ***/
    double pmatrix[0x10]; /*** 4x4 projection matrix ***/
    GLint  vmatrix[0x04]; /*** 1x4 viewport matrix    ***/
    G2DRECTANGLE canevas;
    uint32_t width;
    uint32_t height;
    G3DCAMERADOF dof;
    G3DVECTOR ortho; /*** used for orthographic projection, until I find a better idea ***/
};

/******************************************************************************/
LIST *processHits ( GLint, GLuint * );

/******************************************************************************/
/*** hash function ***/
/*uint16_t inline float_to_short ( float );

#define _3FLOAT_TO_HASH(f0,f1,f2) ( ( float_to_short ( f0 ) >> 2 ) ^ \
                                    ( float_to_short ( f1 ) >> 1 ) ^ \
                                    ( float_to_short ( f2 ) >> 0 ) );*/

/******************************************************************************/
G3DGLOBALS *g3dcore_getGlobals ( );
void g3dquaternion_slerp ( G3DQUATERNION *q0, 
                           G3DQUATERNION *q1, 
                           double          t,
                           G3DQUATERNION *qout );
void g3dquaternion_toEuler ( G3DQUATERNION   *qua,
                                      G3DDOUBLEVECTOR *rot );
void g3dquaternion_toEulerInDegrees ( G3DQUATERNION   *qua,
                                      G3DDOUBLEVECTOR *rot );
void g3dquaternion_multiply ( G3DQUATERNION *qua0,
                              G3DQUATERNION *qua1,
                              G3DQUATERNION *qout );
void g3dcore_eulerToQuaternion ( G3DDOUBLEVECTOR *angles, G3DQUATERNION *qout );
void g3dcore_eulerInDegreesToQuaternion ( G3DDOUBLEVECTOR *angles, 
                                          G3DQUATERNION *qout );
void       g3dcore_grid3D   ( uint64_t engine_flags );
void       g3dcore_gridXY   ( uint64_t engine_flags );
void       g3dcore_gridYZ   ( uint64_t engine_flags );
void       g3dcore_gridZX   ( uint64_t engine_flags );
float g3dcore_intersect ( G3DDOUBLEVECTOR *plane,
                             G3DVECTOR *p1,
                             G3DVECTOR *p2,
                             G3DVECTOR *pout );
void     g3dcore_multmatrix              ( double *, double *, double * );
uint32_t g3dcore_getNumberOfCPUs         ( );
void     g3dcore_invertMatrix            ( double *, double * );
void     g3dcore_printMatrix             ( double *, uint32_t, uint32_t );
void     g3dcore_transposeMatrix         ( double *, double * );
void     g3dcore_getMatrixScale          ( double *, G3DVECTOR * );
void     g3dcore_getMatrixRotation       ( double *, G3DVECTOR * );
void     g3dcore_getMatrixTranslation    ( double *, G3DVECTOR * );
void     g3dobject_buildRotationMatrix   ( G3DOBJECT * );
G3DOBJECT *g3dobject_getChildByType ( G3DOBJECT *obj, 
                                      uint64_t   type );
uint32_t g3dobject_seekByPtr_r ( G3DOBJECT *obj, 
                                 G3DOBJECT *ptr );
void     g3dcore_identityMatrix          ( double * );
void     g3dcore_multmatrixdirect        ( double *, double * );
void     g3dcore_symmetryMatrix          ( double *, uint32_t );
void     g3dcore_loadJpeg                ( const char *, uint32_t *,
                                                         uint32_t *,
                                                         uint32_t *,
                                                         unsigned char ** );
void     g3dcore_writeJpeg               ( const char *, uint32_t,
                                                         uint32_t,
                                                         uint32_t,
                                                         unsigned char * );
uint32_t g3dcore_getNextPowerOfTwo       ( uint32_t );
void g3dcore_drawCircle ( uint32_t orientation,
                          float    radius,
                          uint64_t engine_flags );
void     g3dcore_drawXYCircle ( float    radius, 
                                uint64_t engine_flags );
void     g3dcore_drawYZCircle ( float    radius, 
                                uint64_t engine_flags );
void     g3dcore_drawZXCircle ( float    radius, 
                                uint64_t engine_flags );
char    *g3dcore_strclone                ( char *   );
void     g3dcore_extractRotationMatrix   ( double *, double * );

/******************************************************************************/



double g3ddoublevector_length ( G3DDOUBLEVECTOR * );
double g3ddoublevector_scalar ( G3DDOUBLEVECTOR *, G3DDOUBLEVECTOR * );
void g3ddoublevector_cross ( G3DDOUBLEVECTOR *, G3DDOUBLEVECTOR *, 
                                                G3DDOUBLEVECTOR * );
void g3ddoublevector_normalize ( G3DDOUBLEVECTOR *vec, float *len );
void g3ddoublevector_print ( G3DDOUBLEVECTOR *vec );

/******************************************************************************/
void g3dquaternion_multiply ( G3DQUATERNION *, G3DQUATERNION *,
                              G3DQUATERNION * );
void g3dquaternion_inverse ( G3DQUATERNION *, G3DQUATERNION * );
void g3dquaternion_set ( G3DQUATERNION *, float , float, float );
void g3dquaternion_convert ( G3DQUATERNION *, double * );
void g3dquaternion_init ( G3DQUATERNION *, float, float, float, float );
double g3dquaternion_length ( G3DQUATERNION *qua );
void g3dquaternion_normalize ( G3DQUATERNION *qua );
double g3dquaternion_scalar ( G3DQUATERNION *q0, G3DQUATERNION *q1 );
void g3dquaternion_print ( G3DQUATERNION *qua );

/******************************************************************************/
G3DVERTEX *g3dvertex_new        ( float, float, float );
G3DWEIGHT *g3dvertex_getWeight ( G3DVERTEX      *ver, 
                                 G3DWEIGHTGROUP *grp );
G3DVECTOR *g3dvertex_getModifiedPosition ( G3DVERTEX *ver,
                                           G3DVECTOR *stkpos );
G3DVECTOR *g3dvertex_getModifiedNormal ( G3DVERTEX *ver,
                                         G3DVECTOR *stkpos );
G3DVERTEXEXTENSION *g3dvertex_getExtension ( G3DVERTEX *, uint32_t );
void       g3dvertex_addExtension ( G3DVERTEX *, G3DVERTEXEXTENSION * );
void       g3dvertex_removeExtension ( G3DVERTEX *, G3DVERTEXEXTENSION * );
void g3dvertex_normal ( G3DVERTEX *ver, 
                        uint64_t   engine_flags );
void g3dvertex_computeNormal ( G3DVERTEX *ver, 
                               G3DVECTOR *nor,
                               uint64_t   engine_flags );
void       g3dvertex_addFace    ( G3DVERTEX *, G3DFACE * );
void       g3dvertex_removeFace ( G3DVERTEX *, G3DFACE * );
void       g3dvertex_addEdge    ( G3DVERTEX *, G3DEDGE * );
void       g3dvertex_removeEdge ( G3DVERTEX *, G3DEDGE * );
void       g3dvertex_free       ( G3DVERTEX * );
void       g3dvertex_addUV      ( G3DVERTEX *, G3DUV * );
void       g3dvertex_removeUV   ( G3DVERTEX *, G3DUV * );
uint32_t   g3dvertex_isBoundary ( G3DVERTEX * );
int32_t    g3dvertex_getRankFromList ( LIST *lver, G3DVERTEX *ver );
void       g3dvertex_getNormalFromSelectedFaces ( G3DVERTEX *, G3DVECTOR * );
void       g3dvertex_getVectorFromSelectedFaces ( G3DVERTEX *, G3DVECTOR * );
void       g3dvertex_computeEdgePoint           ( G3DVERTEX *, G3DEDGE *,
                                                               G3DEDGE *,
                                                               G3DSUBVERTEX **,
                                                               G3DSUBEDGE   ** );
uint32_t   g3dvertex_getAverageMidPoint  ( G3DVERTEX *, G3DVECTOR * );
uint32_t   g3dvertex_getAverageModifiedMidPoint ( G3DVERTEX *ver, 
                                                  G3DVECTOR *verpos,
                                                  G3DVECTOR *midavg );
uint32_t   g3dvertex_getAverageFacePoint ( G3DVERTEX *, G3DVECTOR * );
uint32_t g3dvertex_getAverageModifiedFacePoint ( G3DVERTEX *ver, 
                                                 G3DVECTOR *verpos,
                                                 G3DVECTOR *facavg );
void       g3dvertex_createSubEdge ( G3DVERTEX *, G3DSUBVERTEX *, G3DFACE *,
                                     G3DSUBVERTEX *, G3DSUBEDGE **, G3DSUBVERTEX **,
                                     G3DEDGE *, G3DSUBVERTEX *,
                                     G3DEDGE *, G3DSUBVERTEX * );
void       g3dsubvertex_addEdge ( G3DSUBVERTEX *, G3DEDGE * );
void g3dsubvertex_renumberArray ( G3DSUBVERTEX *subver, 
                                  uint32_t nbver );
void g3dsubvertex_elevate ( G3DSUBVERTEX *subver,
                            uint64_t      sculptExtensionName,
                            uint32_t    (*tri_indexes)[0x04],
                            uint32_t    (*qua_indexes)[0x04],
                            uint32_t      sculptMode );
uint32_t   g3dvertex_setOuterEdges ( G3DVERTEX *, G3DSUBVERTEX  *,
                                                  G3DEDGE       *,
                                                  G3DEDGE       *,
                                                  G3DSUBVERTEX **,
                                                  G3DSUBEDGE   **,
                                                  uint32_t *,
                                                  uint32_t,
                                                  uint32_t,
                                                  uint32_t );

LIST *g3dvertex_getUnselectedFacesFromList ( LIST * );
void g3dvertex_getSubFaces ( G3DVERTEX *, G3DSUBVERTEX *, 
                                          G3DSUBVERTEX *, G3DFACE * );

void g3dsubvertex_addFace ( G3DSUBVERTEX *, G3DFACE * );
void g3dsubvertex_addUV   ( G3DSUBVERTEX *, G3DUV * );

void g3dvertex_calcSubFaces ( G3DVERTEX *, G3DFACE * );
void g3dvertex_setSubEdges ( G3DVERTEX *, G3DEDGE *,
                                          G3DEDGE *,
                                          G3DSUBVERTEX *,
                                          G3DSUBVERTEX *,
                                          G3DSUBVERTEX *,
                                          G3DSUBEDGE   * );
void g3dvertex_print ( G3DVERTEX * );
void g3dvertex_resetFacesInnerEdges ( G3DVERTEX * );
void g3dvertex_updateFacesPosition ( G3DVERTEX *ver );
uint32_t g3dvertex_copyPositionFromList ( LIST *, G3DVECTOR ** );
G3DVERTEX *g3dvertex_copy ( G3DVERTEX *ver, 
                            uint64_t   engine_flags );
uint32_t g3dvertex_isBorder ( G3DVERTEX * );
void g3dvertex_setPositionFromList ( LIST *, G3DVECTOR * );
LIST *g3dvertex_getFaceListFromVertices ( LIST * );
G3DVERTEX *g3dvertex_weldList ( LIST * );
void g3dvertex_setSelected ( G3DVERTEX * );
void g3dvertex_unsetSelected ( G3DVERTEX * );
void g3dvertex_addWeight    ( G3DVERTEX *, G3DWEIGHT * );
void g3dvertex_removeWeight ( G3DVERTEX *, G3DWEIGHT * );
void g3dvertex_computeSkinnedPosition ( G3DVERTEX * );
LIST *g3dvertex_getAreaFacesFromList ( LIST * );
LIST *g3dvertex_getFacesFromList ( LIST * );
LIST *g3dvertex_getEdgesFromList ( LIST * );
void  g3dvertex_updateFaces ( G3DVERTEX * );
G3DEXTRUDEVERTEX *g3dvertex_extrude ( G3DVERTEX * );
G3DUV *g3dvertex_getUV ( G3DVERTEX *, G3DUVMAP * );
void g3dvertex_displace ( G3DVERTEX *, LIST *, G3DVECTOR * );
uint32_t g3dvertex_belongsToSelectedFacesOnly ( G3DVERTEX *ver );
uint32_t g3dvertex_setSubFaces ( G3DVERTEX *, G3DFACE       *,
                                              G3DSUBVERTEX  *,
                                              G3DSUBVERTEX **,
                                              G3DSUBFACE   **,
                                              G3DSUBUVSET  **,
                                              uint32_t, 
                                              uint32_t,
                                              uint32_t );
void g3dvertex_getAverageUV ( G3DVERTEX *, G3DUVMAP *, float *, float * );
void g3ddoublevector_matrix ( G3DDOUBLEVECTOR *, double *, G3DDOUBLEVECTOR * );
uint32_t g3dvertex_isAdaptive ( G3DVERTEX * );
uint32_t g3dvertex_checkAdaptiveEdges ( G3DVERTEX * );
void g3dvertex_markAdaptiveEdges ( G3DVERTEX * );
void g3dvertex_markAdaptiveFaces ( G3DVERTEX *, G3DSUBVERTEX *, float );
void g3dvertex_clearAdaptiveEdges ( G3DVERTEX * );
void g3dvertex_clearAdaptiveFaces ( G3DVERTEX * );
void g3dvertex_markAdaptiveTopology ( G3DVERTEX * );
void g3dvertex_clearAdaptiveTopology ( G3DVERTEX * );
void g3dvertex_getAveragePositionFromList ( LIST *, G3DVECTOR * );
void g3drtvertex_init ( G3DRTVERTEX *rtver, 
                        G3DVERTEX   *ver,
                        uint32_t     facsel,
                        uint64_t     engine_flags );
void g3dvertex_renumberList ( LIST *, uint32_t );
void g3dvertex_edgePosition ( G3DVERTEX *, uint32_t );
G3DVERTEX *g3dvertex_seekVertexByPosition ( LIST *, float, float, float, float );


/******************************************************************************/
G3DSPLITVERTEX *g3dsplitvertex_seek ( LIST *, G3DVERTEX * );
void g3dsplitvertex_replace ( G3DSPLITVERTEX * );
void g3dsplitvertex_restore ( G3DSPLITVERTEX * );
G3DSPLITVERTEX *g3dsplitvertex_new ( G3DVERTEX * );
void g3dsplitvertex_free ( G3DSPLITVERTEX * );

/******************************************************************************/
G3DSPLITEDGE *g3dsplitedge_seek ( LIST *, G3DEDGE * );
G3DSPLITEDGE *g3dsplitedge_new ( G3DEDGE *, LIST * );
void g3dsplitedge_free ( G3DSPLITEDGE * );
G3DFACE *g3dsplitedge_createFace ( G3DSPLITEDGE * );

/******************************************************************************/
G3DEDGE *g3dedge_seek ( LIST *, G3DVERTEX *, G3DVERTEX * );
void g3dedge_mark ( G3DEDGE *edg );
void g3dedge_unmark ( G3DEDGE *edg );
uint32_t g3dedge_hasOnlyFullyMirroredFaces ( G3DEDGE * );
G3DEDGE *g3dedge_new ( G3DVERTEX *, G3DVERTEX * );
G3DEDGE *g3dedge_newUnique ( uint32_t id, G3DVERTEX *, G3DVERTEX *, LIST ** );
void g3dedge_removeFace ( G3DEDGE *, G3DFACE * );
uint32_t g3dedge_isBorder ( G3DEDGE * );
void g3dedge_addFace ( G3DEDGE *, G3DFACE * );
void g3dedge_position ( G3DEDGE *, uint32_t );
uint32_t g3dedge_createFaceInnerEdge ( G3DEDGE *, G3DFACE *,
                                                  G3DSUBVERTEX **,
                                                  G3DSUBEDGE   **,
                                                  uint32_t *,
                                                  uint32_t, uint32_t, uint32_t );
void g3dedge_draw ( G3DEDGE *edg,
                    uint64_t engine_flags );
void g3dedge_createBothSubEdge ( G3DEDGE *, G3DSUBEDGE **, G3DSUBVERTEX ** );
G3DSUBVERTEX *g3dedge_getAverageVertex ( G3DEDGE * );
void g3dedge_setSelected ( G3DEDGE * );
void g3dedge_unsetSelected ( G3DEDGE * );
LIST *g3dedge_getVerticesFromList ( LIST * );
void g3dedge_replaceVertex ( G3DEDGE *, G3DVERTEX *, G3DVERTEX * );
void g3dedge_free ( G3DEDGE * );
G3DFACE *g3dedge_createFace ( G3DEDGE *, LIST * );
G3DSUBEDGE *g3dedge_getSubEdge ( G3DEDGE *, G3DVERTEX *, G3DVERTEX *);
uint32_t g3dedge_hasSelectedFace ( G3DEDGE * );
LIST *g3dedge_getFacesFromList ( LIST * );  
void g3dsubedge_addFace   ( G3DSUBEDGE *, G3DFACE * );
void g3dsubedge_normal ( G3DSUBEDGE * );
void g3dsubedge_init ( G3DSUBEDGE *, G3DVERTEX *, G3DVERTEX * );
void g3dedge_getChildrenFaces ( G3DEDGE *, G3DVERTEX *, G3DEDGE *,  G3DEDGE * );
void g3dedge_addUVSet ( G3DEDGE *, G3DUVSET * );
void g3dedge_removeUVSet ( G3DEDGE *, G3DUVSET * );
void g3dedge_normal ( G3DEDGE * );
LIST *g3dedge_getUnselectedFacesFromList ( LIST * );
uint32_t g3dedge_isAdaptive ( G3DEDGE * );
void g3dedge_getAveragePosition ( G3DEDGE *, G3DVECTOR * );
void g3dedge_getAverageModifiedPosition ( G3DEDGE   *edg,
                                          G3DVECTOR *verpos,
                                          G3DVECTOR *vout );
void g3dedge_getAverageNormal ( G3DEDGE *, G3DVECTOR * );
void g3dedge_drawSimple ( G3DEDGE *edg,
                          uint32_t object_flags,
                          uint64_t engine_flags );
void g3dsubedge_position ( G3DSUBEDGE * );
G3DSUBEDGE *g3dsubedge_getSubEdge ( G3DSUBEDGE *, G3DVERTEX *, G3DVERTEX * );

void g3dedge_getSubdivisionNormal ( G3DEDGE   *edg,
                                    G3DVECTOR *stkverpos,
                                    G3DVECTOR *stkvernor,
                                    G3DVECTOR *nor );

void g3dedge_getSubdivisionPosition ( G3DEDGE   *edg,
                                      G3DVECTOR *stkverpos,
                                      G3DVECTOR *pos );

/******************************************************************************/
G3DCUTEDGE *g3dcutedge_new ( G3DEDGE *, G3DVERTEX * );
G3DCUTEDGE *g3dcutedge_seek ( LIST *, G3DEDGE * );
void g3dcutedge_free ( G3DCUTEDGE * );

/******************************************************************************/
void     g3dface_evalSubdivision        ( G3DFACE *, 
                                          uint32_t, 
                                          uint32_t *, 
                                          uint32_t *,
                                          uint32_t * );
void g3dface_getSharedEdgesFromList ( LIST *lfac, LIST **ledg );
void g3dface_unmark ( G3DFACE *fac );
void g3dface_mark ( G3DFACE *fac );
void     g3dface_init                   ( G3DFACE *, 
                                          G3DVERTEX **, 
                                          uint32_t );
void     g3dtriangle_evalSubdivision    ( uint32_t,
                                          uint32_t *, 
                                          uint32_t *,
                                          uint32_t * );
void     g3dquad_evalSubdivision        ( uint32_t, 
                                          uint32_t *, 
                                          uint32_t *,
                                          uint32_t * );
void     g3dface_updateModifiedPosition ( G3DFACE   *fac,
                                          G3DVECTOR *stkpos );

void     g3dface_computeModifiedPosition ( G3DFACE   *fac,
                                           G3DVECTOR *stkverpos,
                                           G3DVECTOR *facpos );
void     g3dface_computeModifiedNormal ( G3DFACE   *fac,
                                         G3DVECTOR *stkverpos,
                                         G3DVECTOR *facnor );
void     g3dface_updateModifiedNormal   ( G3DFACE   *fac,
                                          G3DVECTOR *stkpos );
void     g3dface_updateModified         ( G3DFACE   *fac,
                                          G3DVECTOR *stkverpos );

void     g3dface_attachEdges            ( G3DFACE *,
                                          G3DEDGE ** );
G3DFACE *g3dquad_new                    ( G3DVERTEX *, 
                                          G3DVERTEX *,
                                          G3DVERTEX *,
                                          G3DVERTEX * );
G3DFACE *g3dtriangle_new                ( G3DVERTEX *, 
                                          G3DVERTEX *, 
                                          G3DVERTEX * );
void     g3dface_normal                 ( G3DFACE * );
void g3dface_draw  ( G3DFACE   *fac, 
                     G3DVECTOR *verpos,
                     G3DVECTOR *vernor,
                     float      gouraudScalarLimit,
                     LIST      *ltex,
                     uint32_t   object_flags,
                     uint64_t   engine_flags );
void     g3dface_drawSkin               ( G3DFACE *fac, 
                                          uint32_t oflags,
                                          uint64_t engine_flags );
void     g3dface_getModifiedPosition    ( G3DFACE   *fac,
                                          G3DVECTOR *stkpos,
                                          G3DVECTOR *facpos );
uint32_t g3dface_intersect              ( G3DFACE *, 
                                          G3DVECTOR *, 
                                          G3DVECTOR *,
                                          G3DVECTOR * );
void     g3dface_free                   ( G3DFACE * );
void     g3dface_update                 ( G3DFACE * );
G3DEDGE *g3dface_compare                ( G3DFACE *, 
                                          G3DFACE * );
G3DFACE *g3dface_merge                  ( G3DFACE *, 
                                          G3DFACE *, 
                                          G3DEDGE * );
void     g3dface_unsetSelected          ( G3DFACE * );
void     g3dface_setSelected            ( G3DFACE * );
G3DFACE *g3dface_new                    ( G3DVERTEX **, uint32_t );
void     g3dface_delete                 ( G3DFACE * );
void     g3dface_invertNormal           ( G3DFACE * );
uint32_t g3dface_catmull_clark_draw     ( G3DSUBDIVISIONTHREAD *,
                                          G3DFACE *, 
                                          G3DFACE *, 
                                          uint32_t,
                                          float,
                         /*** get triangles ***/ G3DRTTRIANGLE **,
                         /*** get quads     ***/ G3DRTQUAD     **,
                                                 G3DRTUVSET    **,
                                                 G3DVECTOR *, uint32_t *,
                                                 LIST          *,
                                                 uint32_t,
                                                 uint32_t );
uint32_t g3dface_setInnerEdges    ( G3DFACE *, G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBEDGE   **, uint32_t, uint32_t );
uint32_t g3dface_setOuterEdges    ( G3DFACE *, G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBEDGE   **, uint32_t, uint32_t );
void     g3dface_position         ( G3DFACE * );
G3DFACE *g3dface_getAdjacentFace  ( G3DFACE *, uint32_t );
void     g3dface_getAdjacentFaceSubQuads ( G3DFACE *fac, G3DVERTEX *,
                                                         G3DVERTEX *,
                                                         G3DFACE   *,
                                                         G3DFACE   * );
void     g3dface_computeEdgePoints( G3DFACE * );
void     g3dface_addEdgeByIndex   ( G3DFACE *, G3DEDGE *, uint32_t );
uint32_t g3dface_setSubFace       ( G3DFACE *, G3DVERTEX     *,
                                               G3DFACE       *,
                                               G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBFACE   **,
                                               G3DSUBUVSET  **, 
                                               uint32_t,
                                               uint32_t,
                                               uint32_t );
void g3dface_addFacegroup ( G3DFACE      *fac,
                            G3DFACEGROUP *facgrp );
void g3dface_removeFacegroup ( G3DFACE      *fac,
                               G3DFACEGROUP *facgrp );
void     g3dface_getSubFace       ( G3DFACE *, G3DVERTEX *, G3DSUBVERTEX *,
                                                            G3DSUBVERTEX * );
void     g3dface_assignSubVertex  ( G3DFACE *, G3DEDGE * );
void     g3dface_calcSubFace      ( G3DFACE *, G3DFACE *, int );
void     g3dface_replaceVertex    ( G3DFACE *, G3DVERTEX *, G3DVERTEX * );
void     g3dface_getSubFacesFromEdge ( G3DFACE *, G3DEDGE *, G3DFACE **,
                                                             G3DFACE ** );
G3DFACE *g3dface_retSubFace       ( G3DFACE *, G3DVERTEX * );
void g3dface_drawEdges ( G3DFACE *fac, 
                         uint64_t engine_flags );
void g3dface_drawCenter ( G3DFACE *fac, 
                          uint64_t engine_flags );
G3DFACE *g3dface_weld             ( G3DFACE *, LIST *, G3DVERTEX * );
int      g3dface_applyCatmullScheme ( G3DFACE *, G3DSUBVERTEX *, uint32_t, uint32_t );
void     g3dface_recurseAlign     ( G3DFACE * );
uint32_t g3dface_isAligned        ( G3DFACE *, uint32_t, G3DFACE * );
void     g3dface_alignUnselected  ( G3DFACE * );
void     g3dface_linkVertices     ( G3DFACE * );
void     g3dface_calcSubFaces     ( G3DFACE *, G3DSUBVERTEX *, 
                                               G3DSUBVERTEX *[0x04],
                                               uint32_t,
                                               uint32_t, 
                                               uint32_t );
void     g3dface_setSubEdges      ( G3DFACE *, G3DSUBVERTEX *,
                                               G3DSUBVERTEX *,
                                               G3DSUBVERTEX *,
                                               G3DSUBEDGE   * );
void     g3dface_initWithEdges    ( G3DFACE *, G3DVERTEX **, 
                                               G3DEDGE   **,
                                               uint32_t );
uint32_t g3dface_setSubFaces      ( G3DFACE *, G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBFACE   **,
                                               G3DSUBUVSET  **,
                                               uint32_t,
                                               uint32_t,
                                               uint32_t );
uint32_t g3dface_initsubmem       ( G3DFACE *, G3DSUBVERTEX **,
                                               G3DSUBEDGE   **,
                                               G3DSUBFACE   **,
                                               G3DSUBUVSET  ** );
uint32_t g3dface_hasTextureSlot ( G3DFACE *fac, 
                                  uint32_t texSlotBit  );
LIST    *g3dface_getEdgesFromList ( LIST * );
void g3dface_drawTriangleList ( LIST    *ltri,
                                float    gouraudScalarLimit, 
                                LIST    *ltex, 
                                uint32_t object_flags,
                                uint64_t engine_flags );
void g3dface_drawQuadList ( LIST    *lqua,
                            float    gouraudScalarLimit, 
                            LIST    *ltex, 
                            uint32_t object_flags,
                            uint64_t engine_flags );
void g3dface_drawTriangle  ( G3DFACE *fac,
                             float    gouraudScalarLimit,
                             LIST    *ltex, 
                             uint32_t object_flags,
                             uint64_t engine_flags );
void g3dface_drawQuad ( G3DFACE *fac,
                        float    gouraudScalarLimit,
                        LIST    *ltex, 
                        uint32_t object_flags,
                        uint64_t engine_flags );
void     g3dface_updateBufferedSubdivision ( G3DFACE *, G3DSUBDIVISION *,
                                                        uint32_t, 
                                                        float, 
                                                        uint32_t,
                                                        uint32_t );
LIST    *g3dface_getExtendedFacesFromList ( LIST * );
LIST    *g3dface_getVerticesFromList ( LIST * );
void     g3dface_triangulate      ( G3DFACE *, G3DFACE *[0x02], int  );
void     g3dface_unsetHidden      ( G3DFACE * );
void     g3dface_setHidden        ( G3DFACE * );
void     g3dface_getOrphanedEdgesFromList ( LIST *lfac, LIST **ledg );
uint32_t g3dface_convert ( G3DFACE *fac, 
                           G3DFACE        *ancestor,
                           G3DRTTRIANGLE **rttriptr,
                           G3DRTQUAD     **rtquaptr,
                           G3DRTUVSET    **rtuvsptr,
                           uint32_t        object_flags,
                           uint64_t        engine_flags );
G3DUVSET *g3dface_getUVSet        ( G3DFACE *, G3DUVMAP * );
void     g3dface_addUVSet         ( G3DFACE *, G3DUVSET * );
void     g3dface_removeUVSet      ( G3DFACE *, G3DUVSET * );
void     g3dface_allocSubdividedUVSets ( G3DFACE *, uint32_t );
void     g3dface_displace         ( G3DFACE *fac, G3DSUBVERTEX *,
                                                  G3DVERTEX    *,
                                                  float *, float *,
                                                  float *, float *,
                                                  float  , float  ,
                                                  G3DUVMAP  *,
                                                  LIST * );
void     g3dface_setSubUVSet ( G3DFACE *, G3DSUBVERTEX *, G3DVERTEX *, G3DSUBUVSET *, uint32_t );
void    *g3dface_catmull_clark_draw_t ( G3DSUBDIVISIONTHREAD * );
void     g3dface_resetInnerEdges ( G3DFACE * );
void     g3dface_resetOuterEdges ( G3DFACE * );
uint32_t g3dface_setInnerVertex ( G3DFACE *, G3DSUBVERTEX **,
                                             uint32_t, 
                                             uint32_t,
                                             uint32_t );
uint32_t g3dface_bindMaterials ( G3DFACE        *fac, 
                                 LIST           *ltex, 
                                 G3DARBTEXCOORD *texcoord,
                                 uint32_t        object_flags,
                                 uint64_t        engine_flags );
void g3dface_unbindMaterials ( G3DFACE *fac, 
                               LIST    *ltex,
                               uint32_t object_flags,
                               uint64_t engine_flags );
void     g3dface_removeAllUVSets ( G3DFACE * );
uint32_t g3dface_countUVSetsFromList ( LIST *, uint32_t );
void     g3dface_importUVSets ( G3DFACE *, G3DFACE * );
void     g3dface_resetAll ( G3DFACE * );
uint32_t g3dface_isAdaptive ( G3DFACE *, float );
void     g3dface_markAdaptive ( G3DFACE *, G3DSUBVERTEX *, float );
uint32_t g3dface_setAllEdges ( G3DFACE *, G3DSUBVERTEX  *,
                                          G3DSUBVERTEX **,
                                          G3DSUBEDGE   **, 
                                          uint32_t *,
                                          uint32_t       ,
                                          uint32_t       ,
                                          uint32_t        );
G3DVERTEX *g3dface_getVertexByID ( G3DFACE *, uint32_t );
G3DEDGE   *g3dface_getEdgeByID   ( G3DFACE *, uint32_t );
void g3dface_drawSimple  ( G3DFACE *fac, 
                           uint32_t object_flags,
                           uint64_t engine_flags );
uint32_t g3dface_checkOrientation ( G3DFACE * );
void g3dface_initSubface ( G3DFACE *fac, 
                           G3DSUBFACE   *subfac,
                           G3DVERTEX    *oriver,
                           G3DVERTEX    *orivercpy,
                           G3DSUBUVSET  *subuvs,
                           uint32_t    (*qua_indexes)[0x04],
                           uint32_t    (*tri_indexes)[0x04],
                           uint32_t      iteration,
                           uint32_t      curdiv,
                           uint32_t      object_flags,
                           uint32_t      subdiv_flags,
                           uint64_t      engine_flags );
void g3dface_subdivideUVSets ( G3DFACE * );
uint32_t g3dface_isFullyMirrored ( G3DFACE * );
G3DFACE *g3dface_newWithEdges ( G3DVERTEX **, G3DEDGE **, uint32_t );
void g3dface_getAveragePositionFromList ( LIST *lver, G3DVECTOR *pos );

/******************************************************************************/
void g3dsubface_addUVSet   ( G3DSUBFACE *, G3DUVSET *, uint32_t );
void g3dsubface_isAdaptive ( G3DSUBFACE *  );
void g3dsubface_topology   ( G3DSUBFACE *, uint32_t  );
void g3dsubface_position   ( G3DSUBFACE * );
void g3dsubface_importUVSets ( G3DSUBFACE *, G3DFACE   *,
                                             uint32_t  ,
                                             G3DUVSET *,
                                             uint32_t  );

/******************************************************************************/
G3DCUTFACE *g3dcutface_new ( G3DFACE *, G3DCUTEDGE *[0x04] );
uint32_t g3dcutface_divide         ( G3DCUTFACE *, G3DFACE ** );
uint32_t g3dcutface_divideTriangle ( G3DCUTFACE *, G3DFACE ** );
uint32_t g3dcutface_divideQuad     ( G3DCUTFACE *, G3DFACE ** );
uint32_t g3dcutface_getCutEdgeIndexes ( G3DCUTFACE *, uint32_t *, uint32_t );
void     g3dcutface_free ( G3DCUTFACE * );

/******************************************************************************/
G3DKEY  *g3dkey_new                       ( float, G3DVECTOR *, 
                                                   G3DVECTOR *, 
                                                   G3DVECTOR *, 
                                                   uint32_t );
void     g3dkey_init                      ( G3DKEY *, float, 
                                                      G3DVECTOR *, 
                                                      G3DVECTOR *,
                                                      G3DVECTOR *,
                                                      uint32_t );
void     g3dkey_getTransformationFromList ( LIST *, G3DVECTOR *,
                                                    G3DVECTOR *,
                                                    G3DVECTOR * );
void     g3dkey_setSelected               ( G3DKEY * );
void     g3dkey_unsetSelected             ( G3DKEY * );
void     g3dkey_setLoopFrame              ( G3DKEY *, float );
void     g3dkey_setLoop                   ( G3DKEY * );
void     g3dkey_unsetLoop                 ( G3DKEY * );
uint32_t g3dkey_getLoopFrameFromList      ( LIST *, float * );
void     g3dkey_setLoopFrameFromList      ( LIST *, float   );
void     g3dkey_unsetLoopFrameFromList    ( LIST * );
void     g3dkey_setLoopFromList           ( LIST * );
void     g3dkey_unsetLoopFromList         ( LIST * );
void     g3dkey_free                      ( G3DKEY * );
uint32_t g3dkey_getFlagFromList           ( LIST *, uint32_t );
uint32_t g3dkey_getUsePositionFromList    ( LIST * );
uint32_t g3dkey_getUseRotationFromList    ( LIST * );
uint32_t g3dkey_getUseScalingFromList     ( LIST * );
uint32_t g3dkey_getUseDataFromList        ( LIST * );
void     g3dkey_setFlagFromList           ( LIST *, uint32_t );
void     g3dkey_unsetFlagFromList         ( LIST *, uint32_t );
void     g3dkey_setUsePositionFromList    ( LIST * );
void     g3dkey_setUseRotationFromList    ( LIST * );
void     g3dkey_setUseScalingFromList     ( LIST * );
void     g3dkey_enableTranslation         ( G3DKEY *key );
void     g3dkey_disableTranslation        ( G3DKEY *key );
void     g3dkey_enableRotation            ( G3DKEY *key );
void     g3dkey_disableRotation           ( G3DKEY *key );
void     g3dkey_enableScaling             ( G3DKEY *key );
void     g3dkey_disableScaling            ( G3DKEY *key );
void     g3dkey_enableData                ( G3DKEY *key );
void     g3dkey_disableData               ( G3DKEY *key );

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
                         LIST     **lremovedScaSegments );

void g3dobject_scaleSelectedKeys ( G3DOBJECT *obj,
                                   float      factor, 
                                   float      reference,
                                   LIST     **lremovedKeys,
                                   LIST     **lremovedPosSegments,
                                   LIST     **lremovedRotSegments,
                                   LIST     **lremovedScaSegments,
                                   LIST     **laddedPosSegments,
                                   LIST     **laddedRotSegments,
                                   LIST     **laddedScaSegments );

G3DOBJECT *g3dobject_findReferred_r ( G3DOBJECT *obj );
G3DOBJECT *g3dobject_getRandomChild ( G3DOBJECT *obj );
uint32_t g3dobject_hasRiggedBone_r ( G3DOBJECT *obj );
void g3dobject_update            ( G3DOBJECT *obj,
                                   uint64_t   engine_flags );
void g3dobject_update_r          ( G3DOBJECT *obj,
                                   uint64_t   engine_flags );
void g3dobject_driftSelectedKeys ( G3DOBJECT *obj,
                                   float      drift,
                                   LIST     **lremovedKeys,
                                   LIST     **lremovedPosSegments,
                                   LIST     **lremovedRotSegments,
                                   LIST     **lremovedScaSegments,
                                   LIST     **laddedPosSegments,
                                   LIST     **laddedRotSegments,
                                   LIST     **laddedScaSegments );

G3DKEY *g3dobject_addKey ( G3DOBJECT *obj,
                           G3DKEY    *key,
                           LIST     **laddedPosSegments, 
                           LIST     **laddedRotSegments, 
                           LIST     **laddedScaSegments, 
                           LIST     **lremovedPosSegments, 
                           LIST     **lremovedRotSegments, 
                           LIST     **lremovedScaSegments );

void g3dobject_setKeyTransformations ( G3DOBJECT *obj, 
                                       G3DKEY    *key,
                                       uint32_t   keyFlags,
                                       LIST     **laddedPosSegments,
                                       LIST     **laddedRotSegments,
                                       LIST     **laddedScaSegments,
                                       LIST     **lremovedPosSegments,
                                       LIST     **lremovedRotSegments,
                                       LIST     **lremovedScaSegments );

void g3dobject_unsetKeyTransformations ( G3DOBJECT *obj,
                                         G3DKEY    *key,
                                         uint32_t   keyFlags,
                                         LIST     **lremovedPosSegments,
                                         LIST     **lremovedRotSegments,
                                         LIST     **lremovedScaSegments );

void g3dobject_preanim_tags ( G3DOBJECT *obj, 
                              float      frame, 
                              uint64_t   engine_flags );
void g3dobject_postanim_tags ( G3DOBJECT *obj, 
                               float      frame, 
                               uint64_t   engine_flags );
void g3dobject_updateMeshes_r ( G3DOBJECT *obj, uint64_t engine_flags );
G3DTAG *g3dobject_getTagByID ( G3DOBJECT *obj, uint32_t id );
uint32_t g3dobject_getNextTagID ( G3DOBJECT *obj );
void g3dobject_connectScalingSegmentFromFrame ( G3DOBJECT *obj, 
                                                float      frame );
void g3dobject_connectRotationSegmentFromFrame ( G3DOBJECT *obj, 
                                                 float      frame );
void g3dobject_connectPositionSegmentFromFrame ( G3DOBJECT *obj, 
                                                 float      frame );

void g3dobject_stitchScalingCurve   ( G3DOBJECT *obj, 
                                      LIST      **laddedSegments );
void g3dobject_stitchRotationCurve  ( G3DOBJECT *obj, 
                                      LIST      **laddedSegments );
void g3dobject_stitchPositionCurve  ( G3DOBJECT *obj, 
                                      LIST      **laddedSegments );

void g3dobject_stitchTransformations ( G3DOBJECT *obj,
                                       LIST     **laddedPosSegments,
                                       LIST     **laddedRotSegments,
                                       LIST     **laddedScaSegments );

/******************************************************************************/
void g3dbbox_draw ( G3DBBOX *bbox, 
                    uint64_t engine_flags );
void g3dbbox_adjust  ( G3DBBOX *, G3DVERTEX * );
void g3dbbox_getSize ( G3DBBOX *, float *, float *, float * );

/******************************************************************************/
G3DOBJECT *g3dobject_new  ( uint32_t, const char *, uint32_t );
void       g3dobject_init ( G3DOBJECT   *obj,
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
                                                      uint64_t ) );
void g3dobject_addTag ( G3DOBJECT *obj, 
                        G3DTAG    *tag );
void g3dobject_removeTag ( G3DOBJECT *obj, 
                           G3DTAG    *tag );
uint32_t g3dobject_pickModifiers ( G3DOBJECT *obj, 
                                   G3DCAMERA *cam,
                                   uint64_t   engine_flags  );
uint32_t g3dobject_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint64_t   engine_flags );
uint32_t g3dobject_draw_r ( G3DOBJECT *obj, 
                            G3DCAMERA *curcam, 
                            uint64_t   engine_flags );

void g3dobject_removeKey ( G3DOBJECT *obj, 
                           G3DKEY    *key, 
                           uint32_t   stitch,
                           LIST     **lremovedPosSegments,
                           LIST     **lremovedRotSegments,
                           LIST     **lremovedScaSegments,
                           LIST     **laddedPosSegments,
                           LIST     **laddedRotSegments,
                           LIST     **laddedScaSegments );

/* modifier is responsible for the drawing */
#define MODIFIERBUILDSNEWMESH ( 1 << 0 ) 
#define MODIFIERCHANGESCOORDS ( 1 << 1 ) 
#define MODIFIERTAKESOVER     ( 1 << 2 ) 

void       g3dobject_free                  ( G3DOBJECT *  );
void g3dobject_removeChild ( G3DOBJECT *obj, 
                             G3DOBJECT *child,
                             uint64_t   engine_flags );
void g3dobject_selectKeyRange ( G3DOBJECT *obj,
                                float      firstFrame, 
                                float      lastFrame );
G3DKEY *g3dobject_getNextKey ( G3DOBJECT *obj,
                               G3DKEY    *key );
void g3dobject_addChild ( G3DOBJECT *obj, 
                          G3DOBJECT *child,
                          uint64_t engine_flags );
G3DOBJECT *g3dobject_getChildByID          ( G3DOBJECT *, uint32_t );
void       g3dobject_importTransformations ( G3DOBJECT *, G3DOBJECT * );
void g3dobject_drawCenter ( G3DOBJECT *obj,
                            uint64_t engine_flags );
void       g3dobject_getObjectsByType_r    ( G3DOBJECT *, uint32_t, LIST ** );
void       g3dobject_unsetSelected         ( G3DOBJECT * );
void       g3dobject_setSelected           ( G3DOBJECT * );
uint32_t g3dobject_pick_r ( G3DOBJECT *obj, 
                            G3DCAMERA *curcam, 
                            uint64_t engine_flags );
uint32_t g3dobject_pick ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint64_t engine_flags );
void g3dobject_anim_r ( G3DOBJECT *obj,
                        float      frame,
                        uint64_t  engine_flags );
void       g3dobject_updateMatrix          ( G3DOBJECT *, uint64_t );
void g3dobject_drawKeys ( G3DOBJECT *obj, 
                          G3DCAMERA *cam, 
                          uint64_t   engine_flags );
uint32_t   g3dobject_countChildren_r       ( G3DOBJECT * );
G3DOBJECT *g3dobject_getSelectedChild      ( G3DOBJECT * );
void       g3dobject_name                  ( G3DOBJECT *, const char * );
void g3dobject_updateMatrix_r ( G3DOBJECT *obj, 
                                uint64_t   engine_flags );
G3DKEY    *g3dobject_getKey                ( G3DOBJECT *, float );
uint32_t   g3dobject_getID                 ( G3DOBJECT * );
char      *g3dobject_getName               ( G3DOBJECT * );
void       g3dobject_selectKey             ( G3DOBJECT *, G3DKEY * );
void       g3dobject_selectAllKeys         ( G3DOBJECT * );
void       g3dobject_unselectKey           ( G3DOBJECT *, G3DKEY * );
void       g3dobject_unselectAllKeys       ( G3DOBJECT * );
G3DKEY    *g3dobject_getKeyByFrame         ( G3DOBJECT *, float );
uint32_t   g3dobject_nbkeyloop             ( G3DOBJECT * );
uint32_t   g3dobject_isChild               ( G3DOBJECT *, G3DOBJECT * );
G3DOBJECT* g3dobject_default_copy ( G3DOBJECT  *obj,
                                    uint32_t    id,
                                    const char *name,
                                    uint64_t    engine_flags );
void g3dobject_importChild ( G3DOBJECT *newparent, 
                             G3DOBJECT *child, 
                             uint64_t   engine_flags );
G3DOBJECT *g3dobject_copy ( G3DOBJECT  *obj, 
                            uint32_t    id,
                            const char *name, 
                            uint64_t    engine_flags );
void g3dobject_localTranslate ( G3DOBJECT *obj, 
                                float      x,
                                float      y,
                                float      z,
                                uint64_t   engine_flags );
void       g3dobject_initMatrices          ( G3DOBJECT * );
uint32_t   g3dobject_getNumberOfChildrenByType ( G3DOBJECT *, uint64_t );
LIST      *g3dobject_getChildrenByType     ( G3DOBJECT *, uint64_t );
void       g3dobject_printCoordinates      ( G3DOBJECT * );
float      g3dobject_getKeys               ( G3DOBJECT *, float, 
                                                          G3DKEY **,
                                                          G3DKEY **,
                                                          G3DKEY **,
                                                          uint32_t,
                                                          uint32_t );

void g3dobject_anim_position ( G3DOBJECT *obj,
                               float      frame,
                               uint64_t   engine_flags );
void g3dobject_anim_rotation ( G3DOBJECT *obj,
                               float      frame, 
                               uint64_t   engine_flags );
void g3dobject_anim_scaling ( G3DOBJECT *obj,
                              float      frame,
                              uint64_t   engine_flags );

void g3dobject_removeSelectedKeys ( G3DOBJECT *obj,
                                    LIST     **lremovedKeys,
                                    LIST     **lremovedPosSegments,
                                    LIST     **lremovedRotSegments,
                                    LIST     **lremovedScaSegments,
                                    LIST     **laddedPosSegments,
                                    LIST     **laddedRotSegments,
                                    LIST     **laddedScaSegments );

G3DOBJECT *g3dobject_commit ( G3DOBJECT     *obj, 
                              uint32_t       id,
                              unsigned char *name,
                              uint64_t       engine_flags );
G3DOBJECT *g3dobject_getActiveParentByType ( G3DOBJECT *, uint32_t );
uint32_t   g3dobject_isActive              ( G3DOBJECT * );
uint32_t g3dobject_isSelected ( G3DOBJECT *obj );
uint32_t g3dobject_drawModifiers ( G3DOBJECT *obj, 
                                   G3DCAMERA *cam,
                                   uint64_t   engine_flags  );
uint32_t g3dobject_hasSelectedParent ( G3DOBJECT *obj );
void g3dobject_deactivate ( G3DOBJECT *obj, 
                            uint64_t   engine_flags );
void g3dobject_appendChild ( G3DOBJECT *obj, 
                             G3DOBJECT *child,
                             uint64_t engine_flags );
void g3dobject_addChild ( G3DOBJECT *obj, 
                          G3DOBJECT *child,
                          uint64_t engine_flags );
void g3dobject_activate ( G3DOBJECT *obj,
                          uint64_t   engine_flags );
void g3dobject_startUpdateModifiers_r ( G3DOBJECT *obj,
                                        uint64_t   engine_flags );
void g3dobject_updateModifiers_r ( G3DOBJECT *obj,
                                   uint64_t   engine_flags );
void g3dobject_endUpdateModifiers_r ( G3DOBJECT *obj,
                                      uint64_t   engine_flags );
void g3dobject_modify_r ( G3DOBJECT *obj, uint64_t engine_flags );
void g3dobject_moveAxis ( G3DOBJECT *obj, 
                          double    *PREVMVX, /* previous world matrix */
                          uint64_t engine_flags );
void g3dobject_getSurroundingKeys ( G3DOBJECT *obj,
                                    G3DKEY    *key,
                                    G3DKEY   **prevKey,
                                    G3DKEY   **nextKey,
                                    uint32_t   key_flags );
void g3dobject_treeToList_r ( G3DOBJECT *obj, LIST **lis );
void g3dobject_browse ( G3DOBJECT  *obj,
                        uint32_t  (*action)(G3DOBJECT *, void *, uint64_t ),
                        void       *data,
                        uint64_t    engine_flags );

/******************************************************************************/
G3DSYMMETRY *g3dsymmetry_new      ( uint32_t, char * );
void         g3dsymmetry_free     ( G3DOBJECT * );
uint32_t g3dsymmetry_draw ( G3DOBJECT *obj,
                            G3DCAMERA *curcam, 
                            uint64_t   engine_flags );
void         g3dsymmetry_setPlane ( G3DSYMMETRY *, uint32_t );
G3DMESH *g3dsymmetry_convert ( G3DSYMMETRY *sym,
                               LIST       **loldobj, 
                               uint64_t     engine_flags );
void         g3dsymmetry_meshChildChange ( G3DSYMMETRY *, G3DMESH * );
void         g3dsymmetry_childVertexChange ( G3DOBJECT *, G3DOBJECT *, G3DVERTEX * );
void         g3dsymmetry_setMergeLimit ( G3DSYMMETRY *, float );

/******************************************************************************/
G3DPRIMITIVE *g3dprimitive_new ( uint32_t id, 
                                 char    *name, 
                                 void    *data,
                                 uint32_t datalen );
void          g3dprimitive_free    ( G3DPRIMITIVE *pri );
uint32_t g3dprimitive_draw ( G3DPRIMITIVE *pri, 
                             G3DCAMERA *curcam, 
                             uint64_t   engine_flags );
G3DMESH *g3dprimitive_convert ( G3DPRIMITIVE *pri, 
                                uint64_t      engine_flags );
void g3dprimitive_init ( G3DPRIMITIVE *pri, 
                         uint32_t     id,
                         char        *name,
                         void        *data,
                         uint32_t     datalen );
G3DOBJECT *g3dprimitive_copy ( G3DOBJECT *obj, 
                               uint64_t   engine_flags );

/******************************************************************************/
G3DPRIMITIVE *g3dsphere_new ( uint32_t, char * );
void          g3dsphere_build ( G3DPRIMITIVE *, int, int, float );
void          g3dsphere_size ( G3DPRIMITIVE *, float );

/******************************************************************************/
void          g3dplane_allocArrays ( G3DPRIMITIVE *, int, int );
void          g3dplane_boundaries  ( G3DPRIMITIVE * );
void          g3dplane_build ( G3DPRIMITIVE *, int, int, int, float, float );
G3DPRIMITIVE *g3dplane_new ( uint32_t, char * );

/******************************************************************************/
G3DPRIMITIVE *g3dcube_new ( uint32_t, char * );
void          g3dcube_build ( G3DPRIMITIVE *, int, int, int, float );
void          g3dcube_allocArrays ( G3DPRIMITIVE *, int, int, int );
void          g3dcube_draw ( G3DOBJECT *, uint32_t );

/******************************************************************************/
void          g3dtorus_allocArrays ( G3DPRIMITIVE *, int, int );
void          g3dtorus_boundaries ( G3DPRIMITIVE * );
void          g3dtorus_build ( G3DPRIMITIVE *, int, int, int, float, float );
G3DPRIMITIVE *g3dtorus_new ( uint32_t, char * );

/******************************************************************************/
void g3dcylinder_allocArrays ( G3DPRIMITIVE *, int, int, int, float, float );
void g3dcylinder_build ( G3DPRIMITIVE *pri, int, int, int, float, float );
G3DPRIMITIVE *g3dcylinder_new ( uint32_t, char * );

/******************************************************************************/
void g3dtube_build ( G3DPRIMITIVE *pri, 
                     int           slice,
                     int           capx,
                     int           capy,
                     float         radius, 
                     float         thickness,
                     float         length );
G3DPRIMITIVE *g3dtube_new ( uint32_t id, 
                            char    *name );

/******************************************************************************/
G3DMESH *g3dmesh_new ( uint32_t id, 
                       char    *name, 
                       uint64_t engine_flags );
void g3dmesh_init ( G3DMESH *mes, 
                    uint32_t id, 
                    char    *name,
                    uint64_t engine_flags );
void       g3dmesh_free                 ( G3DOBJECT * );
void g3dmesh_paintWeightgroup ( G3DMESH        *mes,
                                G3DWEIGHTGROUP *grp );
void g3dmesh_unpaintWeightgroup ( G3DMESH        *mes,
                                  G3DWEIGHTGROUP *grp );
void g3dmesh_emptyWeightgroup ( G3DMESH        *mes,
                                G3DWEIGHTGROUP *grp );
G3DWEIGHTGROUP *g3dmesh_mirrorWeightgroup ( G3DMESH        *mes,
                                            G3DWEIGHTGROUP *grp,
                                            uint32_t        orientation );
void g3dmesh_fixWeightgroup ( G3DMESH        *mes,
                              G3DWEIGHTGROUP *grp,
                              G3DRIG         *rig );
void g3dmesh_unfixWeightgroup ( G3DMESH        *mes,
                                G3DWEIGHTGROUP *grp );
uint32_t g3dmesh_getWeightgroupCount ( G3DMESH        *mes,
                                       G3DWEIGHTGROUP *grp );

/******************************** Mesh API ************************************/
G3DUVMAP  *g3dmesh_getSelectedUVMap     ( G3DMESH *mes );
void       g3dmesh_selectUVMap          ( G3DMESH *mes, G3DUVMAP *map );
void       g3dmesh_unselectAllUVMaps    ( G3DMESH *mes );
void       g3dmesh_addFace              ( G3DMESH *, G3DFACE * );
void g3dmesh_attachFaceEdges ( G3DMESH *mes, G3DFACE *fac );
void       g3dmesh_addFaceWithEdges     ( G3DMESH *mes, 
                                          G3DFACE *fac,
                                          G3DEDGE *edg0,
                                          G3DEDGE *edg1,
                                          G3DEDGE *edg2,
                                          G3DEDGE *edg3 );
void       g3dmesh_addFaceFromSplitEdge ( G3DMESH *, G3DSPLITEDGE * );
void       g3dmesh_addMaterial          ( G3DMESH *, G3DMATERIAL  *,
                                                     G3DUVMAP     * );
void       g3dmesh_addVertex            ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_addSelectedFace      ( G3DMESH *, G3DFACE * );
void       g3dmesh_addSelectedVertex    ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_addTexture           ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_addUVMap ( G3DMESH  *mes, 
                              G3DUVMAP *map,
                              LIST    **lnewuvset,
                              uint64_t engine_flags );
void       g3dmesh_addWeightGroup       ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_alignNormals         ( G3DMESH * );
void       g3dmesh_alignFaces           ( G3DMESH * );
uint32_t   g3dmesh_getUVMapCount        ( G3DMESH *mes );
G3DUVMAP *g3dmesh_getUVMapByRank ( G3DMESH *mes, uint32_t rank );
void       g3dmesh_drawSelectedUVMap ( G3DMESH   *mes,
                                       G3DCAMERA *curcam,
                                       uint64_t   engine_flags );
void       g3dmesh_attachFaceEdges      ( G3DMESH *, G3DFACE * );
void g3dmesh_fixBones ( G3DMESH *mes, 
                        uint64_t engine_flags );
void g3dmesh_drawModified ( G3DMESH   *mes,
                            G3DCAMERA *cam, 
                            G3DVECTOR *verpos,
                            G3DVECTOR *vernor,
                            uint64_t   engine_flags );
void g3dmesh_cut ( G3DMESH *mes, 
                   G3DFACE *knife,
                   LIST   **loldfac,
                   LIST   **lnewver,
                   LIST   **lnewfac,
                   uint32_t restricted,
                   uint64_t engine_flags );
void g3dmesh_setGeometryInArrays ( G3DMESH *mes, G3DVERTEX *ver,
                                                 uint32_t   nbver,
                                                 G3DEDGE   *edg,
                                                 uint32_t   nbedg,
                                                 G3DFACE   *fac,
                                                 uint32_t   nbtri,
                                                 uint32_t   nbqua );
uint32_t g3dmesh_draw ( G3DOBJECT *obj, 
                        G3DCAMERA *curcam, 
                        uint64_t engine_flags );
void g3dmesh_drawEdges ( G3DMESH *mes,
                         uint32_t object_flags, 
                         uint64_t engine_flags );
void       g3dmesh_drawFace             ( G3DMESH *, uint32_t );
void g3dmesh_drawFaces ( G3DMESH *mes,
                         uint32_t object_flags, 
                         uint64_t engine_flags );
void g3dmesh_drawFaceNormal ( G3DMESH *mes,
                              uint64_t engine_flags );
void g3dmesh_drawObject ( G3DMESH   *mes, 
                          G3DCAMERA *curcam, 
                          uint64_t   engine_flags );
void g3dmesh_drawSelectedVertices ( G3DMESH *mes,
                                    uint64_t engine_flags );
void g3dmesh_drawVertexNormal ( G3DMESH *mes,
                                uint64_t engine_flags );
void g3dmesh_drawVertices  ( G3DMESH *mes,
                             uint64_t engine_flags );
void       g3dmesh_empty                ( G3DMESH * );
void       g3dmesh_extrude              ( G3DMESH *, LIST **, 
                                                     LIST **,
                                                     LIST **,
                                                     LIST ** );
G3DEDGE   *g3dmesh_getEdgeByID          ( G3DMESH *, uint32_t );
G3DFACE   *g3dmesh_getFaceByID          ( G3DMESH *, uint32_t  );
LIST      *g3dmesh_getFaceListFromSelectedEdges         ( G3DMESH * );
LIST      *g3dmesh_getFaceListFromSelectedVertices      ( G3DMESH * );
uint32_t   g3dmesh_getNextVertexID      ( G3DMESH * );
uint32_t   g3dmesh_getNextEdgeID        ( G3DMESH * );
uint32_t   g3dmesh_getNextFaceID        ( G3DMESH * );
G3DVERTEX *g3dmesh_getVertexByID        ( G3DMESH *, uint32_t );
LIST      *g3dmesh_getVertexListFromSelectedFaces       ( G3DMESH * );
LIST      *g3dmesh_getVertexListFromSelectedVertices    ( G3DMESH * );
LIST      *g3dmesh_getVertexListFromSelectedEdges       ( G3DMESH * );

LIST      *g3dmesh_getEdgeBoundariesFromSelectedFaces   ( G3DMESH * );
LIST      *g3dmesh_getVertexBoundariesFromSelectedFaces ( G3DMESH * );
uint32_t g3dmesh_pick ( G3DMESH   *mes, 
                        G3DCAMERA *curcam, 
                        uint64_t engine_flags );

void       g3dmesh_selectFace           ( G3DMESH *, G3DFACE * );
void       g3dmesh_selectVertex         ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_stats                ( G3DMESH * );
void       g3dmesh_unselectAllEdges     ( G3DMESH * );
void       g3dmesh_unselectAllFaces     ( G3DMESH * );
void       g3dmesh_unselectAllVertices  ( G3DMESH * );
void       g3dmesh_updateBbox ( G3DMESH * );
void       g3dmesh_vertexNormal         ( G3DMESH * );
void       g3dmesh_getCenterFromVertexList ( LIST *, G3DVECTOR * );
G3DVERTEX *g3dmesh_getLastSelectedVertex ( G3DMESH * );
void       g3dmesh_updateFacesFromVertexList ( G3DMESH *, LIST * );
void       g3dmesh_removeFace ( G3DMESH *, G3DFACE * );
void       g3dmesh_untriangulate ( G3DMESH *, LIST **, LIST ** );
void       g3dmesh_invertNormal ( G3DMESH * );
void       g3dmesh_removeEdge ( G3DMESH *, G3DEDGE * );
void       g3dmesh_unselectFace ( G3DMESH *, G3DFACE * );
void       g3dmesh_removeSelectedVertices ( G3DMESH * );
void       g3dmesh_removeSelectedFaces    ( G3DMESH * );
void       g3dmesh_getSelectedVerticesTranslation ( G3DMESH *, double * );
void       g3dmesh_getSelectedFacesTranslation    ( G3DMESH *, double * );
void       g3dmesh_getSelectedEdgesTranslation    ( G3DMESH *, double * );
void       g3dmesh_drawSubdividedObject ( G3DMESH *, uint32_t );

void       g3dmesh_getSelectedVerticesWorldPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedVerticesLocalPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedFacesWorldPosition    ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedFacesLocalPosition    ( G3DMESH *, G3DVECTOR * );
G3DVERTEX *g3dmesh_weldSelectedVertices ( G3DMESH *, uint32_t, LIST **,
                                                               LIST ** );
void g3dmesh_removeTexture ( G3DMESH    *mes,
                             G3DTEXTURE *tex );
void g3dmesh_weldNeighbourVertices ( G3DMESH *mes,
                                      float   distance,
                                      LIST **lnewver,
                                      LIST **loldver,
                                      LIST **lnewfac,
                                      LIST **loldfac );
void       g3dmesh_removeFacesFromVertex ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_removeFacesFromSelectedVertices ( G3DMESH * );
void       g3dmesh_getSelectedEdgesLocalPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedEdgesWorldPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_unselectVertex ( G3DMESH *, G3DVERTEX * );

void       g3dmesh_removeFacesFromList ( G3DMESH *, LIST * );
void       g3dmesh_removeVerticesFromList        ( G3DMESH *, LIST * );
void       g3dmesh_unsetFacesAlignedFlag         ( G3DMESH * );
void g3dmesh_moveVerticesTo ( G3DMESH   *mes, 
                              LIST      *lver, 
                              G3DVECTOR *avg,
                              G3DVECTOR *to,
                              uint32_t   absolute,
                              uint32_t   axis_flags,
                              uint64_t   engine_flags );
void       g3dmesh_removeUnusedEdges             ( G3DMESH * );
void       g3dmesh_removeVertex                  ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_faceNormal                    ( G3DMESH * );
void       g3dmesh_createFaceFromEdge            ( G3DMESH *, G3DEDGE *, LIST * );
void       g3dmesh_keepVisibleVerticesOnly       ( G3DMESH *, double *, double *, int *, int );
void       g3dmesh_keepVisibleFacesOnly          ( G3DMESH *, double *, double *, int *, int );
void       g3dmesh_selectUniqueVertex            ( G3DMESH *mes, G3DVERTEX *ver );
void       g3dmesh_selectUniqueEdge              ( G3DMESH *mes, G3DEDGE   *edg );
void       g3dmesh_selectUniqueFace              ( G3DMESH *mes, G3DFACE   *fac );
void g3dmesh_color ( G3DMESH *mes,
                     uint64_t engine_flags );
void       g3dmesh_removeWeightGroup             ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_unselectWeightGroup           ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_selectWeightGroup             ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_paintVertices                 ( G3DMESH *, G3DCAMERA *, float, uint32_t, uint32_t );
G3DVERTEX *g3dmesh_seekVertexByPosition          ( G3DMESH *, float, float, float, float );
void       g3dmesh_setSubdivisionLevel           ( G3DMESH *, uint32_t, uint32_t );
void       g3dmesh_freeSubdivisionBuffer         ( G3DMESH * );
void       g3dmesh_freeFaceSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_freeEdgeSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_freeVertexSubdivisionBuffer   ( G3DMESH * );

void       g3dmesh_setBufferedSubdivision        ( G3DMESH *, uint32_t );
void       g3dmesh_unsetBufferedSubdivision      ( G3DMESH * );
LIST      *g3dmesh_getSubdividedFacesFromSelectedVertices ( G3DMESH * );
void       g3dmesh_updateSubdividedFacesFromList ( G3DMESH *mes, LIST *,
                                                                 uint32_t );
G3DOBJECT *g3dmesh_copy ( G3DOBJECT     *obj, 
                          uint32_t       id, 
                          unsigned char *name, 
                          uint64_t       engine_flags );
void       g3dmesh_removeUnusedVertices          ( G3DMESH *, LIST ** );
void       g3dmesh_selectAllVertices             ( G3DMESH * );
void       g3dmesh_updateFacesFromList           ( G3DMESH *, LIST *,
                                                              LIST *, 
                                                              uint32_t );

void       g3dmesh_deleteTexture                 ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_removeMaterial                ( G3DMESH *, G3DMATERIAL * );
LIST      *g3dmesh_getTexturesFromMaterial       ( G3DMESH *, G3DMATERIAL * );
LIST      *g3dmesh_getTexturesFromUVMap          ( G3DMESH *, G3DUVMAP * );

void g3dmesh_update ( G3DMESH *mes,
                      uint64_t engine_flags );
void g3dmesh_fillSubdividedFaces ( G3DMESH *mes,
                                   LIST    *lfac,
                                   uint64_t engine_flags );
G3DFACE   *g3dmesh_getNextFace                   ( G3DMESH *, LIST * );
uint32_t g3dmesh_isDisplaced ( G3DMESH *mes, 
                               uint64_t engine_flags );
uint32_t g3dmesh_isTextured ( G3DMESH *mes, 
                              uint64_t engine_flags );
G3DUVMAP  *g3dmesh_getLastUVMap                  ( G3DMESH * );
void       g3dmesh_selectTexture                 ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_unselectTexture               ( G3DMESH *, G3DTEXTURE * );
G3DTEXTURE*g3dmesh_getTextureByID                ( G3DMESH *, uint32_t     );
void       g3dmesh_unselectAllTextures           ( G3DMESH * );
G3DUVMAP  *g3dmesh_getUVMapByID                  ( G3DMESH *, uint32_t );
void       g3dmesh_selectEdge                    ( G3DMESH *, G3DEDGE * );
void       g3dmesh_unselectEdge                  ( G3DMESH *, G3DEDGE * );
void       g3dmesh_assignFaceUVSets              ( G3DMESH *, G3DFACE * );
uint64_t   g3dmesh_evalSubdivisionBuffer         ( G3DMESH *, uint32_t, 
                                                              uint32_t );
void       g3dmesh_renumberFaces                 ( G3DMESH * );
void       g3dmesh_renumberEdges                 ( G3DMESH * );
void       g3dmesh_setSyncSubdivision            ( G3DMESH * );
void       g3dmesh_unsetSyncSubdivision          ( G3DMESH * );
void g3dmesh_invertFaceSelection ( G3DMESH *mes, 
                                   uint64_t engine_flags );
LIST *g3dmesh_getVerticesFromWeightgroup ( G3DMESH        *mes,
                                           G3DWEIGHTGROUP *grp );
void       g3dmesh_invertEdgeeSelection( G3DMESH *mes, 
                                         uint64_t engine_flags );
void       g3dmesh_invertVertexSelection ( G3DMESH *mes, 
                                           uint64_t engine_flags );
uint32_t   g3dmesh_isSubdivided                  ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isBuffered                    ( G3DMESH *, uint32_t );
void       g3dmesh_renumberVertices              ( G3DMESH * );
void g3dmesh_clone ( G3DMESH   *mes, 
                     G3DVECTOR *verpos,
                     G3DMESH   *cpymes, 
                     uint64_t engine_flags );
G3DMESH *g3dmesh_splitSelectedFaces ( G3DMESH *mes, 
                                      uint32_t splID,
                                      uint32_t keep, 
                                      LIST   **loldver,
                                      LIST   **loldfac,
                                      uint64_t engine_flags );
void g3dmesh_updateModified ( G3DMESH     *mes,
                              G3DMODIFIER *mod,
                              uint64_t     engine_flags );
                              
void g3dmesh_invertEdgeSelection ( G3DMESH *mes,
                                   uint64_t engine_flags );
void       g3dmesh_triangulate                   ( G3DMESH *, LIST **,
                                                              LIST **,
                                                              uint32_t );
void       g3dmesh_updateFaceIndex               ( G3DMESH * );
uint32_t g3dmesh_default_dump ( G3DMESH *mes, 
                                void (*Alloc)( uint32_t, /*nbver */
                                               uint32_t, /* nbtris */
                                               uint32_t, /* nbquads */
                                               uint32_t, /* nbuv */
                                               void * ),
                                void (*Dump) ( G3DFACE *,
                                               G3DVECTOR *,
                                               G3DVECTOR *,
                                               void * ),
                                void *data,
                                uint64_t engine_flags );
void g3dmesh_dump ( G3DMESH *mes, 
                    void (*Alloc)( uint32_t, /* nbver */
                                   uint32_t, /* nbtris */
                                   uint32_t, /* nbquads */
                                   uint32_t, /* nbuv */
                                   void * ),
                    void (*Dump) ( G3DFACE *,
                                   G3DVECTOR *,
                                   G3DVECTOR *,
                                   void * ),
                    void *data,
                    uint64_t engine_flags );
uint32_t g3dmesh_dumpModifiers_r ( G3DMESH *mes, 
                                   void (*Alloc)( uint32_t, /* nbver */
                                                  uint32_t, /* nbtris */
                                                  uint32_t, /* nbquads */
                                                  uint32_t, /* nbuv */
                                                  void * ),
                                   void (*Dump) ( G3DFACE *,
                                                  G3DVECTOR *,
                                                  G3DVECTOR *,
                                                  void * ),
                                   void *data,
                                   uint64_t engine_flags );
void g3dmesh_addChild ( G3DMESH   *mes,
                        G3DOBJECT *child, 
                        uint64_t   engine_flags );
void       g3dmesh_clearGeometry ( G3DMESH * );
G3DMESH *g3dmesh_symmetricMerge ( G3DMESH *mes,
                                  double  *MVX,
                                  uint64_t engine_flags );
void g3dmesh_moveAxis ( G3DMESH *mes, 
                        double  *PREVMVX,
                        uint64_t engine_flags );
void       g3dmesh_selectAllEdges         ( G3DMESH * );
void       g3dmesh_selectAllFaces         ( G3DMESH * );
G3DMESH *g3dmesh_merge ( LIST    *lobj, 
                         uint32_t mesID, 
                         uint64_t engine_flags );
G3DTEXTURE *g3dmesh_getSelectedTexture ( G3DMESH *mes );
void g3dmesh_removeUVMap ( G3DMESH  *mes,
                           G3DUVMAP *map,
                           LIST    **lolduvset,
                           LIST    **loldtex,
                           uint64_t engine_flags );
void g3dmesh_pickVertexUVs ( G3DMESH *mes,
                             uint64_t engine_flags );
void g3dmesh_pickFaceUVs ( G3DMESH *mes,
                           uint64_t engine_flags );
void g3dmesh_drawVertexUVs ( G3DMESH *mes,
                             uint64_t engine_flags );
uint32_t g3dmesh_modify ( G3DMESH    *mes,
                          G3DMODIFYOP op,
                          uint64_t    engine_flags );
void g3dmesh_drawFaceUVs ( G3DMESH *mes,
                           uint64_t engine_flags );
G3DTEXTURE *g3dmesh_getDefaultTexture ( G3DMESH *mes );
uint32_t g3dmesh_getAvailableTextureSlot ( G3DMESH *mes );
void g3dmesh_removeFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp );
void g3dmesh_addFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp );
void g3dmesh_selectFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp );
void g3dmesh_unselectAllFacegroups ( G3DMESH *mes );
void g3dmesh_unselectFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp );
void g3dmesh_selectFacesFromSelectedFacegroups ( G3DMESH *mes );
G3DFACEGROUP *g3dmesh_getLastSelectedFacegroup ( G3DMESH *mes );
G3DFACEGROUP *g3dmesh_getFacegroupByID ( G3DMESH *mes, uint32_t id );

/******************************************************************************/
G3DSCENE  *g3dscene_new  ( uint32_t, char * );
void g3dscene_checkReferredObjects ( G3DSCENE *sce );
void g3dscene_removeReferredObject ( G3DSCENE *sce, 
                                     G3DOBJECT *obj );
void g3dscene_addReferredObject ( G3DSCENE *sce, 
                                  G3DOBJECT *obj );
uint32_t g3dscene_isObjectReferred ( G3DSCENE *sce, 
                                     G3DOBJECT *obj );
uint32_t g3dscene_getSelectionPosition ( G3DSCENE *sce, G3DVECTOR *vout );
uint32_t g3dscene_getSelectionMatrix ( G3DSCENE *sce, 
                                       double   *matrix, 
                                       uint64_t  engine_flags );
uint32_t g3dscene_getPivotFromSelection ( G3DSCENE  *sce,
                                          uint64_t   engine_flags );
void g3dscene_updatePivot ( G3DSCENE  *sce,
                            uint64_t   engine_flags );
void       g3dscene_free ( G3DOBJECT * );
uint32_t g3dscene_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam, 
                         uint64_t   engine_flags );
void g3dscene_unselectObject ( G3DSCENE *sce,
                               G3DOBJECT *obj, 
                               uint64_t   engine_flags );
void g3dscene_selectObject ( G3DSCENE  *sce, 
                             G3DOBJECT *obj,
                             uint64_t   engine_flags );
void       g3dscene_selectTree               ( G3DSCENE *, G3DOBJECT *, int );
G3DOBJECT *g3dscene_getLastSelected          ( G3DSCENE * );
uint32_t   g3dscene_selectCount              ( G3DSCENE * );
G3DOBJECT *g3dscene_selectObjectByName ( G3DSCENE *sce, 
                                         char     *name,
                                         uint64_t  engine_flags );
void g3dscene_unselectAllObjects ( G3DSCENE *sce, 
                                   uint64_t  engine_flags );
uint32_t g3dscene_deleteSelectedObjects ( G3DSCENE *sce, 
                                          LIST    **lremovedObjects,
                                          uint64_t  engine_flags );
G3DOBJECT *g3dscene_getSelectedObject        ( G3DSCENE * );
void       g3dscene_pickChild                ( G3DSCENE *, uint32_t );
uint32_t   g3dscene_getNextObjectID          ( G3DSCENE * );
void       g3dscene_drawScene                ( G3DSCENE *, uint32_t );
LIST      *g3dscene_getAllMeshes             ( G3DSCENE * );
void       g3dscene_addMaterial              ( G3DSCENE *, G3DMATERIAL * );
void       g3dscene_appendMaterial           ( G3DSCENE *sce, G3DMATERIAL *mat );
void       g3dscene_removeMaterial           ( G3DSCENE *, G3DMATERIAL * );
void       g3dscene_freeMaterials            ( G3DSCENE * );
void       g3dscene_drawSelectedObjectCursor ( G3DSCENE *, uint32_t );
void       g3dscene_checkLights              ( G3DSCENE *, uint64_t );
void       g3dscene_updateBufferedMeshes     ( G3DSCENE *, uint32_t );
void       g3dscene_turnLightsOn             ( G3DSCENE * );
void       g3dscene_turnLightsOff            ( G3DSCENE * );
void g3dscene_selectAllObjects ( G3DSCENE *sce, 
                                 uint64_t  engine_flags );
uint32_t   g3dscene_noLightOn                ( G3DSCENE *, uint64_t );
void g3dscene_invertSelection ( G3DSCENE *sce, 
                                uint64_t  engine_flags );
void g3dscene_updateMeshes ( G3DSCENE *sce, 
                             uint64_t  engine_flags );
void       g3dscene_exportStlA               ( G3DSCENE *, const char *,
                                                           const char *, 
                                                           uint32_t );
G3DIMAGE *g3dscene_getImage ( G3DSCENE *sce, 
                              char     *filename );
void g3dscene_registerImage ( G3DSCENE *sce, 
                              G3DIMAGE *img );
void g3dscene_unregisterImage ( G3DSCENE *sce, 
                                G3DIMAGE *img );
void g3dscene_updateMeshesFromImage ( G3DSCENE *sce,
                                      G3DIMAGE *img,
                                      uint64_t  engine_flags );
void g3dscene_processAnimatedImages ( G3DSCENE *sce, 
                                      float     sceneStartFrame,
                                      float     sceneCurrentFrame,
                                      float     sceneEndFrame,
                                      float     sceneFramesPerSecond,
                                      uint64_t  engine_flags );

/******************************************************************************/
G3DCAMERA *g3dcamera_new ( uint32_t id,
                           char    *name, 
                           float    focal, 
                           float    ratio,
                           float    znear, 
                           float    zfar );
void g3dcamera_setTarget ( G3DCAMERA *cam, 
                           G3DCURSOR *csr,
                           uint64_t   engine_flags );
void g3dcamera_view ( G3DCAMERA *cam, 
                      uint64_t   engine_flags );
void g3dcamera_spin ( G3DCAMERA *cam, float diffz );
uint32_t g3dbone_draw ( G3DOBJECT *obj, 
                        G3DCAMERA *curcam, 
                        uint64_t   engine_flags );
G3DRIG *g3dbone_addRig ( G3DBONE *bon,
                         G3DSKIN *skn );
void g3dbone_removeRig ( G3DBONE *bon,
                         G3DSKIN *skn );
void g3dbone_updateRigs ( G3DBONE *bon, 
                          uint64_t engine_flags );
void       g3dcamera_free     ( G3DOBJECT * );
uint32_t g3dcamera_pick ( G3DCAMERA *cam, 
                          G3DCAMERA *curcam, 
                          uint64_t engine_flags );
void       g3dcamera_setGrid  ( G3DCAMERA *, void (*)(G3DCAMERA *, uint64_t) );
void g3dcamera_project ( G3DCAMERA *cam, 
                         uint64_t   engine_flags );
void       g3dcamera_setPivot ( G3DCAMERA *, float, float, float );
void       g3dcamera_import   ( G3DCAMERA *, G3DCAMERA * );
void g3dcamera_setOrtho ( G3DCAMERA *cam,
                          uint32_t   width, 
                          uint32_t   height );
void g3dcamera_updatePivotFromScene ( G3DCAMERA *cam, 
                                      G3DSCENE  *sce,
                                      uint64_t   engine_flags );
float g3dcamera_getDistanceToCursor ( G3DCAMERA *cam, 
                                      G3DCURSOR *csr );

/******************************************************************************/
void g3dcursor_draw ( G3DCURSOR *csr, 
                      G3DCAMERA *curcam,
                      uint64_t   engine_flags );
void g3dcursor_pick ( G3DCURSOR *csr, 
                      G3DCAMERA *cam, 
                      uint64_t   engine_flags );
void g3dcursor_init  ( G3DCURSOR * );
void g3dcursor_reset ( G3DCURSOR * );

/******************************************************************************/
G3DFFD *g3dffd_new          ( uint32_t, char * );
void    g3dffd_free         ( G3DOBJECT * );
void    g3dffd_shape        ( G3DFFD *, uint32_t, uint32_t, uint32_t, float, float, float );
void    g3dffd_assign       ( G3DFFD *, G3DMESH * );
void    g3dffd_addVertex    ( G3DFFD *, G3DVERTEX * );
void    g3dffd_appendVertex ( G3DFFD *, G3DVERTEX * );
void    g3dffd_unassign     ( G3DFFD * );
void    g3dffd_load ( G3DFFD *ffd, LIST *lver, G3DVECTOR *pos, G3DVECTOR *uvw );

/******************************************************************************/
G3DMATERIAL *g3dmaterial_new                  ( const char * );
void         g3dmaterial_free                 ( G3DMATERIAL * );
void         g3dmaterial_addDisplacementImage ( G3DMATERIAL *, G3DIMAGE * );
void         g3dmaterial_addDiffuseImage      ( G3DMATERIAL *, G3DIMAGE * );
G3DPROCEDURAL *g3dmaterial_addDiffuseProcedural      ( G3DMATERIAL *,
                                                       G3DPROCEDURAL * );
G3DPROCEDURAL *g3dmaterial_addDisplacementProcedural ( G3DMATERIAL *, 
                                                       G3DPROCEDURAL * );
void         g3dmaterial_addObject            ( G3DMATERIAL *, G3DOBJECT * );
void         g3dmaterial_removeObject         ( G3DMATERIAL *, G3DOBJECT * );
void g3dmaterial_updateMeshes ( G3DMATERIAL *mat, 
                                G3DSCENE    *sce, 
                                uint64_t     engine_flags );
void         g3dmaterial_draw                 ( G3DMATERIAL *, G3DFACE *, uint32_t );
void         g3dmaterial_disableDisplacement  ( G3DMATERIAL *mat );
void         g3dmaterial_enableDisplacement   ( G3DMATERIAL *mat );
void         g3dmaterial_disableBump  ( G3DMATERIAL *mat );
void         g3dmaterial_enableBump   ( G3DMATERIAL *mat );
void         g3dmaterial_disableAlpha  ( G3DMATERIAL *mat );
void         g3dmaterial_enableAlpha   ( G3DMATERIAL *mat );
void         g3dmaterial_enableDisplacementImageColor ( G3DMATERIAL * );
void         g3dmaterial_enableDisplacementProcedural ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseImageColor ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseProcedural ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseSolidColor ( G3DMATERIAL * );
void         g3dmaterial_enableReflectionImageColor ( G3DMATERIAL * );
void         g3dmaterial_name ( G3DMATERIAL *, const char * );
LIST        *g3dmaterial_getObjects ( G3DMATERIAL *mat, G3DSCENE *sce );
G3DCHANNEL *g3dmaterial_getChannelByID ( G3DMATERIAL *mat, 
                                         uint32_t     id );

/******************************************************************************/
/*void  g3drttriangle_getposition ( G3DRTTRIANGLE *, G3DDOUBLEVECTOR * );
void  g3drttriangle_getnormal   ( G3DRTTRIANGLE *, G3DDOUBLEVECTOR * );*/

/******************************************************************************/
G3DBONE *g3dbone_new                    ( uint32_t, char *, float );
void     g3dbone_free                   ( G3DOBJECT * );
uint32_t g3dbone_draw ( G3DOBJECT *obj, 
                        G3DCAMERA *curcam, 
                        uint64_t   engine_flags );
LIST    *g3dbone_seekMeshHierarchy      ( G3DBONE * );
LIST    *g3dbone_seekWeightGroups       ( G3DBONE * );
G3DRIG  *g3dbone_seekRig                ( G3DBONE *, G3DWEIGHTGROUP * );
void     g3dbone_removeWeightGroup      ( G3DBONE        *bon, 
                                          G3DSKIN        *skn,
                                          G3DWEIGHTGROUP *grp );
G3DRIG  *g3dbone_addWeightGroup         ( G3DBONE        *bon,
                                          G3DSKIN        *skn,
                                          G3DWEIGHTGROUP *grp );
void     g3dbone_fix                    ( G3DBONE *, uint64_t engine_flags );
void     g3dbone_fix_r                  ( G3DBONE *, uint64_t engine_flags );
void     g3dbone_unfix                  ( G3DBONE *, uint64_t engine_flags );
void     g3dbone_unfix_r                ( G3DBONE *, uint64_t engine_flags );
void     g3dbone_update                 ( G3DBONE * );
G3DRIG  *g3dbone_getRigBySkin           ( G3DBONE *bon,
                                          G3DSKIN *skn );
G3DBONE *g3dbone_mirror ( G3DBONE *bon, 
                          uint32_t orientation,
                          uint32_t recurse,
                          uint64_t engine_flags );
LIST    *g3dbone_getVertexList          ( G3DBONE *, G3DMESH * );
void     g3dbone_updateVertices         ( G3DBONE * );
void g3dbone_updateVertexNormals ( G3DBONE *bon, 
                                   uint64_t engine_flags );
void     g3dbone_updateSubdividedFaces  ( G3DBONE *, uint32_t );
void     g3dbone_getMeshes_r            ( G3DBONE *, LIST ** );
void     g3dbone_getMeshExtendedFaces_r ( G3DBONE *, G3DMESH *, LIST ** );
void     g3dbone_anim                   ( G3DOBJECT *, G3DKEY *, G3DKEY * );
LIST    *g3dbone_getMeshHierarchy       ( G3DBONE * );
void     g3dbone_updateEdges            ( G3DBONE * );
void     g3dbone_updateFaces            ( G3DBONE * );
void g3dbone_transform ( G3DBONE *bon, 
                         uint64_t   engine_flags );
G3DWEIGHTGROUP *g3dbone_seekWeightGroupByID ( G3DBONE *, uint32_t );
LIST    *g3dbone_getAllWeightGroups     ( G3DBONE * );

/******************************************************************************/
void            g3dweightgroup_name          ( G3DWEIGHTGROUP *, char * );
void            g3dweightgroup_free          ( G3DWEIGHTGROUP * );
G3DWEIGHTGROUP *g3dweightgroup_new           ( G3DMESH *mes, char * );
void            g3dweightgroup_removeWeight  ( G3DWEIGHTGROUP *, G3DWEIGHT * );
G3DWEIGHTGROUP *g3dmesh_getWeightGroupByID   ( G3DMESH *, uint32_t );

/******************************************************************************/
G3DWEIGHT *g3dweight_new ( float           weight,
                           G3DWEIGHTGROUP *grp );
void g3dweight_free      ( G3DWEIGHT * );
void g3dweight_fix       ( G3DWEIGHT *, G3DRIG * );
void g3dweight_reset     ( G3DWEIGHT *, G3DRIG * );

/******************************************************************************/
void    g3drig_free  ( G3DRIG * );
G3DRIG *g3drig_new   ( G3DSKIN *skn );
void    g3drig_fix   ( G3DRIG *rig );
void    g3drig_unfix ( G3DRIG *rig );

/******************************************************************************/
G3DTEXTURE *g3dtexture_new           ( G3DOBJECT   *obj,
                                       G3DMATERIAL *mat, 
                                       G3DUVMAP    *map );
G3DTEXTURE *g3dtexture_getFromUVMap  ( LIST *, G3DUVMAP * );
void        g3dtexture_unsetSelected ( G3DTEXTURE * );
void g3dtexture_restrictFacegroup ( G3DTEXTURE *tex, G3DFACEGROUP *facgrp );
void g3dtexture_unrestrictFacegroup ( G3DTEXTURE *tex, G3DFACEGROUP *facgrp );
void g3dtexture_restrict ( G3DTEXTURE *tex );
void g3dtexture_unrestrict ( G3DTEXTURE *tex );
void g3dtexture_unrestrictAllFacegroups ( G3DTEXTURE *tex );
void g3dtexture_free ( G3DTEXTURE *tex );

/******************************************************************************/
G3DUVSET  *g3duvset_new                  ( G3DUVMAP * );
void g3duvset_free ( G3DUVSET *uvset );
void g3duvset_mapFaceWithBackgroundProjection ( G3DUVSET *uvs, 
                                                G3DFACE  *fac,
                                                uint64_t engine_flags );
void       g3duvmap_mapFace              ( G3DUVMAP *, G3DMESH *, G3DFACE * );
void       g3duvmap_adjustFlatProjection ( G3DUVMAP *, G3DMESH *mes );
void       g3duvmap_applyProjection      ( G3DUVMAP *, G3DMESH *mes );
uint32_t g3duvmap_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam, 
                         uint64_t   engine_flags );
void       g3duvmap_init                 ( G3DUVMAP *, char *, uint32_t );
G3DUVMAP  *g3duvmap_new                  ( char *, uint32_t );
void       g3duvmap_free                 ( G3DOBJECT * );
G3DUV     *g3duv_new                     ( G3DUVSET * );
void       g3duvmap_unfix                ( G3DUVMAP * );
void       g3duvmap_fix                  ( G3DUVMAP * );
void       g3duvset_subdivide            ( G3DUVSET *, G3DFACE * );
void       g3duvmap_insertFace           ( G3DUVMAP *, G3DMESH *, G3DFACE * );
void       g3duvmap_addMaterial          ( G3DUVMAP *, G3DMATERIAL * );
void       g3duvmap_removeMaterial       ( G3DUVMAP *, G3DMATERIAL * );
G3DMATERIAL *g3dscene_getMaterialByID      ( G3DSCENE *sce, uint32_t id );
uint32_t g3duvmap_isFixed ( G3DUVMAP *map );
void g3duvmap_selectUV ( G3DUVMAP *uvmap, G3DUV *uv );
void g3duvmap_unselectUV ( G3DUVMAP *uvmap, G3DUV *uv );
void g3duvmap_unselectAllUVs ( G3DUVMAP *uvmap );
uint32_t g3duv_copyUVFromList ( LIST *luv, G3DUV **curuv );
uint32_t g3duv_getAverageFromList ( LIST *luv, G3DUV *uvout );

void g3duvmap_unselectAllUVSets ( G3DUVMAP *uvmap );
void g3duvmap_selectUVSet ( G3DUVMAP *uvmap, G3DUVSET *uvset );
void g3duvmap_unselectUVSet ( G3DUVMAP *uvmap, G3DUVSET *uvset );
void g3duvset_unsetSelected ( G3DUVSET *uvset );
LIST *g3duvset_getUVsFromList ( LIST *luvset );

/******************************************************************************/
void      g3dpivot_free ( G3DOBJECT * );
uint32_t g3dpivot_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam, 
                         uint64_t   engine_flags );
void g3dpivot_init ( G3DPIVOT  *piv, 
                     G3DCAMERA *cam, 
                     G3DCURSOR *csr, 
                     uint64_t engine_flags );
G3DPIVOT *g3dpivot_new ( G3DCAMERA *cam, 
                         G3DCURSOR *csr, 
                         uint64_t   engine_flags );
void      g3dpivot_orbit ( G3DPIVOT *, float, float );

/******************************************************************************/
/*
void          g3dheightmap_realloc ( G3DHEIGHTMAP *, uint32_t );
G3DHEIGHTMAP *g3dheightmap_new     ( uint32_t );
*/

uint32_t *g3dsubindex_get ( uint32_t, uint32_t );

/******************************************************************************/
G3DRTTRIANGLEUVW *g3drttriangleuvw_new ( float, float,
                                         float, float,
                                         float, float, G3DUVMAP * );
void g3drttriangle_addUVW ( G3DRTTRIANGLE *, G3DRTTRIANGLEUVW * );
void g3drttriangleuvw_free ( G3DRTTRIANGLEUVW * );

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_new ( uint32_t id, 
                                   char    *name, 
                                   uint64_t engine_flags );
void g3dsubdivider_init ( G3DSUBDIVIDER *sdr, 
                          uint32_t       id, 
                          char          *name, 
                          uint64_t       engine_flags );
void           g3dsubdivider_unsetSyncSubdivision ( G3DSUBDIVIDER * );
void           g3dsubdivider_setSyncSubdivision   ( G3DSUBDIVIDER * );
void g3dsubdivider_setLevels ( G3DSUBDIVIDER *sdr, 
                               uint32_t       preview,
                               uint32_t       render,
                               uint64_t       engine_flags );
void g3dsubdivider_fillBuffers ( G3DSUBDIVIDER *sdr,
                                 LIST          *lfac,
                                 uint64_t       engine_flags );
void g3dsubdivider_allocBuffers ( G3DSUBDIVIDER *sdr, 
                                  uint64_t       engine_flags );
void g3dsubdivider_setSculptResolution ( G3DSUBDIVIDER *sdr,
                                         uint32_t       sculptResolution );
void g3dsubdivider_setSculptMode ( G3DSUBDIVIDER *sdr,
                                   uint32_t       sculptMode,
                                   uint64_t       engine_flags );
uint32_t g3dsubdivider_hasScultMaps ( G3DSUBDIVIDER *sdr );

/******************************************************************************/
void g3dprocedural_init ( G3DPROCEDURAL *,
                          uint32_t       ,
                          uint32_t       ,
                          void         (*)( G3DPROCEDURAL *, 
                                            double, 
                                            double, 
                                            double, 
                                            G3DCOLOR * ) );
void g3dprocedural_fill ( G3DPROCEDURAL *, uint32_t,
                                           uint32_t,
                                           uint32_t, uint32_t );
void g3dprocedural_getNormal ( G3DPROCEDURAL *proc,
                               float          u,
                               float          v,
                               G3DVECTOR     *nor,
                               float          precU,
                               float          precV,
                               uint32_t       fromBuffer );

/******************************************************************************/
uint32_t g3dmodifier_moddraw ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               uint64_t     engine_flags );
uint32_t g3dmodifier_modpick ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               uint64_t     engine_flags );

void g3dmodifier_setParent ( G3DMODIFIER *mod, 
                             G3DOBJECT   *parent,
                             G3DOBJECT   *oldParent,
                             uint64_t     engine_flags );

void g3dmodifier_init ( G3DMODIFIER *mod,
                        uint32_t     type,
                        uint32_t     id,
                        char        *name,
                        uint32_t     object_flags,
                        uint32_t   (*Draw)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint64_t ),
                        void       (*Free)        ( G3DOBJECT * ),
                        uint32_t   (*Pick)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint64_t ),
                        void       (*Pose)        ( G3DOBJECT *, G3DKEY * ),
                        G3DOBJECT* (*Copy)        ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint64_t ),
                        void       (*Activate)    ( G3DOBJECT *, uint64_t ),
                        void       (*Deactivate)  ( G3DOBJECT *, uint64_t ),
                        G3DOBJECT* (*Commit)      ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint64_t ),
                        void       (*AddChild)    ( G3DOBJECT *, G3DOBJECT *,
                                                                 uint64_t ),
                        void       (*SetParent)   ( G3DOBJECT *, 
                                                    G3DOBJECT *,
                                                    G3DOBJECT *, 
                                                    uint64_t ),
                        uint32_t   (*Modify)      ( G3DMODIFIER *, 
                                                    G3DMODIFYOP,
                                                    uint64_t ) );
uint32_t g3dmodifier_draw ( G3DMODIFIER *mod,
                            G3DCAMERA   *cam, 
                            uint64_t     engine_flags );
G3DMODIFIER *g3dmodifier_modify_r ( G3DMODIFIER *mod,
                                    G3DOBJECT   *oriobj,
                                    G3DVECTOR   *verpos,
                                    G3DVECTOR   *vernor,
                                    G3DMODIFYOP  op,
                                    uint64_t     engine_flags );
uint32_t g3dmodifier_pick ( G3DMODIFIER *mod,
                            G3DCAMERA   *cam, 
                            uint64_t     engine_flags );
void g3dmodifier_modifyChildren ( G3DMODIFIER *mod,
                                  G3DMODIFYOP  op,
                                  uint64_t     engine_flags );
uint32_t g3dmodifier_default_dump ( G3DMODIFIER *mod, 
                                    void (*Alloc)( uint32_t, /*nbver */
                                                   uint32_t, /* nbtris */
                                                   uint32_t, /* nbquads */
                                                   uint32_t, /* nbuv */
                                                   void * ),
                                    void (*Dump) ( G3DFACE *,
                                                   G3DVECTOR *,
                                                   G3DVECTOR *,
                                                   void * ),
                                    void *data,
                                    uint64_t engine_flags );

/******************************************************************************/
G3DWIREFRAME *g3dwireframe_new          ( uint32_t, char * );
void g3dwireframe_setThickness ( G3DWIREFRAME *wir, 
                                 float         thickness,
                                 uint64_t      engine_flags );

/******************************************************************************/
void g3dchannel_getColor ( G3DCHANNEL *, float, float, G3DRGBA *, uint32_t );
void g3dchannel_enableProcedural ( G3DCHANNEL *cha );
void g3dchannel_enableImageColor ( G3DCHANNEL *cha );
void g3dchannel_enableSolidColor ( G3DCHANNEL *cha );
G3DPROCEDURAL *g3dchannel_setProcedural ( G3DCHANNEL    *cha, 
                                          G3DPROCEDURAL *proc );
void g3dchannel_getBumpVector ( G3DCHANNEL *cha,
                                G3DVECTOR  *ray,
                                float       u,
                                float       v,
                                G3DVECTOR  *vout );
void g3dchannel_getNormal ( G3DCHANNEL *cha, 
                            float       u,
                            float       v,
                            G3DVECTOR  *nor,
                            uint32_t    repeat,
                            float       precU,
                            float       precV,
                            uint32_t    fromBuffer );

/******************************************************************************/
G3DPROCEDURALNOISE    *g3dproceduralnoise_new ( );
G3DPROCEDURALCHESS    *g3dproceduralchess_new ( );
G3DPROCEDURALBRICK    *g3dproceduralbrick_new ( );
G3DPROCEDURALGRADIENT *g3dproceduralgradient_new ( );
void g3dproceduralnoise_buildGradients ( G3DPROCEDURALNOISE *noise,
                                         uint32_t            nbGradientX,
                                         uint32_t            nbGradientY );
void g3dproceduralbrick_copySettings ( G3DPROCEDURALBRICK *brick, 
                                       G3DPROCEDURALBRICK *pout );
void g3dproceduralchess_copySettings ( G3DPROCEDURALCHESS *chess, 
                                       G3DPROCEDURALCHESS *pout );
void g3dproceduralnoise_copySettings ( G3DPROCEDURALNOISE *chess, 
                                       G3DPROCEDURALNOISE *pout );
void g3dproceduralnoise_copySettings ( G3DPROCEDURALNOISE *noise, 
                                       G3DPROCEDURALNOISE *pout );
void g3dproceduralgradient_copySettings ( G3DPROCEDURALGRADIENT *noise, 
                                          G3DPROCEDURALGRADIENT *pout );

/******************************************************************************/
void g3dfacegroup_addTextureSlot ( G3DFACEGROUP *facgrp,
                                   uint32_t      slotBit );
void g3dfacegroup_removeTextureSlot ( G3DFACEGROUP *facgrp, 
                                      uint32_t      slotBit );
G3DFACEGROUP *g3dfacegroup_new ( const char *name, LIST *lfac );
void g3dfacegroup_unsetSelected ( G3DFACEGROUP *facgrp );
void g3dfacegroup_free ( G3DFACEGROUP *facgrp );

#endif

