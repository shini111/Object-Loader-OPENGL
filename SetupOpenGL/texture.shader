#shader vertex
#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    Color = color;
    TexCoord = texCoord;
}

#shader fragment
#version 330 core

in vec3 Color;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
    outColor = texture(texture_diffuse1, TexCoord);
}