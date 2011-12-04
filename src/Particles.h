#ifndef __PARTICLES_H
#define __PARTICLES_H

#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gloostMath.h>
#include <gloostGlUtil.h>

extern unsigned BufferIds[6]; // globally defined in main.cpp

class Particles
{
public:
    Particles();
    ~Particles();

    void draw() const;
    void prepare();

private:
    std::vector < gloost::Point3 > _data;
};

#endif // __PARTICLES_H
