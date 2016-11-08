#ifndef SCROLLER_PART_H_
#define SCROLLER_PART_H_

#include "demosystem/demosys.h"

struct CloudPlane {
	Object *quad;
	float z;
	int tex_index;
};

class ScrollerPart : public Part {
private:

	Object *stefania;
	Object *text;
	Object *stars;
	CloudPlane clouds[3];

	Texture *tex1, *tex2;

	Camera *cam;

public:
	ScrollerPart(GraphicsContext *gc);
	~ScrollerPart();

	virtual void MainLoop();
};

#endif	// SCROLLER_PART_H_