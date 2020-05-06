//----------------------------------------------------------------
#ifndef _GED3DShaderModelKK_h_
#define _GED3DShaderModelKK_h_
//----------------------------------------------------------------
#include "GED3DShaderBase.h"
#include "ShadowMap.h"
//----------------------------------------------------------------
struct stShaderModelKKParam
{
	//顶点数据
	ID3D11Buffer* pVB;
	//实例化数据
	ID3D11Buffer* pInstanceVB;
	//顶点索引数据
	ID3D11Buffer* pIB;
	//顶点结构体的sizeof
	unsigned int uiSizeofVertex;
	//顶点索引的个数
	unsigned int uiIndexCount;
	//贴图
	ID3D11ShaderResourceView* pTextureSRV;

	//法线贴图
	ID3D11ShaderResourceView* pNormalTextureSRV;

	//世界坐标系矩阵
	XMMATRIX* pMatWorld;
	//
	//存放顶点结构体成员真实数值的内存块，这个内存块被包装成了贴图
	ID3D11ShaderResourceView* pPosSRV;
	//贴图宽度，单位像素
	int nPosSRVWidth;
	//内存块内有多少个真实的位置数值
	int nPosCount;
	//内存块内有多少个真实的法线数值
	int nNormalCount;
	int nTangentCount;
	//内存块内有多少个真实的UV数值
	int nUVCount;
	//
	//当前正在播放哪个动画。
	//该动画的骨骼的旋转和位移数据，被包装成了贴图
	ID3D11ShaderResourceView* pAnimSRV;
	//贴图宽度，单位像素
	int nAnimSRVWidth;
	//有多少个骨骼
	int nBoneCount;
	//当前正在播放动画的哪一帧。
	//值为-1表示不计算骨骼动画。
	int nKeyFrameIndex;
	//实例化数量
	int nInstanceCount;
	//阴影
	ShadowMap* mShadowMap;
};
//----------------------------------------------------------------
class GED3DShaderModelKK : public GED3DShaderBase
{
public:
	GED3DShaderModelKK();
	~GED3DShaderModelKK();
	bool InitShaderModelKK();
	void ClearShaderModelKK();
	//重写基类的函数
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static GED3DShaderModelKK*  ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectMatrixVariable* m_pFxWorldMatrix;
	ID3DX11EffectMatrixVariable* m_pFxViewProjMatrix;
	ID3DX11EffectMatrixVariable* m_pFxWorldInvTransposeMatrix;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxVertexTexturePosCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureNormalCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureUVCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureTangentCount;
	ID3DX11EffectScalarVariable* m_pFxhaveTangent;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureBoneCount;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureKeyFrameIndex;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxVertexTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxAnimTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxNormalTexture;
};
//----------------------------------------------------------------
#endif //_GED3DShaderModelKK_h_
//----------------------------------------------------------------
