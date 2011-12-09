#ifndef __PARTICLES_H
#define __PARTICLES_H

#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gloostMath.h>
#include <gloostGlUtil.h>

extern unsigned BufferIds[6]; // globally defined in main.cpp

struct Particle {
    Particle() :
        position (gloost::Point3(0.0f, 0.0f, 0.0f)),
        isActive(true),
        lifetime(gloost::frand()),
        direction({0, 0, 0}),
        decelaration({0, 0, 0}),
        fade(gloost::getRandomMinMax(0.0f, 99.0f)/1000.0f + 0.003f)
    {}

    void reset()
    {
        position  = gloost::Point3(0.0f, 0.0f, 0.0f);
        isActive  = true;
        lifetime  = gloost::frand();
        direction[0] = (gloost::frand() - 1)/10000;
        direction[1] = gloost::crand()/100000;
        direction[2] = 0;
        decelaration[0] = 0;
        decelaration[1] = 0;
        decelaration[2] = 0;
        fade = gloost::getRandomMinMax(3.0f, 99.0f)/50000.0f;
    }

    gloost::Point3 position;
    bool isActive;
    float lifetime;
    float direction[3];
    float decelaration[3];
    float fade;
};

class Particles
{
public:
    Particles(int quantity);
    ~Particles();

    void draw() const;
    void prepare();
    void update();
    std::vector< Particle > const& getParticles();

private:

    int _quantity;
    std::vector < Particle > _data;
};

#endif // __PARTICLES_H
