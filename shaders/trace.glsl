#version 440

struct Sphere{
    vec3 Position;
    float Radius;
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1)in;

layout(std140, binding = 0)buffer u_Spheres{
    int Count;
    vec2 Positions[];
};

layout(rgba8, binding = 1)uniform writeonly image2D u_Output;

void main(){
    const ivec2 coords = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
    const vec2 uv = vec2(coords) / vec2(imageSize(u_Output));
    vec4 color = vec4(uv.xy, 0, 1);
    
    for(int i = 0; i<Count; i++){
        if(length(vec2(coords) - Positions[i]) < 60)
            color = vec4(1, 0, 0, 1);
    }


    imageStore(u_Output, coords, color);
}

