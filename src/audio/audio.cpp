/**
 * @file audio.cpp
 * @brief Audio implementation, and the one place where miniaudio is compiled.
 * @author DreGi0
 * @date October 3rd, 2026
 */

// miniaudio is a single header: this define makes THIS file contain its code.
// It must appear in exactly one .cpp of the whole project.
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "audio.h"

#include <cstdio>

namespace
{
    constexpr float ENGINE_VOLUME = 0.5f;
    constexpr ma_uint64 FADE_MS = 150;
}

namespace Stellar
{
    struct Audio::State
    {
        ma_engine engine {};
        ma_noise noise {};
        ma_sound engineSound {};
    };

    Audio::Audio() :
    m_state(std::make_unique<State>())
    {
        if (ma_engine_init(nullptr, &m_state->engine) != MA_SUCCESS)
        {
            std::fprintf(stderr, "[Audio] No sound device, running without sound\n");
            m_state.reset();
            return;
        }

        // Pink noise: random with a mix of low and mid tones, a steady roar that small speakers can play
        const ma_noise_config noiseConfig = ma_noise_config_init(ma_format_f32, 2, ma_noise_type_pink, 0, 1.0);
        ma_noise_init(&noiseConfig, nullptr, &m_state->noise);

        // No spatialization: it's the vessel's own engine, not a sound at a point in the world
        ma_sound_init_from_data_source(&m_state->engine, &m_state->noise, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &m_state->engineSound);
        ma_sound_set_volume(&m_state->engineSound, ENGINE_VOLUME);

        // Start playing but faded out to silence; setEngineBurning() fades it in and out
        ma_sound_set_fade_in_milliseconds(&m_state->engineSound, 0.0f, 0.0f, 0);
        ma_sound_start(&m_state->engineSound);
    }

    Audio::~Audio()
    {
        if (!m_state)
        {
            return;
        }

        // Reverse order of creation: the sound uses the noise and the engine
        ma_sound_uninit(&m_state->engineSound);
        ma_noise_uninit(&m_state->noise, nullptr);
        ma_engine_uninit(&m_state->engine);
    }

    void Audio::setEngineBurning(const bool burning)
    {
        if (!m_state || burning == m_burning)
        {
            return;
        }

        m_burning = burning;

        // -1 = fade from the current level, so quick taps don't jump
        ma_sound_set_fade_in_milliseconds(&m_state->engineSound, -1.0f, burning ? 1.0f : 0.0f, FADE_MS);
    }
} // Stellar