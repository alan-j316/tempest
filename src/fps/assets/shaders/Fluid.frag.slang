import Simple_shared;

struct LocalUniform {
    float4 diffuseColor;
};

[[vk::binding(0, 1)]]
ConstantBuffer<LocalUniform> localUniform;

[shader("fragment")]
float4 fragMain(VertexOutput inVert): SV_Target {
    return float4(1, 1, 0, 1);
}