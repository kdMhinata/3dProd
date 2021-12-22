#pragma once

#include "Src/Application/Game/GameObject/GameObject.h"
class ResultObject : public GameObject
{
public:
	ResultObject() {}
	~ResultObject() { Release(); }

	void Init() override;
	void Update() override;
	void Draw2D()override;

private:
	void Release();		// ‰ð•ú

	std::shared_ptr<KdTexture> m_spResultTex;
	std::shared_ptr<PlayerInput> m_input;
};
