#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
// Parameters:
//
//   float4x4 matProjection;
//
//
// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   matProjection c0       4
//

vs_2_0
dcl_position v0
dcl_texcoord v1
dcl_color v2
mul r0, v0.y, c1
mad r0, c0, v0.x, r0
add r0, r0, c3
mov oPos, r0
mov oT1.xy, r0
mov oT0.xy, v1
mov oD0, v2
mov oT2.xy, v0

// approximately 8 instruction slots used
#endif

const unsigned char arrVertexShaderData[] =
{
      0,   2, 254, 255, 254, 255,
     33,   0,  67,  84,  65,  66,
     28,   0,   0,   0,  87,   0,
      0,   0,   0,   2, 254, 255,
      1,   0,   0,   0,  28,   0,
      0,   0,   0,   1,   0,   0,
     80,   0,   0,   0,  48,   0,
      0,   0,   2,   0,   0,   0,
      4,   0,   2,   0,  64,   0,
      0,   0,   0,   0,   0,   0,
    109,  97, 116,  80, 114, 111,
    106, 101,  99, 116, 105, 111,
    110,   0, 171, 171,   3,   0,
      3,   0,   4,   0,   4,   0,
      1,   0,   0,   0,   0,   0,
      0,   0, 118, 115,  95,  50,
     95,  48,   0,  77, 105,  99,
    114, 111, 115, 111, 102, 116,
     32,  40,  82,  41,  32,  72,
     76,  83,  76,  32,  83, 104,
     97, 100, 101, 114,  32,  67,
    111, 109, 112, 105, 108, 101,
    114,  32,  49,  48,  46,  49,
      0, 171,  31,   0,   0,   2,
      0,   0,   0, 128,   0,   0,
     15, 144,  31,   0,   0,   2,
      5,   0,   0, 128,   1,   0,
     15, 144,  31,   0,   0,   2,
     10,   0,   0, 128,   2,   0,
     15, 144,   5,   0,   0,   3,
      0,   0,  15, 128,   0,   0,
     85, 144,   1,   0, 228, 160,
      4,   0,   0,   4,   0,   0,
     15, 128,   0,   0, 228, 160,
      0,   0,   0, 144,   0,   0,
    228, 128,   2,   0,   0,   3,
      0,   0,  15, 128,   0,   0,
    228, 128,   3,   0, 228, 160,
      1,   0,   0,   2,   0,   0,
     15, 192,   0,   0, 228, 128,
      1,   0,   0,   2,   1,   0,
      3, 224,   0,   0, 228, 128,
      1,   0,   0,   2,   0,   0,
      3, 224,   1,   0, 228, 144,
      1,   0,   0,   2,   0,   0,
     15, 208,   2,   0, 228, 144,
      1,   0,   0,   2,   2,   0,
      3, 224,   0,   0, 228, 144,
    255, 255,   0,   0
};