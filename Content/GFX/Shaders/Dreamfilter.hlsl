Texture2D screen;
SamplerState smp;

struct VS_INPUT {
    uint id : TEXCOORD0;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_INPUT VS(VS_INPUT input) {
    uint id = input.id;
    PS_INPUT output = (PS_INPUT)0;
    output.uv = float2(id % 2, (id % 4) >> 1);
    output.position = float4((output.uv.x - 0.5f) * 2, -(output.uv.y - 0.5f) * 2, 0, 1);
    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = float4(screen.Sample(smp, input.uv).rgb, 0.1);
    return output;
}
