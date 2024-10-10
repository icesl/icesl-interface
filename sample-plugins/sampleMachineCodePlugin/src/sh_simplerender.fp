#ifdef Emscripten
  precision mediump float;
  varying vec3      v_vertex;
  #define OUT_COLOR gl_FragColor
#else
#version 430 core
  in  vec4 color;
  out vec4 out_color;
  #define OUT_COLOR out_color
#endif

// SP 2018-03-26
// SL 2023-12-21: allowing WebGL1

void main()
{
  OUT_COLOR = color;
}
