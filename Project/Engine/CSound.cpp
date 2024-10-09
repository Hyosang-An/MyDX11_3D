#include "pch.h"
#include "CSound.h"

#include "CAssetMgr.h"

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2);




CSound::CSound()
	: CAsset(ASSET_TYPE::SOUND)
{
}

CSound::~CSound()
{
	if (nullptr != m_Sound)
	{
		FMOD_RESULT result = m_Sound->release();
		m_Sound = nullptr;
	}
}

int CSound::Play(int _iRoopCount, float _fVolume, bool _bOverlap)
{
	if (_iRoopCount <= -1)
	{
		assert(nullptr);
	}

	// 재생되고 있는 채널이 있는데, 중복재생을 허용하지 않았다 -> 재생 안함
	if (!_bOverlap && !m_listChannel.empty())
	{
		return E_FAIL;
	}

	// FMOD에서는 -1이 무한반복을 의미하고 0부터 한번 재생하는 방식이므로 _iRoopCount를 1 감소시킴
	_iRoopCount -= 1;

	FMOD::Channel* pChannel = nullptr;
	CEngine::GetInst()->GetFMODSystem()->playSound(m_Sound, nullptr, false, &pChannel);

	// 재생 실패
	if (nullptr == pChannel)
		return E_FAIL;

	pChannel->setVolume(_fVolume);

	pChannel->setCallback(&CHANNEL_CALLBACK); 
	pChannel->setUserData(this); // user data에 자신을 넣어줌

	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(_iRoopCount);

	m_listChannel.push_back(pChannel);

	int iIdx = -1;
	pChannel->getIndex(&iIdx);

	return iIdx;
}

void CSound::Stop()
{
	list<FMOD::Channel*>::iterator iter;

	while (!m_listChannel.empty())
	{
		iter = m_listChannel.begin();
		(*iter)->stop();				// stop시에도 콜백이 호출되어 리스트에서 제거됨
	}
}

void CSound::StopChannel(int channelIndex)
{
	for (auto iter = m_listChannel.begin(); iter != m_listChannel.end(); ++iter)
	{
		int iIdx = -1;
		(*iter)->getIndex(&iIdx);

		// 특정 인덱스의 채널을 찾았다면, 해당 채널을 중지하고 리스트에서 제거
		if (iIdx == channelIndex)
		{
			(*iter)->stop();
			break;
		}
	}
}

void CSound::SetVolume(float _f, int _iChannelIdx)
{
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();

	int iIdx = -1;
	for (; iter != m_listChannel.end(); ++iter)
	{
		(*iter)->getIndex(&iIdx);
		if (_iChannelIdx == iIdx)
		{
			(*iter)->setVolume(_f);
			return;
		}
	}
}

void CSound::RemoveChannel(FMOD::Channel* _pTargetChannel)
{
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();
	for (; iter != m_listChannel.end(); ++iter)
	{
		if (*iter == _pTargetChannel)
		{
			m_listChannel.erase(iter);
			return;
		}
	}
}






int CSound::Load(const wstring& _FilePath)
{
	// _FilePath(wchar_t*) -> UTF-8 (char*) 변환
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, _FilePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string path(bufferSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, _FilePath.c_str(), -1, &path[0], bufferSize, nullptr, nullptr);

	// FMOD::System::createSound 호출
	FMOD_RESULT result = CEngine::GetInst()->GetFMODSystem()->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_Sound);
	if (result != FMOD_OK)
	{
		// 에러 처리
		assert(nullptr);
	}

	return S_OK;
}

int CSound::Save(const wstring& _FilePath)
{
	return 0;
}









// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
	CSound* pSound = nullptr;

	switch (controltype)
	{
		// 채널이 종료되었을 때
		case FMOD_CHANNELCONTROL_CALLBACK_END:
		{
			cppchannel->getUserData((void**)&pSound);
			pSound->RemoveChannel(cppchannel);
		}

		break;
	}

	return FMOD_OK;
}