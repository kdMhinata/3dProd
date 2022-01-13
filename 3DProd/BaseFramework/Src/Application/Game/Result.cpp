#include"Result.h"

void ResultObject::Init()
{
	m_spResultTex = GameResourceFactory.GetTexture("Data/Textures/gameover.png");
	m_input = std::make_shared<PlayerInput>();
}

void ResultObject::Update()
{
	m_input->Update();
	if (m_input->IsPressButton(0, false))
	{
		GameInstance.TitleInit();
	}
}

void ResultObject::Draw2D()
{
	if (!m_spResultTex) { return; }

	//ビューポートを利用して解像度を得る
	Math::Viewport vp;
	D3D.GetViewport(vp);

	int posX = -(vp.width * 0.5f) + (m_spResultTex.get()->GetWidth() * 0.5f);
	int posY = (vp.height * 0.5f) - (m_spResultTex.get()->GetHeight() * 0.5f);

	SHADER->m_spriteShader.SetMatrix(Math::Matrix::Identity);
	SHADER->m_spriteShader.DrawTex(m_spResultTex.get(), posX, posY);
}

void ResultObject::Release()
{
}
