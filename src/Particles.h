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
    Particles(int quantity);
    ~Particles();

    void draw() const;
    void prepare();
    void manipulate();

private:

    int _quantity;
    struct Particle {
        Particle() :
            position (gloost::Point3(0.0f, 0.0f, 0.0f)),
            isActive(true),
            lifetime(0),
            direction({0, 0, 0}),
            decelaration({0, 0, 0}),
            fade(gloost::getRandomMinMax(0.0f, 99.0f)/1000.0f + 0.003f)
        {}

        gloost::Point3 position;
        bool isActive;
        float lifetime;
        float direction[3];
        float decelaration[3];
        float fade;
    };

    std::vector < gloost::Point3 > _data; // TODO
    std::vector < Particle > _prt;        // TODO
};

#endif // __PARTICLES_H
