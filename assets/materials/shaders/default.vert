#version 110

uniform mat4 _modelViewMatrix;
uniform mat4 _modelViewProjectionMatrix;
uniform mat4 _normalMatrix;

attribute vec4 _vertex;
attribute vec4 _normal;

uniform vec4 _diffuseColor;
uniform vec4 _ambientColor;
uniform vec4 _specularColor;
uniform float _shininess;

uniform vec3 g_lightPos;
varying vec4 g_color;

void main() {
    // get surface normal in eye coordinates
    vec3 eyeNormal = (_normalMatrix * _normal).xyz;

    // get vertex position in eye coordinates
    vec4 pos4 = _modelViewMatrix * _vertex;
    vec3 pos3 = pos4.xyz / pos4.w;

    // get vector to light source
    vec3 lightDir = normalize(g_lightPos - pos3);

    // dot product gives diffuse intensity
    float diffuseIntensity = max(0.0, dot(eyeNormal, lightDir));

    // multiply intensity by diffuse color
    g_color = diffuseIntensity * _diffuseColor;

    // transform geometry
    gl_Position = _modelViewProjectionMatrix * _vertex;
}
