#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out PROPERTIES {
    vec3 Normal;
    vec3 FragmentPosition;
} vs_out;

void main() {

    vs_out.Normal = transpose(inverse(mat3(model))) * normal;
    vs_out.FragmentPosition = vec3(model * vec4(position, 1.0));

    gl_Position = projection * view * model * vec4(position, 1.0);
}