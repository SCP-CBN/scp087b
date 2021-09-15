Texture2D diff;
Texture2D rough;
Texture2D norm;
Texture2D disp;
SamplerState smp;

cbuffer cbVertex {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float2 uvOff;
    float2 uvScale;
}

cbuffer cbFrag {
    float3 lightPos;
    float3 viewPos;
    float3 intensity;
    float effectiveRangeSquared;
    uint debug;
}

struct VS_INPUT {
    float3 position : POSITION;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BITANGENT0;
    float2 uv : TEXCOORD0;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float3 worldPos : WORLDPOS0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BITANGENT0;
    float2 uv : TEXCOORD0;
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
    output.tangent = uvScale.x * normalize(mul(worldMatrix, float4(input.tangent, 0.0)).xyz);
    output.bitangent = uvScale.y * normalize(mul(worldMatrix, float4(input.bitangent, 0.0)).xyz);
    output.uv = uvScale * input.uv + uvOff;
    return output;
}

static const float LAYER_COUNT_MIN = 10.0;
static const float LAYER_COUNT_MAX = 25.0;
static const float HEIGHT_SCALE = 0.015;

float4 rescaleVector(float3 xyz) {
    return float4((xyz + 1.0) / 2.0, 1.0);
}

PS_OUTPUT PS(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;

    float3 viewDir = normalize(viewPos - input.worldPos);

    float3x3 worldToTangent = float3x3(-input.tangent, -input.bitangent, -input.normal);
    float3 transDir = mul(worldToTangent, viewDir);

    float layerCount = lerp(LAYER_COUNT_MAX, LAYER_COUNT_MIN, saturate(transDir.z));
    float2 deltaUV = transDir.xy * HEIGHT_SCALE / layerCount;
    float deltaLayerDepth = 1.0 / layerCount;

    float2 uv = input.uv;
    float layerDepth = 1.0;
    float texDepth = disp.Sample(smp, uv).r;
    float prevTexDepth = texDepth;
    float2 prevUV = uv;
    [loop]
    while (layerDepth > texDepth) {
        prevUV = uv;
        uv += deltaUV;
        // TODO: Check if we should do this.
        // If so, remove the mipmapping from the loaded texture.
        // https://stackoverflow.com/questions/56581141/direct3d11-gradient-instruction-used-in-a-loop-with-varying-iteration-forcing
        prevTexDepth = texDepth;
        texDepth = disp.SampleLevel(smp, uv, 0).r;
        layerDepth -= deltaLayerDepth;
    }

    float weight = (layerDepth - texDepth) / (prevTexDepth - texDepth - deltaLayerDepth);
    uv = lerp(uv, prevUV, weight);

    float3 normSmp = norm.Sample(smp, uv).rgb;
    float3 normal = normalize((normSmp.r - 0.5) * 2.0 * input.tangent + (normSmp.g - 0.5) * 2.0 * input.bitangent + normSmp.b * input.normal);

    float3 dist = lightPos - input.worldPos;
    float acDist = 1.0 - saturate((dist.x * dist.x + dist.y * dist.y + dist.z * dist.z) / effectiveRangeSquared);

    float3 lightDir = normalize(dist);
    float diffuse = saturate(dot(lightDir, normal));
    
    float3 reflectDir = normalize(2.0 * diffuse * normal - lightDir);
    float roughness = rough.Sample(smp, uv).r;
    float specular = pow(saturate(dot(viewDir, reflectDir)), 32);

    switch(debug) {
        default: {
            output.color = float4(((diff.Sample(smp, uv).rgb * diffuse) + (1.0 - roughness) * saturate(4 * diffuse) * specular) * acDist * intensity, 1.0);
        } break;
        case 1u: {
            output.color = rescaleVector(input.normal);
        } break;
        case 2u: {
            output.color = rescaleVector(input.tangent);
        } break;
        case 3u: {
            output.color = rescaleVector(input.bitangent);
        } break;
        case 4u: {
            output.color = rescaleVector(transDir);
        } break;
        case 5u: {
            output.color = diff.Sample(smp, input.uv);
        } break;
    }
    
    return output;
}
