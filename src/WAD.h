#pragma once

struct wad_info_t {
	char magic[4];
	int numlumps;
	int infotableofs;
};

struct wad_file_lump_t {
	int filepos;
	int size;
	char name[8];
};

// loads a doom wad file from disk.
bool WADLoadFromFile(const char* filename);

#define ML_LABEL		0
#define ML_THINGS		1
#define ML_LINEDEFS		2
#define ML_SIDEDEFS		3
#define ML_VERTEXES		4
#define ML_SEGS			5
#define ML_SSECTORS		6
#define ML_NODES		7
#define ML_SECTORS		8
#define ML_REJECT		9
#define ML_BLOCKMAP		10
#define ML_GL_LABEL		11
#define ML_GL_VERT		12
#define ML_GL_SEGS		13
#define ML_GL_SSECT		14
#define ML_GL_NODES		15
#define ML_GL_PVS		16

struct mvertex_t {
	short x;
	short y;
};

struct msidedef_t {
	short texofs;
	short rowofs;
	char toptex[8];
	char bottomtex[8];
	char midtex[8];
	short sector;
};

struct mlinedef_t {
	short v1;
	short v2;
	short flags;
	short special;
	short tag;
	short sidenum[2];
};

struct msector_t {
	short floorheight;
	short ceilheight;
	char floortex[8];
	char ceiltex[8];
	short lightlevel;
	short special;
	short tag;
};

struct msubsector_t {
	short numsegs;
	short firstseg;
};

struct mseg_t {
	short v1;
	short v2;
	short angle;
	short linedef;
	short side;
	short offset;
};

#define	NF_SUBSECTOR 0x8000

struct mnode_t {
	short x;
	short y;
	short dx;
	short dy;
	short bbox[2][4];
	unsigned short children[2];
};

struct mthing_t {
	short x;
	short y;
	short angle;
	short type;
	short options;
};

struct glvertex_t {
	int x;
	int y;
};

struct glseg_t {
	unsigned short v1;
	unsigned short v2;
	unsigned short linedef;
	unsigned short side;
	unsigned short partnerseg;
};

struct glsubsector_t {
	unsigned short count;
	unsigned short firstseg;
};

struct glnode_t {
	short x;
	short y;
	short dx;
	short dy;
	short bbox[2][4];
	unsigned short children[2];
};

extern mvertex_t* mvertices;
extern int numvertices;

extern msidedef_t* msidedefs;
extern int numsidefs;

extern mlinedef_t* mlinedefs;
extern int numlinedefs;

extern msector_t* msectors;
extern int numsectors;

extern msubsector_t* mssectors;
extern int numssectors;

extern mseg_t* msegs;
extern int numsegs;

extern glvertex_t* glvertices;
extern int numglvertices;

extern glseg_t* glsegs;
extern int numglsegs;

extern glsubsector_t* glssectors;
extern int numglssectors;

extern glnode_t* glnodes;
extern int numglnodes;

void MapLoadLevel(const char* name);