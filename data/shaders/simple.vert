#version 330 core

in vec3 vtx_position;
in vec3 vtx_color;
out vec3 var_color;

// uniform mat4 M;
uniform mat4 mat_cam;

void main()
{
  gl_Position = mat_cam * vec4(vtx_position, 1.); // * M;
  
  var_color = vtx_color;
}
