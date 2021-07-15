#include "KdAudio.h"

void KdAudioManager::Init()
{
	// AudioEngine初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_EnvironmentalReverb | DirectX::AudioEngine_ReverbUseFilters;

	m_audioEng = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEng->SetReverb(DirectX::Reverb_Default);

	m_listener.OrientFront = { 0, 0, 1 };
}

void KdAudioManager::Release()
{
	m_audioEng = nullptr;

	m_playList.clear();
}

// 更新
void KdAudioManager::Update(const Math::Vector3& rPos, const Math::Vector3& rDir)
{
	// 実はこれを実行しなくても音はなる
	if (m_audioEng != nullptr)
	{
		m_audioEng->Update();
	}

	// リスナーの更新
	m_listener.SetPosition(rPos);

	m_listener.OrientFront = rDir;

	// 再生中じゃないインスタンスは終了したと判断してリストから削除
	for (auto iter = m_playList.begin(); iter != m_playList.end();)
	{
		if (!iter->second->IsPlaying())
		{
			iter = m_playList.erase(iter);

			continue;
		}

		++iter;
	}
}

bool KdAudioManager::Play(const std::string& rName, bool loop)
{
	//if (!m_audioEng) { return false; }

	//std::shared_ptr<KdSoundEffect> soundData = KdResFac.GetSound(rName);

	//if (!soundData) { return false; }

	//std::shared_ptr<KdSoundInstance> instance = std::make_shared<KdSoundInstance>();

	//if (!instance) { return false; }

	//instance->Init(soundData);

	//instance->Play(loop);

	return true;
}

bool KdAudioManager::Play3D(const std::string& rName, const Math::Vector3& rPos, bool loop)
{
	//if (!m_audioEng) { return false; }

	//std::shared_ptr<KdSoundEffect> soundData = KdResFac.GetSound(rName);

	//if (!soundData) { return false; }

	//std::shared_ptr<KdSoundInstance3D> instance = std::make_shared<KdSoundInstance3D>();

	//if (!instance) { return false; }

	//instance->Init(soundData);

	//instance->Play(loop);

	//instance->SetPos(rPos);

	return true;
}

void KdAudioManager::StopAllSound()
{
	if (!m_audioEng) { return; }

	// 再生リストの全ての音を停止する
	auto it = m_playList.begin();
	while (it != m_playList.end()) {
		(*it).second->Stop();
		++it;
	}
}

// 音データの読み込み
bool KdSoundEffect::Load(const std::string& fileName)
{
	if (KD_AUDIO.GetAudioEngine().get() != nullptr)
	{
		try
		{
			// wstringに変換
			std::wstring wFilename = sjis_to_wide(fileName);

			// 読み込み
			m_soundEffect = std::make_unique<DirectX::SoundEffect>(KD_AUDIO_ENGINE.get(), wFilename.c_str());
		}
		catch (...)
		{
			assert(0 && "Sound File Load Error");

			return false;
		}
	}

	return true;
}

void KdSoundInstance::Init(const std::shared_ptr<KdSoundEffect>& soundEffect)
{
	if (!soundEffect) { return; }

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;
	m_instance = (soundEffect->CreateInstance(flags));
	m_soundData = soundEffect;
}

void KdSoundInstance::Play(bool loop)
{
	if (!m_instance) { return; }

	// 再生状態がどんな状況か分からないので一度停止してから
	Stop();

	m_instance->Play(loop);

	// 再生時にリストに登録する
	KD_AUDIO.AddPlayList(shared_from_this());
}

void KdSoundInstance::SetVolume(float vol)
{
	if (m_instance == nullptr)return;

	m_instance->SetVolume(vol);
}

bool KdSoundInstance::IsPlaying()
{
	if (!m_instance) { return false; }

	return (m_instance->GetState() == DirectX::SoundState::PLAYING);
}

void KdSoundInstance3D::Init(const std::shared_ptr<KdSoundEffect>& soundEffect)
{
	if (!soundEffect) { return; }

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default |
		DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters;
	m_instance = (soundEffect->CreateInstance(flags));
	m_soundData = soundEffect;

}

void KdSoundInstance3D::Play(bool loop)
{
	if (!m_instance) { return; }

	KdSoundInstance::Play(loop);
}

void KdSoundInstance3D::SetPos(const Math::Vector3& rPos)
{
	if (!m_instance) { return; }

	m_emitter.SetPosition(rPos);

	m_instance->Apply3D(KD_AUDIO.GetListener(), m_emitter, false);
}

void KdSoundInstance3D::SetCurveDistanceScaler(float val)
{
	if (!m_instance) { return; }

	m_emitter.CurveDistanceScaler = val;

	m_instance->Apply3D(KD_AUDIO.GetListener(), m_emitter, false);
}