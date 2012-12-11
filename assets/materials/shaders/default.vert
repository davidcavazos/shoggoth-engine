#version 110

uniform mat4 _modelViewProjectionMatrix;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; // ftransform();
//     gl_Position = mvpMatrix * vertex;
//     gl_Position = _modelViewProjectionMatrix * gl_Vertex;
//     if (gl_ModelViewProjectionMatrix[0][0] >= _modelViewProjectionMatrix[0][0]) {
//         gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//     }
}
