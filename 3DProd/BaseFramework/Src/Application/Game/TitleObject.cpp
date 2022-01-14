#include"TitleObject.h"

void TitleObject::Init()
{
	m_spTitleTex = GameResourceFactory.GetTexture("SpriteTex_TItle");
	m_input=std::make_shared<PlayerInput>();
}

void TitleObject::Update()
{
	m_input->Update();
	if (m_input->IsPressButton(0, false))
	{
		GameInstance.ReserveChangeScene("Data/Save/Dungeon1");
	}
}

void TitleObject::Draw2D()
{
	if (!m_spTitleTex) { return; }

	//ビューポートを利用して解像度を得る
	Math::Viewport vp;
	D3D.GetViewport(vp);

	int posX = -(vp.width * 0.5f) + (m_spTitleTex.get()->GetWidth() * 0.5f);
	int posY = (vp.height * 0.5f) - (m_spTitleTex.get()->GetHeight() * 0.5f);

	SHADER->m_spriteShader.SetMatrix(Math::Matrix::Identity);
	SHADER->m_spriteShader.DrawTex(m_spTitleTex.get(), posX, posY);
}

void TitleObject::Release()
{
}
