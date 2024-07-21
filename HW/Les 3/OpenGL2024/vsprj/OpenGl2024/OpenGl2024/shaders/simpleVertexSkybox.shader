#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 3) in vec3 vNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    FragPos = vec3(model * vec4(animatedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
