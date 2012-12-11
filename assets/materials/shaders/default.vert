#version 110

uniform mat4 _modelViewProjectionMatrix;

void main() {
    gl_Position = _modelViewProjectionMatrix * gl_Vertex;
}
