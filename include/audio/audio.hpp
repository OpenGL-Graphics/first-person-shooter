#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "fmod/fmod.hpp"

/* Wrapper class around fmod sound engine for playing sounds */
class Audio {
public:
  Audio();
  void shot();
  void update();
  void free();

private:
  FMOD::System* m_system;
  FMOD::Sound* m_shot_sound;

  void check_error(FMOD_RESULT result);
};

#endif // AUDIO_HPP
