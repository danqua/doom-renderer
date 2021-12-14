#include "WAD.h"
#include <fstream>
#include <string>

std::string reload_name;
wad_file_lump_t* lumps;
int numlumps;

mvertex_t* mvertices;
int numvertices;

msidedef_t* msidedefs;
int numsidefs;

mlinedef_t* mlinedefs;
int numlinedefs;

msector_t* msectors;
int numsectors;

msubsector_t* mssectors;
int numssectors;

mseg_t* msegs;
int numsegs;

glvertex_t* glvertices;
int numglvertices;

glseg_t* glsegs;
int numglsegs;

glsubsector_t* glssectors;
int numglssectors;

glnode_t* glnodes;
int numglnodes;

int GetLumpFromName(const char* name)
{
	wad_file_lump_t* lump = &lumps[numlumps - 1];
	std::string name_str(name, 8);
	while (lump-- != lumps)
	{
		std::string lump_name(lump->name, 8);
		if (name_str == lump_name)
		{
			return static_cast<int>(lump - lumps);
		}
	}
	return -1;
}

bool WADLoadFromFile(const char* filename)
{	
	std::ifstream fs(filename, std::ios::binary);

	if (!fs.is_open())
		return false;

	wad_info_t header;
	fs.read((char*)&header, sizeof(wad_info_t));
	
	numlumps = header.numlumps;
	lumps = new wad_file_lump_t[numlumps];
	fs.seekg(header.infotableofs, std::ios::beg);
	fs.read((char*)lumps, sizeof(wad_file_lump_t) * numlumps);
	
	fs.close();

	reload_name = std::string(filename);
	
	return true;
}

template <typename T>
int LoadLump(T*& buffer, int lump)
{	
	std::ifstream fs(reload_name, std::ios::binary);

	int bytes_to_read = lumps[lump].size;
	int filepos = lumps[lump].filepos;

	char magic[4] = {};
	fs.seekg(filepos, std::ios::beg);
	fs.read(magic, 4);

	if (!strncmp(magic, "gNd2", 4))
	{
		bytes_to_read -= 4;
		filepos += 4;
	}

	buffer = (T*)malloc(bytes_to_read);
	fs.seekg(filepos, std::ios::beg);
	fs.read((char*)buffer, bytes_to_read);

	fs.close();

	return static_cast<int>(bytes_to_read / sizeof(T));
}

void MapLoadLevel(const char* name)
{
	int lump = GetLumpFromName(name);

	numvertices		= LoadLump(mvertices,	lump + ML_VERTEXES);
	numsidefs		= LoadLump(msidedefs,	lump + ML_SIDEDEFS);
	numlinedefs		= LoadLump(mlinedefs,	lump + ML_LINEDEFS);
	numsectors		= LoadLump(msectors,	lump + ML_SECTORS);
	numssectors		= LoadLump(mssectors,	lump + ML_SSECTORS);
	numsegs			= LoadLump(msegs,		lump + ML_SEGS);
	numglvertices	= LoadLump(glvertices,	lump + ML_GL_VERT);
	numglsegs		= LoadLump(glsegs,		lump + ML_GL_SEGS);
	numglssectors	= LoadLump(glssectors, lump + ML_GL_SSECT);
	numglnodes		= LoadLump(glnodes,	lump + ML_GL_NODES);
}