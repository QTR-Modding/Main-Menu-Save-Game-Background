cbuffer BlurCB : register(b0)
{
    float2 texelSize;
    float radius;
    float brightness;
    float saturation;
    float contrast;
    float rMultiply;
    float gMultiply;
    float bMultiply;
};

Texture2D inputTex : register(t0);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 sum = float4(0, 0, 0, 0);
    float sigma = radius / 2.0f;
    float twoSigmaSq = 2.0f * sigma * sigma;
    float piSigma = 3.14159265 * twoSigmaSq;
    float weightSum = 0.0f;

    int r = int(radius);

    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            float2 offset = float2(x, y) * texelSize;
            float weight = exp(-(x * x + y * y) / twoSigmaSq) / piSigma;
            sum += inputTex.Load(int3(DTid.xy + int2(x, y), 0)) * weight;
            weightSum += weight;
        }
    }

    float4 blurred = sum / weightSum;

    blurred.rgb *= brightness;

    blurred.rgb = ((blurred.rgb - 0.5f) * contrast) + 0.5f;

    float lum = dot(blurred.rgb, float3(0.299, 0.587, 0.114));
    blurred.rgb = lerp(float3(lum, lum, lum), blurred.rgb, saturation);

    blurred.r *= rMultiply;
    blurred.g *= gMultiply;
    blurred.b *= bMultiply;

    outputTex[DTid.xy] = blurred;
}