#ifndef CONFIGS_H
#define CONFIGS_H

#define COLOR_PER_NSIDE false
#define DISPLAY_TEXTURE true
#define CACHE_DIR "cache"
// number of histogram bins
#define NUMBER_BINS 2094
#define DEBUG 0
#define GRID_LINES 6



// min and max nside for tesselation
#define TESSELATION_MIN_NSIDE 32
#define TESSELATION_MAX_NSIDE 256
#define TESSELATION_DEFAULT_NSIDE 64


#define CAMERA_3D_MAX_X 1.02
#define CAMERA_MOLL_MAX_X 0.03

// mapviewer
#define MIN_NSIDE 64
#define MAX_NSIDE 2048
#define EXP_NSIDE 6
#define MIN_ZOOM 0
//3D
//#define MAX_ZOOM 10
//Mollweide
#define MAX_ZOOM 30//17
#define ZOOM_LEVEL 2
#define INVISIBLE_X 0.10//0.37//0.25    // last 0.37
#define CAMERA_ZOOM_INC 0.05//0.16
#define PRELOAD_FACES false
#define DISPLAY_ONLY_VISIBLE_FACES false

#define SPHERE_VISIBLE 0.25
#define SPHERE_PROPORTION 0.215
#define CHANGE_VECTORS_WITH_NSIDE true

#define AUTO_TEXTURE_NSIDE true
#define AUTO_PVECTORS_NSIDE true
#define PVECTORS_NSIDE_FACTOR 1

#define DEFAULT_VECTORS_SPACING 0
#define MIN_VECTORS_SPACING 0
#define MAX_VECTORS_SPACING 8

#define MIN_MOUSE_SENSITIVITY 0.1


#define COLOR_SELECTED "#fa8252"
#define COLOR_INACTIVE "#c3c3c3"
#define DEFAULT_VIEW_3D true
#define FIT_CONTENT_RESIZE true

#define DEFAULT_SENTINEL_COLOR "#c3c3c3"

// viewportManager
// max number of viewports open
#define MAXVIEWPORTS 20

// workspace
// max viewports that can be displayed in workspace
#define MAX_WORKSPACE_VIEWPORTS 16


// util
#define BUFFER_OFFSET(i) ((GLbyte *)NULL + (i))


// cache maxtiles
#define OVERLAYCACHE_MAX_TILES 12500//14000//59//10000//12500
#define TEXTURECACHE_MAX_TILES 10000//12500//59//10000//12500
#define FACECACHE_MAX_TILES 10000//12500//14000//59//10000//12500


// face
#define GPU_BUFFER true


#define REALTIME_TEXTURES false

#define DEFAULT_COLOR_MAP 4


#endif // CONFIGS_H
