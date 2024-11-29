#version 330 core

layout(location = 0) in vec2 vertexPosition; // Vertex position
out vec2 fragCoord; // Pass fragment coordinates to the fragment shader

void main() {
    fragCoord = vertexPosition; // Pass the vertex position as frag coordinates
    gl_Position = vec4(vertexPosition, 0.0, 1.0); // Standard transformation
}
