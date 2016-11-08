#ifndef TOURTA_PART_H_
#define TOURTA_PART_H_

#include "demosystem/demosys.h"

extern Texture **tex_array;
extern int frames;
extern Color back_color;

class Firework {
private:
	ParticleSystem *tail, *explosion;
	Curve *path;
	int flight_dur, expl_dur, start_time;

public:
	Firework(GraphicsContext *gc, Curve *path, int fl_time, int expl_time, const Color &base_col);
	~Firework();

	void Start(int msec);

	void Update(int msec);
};

class TourtaPart : public Part {
private:

	Firework *fireworks[4];
	Object *sky, *heart, *text;
	Matrix4x4 heart_rot;

	Camera *cam;

	void RespawnFirework(int pathnum);

public:

	TourtaPart(GraphicsContext *gc);
	~TourtaPart();

	virtual void Launch();
	virtual void MainLoop();
};

#endif	// TOURTA_PART_H_