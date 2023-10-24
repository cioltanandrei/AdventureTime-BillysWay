#version 330 core
out vec4 fragColor;

in float gradientFactor; // Input gradient factor from the vertex shader

void main() {
    // Define the two colors for the gradient
    vec4 bottomColor = vec4(0.0, 0.0, 0.5, 1.0); // Dark blue
    vec4 topColor = vec4(0.0, 0.0, 1.0, 1.0);   // Light blue

    // Interpolate between the two colors based on the gradientFactor
    fragColor = mix(bottomColor, topColor, gradientFactor);
}
