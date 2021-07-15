#include "../../Application/main.h"
#include "System/KdSystem.h"
#include "../../Application/main.h"

#include "KdUtility.h"

// viewから画像情報を取得する
void KdGetTextuxxreInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}