#include "Particles.h"

Particles::Particles()
{
    //ctor
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
    // glDrawElements(GL_TRIANGLES, _data.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // glUseProgram(0);

}

void Particles::prepare()
{
    _data.push_back(gloost::Point3( 0.0f, 1.0f, 0.0f));
    _data.push_back(gloost::Point3( 1.0f, 0.0f, 0.0f));
    _data.push_back(gloost::Point3(-1.0f, 0.0f, 0.0f));

    // unsigned index[] = {0, 1, 2}; // only needed for glDrawElements

    // create VAO which holds the state of our Vertex Attributes and VertexBufferObjects - a control structure
    // note: for different objects more of these are needed
    glGenVertexArrays(1, &BufferIds[3]);

    // bind Vertex Array - Scope begins
    glBindVertexArray(BufferIds[3]);

    // Create a VertexBufferObject and bind it to set its data
    glGenBuffers(2, &BufferIds[4]);
    glBindBuffer(GL_ARRAY_BUFFER, BufferIds[4]);

    // set the vertex data for the actual buffer; the second parameter is the size in bytes of all Vertices together
    // the third parameter is a pointer to the vertexdata
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(gloost::Point3) * _data.size(),
                 &(_data[0]),
                 GL_STATIC_DRAW);

    // enables a VertexAttributeArray
    glEnableVertexAttribArray(0);

    // specifies where in the GL_ARRAY_BUFFER our data (the vertex position) is exactly
    glVertexAttribPointer(0,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);

/*  TODO: only needed for glDrawElements

    // the second VertexBufferObject ist bound
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[5]);

    // its data are the indices of the vertices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned) * 3,
                 &(index[0]),
                 GL_STATIC_DRAW);

*/

    // unbind the VertexArray - Scope ends
    glBindVertexArray(0);

}
