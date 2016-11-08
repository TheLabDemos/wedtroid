#include "scroller_part.h"
#include "tourta_part.h"

ScrollerPart::ScrollerPart(GraphicsContext *gc) {
	this->gc = gc;

	cam = new Camera;
	cam->SetCamera(Vector3(0, 0, -1), Vector3(0, 0, 1), VECTOR3_J);

	SceneLoader::SetGraphicsContext(gc);
	SceneLoader::SetDataPath("data/");

	for(int i=0; i<3; i++) {
		clouds[i].quad = new Object(gc);
		SceneLoader::LoadObject("data/stefania.3ds", "Plane01", &clouds[i].quad);
		clouds[i].z = 8.0f + (i << 2);
		clouds[i].tex_index = (i * 20) % frames;
	}

	DirLight light(VECTOR3_K);
	light.SetLight(0, gc);
	gc->D3DDevice->LightEnable(1, 0);
	gc->D3DDevice->LightEnable(2, 0);
	gc->D3DDevice->LightEnable(3, 0);

	SceneLoader::LoadObject("data/stefania.3ds", "Stefania", &stefania);

	SceneLoader::LoadObject("data/stefania.3ds", "Starfield", &stars);

	text = new Object(gc);
	text->CreatePlane(3.0f, 0);
	text->Scale(1.33333f, 1.0f, 1.0f);

	tex1 = gc->texman->LoadTexture("data/scroller.png");
	tex2 = gc->texman->LoadTexture("data/scroller_falloff.png");
}

ScrollerPart::~ScrollerPart() {
	delete cam;
	for(int i=0; i<3; i++) {
        delete clouds[i].quad;
	}
	delete stefania;
	delete text;
}

static float prev_t = 0;
const float fps = 30.0f;
const float zdisp = 0.1f;
const float max_cloud_z = 12.0f;
float prev_index_update[3] = {0, 0, 0};
const Color start_col(150, 177, 255);
const Color end_col(0, 0, 0);

void ScrollerPart::MainLoop() {
	cam->CreateCameraMatrix();
	gc->SetViewMatrix(cam->GetCameraMatrix());

	dword msec = timer.GetMilliSec();
	float t = (float)msec / 1000.0f;

	gc->ClearZBufferStencil(1.0f, 0);

	back_color = BlendColors(start_col, end_col, min((t / 30.0f), 1.0f));
	
	stars->material.SetAlpha(min(t / 30.0f, 0.9f) + sin(t * 1.3f) * 0.1f);
	stars->Render();
	
	// animate & render the stefania
	float x = (sin(t) + cos(t/2) * 2 + sin(t*2) / 2) * 0.7;
	float y = (cos(t) + sin(t*3) / 3 + cos(t/3) * 3) * 0.4;
	if(t <= 2.0f) {
        stefania->material.SetAlpha(max(min((t-0.5f) * 0.75f, 1.0f), 0.0f));
	} else {
		stefania->material.SetAlpha(1.0f);
	}
	stefania->ResetTransform();
	stefania->Rotate(t, 0, t);
	stefania->Translate(x, y, 3);
	stefania->Render();

	// animate & render the cloud planes
	for(int i=0; i<3; i++) {
		if(t - prev_index_update[i] > 1.0f / 30.0f) {
            clouds[i].tex_index = (clouds[i].tex_index + (int)((t - prev_index_update[i]) * fps)) % frames;
			prev_index_update[i] = t;
		}
		clouds[i].quad->material.SetTexture(tex_array[clouds[i].tex_index], TextureMap);
		if(clouds[i].z < 0.0f) clouds[i].z = max_cloud_z;
		clouds[i].z -= zdisp * ((t - prev_t) * fps);
		clouds[i].quad->material.SetAlpha(sin(Pi * clouds[i].z / max_cloud_z));
		clouds[i].quad->ResetTranslation();
		clouds[i].quad->Translate(0, 0, clouds[i].z);
		clouds[i].quad->Render();
	}

	text->ResetTranslation();
	text->Translate(0, 0, 1.2f);

    text->SetRenderStates();
	gc->SetTexture(0, tex1);
	gc->SetTexture(1, tex2);
	Matrix4x4 trans(1, 0, 0, 0,
					0, 1, 0, 0,
					0, t/100.0f, 1, 0,
					0, 0, 0, 1);
	Matrix4x4 scale, mat;
	scale.Scale(1, 1.0f/3.0f, 1);
	mat = scale * trans;

	gc->SetTextureMatrix(mat, 0);
	gc->D3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	gc->SetTextureCoordIndex(0, 0);
	gc->SetTextureCoordIndex(1, 1);
	
	gc->SetTextureStageColor(0, TexBlendModulate, TexArgTexture, TexArgCurrent);
	gc->SetTextureStageColor(1, TexBlendModulate, TexArgCurrent, TexArgTexture);

	gc->SetAlphaBlending(true);
	gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);

	text->RenderBare();

	gc->SetTextureMatrix(Matrix4x4(), 0);
	gc->SetTextureTransformState(0, TexTransformDisable);

	prev_t = t;

}