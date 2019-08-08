#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];
uniform float time;
out vec2 TexCoords;

vec3 getNorm() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 pos, vec3 norm){
    float magnitude = 2.;
    vec3 direction = ((sin(time)+1)/2) * norm * magnitude;
    //return pos + vec4(direction, 0.);
    return vec4(0.);
}

void main() {
    vec3 norm = getNorm();
    gl_Position = gl_in[0].gl_Position + explode(gl_in[0].gl_Position, norm);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + explode(gl_in[1].gl_Position, norm);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position + explode(gl_in[2].gl_Position, norm);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}

