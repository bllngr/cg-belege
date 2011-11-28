#version 330

in vec4 N;
in vec4 v;
in vec4 L;
in vec2 texCoords;

out vec4 out_Color;

uniform sampler2D ObjectSampler;

vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); // usage: vec4(ambient, diffuse, specular, alpha);

vec4 ambientColor  = vec4(vec3(0.25f), 1.0f);
vec4 diffuseColor  = vec4(vec3(1.0f), 1.0f);
vec4 specularColor = vec4(1.0f);

void main(void)
{
    vec4 eye              = normalize(-v);
    vec4 reflectionVector = normalize(reflect(normalize(-L), normalize(N)));

    // diffuse shading
    float diffuseTerm = max(0, dot(normalize(N), normalize(L)));

    // specular shading
    float spec = max(0.0f, dot(normalize(eye), reflectionVector));
    float specularTerm = pow(spec, 128.0f);

    // texture color
    vec4 textureColor = /* vec4(1.0f, 0.0f, 0.0f, 1.0f); // */ texture2D(ObjectSampler, texCoords);

    out_Color = (
//                        lightColor.x * ambientColor
                     + lightColor.y * diffuseTerm  * diffuseColor
//                     + lightColor.z * specularTerm * specularColor
                ) * textureColor;
}
