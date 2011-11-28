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
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

vec4 lightSource = ModelViewMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);

void main(void)
{
    //^= old ModelViewProjectionMatrix

    // vertex position in eye coordinates (?)
    v = ModelViewMatrix * vec4(in_Position, 1.0);

    // surface normal in eye coordinates
    N = normalize(NormalMatrix * vec4(in_Normal, 0.0));

    // vector to light source
    // TODO: why the hell has this to have the wrong direction?
    L = normalize(lightSource - v);

    // transform the geometry
    gl_Position = ProjectionMatrix * v;
    // = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);

    // hand over the texture coordinates
    texCoords = in_TexCoords;
}
