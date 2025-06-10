#ifdef BRIGHTFIX_API
#define API //__declspec(dllexport)
#else
#define API __declspec(dllexport) //__declspec(dllimport)
#endif

#define BRIGHTFIX_PLUS
#define SHINY_JEWEL_MONOTONE

extern "C"
{
	API __declspec(noinline) void LoadNewShaders();
	API __declspec(noinline) void CancelNewShaders();

	//Resets texture with the index
	API void __cdecl SetChunkTexIndexNull(int a1);
	//sets primary texture
	API void  SetChunkTexIndexPrimary(int index, int a2, int a3);
	//Sets secondary texture
	API void SetChunkTexIndexSecondary(int index, int a2, int a3);
	//gets texture index of model
	API unsigned int GetChunkTexIndex(NJS_CNK_MODEL* a1);
	//needed for ChaoColoring in CWEv8
	API void BrightFixPlus_ShinyCheck(int shiny);
	//set pixel shader constant reg float
	API void SetPixelShaderFloat(int reg, float val);
	//init, only for v8
	#ifdef BRIGHTFIX_PLUS
		API void __cdecl BrightFix_Init(const char* path, BYTE* vertexShader, BYTE* shaderData);
	#endif
}