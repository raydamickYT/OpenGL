#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 vNormal;

out vec3 worldPosition;

uniform mat4 world;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * * world * vec4(aPos, 1.0);
    worldPosition = mat3(world) * aPos;
}
