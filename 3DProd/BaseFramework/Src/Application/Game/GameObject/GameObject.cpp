#include "GameObject.h"

void GameObject::Draw()
{
	SHADER->m_standardShader.SetAlpha(m_alpha);

	SHADER->m_standardShader.DrawModel(m_modelWork, m_mWorld);

	SHADER->m_standardShader.SetAlpha(1.0f);
}

void GameObject::ImGuiUpdate()
{
	if (ImGui::Button("SaveToFile"))
	{
		std::string path;
		if (KdWindow::SaveFileDialog(path))
		{
			// Jsonオブジェクト化
			json11::Json::object obj;
			Serialize(obj);
			// 文字列化
			json11::Json json(obj);
			std::string strJson = json.dump(true);

			std::ofstream ofs(path);
			if(ofs)
			{
				ofs.write(strJson.c_str(), strJson.size());
			}
		}
	}
	ImGui::InputText("Name", &m_name);
	ImGui::InputText("Tag", &m_tag);

	// 座標
	ImGui::DragFloat3("Pos", &m_mWorld._41, 0.01f);
}

bool GameObject::CheckCollisionBump(const SphereInfo& info, BumpResult& result)
{
	Math::Vector3 selfPos = GetPos() + m_bumpSphereInfo.m_pos;

	Math::Vector3 distVec = info.m_pos - selfPos;

	// 2点間の距離
	float distanceSqr = distVec.LengthSquared();

	// 当たり判定の半径合計
	float hitRadius = m_bumpSphereInfo.m_radius + info.m_radius;

	// 判定の結果
	result.m_isHit = (distanceSqr <= (hitRadius * hitRadius));

	if (result.m_isHit)
	{
		// 押し戻し
		result.m_pushVec = distVec;
		result.m_pushVec.Normalize();

		float distance = std::sqrt(distanceSqr);

		// 押し戻すベクトル×押し戻す値
		result.m_pushVec *= hitRadius - distance;
	}
	return result.m_isHit;
}

bool GameObject::CheckCollisionBump(const RayInfo& info, BumpResult& result)
{
	float minDistance = FLT_MAX;

	for (UINT i = 0; i < m_modelWork.GetDataNodes().size(); ++i)
	{
		const KdModelData::Node& dataNode = m_modelWork.GetDataNodes()[i];

		if (!dataNode.m_spMesh) { continue; }

		const KdModelWork::Node& workNode = m_modelWork.GetNodes()[i];

		KdRayResult localResult;

		KdRayToMesh(info.m_pos, info.m_dir, info.m_range, *(dataNode.m_spMesh.get()),
			workNode.m_worldTransform * m_mWorld, &localResult);

		if (localResult.m_hit && localResult.m_distance < minDistance)
		{
			result.m_pushVec = info.m_dir * (localResult.m_distance - info.m_range);

			result.m_isHit = true;

			minDistance = localResult.m_distance;
		}
	}

	return result.m_isHit;

}

