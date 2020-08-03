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
void common_g3duipentooledit_setRadiusCbk ( G3DUIUVMAPEDITOR *uvme, 
                                            float             radius ) {
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    if ( uvme->gui->lock ) return;

    l3dpattern_resize ( sysinfo->pattern, radius );
}

/******************************************************************************/
void common_g3duipentooledit_setPressureCbk ( G3DUIUVMAPEDITOR *uvme, 
                                              float             pressure ) {
    L3DMOUSETOOL *mou = common_g3dui_getMouseTool ( uvme->gui, PENTOOL );

    if ( uvme->gui->lock ) return;

    if ( mou ) {
        mou->obj->pressure = pressure;
    }
}
