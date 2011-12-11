#include "Particles.h"

#include <gloostMath.h>

Particles::Particles(int quantity)
{
    _quantity = quantity;
    _data.resize(_quantity);
    std::fill(_data.begin(), _data.end(), Particle());
}

Particles::~Particles()
{
    //dtor
}

void Particles::update()
{

    for (Particle& p : _data) { // C++11
        if (p.lifetime <= 0.0f) p.isActive = false;

        if (p.isActive) {
            gloost::Vector3 translation(p.direction[0], p.direction[1], p.direction[2]);
            p.position += translation;
            p.lifetime -= p.fade;
            p.color.a  -= p.fade;
        } else {
            p.reset();
        }
    }

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,                                  // from start
                    sizeof(gloost::Point3) * _quantity, // to end
                    &(_data.front()));

}

void Particles::reset()
{
    for (Particle& p : _data) { // C++11
        p.reset();
    }
}

std::vector< Particle > const& Particles::getParticles() const
{
    return _data;
}
