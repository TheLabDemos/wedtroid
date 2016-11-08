#include "color.h"
#include "texgen_wrap.h"

int TexGen::CreateTexArray(GraphicsContext *gc, Texture **&tex_array) {

	tex_array = new Texture*[tdata.frames_];

	for(int i=0; i<tdata.frames_; i++) {
        Surface *surf;

		Texture *tmp = gc->texman->CreateTexture(tdata.width_, tdata.height_, 0, true, true);
		tmp->GetSurfaceLevel(0, &surf);

		dword *ptr, offs;
		D3DLOCKED_RECT d3dlock;
		assert(surf->LockRect(&d3dlock, 0, 0) == D3D_OK);
		ptr = (dword*)d3dlock.pBits;
		offs = (d3dlock.Pitch >> 2) - tdata.width_;

		for(int y=0; y<tdata.height_; y++) {
			for(int x=0; x<tdata.width_; x++) {
				IColor icol = tdata.data_[(y * tdata.width_ + x) + tdata.fsize_ * i];

				Color col(255, 255, 255, icol.Red);
				*ptr++ = col.GetPacked32();
			}
			ptr += offs;
		}

		surf->UnlockRect();

		Texture *tex = gc->texman->CreateTexture(tdata.width_, tdata.height_, 0, true);
		gc->D3DDevice->UpdateTexture(tmp, tex);
		UpdateMipmapChain(tex);	
		tmp->Release();

		tex_array[i] = tex;
	}

	return tdata.frames_;
}