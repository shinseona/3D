#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 LightDirection_cameraspace2;


// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
//uniform float uvoffset;
void main() {

    vec3 LightColor = vec3(1, 0, 0);
    vec3 LightColor2 = vec3(0, 1, 0);
    float LightPower = 35.0f;
    float LightPower2 = 60.0f;
    
    vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
    vec3 MaterialAmbientColor = vec3(0.3, 0.3, 0.3) * MaterialDiffuseColor;

    float distance = length(LightPosition_worldspace - Position_worldspace);

    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);
    vec3 m = normalize(LightDirection_cameraspace2);
    float cosTheta = clamp(dot(n, l), 0, 1);
    float cosTheta2 = clamp(dot(n, m), 0, 1);
    vec3 E = normalize(EyeDirection_cameraspace);
    vec3 R = reflect(-m, n);
    float cosAlpha = clamp(dot(E, R), 0, 1);
    float cosAlpha2 = clamp(0, dot(E, R), 1);

    color = (MaterialDiffuseColor * LightColor2 * LightPower2 * cosAlpha / (distance * distance))+(MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance))
        + MaterialAmbientColor ;

}