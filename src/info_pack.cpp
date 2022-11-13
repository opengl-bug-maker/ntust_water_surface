#include "info_pack.h"

static GLubyte black_color[4] = { 0,0,0,1 };

InfoPack::InfoPack(){

}

InfoPack::InfoPack(const Pnt3f& pos)
    : InfoPack(pos, Pnt3f(1, 1, 1)) {
}

InfoPack::InfoPack(const Pnt3f& pos, const Pnt3f& size)
    : InfoPack(pos, size, Pnt3f(1, 0, 0), Pnt3f(0, 1, 0)) {
}

InfoPack::InfoPack(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top)
    : InfoPack(pos, size, front, top, black_color) {
}

InfoPack::InfoPack(const Pnt3f &pos, const Pnt3f &size, const Pnt3f &front, const Pnt3f &top, GLubyte color[4])
    :visible(true), Pos(pos), Size(size), Front(front), Top(top), color{color[0], color[1], color[2], color[3]} {
    Front.normalize();
    Top.normalize();
}

bool InfoPack::isVisible() const {
    return visible;
}

void InfoPack::setVisible(bool visible) { InfoPack::visible = visible; }
const Pnt3f& InfoPack::getPos() const { return Pos; }
void InfoPack::setPos(const Pnt3f& pos) { Pos = pos; }
const Pnt3f& InfoPack::getSize() const { return Size; }
void InfoPack::setSize(const Pnt3f& size) { Size = size; }
const Pnt3f& InfoPack::getFront() const { return Front; }
void InfoPack::setFront(const Pnt3f& front) {
    Front = front;
    Front.normalize();
}
const Pnt3f& InfoPack::getTop() const { return Top; }
void InfoPack::setTop(const Pnt3f& top) {
    Top = top;
    Front.normalize();
}
const GLubyte* InfoPack::getColor() const { return color; }

void InfoPack::setColor(const GLubyte* color) {
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
    this->color[3] = color[3];
}
