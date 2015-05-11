#version 130

uniform vec2  Center;
uniform float Scale;
uniform int  MaxIterations;
uniform vec4 LowOut;
uniform vec4 HighOut;

in vec2 spacepos;
out vec4 FragColor;

vec3 value(float alpha)
{
    vec3 col = vec3(0.0, 0.0, 0.0);
    col =  mix(col, vec3(.33, 0.0, 0.0), max(1.0-alpha*2.0, 0.0));
    col =  mix(col, vec3(0.0, .66, 0.0), 1.0 - abs(alpha-0.5)*2.0);
    return mix(col, vec3(0.0, 0.0, 1.0), max(2.0*alpha-1.0, 0.0));
}

void main()
{
#ifdef double_precision
    dvec2 base, c;
    double real, imag, r2, tmpReal;
#else
    vec2 base, c;
    float real, imag, r2, tmpReal;
#endif

    base = spacepos*Scale + Center;
    real = base.x;
    imag = base.y;
    c = base;
    r2 = 0.0;

    int i;
    for(i=0; i < MaxIterations && r2 < 4.0; ++i)
    {
        tmpReal = real;
        real = (real*real) - (imag*imag) + c.x;
        imag = 2.0 * tmpReal * imag + c.y;
        r2 = (real*real) + (imag*imag);
    }

    vec4 color;

    if(r2 < 4.0)
        discard;
    else
        color = vec4(value(sin(i*log(float(i)) / 100.0)), 1.0);

    FragColor = color;
}
