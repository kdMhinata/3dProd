#pragma once

#include "Src/Application/Game/GameObject/GameObject.h"
class TitleObject : public GameObject
{
public:
	TitleObject() {}
	~TitleObject() { Release(); }

	void Init() override;
	void Update() override;
	void Draw2D()override;

private:
	void Release();		// ‰ð•ú

	std::shared_ptr<KdTexture> m_spTitleTex;
	std::shared_ptr<PlayerInput> m_input;
};
