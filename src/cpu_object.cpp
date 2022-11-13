#include "cpu_object.h"

void cpu_object::init(){
}

cpu_object::cpu_object() {
	this->init();
}

cpu_object::cpu_object(InfoPack infoPack)
	: infoPack(infoPack){
	this->init();
}

cpu_object::cpu_object(const Pnt3f& pos)
	: infoPack(pos) {
	this->init();
}

cpu_object::cpu_object(const Pnt3f& pos, const Pnt3f& size)
	: infoPack(pos, size) {
	this->init();
}

cpu_object::cpu_object(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top)
	: infoPack(pos, size, front, top){
	this->init();
}

cpu_object::cpu_object(const Pnt3f& pos, const Pnt3f& size, const Pnt3f& front, const Pnt3f& top, GLubyte color[4])
	: infoPack(pos, size, front, top, color){
	this->init();
}

bool cpu_object::isVisible() const {
    return infoPack.isVisible();
}

void cpu_object::setVisible(bool visible) {
    infoPack.setVisible(visible);
}

const Pnt3f& cpu_object::getPos() const {
    return infoPack.getPos();
}

void cpu_object::setPos(const Pnt3f& pos) {
    infoPack.setPos(pos);
}

const Pnt3f& cpu_object::getSize() const {
    return infoPack.getSize();
}

void cpu_object::setSize(const Pnt3f& size) {
    infoPack.setSize(size);
}

const Pnt3f& cpu_object::getFront() const {
    return infoPack.getFront();
}

void cpu_object::setFront(const Pnt3f& front) {
    infoPack.setFront(front);
}

const Pnt3f& cpu_object::getTop() const {
    return infoPack.getTop();
}

void cpu_object::setTop(const Pnt3f& top) {
    infoPack.setTop(top);
}

const GLubyte* cpu_object::getColor() const {
    return infoPack.getColor();
}

void cpu_object::setColor(GLubyte* color) {
    infoPack.setColor(color);
}

void cpu_object::AddChild(cpu_object* obj) {
    Children.push_back(obj);
}

void cpu_object::Draw(){
    glPushMatrix();
    Pnt3f Side = (infoPack.getTop() * infoPack.getFront());
    //Side.normalize();
    Pnt3f Top = infoPack.getFront() * Side;
    //Top.normalize();
    GLfloat rotate[16] = {  infoPack.getFront().x, infoPack.getFront().y, infoPack.getFront().z, 0,
                            Top.x, Top.y, Top.z, 0,
                            Side.x, Side.y, Side.z, 0,
                            0, 0, 0, 1};
	
    GLfloat mat[16] = { 1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        infoPack.getPos().x,infoPack.getPos().y,infoPack.getPos().z, 1};

	glMultMatrixf(mat);
    glMultMatrixf(rotate);
    glScalef(infoPack.getSize().x, infoPack.getSize().y, infoPack.getSize().z);
    if(infoPack.isVisible()){
        for(cpu_object* obj : Children){
            obj->Draw();
        }
        glColor4ubv(infoPack.getColor());
        this->GL_Draw();
    }
    glPopMatrix();
}