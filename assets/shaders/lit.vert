#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vColor;
out vec3 vNormal;   // Direction the surface faces (world axes)
out vec3 vPosition; // Position relative to the camera (camera-relative rendering: the camera is at 0,0,0)

void main()
{
    vec4 position = uModel * vec4(aPos, 1.0);

    vColor = aColor;

    // Unit sphere centered at the origin: the normal of each point is the point itself.
    // mat3(uModel) keeps the scale and drops the translation; valid while the scale is the same on x, y, z
    vNormal = mat3(uModel) * aPos;

    vPosition = position.xyz;

    gl_Position = uProjection * uView * position;
}