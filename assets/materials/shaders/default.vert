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
    vec3 position = (_modelViewMatrix * _vertex).xyz;
    vec3 lightDir = normalize(g_lightPos - position);
    vec3 eyeNormal = normalize((_normalMatrix * _normal).xyz);
    vec3 reflection = normalize(2.0 * dot(eyeNormal, lightDir) * eyeNormal - lightDir);
    vec4 ambient = _ambientColor;
    vec4 diffuse = _diffuseColor * max(0.0, dot(eyeNormal, lightDir));
    vec4 specular = _specularColor * pow(max(0.0, dot(reflection, normalize(-position))), _shininess);
    g_color = ambient + diffuse + specular;

    // transform geometry
    gl_Position = _modelViewProjectionMatrix * _vertex;
}
