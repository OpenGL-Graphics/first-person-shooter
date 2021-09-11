#include "audio/audio.hpp"

/* Initialize irrKlang sound engine */
Audio::Audio():
  m_sound_engine(irrklang::createIrrKlangDevice())
{
}

/**
 * @param path Path to sound to play
 */
void Audio::play_2d(const std::string& path) {
  m_sound_engine->play2D(path.c_str());
}

/* Free sound engine */
void Audio::free() {
  m_sound_engine->drop();
}
