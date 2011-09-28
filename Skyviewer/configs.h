#ifndef CONFIGS_H
#define CONFIGS_H

#define COLOR_PER_NSIDE false
#define DISPLAY_TEXTURE true
#define CACHE_DIR "cache"
// number of histogram bins
#define NUMBER_BINS 2094
#define DEBUG 0
#define GRID_LINES 6



// mapviewer
#define MIN_NSIDE 64
#define MAX_NSIDE 2048
#define EXP_NSIDE 6
#define MIN_ZOOM 0
#define MAX_ZOOM 10
#define ZOOM_LEVEL 2
#define INVISIBLE_X 0.10//0.37//0.25    // last 0.37
#define CAMERA_ZOOM_INC 0.16
#define PRELOAD_FACES false
#define INITIAL_CAMERA_POSITION 2.8


// viewportManager
// max number of viewports open
#define MAXVIEWPORTS 20

// workspace
// max viewports that can be displayed in workspace
#define MAX_WORKSPACE_VIEWPORTS 16


// util
#define BUFFER_OFFSET(i) ((GLbyte *)NULL + (i))

#endif // CONFIGS_H
