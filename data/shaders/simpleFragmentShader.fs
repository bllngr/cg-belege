#version 330

in vec4 normal;
in vec4 vector;
in vec2 texCoords;

out vec4 out_Color;

uniform vec4 ObjectColor;
uniform sampler2D ObjectSampler;

float diffuse = 0.9f;

void main(void)
{

    vec3 ray   = normalize( vec3(0.0f) - vector.xyz);
    vec3 color = ObjectColor.xyz * diffuse * max(dot(normal.xyz, ray), 0.0f);


    // out_Color  = vec4(clamp(color, 0.0, 1.0), 1.0);

    out_Color = texture2D(ObjectSampler, texCoords);
}
