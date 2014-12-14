#version 130

uniform mat4 ProjectionViewMatrix;
uniform vec3 EyePos;

in vec3 position;

out vec3 pos;
out vec3 eye;

void main()
{
    pos = position;
    vec3 vpos = position - vec3(0.5);
    eye = EyePos - vpos;
    gl_Position = ProjectionViewMatrix * vec4(vpos, 1.0);
}
