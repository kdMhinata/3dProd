#include "Gimmick.h"

void Gimmick::Update()
{
	{
		if (m_activeFlg)
		{
			Player::ChangeAction < Player::ActionMove>();
			GameInstance.Load("Data/Save/Dungeon2");
		}
	}
}