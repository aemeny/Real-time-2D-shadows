#version 300 es
attribute vec3 aPosition;
attribute vec2 aTexCoord;

out vec2 pos;

void main() {
  pos = aTexCoord;
  pos.y = 1. - pos.y;

  vec4 positionVec4 = vec4(aPosition, 1.0);

  gl_Position = positionVec4;
}