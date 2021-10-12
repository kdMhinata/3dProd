#pragma once

class KdSoundEffect;
class KdSoundInstance;
class KdSoundInstance3D;

// ゲーム内の音を管理するクラス
class KdAudioManager
{
public:

	KdAudioManager() {}
	~KdAudioManager() { Release(); }

	// 初期化
	void Init();

	// 更新
	void Update(const Math::Vector3& rPos, const Math::Vector3& rDir);

	std::shared_ptr<KdSoundInstance>  Play(const std::string& rName, bool loop = false);

	std::shared_ptr<KdSoundInstance3D> Play3D(const std::string& rName, const Math::Vector3& rPos, bool loop = false);

	void AddPlayList(const std::shared_ptr<KdSoundInstance>& rSound)
	{
		if (!rSound.get()) { return; }

		m_playList[(UINT)(rSound.get())] = rSound;
	}

	void StopAllSound();

	// DirectXAudioEngine取得
	std::unique_ptr<DirectX::AudioEngine>& GetAudioEngine() { return m_audioEng; }

	const DirectX::AudioListener& GetListener() { return m_listener; }

	void SoundReset();

private:

	// 解放
	void Release();

	std::shared_ptr<KdSoundEffect> GetSound(const std::string& fileName);

	// DirectXのAudioEngine本体
	std::unique_ptr<DirectX::AudioEngine>	m_audioEng;

	// 3Dサウンド用リスナー
	DirectX::AudioListener					m_listener;

	// 再生中のサウンドリスト
	std::map<UINT, std::shared_ptr<KdSoundInstance>>	m_playList;

	// サウント管理マップ
	std::unordered_map< std::string, std::shared_ptr<KdSoundEffect>> m_soundMap;
};

// サウンド再生用のインスタンス
class KdSoundInstance : public std::enable_shared_from_this<KdSoundInstance>
{
public:
	KdSoundInstance(const std::shared_ptr<KdSoundEffect>& soundEffect);

	virtual bool CreateInstance();

	// 再生
	virtual void Play(bool loop = false);

	// 停止
	void Stop() { if (m_instance) { m_instance->Stop(); } }

	// 一時停止
	void Pause() { if (m_instance) { m_instance->Pause(); } }

	// 再開
	void Resume() { if (m_instance) { m_instance->Resume(); } }

	// ・vol	… ボリューム設定(1.0が100%)
	void SetVolume(float vol);

	// ・pitch	… 振動設定(低音-1.0～1.0高音)
	void SetPitch(float pitch);

	// 再生中かどうか
	bool IsPlaying();

	// 終了したインスタンスかどうか
	bool IsStopped();

protected:

	// サウンドの再生インスタンス
	std::unique_ptr<DirectX::SoundEffectInstance>	m_instance;

	// 再生サウンドの元データ
	std::shared_ptr<KdSoundEffect>					m_soundData;

	// コピー禁止用
	KdSoundInstance(const KdSoundInstance& src) = delete;
	void operator=(const KdSoundInstance& src) = delete;
};

// 3Dサウンド再生用のインスタンス
class KdSoundInstance3D : public KdSoundInstance
{
public:
	KdSoundInstance3D(const std::shared_ptr<KdSoundEffect>& soundEffect, const DirectX::AudioListener& ownerListener);

	bool CreateInstance() override;

	// 再生
	void Play(bool loop = false) override;

	void SetPos(const Math::Vector3& rPos);

	// 減衰倍率設定 1:通常 FLT_MIN～FLT_MAX
	void SetCurveDistanceScaler(float val);

protected:

	// エミッター 主に3Dサウンドソースの定義
	DirectX::AudioEmitter			m_emitter;

	// 3Dサウンド用リスナー
	const DirectX::AudioListener& m_ownerListener;

	// コピー禁止用
	KdSoundInstance3D(const KdSoundInstance3D& src) = delete;
	void operator=(const KdSoundInstance3D& src) = delete;

};

// サウンドデータを扱う
class KdSoundEffect {
public:

	KdSoundEffect() {}

	~KdSoundEffect() {
		m_soundEffect = nullptr;
	}

	std::unique_ptr<DirectX::SoundEffectInstance> CreateInstance(DirectX::SOUND_EFFECT_INSTANCE_FLAGS flag)
	{
		if (!m_soundEffect) { return nullptr; }

		return m_soundEffect->CreateInstance(flag);
	}

	// WAVEサウンド読み込み
	bool Load(const std::string& fileName, const std::unique_ptr<DirectX::AudioEngine>& engine);

private:
	// サウンドエフェクト
	std::unique_ptr<DirectX::SoundEffect>	m_soundEffect;

	// コピー禁止用:単一のデータはコピーできない
	KdSoundEffect(const KdSoundEffect& src) = delete;
	void operator=(const KdSoundEffect& src) = delete;
};