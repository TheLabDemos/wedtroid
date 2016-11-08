#include "tourta_part.h"
#include "texgen_wrap.h"

Texture **tex_array;
int frames;
Color back_color;

Firework::Firework(GraphicsContext *gc, Curve *path, int fl_time, int expl_time, const Color &base_col) {
	flight_dur = fl_time;
	expl_dur = expl_time;

	tail = new ParticleSystem(gc);
	tail->SetShootDirection(Vector3(0, 0, 0));
	tail->SetMaxDispersionAngle(QuarterPi/2.0);
	tail->SetSpawningDifferenceDispersion(1.0);
	tail->SetGravitualForce(1.0f);
	tail->SetFriction(0.01f);
	tail->SetInitialColor(1.0, 1.0, 0.2);
	tail->SetDeathColor(0.1, 0.02, 0.01);
	tail->SetParticleLife(60);
	tail->SetParticleSize(30.0f);
	tail->SetSpawnRadius(15.0f);
	tail->SetSpawnRate(20.0f);
	tail->SetSpawnRateChange(-0.4f);
	tail->SetTexture(gc->texman->LoadTexture("data/star.jpg"));

	explosion = new ParticleSystem(gc);
	explosion->SetPosition(path->Interpolate(0.99));
	explosion->SetShootDirection(Vector3(0, 10.0f, 0));
	explosion->SetMaxDispersionAngle(TwoPi);
	explosion->SetSpawningDifferenceDispersion(1.0);
	explosion->SetGravitualForce(2.0f);
	explosion->SetFriction(0.06f);
	explosion->SetInitialColor(base_col.r, base_col.g, base_col.b);
	explosion->SetDeathColor(0.0, 0.0, 0.0);
	explosion->SetParticleLife(80);
	explosion->SetParticleSize(30.0f);
	explosion->SetSpawnRadius(15.0f);
	explosion->SetSpawnRate(130.0f);
	explosion->SetSpawnRateChange(-10.0f);
	explosion->SetTexture(gc->texman->LoadTexture("data/star.jpg"));

	this->path = path;
	//path->SetArcParametrization(true);
}

Firework::~Firework() {
	delete tail;
	delete explosion;
}

void Firework::Start(int msec) {
	start_time = msec;
}

void Firework::Update(int msec) {
	float t = (float)(msec - start_time) / (float)flight_dur;

	tail->SetPosition(path->Interpolate(t));
	tail->Update((float)msec / 1000.0f);
	tail->Render();

	if(msec >= start_time + flight_dur) {
		tail->SetSpawnRateChange(-8);
		explosion->Update((float)(msec - flight_dur) / 1000.0f);
		explosion->Render();
	}
	
}	

#define TEXSIZE 128
#define TEXFRAMES 200

TourtaPart::TourtaPart(GraphicsContext *gc) {
	this->gc = gc;

	scene = 0;

	back_color = Color(150, 177, 255);

	SceneLoader::SetGraphicsContext(gc);
	SceneLoader::SetDataPath("data/");
	SceneLoader::LoadScene("data/tourta.3ds", &scene);

	cam = scene->GetActiveCamera();
	Curve *path = scene->GetCurve("Path01");
	Curve *tpath = scene->GetCurve("TargPath");
	path->SetArcParametrization(true);
	cam->SetCameraPath(path, tpath, 0, 36000);

	sky = scene->GetObject("Sphere");

	// Texture generation
	TexGen tgen;
	ScalarData crp2;
	tgen.sdata.build(TEXSIZE, TEXSIZE, TEXFRAMES, 25.0f);
	//tgen.tgen.perlin_gn__(tgen.sdata, 2.0f, 7.0f, 2.0f, 0.83f, 1.0f, 0);

	tgen.tgen.perlin_vn(tgen.sdata, 2.0f, 3.0f, 2.0f, 0.85f, 1.0f, 0);
	tgen.sdata.slaughter(1.0f, 0.5f);
	tgen.sdata.normalize();
	crp2.build(TEXSIZE, TEXSIZE, TEXFRAMES, 25.0f);
	tgen.tgen.perlin_gn__(crp2, 2.0f, 8.0f, 2.0f, 0.7f, 1.0f, 0);
	tgen.sdata.shift(crp2, 0.1f, 0.1f, 0.1f);
	crp2.demolish();
	tgen.tdata.build(tgen.sdata, White, Black);

	tgen.sdata.demolish();

	frames = tgen.CreateTexArray(gc, tex_array);

	memset(fireworks, 0, 4 * sizeof(Firework*));

	heart = scene->GetObject("Heart");
	heart_rot = heart->RotMat;

	text = scene->GetObject("Text");
}

TourtaPart::~TourtaPart() {
	delete scene;
}

void TourtaPart::Launch() {
	timer.Start();
}

void TourtaPart::MainLoop() {
	cam->CreateCameraMatrix();
	gc->SetViewMatrix(cam->GetCameraMatrix());

	dword msec = timer.GetMilliSec();
	float t = (float)msec / 1000.0f;

	int choose = (int)t % 12;
	switch(choose) {
	case 0:
	case 3:
	case 6:
	case 9:
		RespawnFirework(choose / 3);
	default:
		break;
	}

	if(msec < 6800) {
		cam->FollowPath((dword)0);
		if(msec > 4000) {
			text->material.SetAlpha((6.8f - t) / 2.8f);
		}
	} else if(msec < 42800) {
        cam->FollowPath(msec - 6800);
	} else {
		cam->SetCameraPath(scene->GetCurve("CPath2"), scene->GetCurve("CTarg2"), 42800, 45000);
		cam->FollowPath(msec);
	}

	sky->material.SetTexture(tex_array[(int)(t*30.0f) % frames], TextureMap);

	heart->ResetRotation();
	heart->Rotate(heart_rot);
	heart->Rotate(0, t, 0);

	scene->Render();

	msec = timer.GetMilliSec();
	for(int i=0; i<4; i++) {
        if(fireworks[i]) fireworks[i]->Update(msec);
	}
}

void TourtaPart::RespawnFirework(int pathnum) {
	if(fireworks[pathnum]) {
		delete fireworks[pathnum];
	}

	char pathname[] = "PPath0x";
	pathname[6] = '1' + pathnum;

	fireworks[pathnum] = new Firework(gc, scene->GetCurve(pathname), 4000, 2000, Color(1.0f, 0.7f, 0.2f));
	fireworks[pathnum]->Start(timer.GetMilliSec());
}