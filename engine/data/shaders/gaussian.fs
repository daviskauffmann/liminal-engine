#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

out vec4 frag_color;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
    vec2 offset = float(1) / textureSize(image, 0);
    vec3 result = texture(image, vertex.uv).rgb * weight[0];

    if(horizontal)
    {
        for(int i = 1; i < 5; i++)
        {
            result += texture(image, vertex.uv + vec2(offset.x * i, 0)).rgb * weight[i];
            result += texture(image, vertex.uv - vec2(offset.x * i, 0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; i++)
        {
            result += texture(image, vertex.uv + vec2(0, offset.y * i)).rgb * weight[i];
            result += texture(image, vertex.uv - vec2(0, offset.y * i)).rgb * weight[i];
        }
    }
    
    frag_color = vec4(result, 1);
}
