#include "Gimmick.h"

void Gimmick::Update()
{
	{
		if (m_activeFlg)
		{
			GameInstance.ReserveChangeScene("Data/Save/Dungeon2");
		}
	}
}