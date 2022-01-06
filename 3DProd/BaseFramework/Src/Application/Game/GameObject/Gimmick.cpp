#include "Gimmick.h"

void Gimmick::Update()
{	
	if (!m_clearFlg)
	{
		if (GameInstance.FindObjectWithTag("Enemy") == nullptr)
		{
			m_clearFlg = true;
		}
	}

	if (m_active==1)
	{
		GameInstance.ExitStage(GetMatrix());
	}
}