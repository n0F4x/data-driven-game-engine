static const float2 positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

static const float3 colors[3] = float3[](
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);

struct VertexOutput {
    float3 color;
    float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexOutput main(uint vertex_id : SV_VertexID) {
    VertexOutput output;
    output.sv_position = float4(positions[vertex_id], 0.0, 1.0);
    output.color = colors[vertex_id];
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    return float4(input.color, 1.0);
}
