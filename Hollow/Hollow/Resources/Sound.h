#pragma once
#include <XAudio2.h>
#include "Hollow/Platform.h"

namespace Hollow {
	class HOLLOW_API Sound
	{
	public:
		Sound(IXAudio2SourceVoice* sourceVoice);
		~Sound();
		void Play();
	private:
		IXAudio2SourceVoice*		pSourceVoice;
	};
}
