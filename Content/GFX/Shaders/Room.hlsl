Texture2D diff;
Texture2D rough;
SamplerState smp;

cbuffer cbVertex {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}

cbuffer cbFrag {
    float3 lightPos;
    float3 viewPos;
}

struct VS_INPUT {
    float3 position : POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output = (PS_INPUT)0;
    float4 worldPos = mul(worldMatrix, float4(input.position, 1.0));
    output.worldPos = worldPos.xyz;
    output.position = mul(viewMatrix, worldPos);
    output.position = mul(projectionMatrix, output.position);
    output.normal = normalize(mul(worldMatrix, float4(input.normal, 0.0)).xyz);
    output.uv = input.uv;
    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float3 dist = lightPos - input.worldPos;
    float acDist = 1.0 - saturate((dist.x * dist.x + dist.y * dist.y + dist.z * dist.y) / 100000.0);
    float3 lightDir = normalize(dist);
    float diffuse = saturate(dot(lightDir, input.normal));
    float3 reflectDir = normalize(2.0 * diffuse * input.normal - lightDir);
    float roughness = rough.Sample(smp, input.uv).r;
    float specular = pow(saturate(dot(normalize(viewPos - input.worldPos), reflectDir)), 128);
    output.color = float4(((diff.Sample(smp, input.uv).rgb * diffuse) + (1.0 - roughness) * saturate(4 * diffuse) * specular) * acDist, 1.0);
    return output;
}
