#version 110

uniform mat4 _modelViewProjectionMatrix;
attribute vec4 _vertex;

void main() {
    gl_Position = _modelViewProjectionMatrix * _vertex;
}
