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
        position(0, 0, 0),
        color({0, 0, 0, 1}),
        isActive(false),
        lifetime(0),
        direction({0, 0, 0}),
        decelaration({0, 0, 0}),
        fade(0)
    {}

    void reset()
    {
        position  = gloost::Point3(0.0f, 0.0f, 0.0f);
        color     = gloost::vec4(0.7176f, 0.870f, 0.99f, 1.0f),
        isActive  = true;
        lifetime  = gloost::frand();
        direction[0] = (gloost::frand() - 1)/1000;
        direction[1] = gloost::crand()/20000;
        direction[2] = gloost::crand()/20000;
        decelaration[0] = 0;
        decelaration[1] = 0;
        decelaration[2] = 0;
        fade = gloost::getRandomMinMax(3.0f, 10.0f)/50000.0f;
    }

    gloost::Point3 position;
    gloost::vec4 color;
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

    // void draw() const;
    // void prepare();
    void reset();
    void update();
    std::vector< Particle > const& getParticles() const;

private:

    int _quantity;
    std::vector < Particle > _data;
};

#endif // __PARTICLES_H
