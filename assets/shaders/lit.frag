#version 460 core

in vec3 vColor;
in vec3 vNormal;
in vec3 vPosition;

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightDir; // Points toward the light (world axes)

const float AMBIENT = 0.05;           // The dark side is not pure black
const float SHININESS = 32.0;         // Bigger = smaller and sharper highlight
const float SPECULAR_STRENGTH = 0.3;  // How bright the highlight is

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 toLight = normalize(uLightDir);
    vec3 toCamera = normalize(-vPosition);

    // DIFFUSE: 1 when the surface faces the light, 0 at 90 degrees or more
    float diffuse = max(dot(normal, toLight), 0.0);

    // SPECULAR (Blinn-Phong): shiny spot where the halfway vector lines up with the normal
    vec3 halfway = normalize(toLight + toCamera);
    float specular = diffuse > 0.0 ? pow(max(dot(normal, halfway), 0.0), SHININESS) * SPECULAR_STRENGTH : 0.0;

    vec3 baseColor = vColor * uColor;
    FragColor = vec4(baseColor * (AMBIENT + diffuse) + vec3(specular), 1.0);
}