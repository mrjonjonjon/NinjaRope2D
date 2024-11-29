#version 330 core

in vec2 fragCoord; // Fragment coordinates (screen space)
out vec4 FragColor;

uniform vec2 center;  // Center of the star (normalized coordinates)
uniform float time;   // Time for pulsing effect

// Function to generate a star-shaped distance field
float star(vec2 p, float radius, float arms, float sharpness) {
    // Polar coordinates
    float angle = atan(p.y, p.x);
    float length = length(p);

    // Star-like pattern: make arms sharp and the body smooth
    float arm = abs(mod(angle / 3.14159, 2.0) - 1.0);
    arm = smoothstep(0.1, 0.2, arm);
    
    // Combine the length and arm for the final distance
    float dist = length - radius * (1.0 - arm);
    return dist;
}

void main() {
    // Get normalized fragment position (-1 to 1)
    vec2 fragCoordNormalized = fragCoord * 2.0 - 1.0;

    // Adjust the time for pulsation, changing the size of the star over time
    float radius = 0.3 + 0.1 * sin(time * 3.0); // Pulsing effect

    // Calculate distance to the star shape
    float distance = star(fragCoordNormalized - center, radius, 5.0, 0.1);

    // Smoothstep to make the edges soft
    float alpha = smoothstep(0.01, 0.02, -distance);

    // Set color with soft edges and pulsing effect
    FragColor = vec4(1.0, 0.5, 0.0, alpha); // Orange, glowing effect
}
