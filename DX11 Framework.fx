//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D    txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register( b0 )
{
	matrix  World;
	matrix  View;
	matrix  Projection;

    float4  DiffuseMtrl;
    float4  DiffuseLight;
    float3  LightVecW;

    float4  AmbientMtrl;
    float4  AmbientLight;

    float4  SpecularMtrl;
    float4  SpecularLight;
    float   SpecularPower;
    float3  EyePosW;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    //converts from model to world space
    output.Pos = mul( Pos, World );

    output.PosW = normalize(EyePosW.xyz - output.Pos.xyz);

    //converts from camera space from world space
    output.Pos = mul( output.Pos, View );

    //converts from view space to projection
    output.Pos = mul( output.Pos, Projection );

    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
    output.Norm = normalize(normalW);

    output.Tex = Tex;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

    input.Norm = normalize(input.Norm);

    float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
    float3 diffuse = diffuseAmount * (textureColour * DiffuseLight).rgb; //F2

    float r = reflect(-LightVecW, input.Norm);
    float specularAmount = pow(max(dot(r, input.PosW), 0.0f), 1.0f);
    float3 specular = specularAmount * SpecularMtrl * SpecularLight;

    float3 ambient = AmbientMtrl * AmbientLight;

    //D3
    float4 outColour;
    outColour.rgb = diffuse + ambient + clamp(specular, 0, 1);
    outColour.a = DiffuseMtrl.a;
    return outColour;
}