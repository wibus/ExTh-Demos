#version 130

uniform samplerCube EnvironmentSampler;

in vec3 tex;

out vec4 Fragment;

void main()
{
    Fragment = textureCube(EnvironmentSampler, normalize(tex));
}
