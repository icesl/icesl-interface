#ifdef Emscripten
#version 300 es
precision mediump float;
#else
#version 430 core
#endif

// SP 2018-03-26

in vec4 color;
out vec4 out_color;

void main()
{
  out_color = color;
}
