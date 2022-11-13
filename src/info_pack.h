#ifndef INFO_PACK_H
#define INFO_PACK_H
#pragma once

#include "Utilities/Pnt3f.H"
#include <GL/glu.h>

class InfoPack {
private:
	bool visible;
	Pnt3f Pos, Size, Front, Top;
	GLubyte color[4];
public:
	InfoPack();
	explicit InfoPack(const Pnt3f& pos);
	InfoPack(const Pnt3f& pos, const Pnt3f& size);
	InfoPack(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top);
	InfoPack(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top, GLubyte color[4]);

	bool isVisible() const;

    void setVisible(bool visible);

    const Pnt3f& getPos() const;

	void setPos(const Pnt3f& pos);

	const Pnt3f& getSize() const;

	void setSize(const Pnt3f& size);

	const Pnt3f& getFront() const;

	void setFront(const Pnt3f& front);

	const Pnt3f& getTop() const;

	void setTop(const Pnt3f& top);

	const GLubyte* getColor() const;
	
	void setColor(const GLubyte* color);

};

#endif