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

void Particles::draw() const
{
    // glUseProgram(ShaderIds[0]);

    // bind the Geometry
    glBindVertexArray(BufferIds[3]);

    // draw Geometry
    // glPointSize(4);
    glDrawArrays(GL_POINTS, 0, _quantity);

    // glUseProgram(0);

}

void Particles::prepare()
{

     // TODO
    for (Particle& p : _data) { // C++11
        p.direction[0] = gloost::crand()/10000;
        p.direction[1] = gloost::crand()/10000;
    }


    // create VAO which holds the state of our Vertex Attributes and VertexBufferObjects
    glGenVertexArrays(1, &BufferIds[3]);

    // bind Vertex Array - Scope begins
    glBindVertexArray(BufferIds[3]);

    // Create a VertexBufferObject and bind it to set its data
    glGenBuffers(2, &BufferIds[4]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferIds[4]);

    // set the vertex data for the actual buffer
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Particle) * _quantity,
                 &(_data.front()),
                 GL_STATIC_DRAW);

    // enables a VertexAttributeArray for the vertices
    glEnableVertexAttribArray(0);

    // specifies where in the GL_ARRAY_BUFFER our data (the vertex position) is exactly
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Particle),
                          0);

    // enables a VertexAttributeArray for the colors
    glEnableVertexAttribArray(1);

    // specifies where in the GL_ARRAY_BUFFER our data (the colors) is exactly
    glVertexAttribPointer(1,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Particle),
                          (GLvoid*) sizeof(gloost::Point3));

    // unbind the VertexArray - Scope ends
    glBindVertexArray(0);

}

void Particles::update()
{

    for (Particle& p : _data) { // C++11
        if (p.lifetime <= 0.0f) p.isActive = false;

        if (p.isActive) {
            gloost::Vector3 translation(p.direction[0], p.direction[1], p.direction[2]);
            p.position += translation;
            p.lifetime -= p.fade;
        } else {
            p.reset();
        }
    }

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,                                  // from start
                    sizeof(gloost::Point3) * _quantity, // to end
                    &(_data.front()));

}
