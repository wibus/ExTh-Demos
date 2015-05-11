#version 130

in  vec2 position;
out vec2 spacepos;

void main()
{
    spacepos = position;
    gl_Position = vec4(position, 0, 1);
}
