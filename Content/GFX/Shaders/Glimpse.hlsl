cbuffer cbVertex {

}

cbuffer cbFrag {

}

struct VS_INPUT {
    float3 position : POSITION;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
    float4 color2 : SV_TARGET1;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output = (PS_INPUT)0;
    output.position = float4(input.position, 1);
    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.color = float4(1, 0, 0, 1);
    output.color2 = float4(0, 0, 1, 1);
    return output;
}
