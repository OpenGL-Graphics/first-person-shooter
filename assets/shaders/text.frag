#version 130

in vec2 tex_coord_vert;
in vec4 color_vert;
in float shift_vert;
in float gamma_vert;

uniform sampler2D texture_in;
uniform vec3 pixel_in;

out vec4 color_out;

void main() {
  // color_out = color_vert * pow(intensity, 1.0 / gamma_vert);
  color_out = texture2D(texture_in, tex_coord_vert);

  /*
  // LCD Off
  if( pixel_in.z == 1.0) {
      float a = texture2D(texture_in, tex_coord_vert).r;
      color_out = color_vert * pow( a, 1.0/gamma_vert );
      return;
  }

  // LCD On
  vec4 current = texture2D(texture_in, tex_coord_vert);
  vec4 previous= texture2D(texture_in, tex_coord_vert+vec2(-1.,0.)*pixel_in.xy);
  vec4 next    = texture2D(texture_in, tex_coord_vert+vec2(+1.,0.)*pixel_in.xy);

  current = pow(current, vec4(1.0/gamma_vert));
  previous= pow(previous, vec4(1.0/gamma_vert));

  float r = current.r;
  float g = current.g;
  float b = current.b;

  if( shift_vert <= 0.333 )
  {
      float z = shift_vert/0.333;
      r = mix(current.r, previous.b, z);
      g = mix(current.g, current.r,  z);
      b = mix(current.b, current.g,  z);
  }
  else if( shift_vert <= 0.666 )
  {
      float z = (shift_vert-0.33)/0.333;
      r = mix(previous.b, previous.g, z);
      g = mix(current.r,  previous.b, z);
      b = mix(current.g,  current.r,  z);
  }
 else if( shift_vert < 1.0 )
  {
      float z = (shift_vert-0.66)/0.334;
      r = mix(previous.g, previous.r, z);
      g = mix(previous.b, previous.g, z);
      b = mix(current.r,  previous.b, z);
  }

 float t = max(max(r,g),b);
 vec4 color = vec4(color_vert.rgb, (r+g+b)/3.0);
 color = t*color + (1.0-t)*vec4(r,g,b, min(min(r,g),b));
 color_out = vec4( color.rgb, color_vert.a*color.a);
 */
}
