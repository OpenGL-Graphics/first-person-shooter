#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <string>

#include "irrklang/irrKlang.h"

/* Wrapper class around IrrKlang's sound engine for playing sounds */
class Audio {
public:
  Audio();
  void play_2d(const std::string& path);
  void free();

private:
  irrklang::ISoundEngine* m_sound_engine;
};

#endif // AUDIO_HPP
