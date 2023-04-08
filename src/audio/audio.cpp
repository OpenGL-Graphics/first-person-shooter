#include <iostream>

#include "audio/audio.hpp"
#include "fmod/fmod_errors.h"

/**
 * Init fmod and sounds to play
 * See example in: https://fmod.com/docs/2.02/api/white-papers-getting-started.html
 * And example in: <fmod-dir>/api/core/examples/play_sound.cpp
 */
Audio::Audio() {
  FMOD_RESULT result;

  result = FMOD::System_Create(&m_system);
  check_error(result);

  result = m_system->init(1, FMOD_INIT_NORMAL, 0);
  check_error(result);

  m_system->createSound("assets/audio/gun_shot.mp3", FMOD_DEFAULT, 0, &m_shot_sound);
  check_error(result);
}

/* Halt execution if fmod result returns an error */
void Audio::check_error(FMOD_RESULT result) {
  if (result != FMOD_OK) {
    std::cout << "FMOD error: " << FMOD_ErrorString(result) << '\n';
    std::exit(EXIT_FAILURE);
  }
}

/* Must be called in each frame of the main loop */
void Audio::update() {
  FMOD_RESULT result = m_system->update();
  check_error(result);
}

/* play shot sound init. in ctor */
void Audio::shot() {
  // m_sound_engine->play2D(path.c_str());
  FMOD_RESULT result = m_system->playSound(m_shot_sound, 0, false, 0);
  check_error(result);
}

/* Free sound engine */
void Audio::free() {
  m_shot_sound->release();
  m_system->release();
}
