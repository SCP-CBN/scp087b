Texture2D diff;
SamplerState smp;

cbuffer cbVertex {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

struct VS_INPUT {
    float3 position : POSITION0;
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
    output.position = mul(float4(input.position, 1),worldMatrix);
    output.position = mul(output.position,viewMatrix);
    output.position = mul(output.position,projectionMatrix);
    output.uv = input.uv;
    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = diff.Sample(smp,input.uv);
    //output.depth = float4(input.position.w,0.0,0.0,1.0);
    return output;
}
