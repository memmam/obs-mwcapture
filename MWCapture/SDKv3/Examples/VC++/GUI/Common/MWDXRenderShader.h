#ifndef MWDXRENDERSHADER_H
#define MWDXRENDERSHADER_H

unsigned char g_cs_vs_grey[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";

unsigned char g_cs_ps_grey[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.5f;\n"
	"float v=0.5f;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_y16[] = "struct VS_INPUT\n"
			      "{ \n"
			      "float4 Pos : POSITION;\n"
			      "float2 Tex : TEXCOORD0;\n"
			      "};\n"
			      "struct PS_INPUT \n"
			      "{\n"
			      "float4 Pos : SV_POSITION;\n"
			      "float2 Tex : TEXCOORD0;\n"
			      "};\n"
			      "PS_INPUT VS(VS_INPUT input)\n"
			      "{\n"
			      "PS_INPUT output;\n"
			      "output.Pos = input.Pos;\n"
			      "output.Tex = input.Tex;\n"
			      "return output;\n"
			      "}";
unsigned char g_cs_ps_y16[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.5f;\n"
	"float v=0.5f;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_rgb16[] = "struct VS_INPUT\n"
				"{ \n"
				"float4 Pos : POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"struct PS_INPUT \n"
				"{\n"
				"float4 Pos : SV_POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"PS_INPUT VS(VS_INPUT input)\n"
				"{\n"
				"PS_INPUT output;\n"
				"output.Pos = input.Pos;\n"
				"output.Tex = input.Tex;\n"
				"return output;\n"
				"}";
;
unsigned char g_cs_ps_rgb16[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"Texture2D txDiffuse;\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float r= txDiffuse.Sample(samLinear, input.Tex).b;\n"
	"float g= txDiffuse.Sample(samLinear, input.Tex).g;\n"
	"float b= txDiffuse.Sample(samLinear, input.Tex).r;\n"
	"float a= 1.0;\n"
	"return float4(r,g,b,a);\n"
	"}";

unsigned char g_cs_vs_bgr16[] = "struct VS_INPUT\n"
				"{ \n"
				"float4 Pos : POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"struct PS_INPUT \n"
				"{\n"
				"float4 Pos : SV_POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"PS_INPUT VS(VS_INPUT input)\n"
				"{\n"
				"PS_INPUT output;\n"
				"output.Pos = input.Pos;\n"
				"output.Tex = input.Tex;\n"
				"return output;\n"
				"}";
;
unsigned char g_cs_ps_bgr16[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"Texture2D txDiffuse;\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float b= txDiffuse.Sample(samLinear, input.Tex).b;\n"
	"float g= txDiffuse.Sample(samLinear, input.Tex).g;\n"
	"float r= txDiffuse.Sample(samLinear, input.Tex).r;\n"
	"float a= 1.0;\n"
	"return float4(r,g,b,a);\n"
	"}";

unsigned char g_cs_vs_yuy2[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_yuy2[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   cbuffer cbNeverChanges : register(b0) \
							   { \
							   int m_cx; \
							   int m_cy; \
							   int t_temp; \
							   int t_temp2; \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   int t_n_y = m_cy * input.Tex.y; \
							   int t_f_x = floor(m_cx *input.Tex.x); \
							   int t_n_index = floor(t_f_x) % 2; \
							   if (t_n_index == 0) { \
							   float t_f_next = input.Tex.x + (1.0 / m_cx); \
							   float2 t_tex; \
							   t_tex.x = t_f_next; \
							   t_tex.y = input.Tex.y; \
							   float y = txDiffuse.Sample(samLinear, input.Tex).x; \
							   float u = txDiffuse.Sample(samLinear, input.Tex).y; \
							   float v = txDiffuse.Sample(samLinear, t_tex).y; \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   } \
							   else if (t_n_index == 1) { \
							   float t_f_next = input.Tex.x - (1.0 / m_cx); \
							   float2 t_tex; \
							   t_tex.x = t_f_next; \
							   t_tex.y = input.Tex.y; \
							   float y = txDiffuse.Sample(samLinear, input.Tex).x; \
							   float u = txDiffuse.Sample(samLinear, t_tex).y; \
							   float v = txDiffuse.Sample(samLinear, input.Tex).y; \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   } \
							   }";

unsigned char g_cs_vs_bgra[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_bgra[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float b= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float r= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float a= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_argb[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_argb[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float a= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float r= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float b= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_abgr[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_abgr[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float a= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float b= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float r= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_rgba[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_rgba[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float r= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float b= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float a= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_bgr24[] = "struct VS_INPUT\n \
							  { \n \
							  float4 Pos : POSITION;\n \
							  float2 Tex : TEXCOORD0;\n \
							  };\n \
							  struct PS_INPUT \n \
							  {\n \
							  float4 Pos : SV_POSITION;\n \
							  float2 Tex : TEXCOORD0;\n \
							  };\n \
							  PS_INPUT VS(VS_INPUT input)\n \
							  {\n \
							  PS_INPUT output;\n \
							  output.Pos = input.Pos;\n \
							  output.Tex = input.Tex;\n \
							  return output;\n \
							  }";
unsigned char g_cs_ps_bgr24[] = "struct VS_INPUT \
								{ \
								float4 Pos : POSITION; \
								float2 Tex : TEXCOORD0; \
								}; \
								struct PS_INPUT \
								{ \
								float4 Pos : SV_POSITION; \
								float2 Tex : TEXCOORD0; \
								}; \
								cbuffer cbNeverChanges : register(b0) \
								{ \
								int m_cx; \
								int m_cy; \
								};\
								Texture2D txDiffuse; \
								SamplerState samLinear; \
								float4 PS(PS_INPUT input) : SV_Target \
								{ \
								int nIdx = floor(input.Tex.x * float(m_cx));\
								int nIdy = floor(input.Tex.y * float(m_cy));\
								int nId = nIdy * m_cx + nIdx;\
								float2 fLocate;\
								float b, g, r;\
								\
								if(nId % 4 == 0)\
								{\
								int nLocate = nId - nId / 4;\
								float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx);\
								float fy = float(nLocate / m_cx) / float(m_cy);\
								fLocate.x = fx;\
								fLocate.y = fy;\
								\
								b = txDiffuse.Sample(samLinear, fLocate).x;\
								g = txDiffuse.Sample(samLinear, fLocate).y;\
								r = txDiffuse.Sample(samLinear, fLocate).z;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else if(nId % 4 == 1)\
								{\
								int nLocateB = nId - (nId - 1) / 4 - 1;\
								int nLocateGR = nId - (nId - 1) / 4;\
								float fxB = float(nLocateB - nLocateB / m_cx * m_cx) / float(m_cx);\
								float fyB = float(nLocateB / m_cx) / float(m_cy);\
								float fxGR = float(nLocateGR - nLocateGR / m_cx * m_cx) / float(m_cx);\
								float fyGR = float(nLocateGR / m_cx) / float(m_cy);\
								\
								fLocate.x = fxB;\
								fLocate.y = fyB;\
								b = txDiffuse.Sample(samLinear, fLocate).w;\
								\
								fLocate.x = fxGR;\
								fLocate.y = fyGR;\
								g = txDiffuse.Sample(samLinear, fLocate).x;\
								r = txDiffuse.Sample(samLinear, fLocate).y;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else if(nId % 4 == 2)\
								{\
								int nLocateBG = nId - (nId - 2) / 4 - 1;\
								int nLocateR = nId - (nId - 2) / 4;\
								float fxBG = float(nLocateBG - nLocateBG / m_cx * m_cx) / float(m_cx);\
								float fyBG = float(nLocateBG / m_cx) / float(m_cy);\
								float fxR = float(nLocateR - nLocateR / m_cx * m_cx) / float(m_cx);\
								float fyR = float(nLocateR / m_cx) / float(m_cy);\
								\
								fLocate.x = fxBG;\
								fLocate.y = fyBG;\
								b = txDiffuse.Sample(samLinear, fLocate).z;\
								g = txDiffuse.Sample(samLinear, fLocate).w;\
								\
								fLocate.x = fxR;\
								fLocate.y = fyR;\
								r = txDiffuse.Sample(samLinear, fLocate).x;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else\
								{\
								int nLocate = nId - (nId - 3) / 4 - 1; \
								float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx); \
								float fy = float(nLocate / m_cx) / float(m_cy); \
								fLocate.x = fx; \
								fLocate.y = fy; \
								\
								b = txDiffuse.Sample(samLinear, fLocate).y; \
								g = txDiffuse.Sample(samLinear, fLocate).z; \
								r = txDiffuse.Sample(samLinear, fLocate).w; \
								\
								return float4(r, g, b, 1.0); \
								}\
								}";

unsigned char g_cs_vs_rgb24[] = "struct VS_INPUT\n"
				"{\n"
				"float4 Pos : POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"struct PS_INPUT \n"
				"{\n"
				"float4 Pos : SV_POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"PS_INPUT VS(VS_INPUT input)\n"
				"{\n"
				"PS_INPUT output;\n"
				"output.Pos = input.Pos;\n"
				"output.Tex = input.Tex;\n"
				"return output;\n"
				"}";

unsigned char g_cs_ps_rgb24[] = "struct VS_INPUT\n \
	{\n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
	}; \
	struct PS_INPUT\n \
	{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
	}; \
	cbuffer cbNeverChanges : register(b0)\n \
	{\n \
	int m_cx;\n \
	int m_cy;\n \
	};\n \
	Texture2D txDiffuse;\n \
	SamplerState samLinear;\n \
	float4 PS(PS_INPUT input) : SV_Target\n \
	{\n \
	int nIdx = floor(input.Tex.x * float(m_cx));\n \
	int nIdy = floor(input.Tex.y * float(m_cy));\n \
	int nId = nIdy * m_cx + nIdx;\n \
	float2 fLocate;\n \
	float b, g, r;\n \
	if(nId % 4 == 0)\n \
	{\n \
	int nLocate = nId - nId / 4;\n \
	float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx);\n \
	float fy = float(nLocate / m_cx) / float(m_cy);\n \
	fLocate.x = fx;\n \
	fLocate.y = fy;\n \
	r = txDiffuse.Sample(samLinear, fLocate).x;\n \
	g = txDiffuse.Sample(samLinear, fLocate).y;\n \
	b = txDiffuse.Sample(samLinear, fLocate).z;\n \
	return float4(r, g, b, 1.0);\n \
	}\n \
	else if(nId % 4 == 1)\n \
	{\n \
	int nLocateB = nId - (nId - 1) / 4 - 1;\n \
	int nLocateGR = nId - (nId - 1) / 4;\n \
	float fxB = float(nLocateB - nLocateB / m_cx * m_cx) / float(m_cx);\n \
	float fyB = float(nLocateB / m_cx) / float(m_cy);\n \
	float fxGR = float(nLocateGR - nLocateGR / m_cx * m_cx) / float(m_cx);\n \
	float fyGR = float(nLocateGR / m_cx) / float(m_cy);\n \
	fLocate.x = fxB;\n \
	fLocate.y = fyB;\n \
	r = txDiffuse.Sample(samLinear, fLocate).w;\n \
	fLocate.x = fxGR;\n \
	fLocate.y = fyGR;\n \
	g = txDiffuse.Sample(samLinear, fLocate).x;\n \
	b = txDiffuse.Sample(samLinear, fLocate).y;\n \
	return float4(r, g, b, 1.0);\n \
	}\n \
	else if(nId % 4 == 2)\n \
	{\n \
	int nLocateBG = nId - (nId - 2) / 4 - 1;\n \
	int nLocateR = nId - (nId - 2) / 4;\n \
	float fxBG = float(nLocateBG - nLocateBG / m_cx * m_cx) / float(m_cx);\n \
	float fyBG = float(nLocateBG / m_cx) / float(m_cy);\n \
	float fxR = float(nLocateR - nLocateR / m_cx * m_cx) / float(m_cx);\n \
	float fyR = float(nLocateR / m_cx) / float(m_cy);\n \
	fLocate.x = fxBG;\n \
	fLocate.y = fyBG;\n \
	r = txDiffuse.Sample(samLinear, fLocate).z;\n \
	g = txDiffuse.Sample(samLinear, fLocate).w;\n \
	fLocate.x = fxR;\n \
	fLocate.y = fyR;\n \
	b = txDiffuse.Sample(samLinear, fLocate).x;\n \
	return float4(r, g, b, 1.0);\n \
	}\n \
	else\n \
	{\n \
	int nLocate = nId - (nId - 3) / 4 - 1;\n \
	float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx);\n \
	float fy = float(nLocate / m_cx) / float(m_cy);\n \
	fLocate.x = fx;\n \
	fLocate.y = fy;\n \
	r = txDiffuse.Sample(samLinear, fLocate).y;\n \
	g = txDiffuse.Sample(samLinear, fLocate).z;\n \
	b = txDiffuse.Sample(samLinear, fLocate).w;\n \
	return float4(r, g, b, 1.0);\n \
	}\n \
	}";

unsigned char g_cs_vs_uyvy[] = "struct VS_INPUT\n \
							 { \n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT \n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 PS_INPUT VS(VS_INPUT input)\n \
							 {\n \
							 PS_INPUT output;\n \
							 output.Pos = input.Pos;\n \
							 output.Tex = input.Tex;\n \
							 return output;\n \
							 }";
unsigned char g_cs_ps_uyvy[] = "struct VS_INPUT\n \
							 {\n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT\n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 cbuffer cbNeverChanges : register(b0)\n \
							 {\n \
							 int m_cx;\n \
							 int m_cy;\n \
							 int t_temp;\n \
							 int t_temp2;\n \
							 float m_csp_coeff_0_0;\n \
							 float m_csp_coeff_0_1;\n \
							 float m_csp_coeff_0_2;\n \
							 float m_csp_coeff_1_0;\n \
							 float m_csp_coeff_1_1;\n \
							 float m_csp_coeff_1_2;\n \
							 float m_csp_coeff_2_0;\n \
							 float m_csp_coeff_2_1;\n \
							 float m_csp_coeff_2_2;\n \
							 float m_csp_const_0;\n \
							 float m_csp_const_1;\n \
							 float m_csp_const_2;\n \
							 };\n \
							 Texture2D txDiffuse;\n \
							 SamplerState samLinear;\n \
							 float4 PS(PS_INPUT input) : SV_Target\n \
							 {\n \
							 int t_n_y = m_cy * input.Tex.y;\n \
							 int t_f_x = floor(m_cx *input.Tex.x);\n \
							 int t_n_index = floor(t_f_x) % 2;\n \
							 if (t_n_index == 0) {\n \
							 float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float u = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float v = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 else if (t_n_index == 1) {\n \
							 float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float u = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float v = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 }\n";

unsigned char g_cs_vs_yvyu[] = "struct VS_INPUT\n \
							 { \n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT \n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 PS_INPUT VS(VS_INPUT input)\n \
							 {\n \
							 PS_INPUT output;\n \
							 output.Pos = input.Pos;\n \
							 output.Tex = input.Tex;\n \
							 return output;\n \
							 }";
unsigned char g_cs_ps_yvyu[] = "struct VS_INPUT\n \
							 {\n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT\n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 cbuffer cbNeverChanges : register(b0)\n \
							 {\n \
							 int m_cx;\n \
							 int m_cy;\n \
							 int t_temp;\n \
							 int t_temp2;\n \
							 float m_csp_coeff_0_0;\n \
							 float m_csp_coeff_0_1;\n \
							 float m_csp_coeff_0_2;\n \
							 float m_csp_coeff_1_0;\n \
							 float m_csp_coeff_1_1;\n \
							 float m_csp_coeff_1_2;\n \
							 float m_csp_coeff_2_0;\n \
							 float m_csp_coeff_2_1;\n \
							 float m_csp_coeff_2_2;\n \
							 float m_csp_const_0;\n \
							 float m_csp_const_1;\n \
							 float m_csp_const_2;\n \
							 };\n \
							 Texture2D txDiffuse;\n \
							 SamplerState samLinear;\n \
							 float4 PS(PS_INPUT input) : SV_Target\n \
							 {\n \
							 int t_n_y = m_cy * input.Tex.y;\n \
							 int t_f_x = floor(m_cx *input.Tex.x);\n \
							 int t_n_index = floor(t_f_x) % 2;\n \
							 if (t_n_index == 0) {\n \
							 float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float v = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float u = txDiffuse.Sample(samLinear, t_tex).y;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 else if (t_n_index == 1) {\n \
							 float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float u = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float v = txDiffuse.Sample(samLinear, t_tex).y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 }\n";

unsigned char g_cs_vs_vyuy[] = "struct VS_INPUT\n \
							 { \n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT \n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 PS_INPUT VS(VS_INPUT input)\n \
							 {\n \
							 PS_INPUT output;\n \
							 output.Pos = input.Pos;\n \
							 output.Tex = input.Tex;\n \
							 return output;\n \
							 }";
unsigned char g_cs_ps_vyuy[] = "struct VS_INPUT\n \
							 {\n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT\n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 cbuffer cbNeverChanges : register(b0)\n \
							 {\n \
							 int m_cx;\n \
							 int m_cy;\n \
							 int t_temp;\n \
							 int t_temp2;\n \
							 float m_csp_coeff_0_0;\n \
							 float m_csp_coeff_0_1;\n \
							 float m_csp_coeff_0_2;\n \
							 float m_csp_coeff_1_0;\n \
							 float m_csp_coeff_1_1;\n \
							 float m_csp_coeff_1_2;\n \
							 float m_csp_coeff_2_0;\n \
							 float m_csp_coeff_2_1;\n \
							 float m_csp_coeff_2_2;\n \
							 float m_csp_const_0;\n \
							 float m_csp_const_1;\n \
							 float m_csp_const_2;\n \
							 };\n \
							 Texture2D txDiffuse;\n \
							 SamplerState samLinear;\n \
							 float4 PS(PS_INPUT input) : SV_Target\n \
							 {\n \
							 int t_n_y = m_cy * input.Tex.y;\n \
							 int t_f_x = floor(m_cx *input.Tex.x);\n \
							 int t_n_index = floor(t_f_x) % 2;\n \
							 if (t_n_index == 0) {\n \
							 float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float v = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float u = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 else if (t_n_index == 1) {\n \
							 float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float v = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float u = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 }\n";

unsigned char g_cs_vs_nv12[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

/**/
unsigned char g_cs_ps_nv12[] = "struct VS_INPUT\n \
							   {\n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT\n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   cbuffer cbNeverChanges : register(b0)\n \
							   {\n \
							   int m_cx;\n \
							   int m_cy;\n \
							   int t_temp;\n \
							   int t_temp2;\n \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   };\n \
							   Texture2D txtYregister: register(t0);\n \
							   Texture2D txtUVregister:register(t1);\n \
							   SamplerState samLinear;\n \
							   float4 PS(PS_INPUT input) : SV_Target\n \
							   {\n \
							   int t_n_nx=input.Tex.x*m_cx;\n \
							   int t_n_ny=input.Tex.y*m_cy;\n \
							   float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n \
							   float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n \
							   float2 t_tex;\n \
							   t_tex.x = t_f_x;\n \
							   t_tex.y = t_f_y;\n \
							   float y=txtYregister.Sample(samLinear, input.Tex).x;\n \
							   float u=0.0f;\n \
							   float v=0.0f;\n \
							   u=txtUVregister.Sample(samLinear, t_tex).x;\n \
							   v=txtUVregister.Sample(samLinear, t_tex).y;\n \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   }\n";

unsigned char g_cs_vs_nv21[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

/**/
unsigned char g_cs_ps_nv21[] = "struct VS_INPUT\n \
							   {\n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT\n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   cbuffer cbNeverChanges : register(b0)\n \
							   {\n \
							   int m_cx;\n \
							   int m_cy;\n \
							   int t_temp;\n \
							   int t_temp2;\n \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   };\n \
							   Texture2D txtYregister: register(t0);\n \
							   Texture2D txtUVregister:register(t1);\n \
							   SamplerState samLinear;\n \
							   float4 PS(PS_INPUT input) : SV_Target\n \
							   {\n \
							   int t_n_nx=input.Tex.x*m_cx;\n \
							   int t_n_ny=input.Tex.y*m_cy;\n \
							   float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n \
							   float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n \
							   float2 t_tex;\n \
							   t_tex.x = t_f_x;\n \
							   t_tex.y = t_f_y;\n \
							   float y=txtYregister.Sample(samLinear, input.Tex).x;\n \
							   float u=0.0f;\n \
							   float v=0.0f;\n \
							   u=txtUVregister.Sample(samLinear, t_tex).y;\n \
							   v=txtUVregister.Sample(samLinear, t_tex).x;\n \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   }\n";

unsigned char g_cs_vs_nv16[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";

unsigned char g_cs_ps_nv16[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUVregister:register(t1);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n"
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_nv61[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";

unsigned char g_cs_ps_nv61[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUVregister:register(t1);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n"
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_i422[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_i422[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 2 == 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUregister.Sample(samLinear, t_tex).x;\n"
	"v=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_yv16[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_yv16[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 2 == 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"v=txtUregister.Sample(samLinear, t_tex).x;\n"
	"u=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_i420[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
;
unsigned char g_cs_ps_i420[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 4 > 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUregister.Sample(samLinear, t_tex).x;\n"
	"v=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_yv12[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
;
unsigned char g_cs_ps_yv12[] =
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 4 > 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"v=txtUregister.Sample(samLinear, t_tex).x;\n"
	"u=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_p010[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
;
unsigned char g_cs_ps_p010[] =
	"struct VS_INPUT\n"
	"{\n "
	"float4 Pos : POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"struct PS_INPUT\n "
	"{\n "
	"float4 Pos : SV_POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txtYregister: register(t0);\n "
	"Texture2D txtUVregister:register(t1);\n "
	"SamplerState samLinear;\n "
	"float4 PS(PS_INPUT input) : SV_Target\n "
	"{\n "
	"int t_n_nx=input.Tex.x*m_cx;\n "
	"int t_n_ny=input.Tex.y*m_cy;\n "
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n "
	"float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n "
	"float2 t_tex;\n "
	"t_tex.x = t_f_x;\n "
	"t_tex.y = t_f_y;\n "
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n "
	"float u=0.0f;\n "
	"float v=0.0f;\n "
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n "
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,1.0f);\n "
	"}\n";

unsigned char g_cs_vs_p210[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
;
unsigned char g_cs_ps_p210[] =
	"struct VS_INPUT\n"
	"{\n "
	"float4 Pos : POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"struct PS_INPUT\n "
	"{\n "
	"float4 Pos : SV_POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txtYregister: register(t0);\n "
	"Texture2D txtUVregister:register(t1);\n "
	"SamplerState samLinear;\n "
	"float4 PS(PS_INPUT input) : SV_Target\n "
	"{\n "
	"int t_n_nx=input.Tex.x*m_cx;\n "
	"int t_n_ny=input.Tex.y*m_cy;\n "
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n "
	"float t_f_y = floor(t_n_ny)/(m_cy);\n "
	"float2 t_tex;\n "
	"t_tex.x = t_f_x;\n "
	"t_tex.y = t_f_y;\n "
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n "
	"float u=0.0f;\n "
	"float v=0.0f;\n "
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n "
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,1.0f);\n "
	"}\n";

unsigned char g_cs_vs_iyu2[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
;
unsigned char g_cs_ps_iyu2[] =
	"struct VS_INPUT\n"
	"{\n "
	"float4 Pos : POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"struct PS_INPUT\n "
	"{\n "
	"float4 Pos : SV_POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txDiffuse: register(t0);\n "
	"SamplerState samLinear;\n "
	"float4 PS(PS_INPUT input) : SV_Target\n "
	"{\n "
	"int nIdx = floor(input.Tex.x * float(m_cx)); \n "
	"int nIdy = floor(input.Tex.y * float(m_cy)); \n "
	"int nId = nIdy * m_cx + nIdx; \n "
	"float2 fLocate; \n "
	"float y, u, v; \n "
	"if (nId % 4 == 0)\n "
	"{\n "
	"int nLocate = nId - nId / 4; \n "
	"float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx); \n "
	"float fy = float(nLocate / m_cx) / float(m_cy); \n "
	"fLocate.x = fx; \n "
	"fLocate.y = fy; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).x; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).y; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).z; \n "
	"}\n "
	"else if (nId % 4 == 1)\n "
	"{\n "
	"int nLocateB = nId - (nId - 1) / 4 - 1; \n "
	"int nLocateGR = nId - (nId - 1) / 4; \n "
	"float fxB = float(nLocateB - nLocateB / m_cx * m_cx) / float(m_cx); \n "
	"float fyB = float(nLocateB / m_cx) / float(m_cy); \n "
	"float fxGR = float(nLocateGR - nLocateGR / m_cx * m_cx) / float(m_cx); \n "
	"float fyGR = float(nLocateGR / m_cx) / float(m_cy); \n "
	"fLocate.x = fxB; \n "
	"fLocate.y = fyB; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).w; \n "
	"fLocate.x = fxGR; \n "
	"fLocate.y = fyGR; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).x; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).y; \n "
	"}\n "
	"else if (nId % 4 == 2)\n "
	"{\n "
	"int nLocateBG = nId - (nId - 2) / 4 - 1; \n "
	"int nLocateR = nId - (nId - 2) / 4; \n "
	"float fxBG = float(nLocateBG - nLocateBG / m_cx * m_cx) / float(m_cx); \n "
	"float fyBG = float(nLocateBG / m_cx) / float(m_cy); \n "
	"float fxR = float(nLocateR - nLocateR / m_cx * m_cx) / float(m_cx); \n "
	"float fyR = float(nLocateR / m_cx) / float(m_cy); \n "
	"fLocate.x = fxBG; \n "
	"fLocate.y = fyBG; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).z; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).w; \n "
	"fLocate.x = fxR; \n "
	"fLocate.y = fyR; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).x; \n "
	"}\n "
	"else\n "
	"{\n "
	"int nLocate = nId - (nId - 3) / 4 - 1; \n "
	"float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx); \n "
	"float fy = float(nLocate / m_cx) / float(m_cy); \n "
	"fLocate.x = fx; \n "
	"fLocate.y = fy; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).y; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).z; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).w; \n "
	"}\n "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,1.0f);\n "
	"}\n";

unsigned char g_cs_vs_v308[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_v308[] =
	"struct VS_INPUT\n"
	"{\n "
	"float4 Pos : POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"struct PS_INPUT\n "
	"{\n "
	"float4 Pos : SV_POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txDiffuse: register(t0);\n "
	"SamplerState samLinear;\n "
	"float4 PS(PS_INPUT input) : SV_Target\n "
	"{\n "
	"int nIdx = floor(input.Tex.x * float(m_cx)); \n "
	"int nIdy = floor(input.Tex.y * float(m_cy)); \n "
	"int nId = nIdy * m_cx + nIdx; \n "
	"float2 fLocate; \n "
	"float y, u, v; \n "
	"if (nId % 4 == 0)\n "
	"{\n "
	"int nLocate = nId - nId / 4; \n "
	"float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx); \n "
	"float fy = float(nLocate / m_cx) / float(m_cy); \n "
	"fLocate.x = fx; \n "
	"fLocate.y = fy; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).x; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).y; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).z; \n "
	"}\n "
	"else if (nId % 4 == 1)\n "
	"{\n "
	"int nLocateB = nId - (nId - 1) / 4 - 1; \n "
	"int nLocateGR = nId - (nId - 1) / 4; \n "
	"float fxB = float(nLocateB - nLocateB / m_cx * m_cx) / float(m_cx); \n "
	"float fyB = float(nLocateB / m_cx) / float(m_cy); \n "
	"float fxGR = float(nLocateGR - nLocateGR / m_cx * m_cx) / float(m_cx); \n "
	"float fyGR = float(nLocateGR / m_cx) / float(m_cy); \n "
	"fLocate.x = fxB; \n "
	"fLocate.y = fyB; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).w; \n "
	"fLocate.x = fxGR; \n "
	"fLocate.y = fyGR; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).x; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).y; \n "
	"}\n "
	"else if (nId % 4 == 2)\n "
	"{\n "
	"int nLocateBG = nId - (nId - 2) / 4 - 1; \n "
	"int nLocateR = nId - (nId - 2) / 4; \n "
	"float fxBG = float(nLocateBG - nLocateBG / m_cx * m_cx) / float(m_cx); \n "
	"float fyBG = float(nLocateBG / m_cx) / float(m_cy); \n "
	"float fxR = float(nLocateR - nLocateR / m_cx * m_cx) / float(m_cx); \n "
	"float fyR = float(nLocateR / m_cx) / float(m_cy); \n "
	"fLocate.x = fxBG; \n "
	"fLocate.y = fyBG; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).z; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).w; \n "
	"fLocate.x = fxR; \n "
	"fLocate.y = fyR; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).x; \n "
	"}\n "
	"else\n "
	"{\n "
	"int nLocate = nId - (nId - 3) / 4 - 1; \n "
	"float fx = float(nLocate - nLocate / m_cx * m_cx) / float(m_cx); \n "
	"float fy = float(nLocate / m_cx) / float(m_cy); \n "
	"fLocate.x = fx; \n "
	"fLocate.y = fy; \n "
	"v = txDiffuse.Sample(samLinear, fLocate).y; \n "
	"y = txDiffuse.Sample(samLinear, fLocate).z; \n "
	"u = txDiffuse.Sample(samLinear, fLocate).w; \n "
	"}\n "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,1.0f);\n "
	"}\n";

unsigned char g_cs_vs_ayuv[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_ayuv[] =
	"struct VS_INPUT"
	"{ "
	"float4 Pos : POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"struct PS_INPUT "
	"{ "
	"float4 Pos : SV_POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txDiffuse; "
	"SamplerState samLinear; "
	"float4 PS(PS_INPUT input) : SV_Target "
	"{ "
	"float a= txDiffuse.Sample(samLinear, input.Tex).x; "
	"float y= txDiffuse.Sample(samLinear, input.Tex).y; "
	"float u= txDiffuse.Sample(samLinear, input.Tex).z; "
	"float v= txDiffuse.Sample(samLinear, input.Tex).w; "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,a);\n "
	"}";

unsigned char g_cs_vs_uyva[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_uyva[] =
	"struct VS_INPUT"
	"{ "
	"float4 Pos : POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"struct PS_INPUT "
	"{ "
	"float4 Pos : SV_POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txDiffuse; "
	"SamplerState samLinear; "
	"float4 PS(PS_INPUT input) : SV_Target "
	"{ "
	"float u= txDiffuse.Sample(samLinear, input.Tex).x; "
	"float y= txDiffuse.Sample(samLinear, input.Tex).y; "
	"float v= txDiffuse.Sample(samLinear, input.Tex).z; "
	"float a= txDiffuse.Sample(samLinear, input.Tex).w; "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,a);\n "
	"}";

unsigned char g_cs_vs_vyua[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_vyua[] =
	"struct VS_INPUT"
	"{ "
	"float4 Pos : POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"struct PS_INPUT "
	"{ "
	"float4 Pos : SV_POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txDiffuse; "
	"SamplerState samLinear; "
	"float4 PS(PS_INPUT input) : SV_Target "
	"{ "
	"float v= txDiffuse.Sample(samLinear, input.Tex).x; "
	"float y= txDiffuse.Sample(samLinear, input.Tex).y; "
	"float u= txDiffuse.Sample(samLinear, input.Tex).z; "
	"float a= txDiffuse.Sample(samLinear, input.Tex).w; "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,a);\n "
	"}";

unsigned char g_cs_vs_y410[] = "struct VS_INPUT\n"
			       "{ \n"
			       "float4 Pos : POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "struct PS_INPUT \n"
			       "{\n"
			       "float4 Pos : SV_POSITION;\n"
			       "float2 Tex : TEXCOORD0;\n"
			       "};\n"
			       "PS_INPUT VS(VS_INPUT input)\n"
			       "{\n"
			       "PS_INPUT output;\n"
			       "output.Pos = input.Pos;\n"
			       "output.Tex = input.Tex;\n"
			       "return output;\n"
			       "}";
unsigned char g_cs_ps_y410[] =
	"struct VS_INPUT"
	"{ "
	"float4 Pos : POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"struct PS_INPUT "
	"{ "
	"float4 Pos : SV_POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txDiffuse; "
	"SamplerState samLinear; "
	"float4 PS(PS_INPUT input) : SV_Target "
	"{ "
	"float u= txDiffuse.Sample(samLinear, input.Tex).r; "
	"float y= txDiffuse.Sample(samLinear, input.Tex).g; "
	"float v= txDiffuse.Sample(samLinear, input.Tex).b; "
	"float a= txDiffuse.Sample(samLinear, input.Tex).a; "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,a);\n "
	"}";

unsigned char g_cs_vs_rgb10[] = "struct VS_INPUT\n"
				"{ \n"
				"float4 Pos : POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"struct PS_INPUT \n"
				"{\n"
				"float4 Pos : SV_POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"PS_INPUT VS(VS_INPUT input)\n"
				"{\n"
				"PS_INPUT output;\n"
				"output.Pos = input.Pos;\n"
				"output.Tex = input.Tex;\n"
				"return output;\n"
				"}";
unsigned char g_cs_ps_rgb10[] =
	"struct VS_INPUT"
	"{ "
	"float4 Pos : POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"struct PS_INPUT "
	"{ "
	"float4 Pos : SV_POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"Texture2D txDiffuse; "
	"SamplerState samLinear; "
	"float4 PS(PS_INPUT input) : SV_Target "
	"{ "
	"float r= txDiffuse.Sample(samLinear, input.Tex).r; "
	"float g= txDiffuse.Sample(samLinear, input.Tex).g; "
	"float b= txDiffuse.Sample(samLinear, input.Tex).b; "
	"float a= txDiffuse.Sample(samLinear, input.Tex).a; "
	"return float4(r,g,b,a);\n "
	"}";

unsigned char g_cs_vs_bgr10[] = "struct VS_INPUT\n"
				"{ \n"
				"float4 Pos : POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"struct PS_INPUT \n"
				"{\n"
				"float4 Pos : SV_POSITION;\n"
				"float2 Tex : TEXCOORD0;\n"
				"};\n"
				"PS_INPUT VS(VS_INPUT input)\n"
				"{\n"
				"PS_INPUT output;\n"
				"output.Pos = input.Pos;\n"
				"output.Tex = input.Tex;\n"
				"return output;\n"
				"}";
unsigned char g_cs_ps_bgr10[] =
	"struct VS_INPUT"
	"{ "
	"float4 Pos : POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"struct PS_INPUT "
	"{ "
	"float4 Pos : SV_POSITION; "
	"float2 Tex : TEXCOORD0; "
	"}; "
	"Texture2D txDiffuse; "
	"SamplerState samLinear; "
	"float4 PS(PS_INPUT input) : SV_Target "
	"{ "
	"float b= txDiffuse.Sample(samLinear, input.Tex).r; "
	"float g= txDiffuse.Sample(samLinear, input.Tex).g; "
	"float r= txDiffuse.Sample(samLinear, input.Tex).b; "
	"float a= txDiffuse.Sample(samLinear, input.Tex).a; "
	"return float4(r,g,b,a);\n "
	"}";
#endif