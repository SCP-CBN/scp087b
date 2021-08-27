Texture2D diff;
Texture2D rough;
Texture2D norm;
Texture2D disp;
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
    float3 tangent : TEXCOORD1;
    float3 bitangent : TEXCOORD2;
    float2 uv : TEXCOORD3;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 tangent : TEXCOORD2;
    float3 bitangent : TEXCOORD3;
    float2 uv : TEXCOORD4;
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
    output.tangent = normalize(mul(worldMatrix, float4(input.tangent, 0.0)).xyz);
    output.bitangent = normalize(mul(worldMatrix, float4(input.bitangent, 0.0)).xyz);
    output.uv = input.uv;
    return output;
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    float3 normSmp = norm.Sample(smp, input.uv).rgb;
    float3 normal = (normSmp.r - 0.5) * 2.0 * input.tangent + (normSmp.g - 0.5) * 2.0 * input.bitangent + normSmp.b * input.normal;

    float3 dist = lightPos - input.worldPos;
    float acDist = 1.0 - saturate((dist.x * dist.x + dist.y * dist.y + dist.z * dist.z) / 100000.0);

    float3 lightDir = normalize(dist);
    float diffuse = saturate(dot(lightDir, normal));
    
    float3 reflectDir = normalize(2.0 * diffuse * normal - lightDir);
    float roughness = rough.Sample(smp, input.uv).r;
    float specular = pow(saturate(dot(normalize(viewPos - input.worldPos), reflectDir)), 128);
    
    output.color = float4(((diff.Sample(smp, input.uv).rgb * diffuse) + (1.0 - roughness) * saturate(4 * diffuse) * specular) * acDist, 1.0);
    return output;
}
