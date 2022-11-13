#ifndef CPU_OBJECT_H
#define CPU_OBJECT_H
#pragma once

#include <vector>
#include <GL/glu.h>
#include "Utilities/Pnt3f.H"
#include "info_pack.h"

class cpu_object {
private:
protected:
	InfoPack infoPack;
    virtual void init();
    virtual void GL_Draw() = 0;
    std::vector<cpu_object*> Children;
public:
    
	cpu_object();
	cpu_object(InfoPack infoPack);

	cpu_object(const Pnt3f& pos);
	cpu_object(const Pnt3f& pos, const Pnt3f& size);
	cpu_object(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top);
	cpu_object(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top, GLubyte color[4]);
	
    bool isVisible() const;

    void setVisible(bool visible);

    const Pnt3f &getPos() const;

    void setPos(const Pnt3f &pos);

    const Pnt3f &getSize() const;

    void setSize(const Pnt3f &size);

    const Pnt3f &getFront() const;

    void setFront(const Pnt3f &front);

    const Pnt3f &getTop() const;

    void setTop(const Pnt3f &top);

    const GLubyte *getColor() const;

    void setColor(GLubyte *color);

    void AddChild(cpu_object*);

    void Draw();
};

#endif