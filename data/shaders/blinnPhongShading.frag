#version 330

in vec4 N;
in vec4 v;
in vec4 L;
in vec2 texCoords;

out vec4 out_Color;

uniform sampler2D ObjectSampler;

vec4 lightColor            = vec4(1.0f,  1.0f, 1.0f, 1.0f); // totally white
vec3 reflectionCoefficient = vec3(0.02f, 1.0f, 1.0f);       // ambient, diffuse, specular

void main(void)
{
    vec4 reflectionVector = normalize(reflect(-normalize(L), normalize(N)));

    // diffuse shading
    float diffuseTerm = max(0.0f, dot(normalize(N), normalize(L)));

    // specular shading
    // hier ist ein Widerspruch zwischen den Folien und der OpenGL Super Bible
    // die Folien sagen dot(normalize(v), reflectionVector)
    // die Bible sagt   dot(normalize(N), reflectionVector)
    float spec = max(0.0f, dot(normalize(v), reflectionVector));
    float specularTerm = pow(spec, 128.0f);

    // texture color
    vec4 textureColor = texture2D(ObjectSampler, texCoords);

    out_Color = (
                       lightColor.x * reflectionCoefficient.x
                     + lightColor.y * reflectionCoefficient.y * diffuseTerm
                     + lightColor.z * reflectionCoefficient.z * specularTerm
                ) * textureColor;
}
