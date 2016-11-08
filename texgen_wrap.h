#ifndef TEXGEN_WRAP_H_
#define TEXGEN_WRAP_H_

#include "3deng_dx8/3deng.h"
#include "imak/imaktgen.h"
#include <cassert>

class TexGen {
public:
	ScalarData sdata;
	TextureGenerator tgen;
	TextureData tdata;

	int CreateTexArray(GraphicsContext *gc, Texture **&tex_array);
};

#endif	// TEXGEN_WRAP_H_