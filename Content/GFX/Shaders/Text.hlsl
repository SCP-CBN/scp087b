Texture2D tex0;
SamplerState smp;

cbuffer cbVertex {
    float2 position;
    matrix projectionMatrix;
}

struct VS_INPUT {
    float2 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output = (PS_INPUT)0;
    output.position = mul(projectionMatrix, float4(position + input.position, -1, 1));
    output.uv = input.uv;
    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = float4(1, 1, 1, tex0.Sample(smp, input.uv).r);
    return output;
}
