#version 130

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

in vec3 position;

out vec3 tex;

void main()
{
    tex = position;
    vec4 vpos = ViewMatrix * vec4(position, 1.0);
    vec4 orig = ViewMatrix * vec4(0, 0, 0, 1);
    vec4 spos = ProjectionMatrix * vec4(vpos.xyz - orig.xyz, 1.0);
    gl_Position = spos;
}
