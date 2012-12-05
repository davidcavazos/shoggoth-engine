#version 110

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; // ftransform();
//    gl_Position = mvpMatrix * vertex;
}
