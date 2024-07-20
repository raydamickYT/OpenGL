#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) in vec2 vUv;
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
    vec3 animatedPos = vPos;

    // Voeg een eenvoudige sinusvormige animatie toe aan de y-positie
    animatedPos.y += sin(animatedPos.x * 2.0 + time) * 0.1;

    FragPos = vec3(model * vec4(animatedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vNormal;
    TexCoord = vUv;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
