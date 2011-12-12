#version 330

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoords;

smooth out vec4 N;
smooth out vec4 v;
smooth out vec4 L;

out vec2 texCoords;

// Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

// light source is the sun, which is at the origin
// TODO: find out why it has to be in world coordinates
vec4 lightSource = vec4(0.0f, 0.0f, 0.0f, 1.0f);

void main(void)
{
    // vertex position to eye coordinates
    v = ModelViewMatrix * vec4(in_Position, 1.0);

    // surface normal to eye coordinates
    N = normalize(NormalMatrix * vec4(in_Normal, 0.0));

    // vector to light source
    L = normalize(lightSource - v);

    // hand over the texture coordinates
    texCoords = in_TexCoords;

    // transform the geometry
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
}
