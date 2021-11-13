#version 330 core
out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D myTexture1;
uniform sampler2D myTexture2;
uniform float c;
void main(){
    FragColor = mix(texture(myTexture1, texCoord), texture(myTexture2, vec2(texCoord.x, -texCoord.y)), c);
}
