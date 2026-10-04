/**
 * @file audio.h
 * @brief Game sound through miniaudio. For now, only the engine rumble while burning.
 * @author DreGi0
 * @date October 3rd, 2026
 */

#pragma once

#include <memory>

namespace Stellar
{
    /**
     * @class Audio
     * @brief Opens the sound device and plays the engine sound (generated noise, no audio file).
     *
     * If there is no sound device (e.g. on CI) it stays silent instead of failing.
     */
    class Audio
    {
    public:
        /**
         * @brief Opens the default sound device and starts the engine sound, silent.
         */
        Audio();

        /**
         * @brief Stops the sound and closes the device.
         */
        ~Audio();

        // miniaudio keeps pointers between its objects, so they must never be copied
        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;

        /**
         * @brief Fades the engine sound in or out when the burn starts or stops.
         * @param burning true while the vessel is burning.
         */
        void setEngineBurning(bool burning);

    private:
        struct State;                  ///< miniaudio objects; defined in audio.cpp so miniaudio.h stays out of the headers
        std::unique_ptr<State> m_state; ///< nullptr when there is no sound device
        bool m_burning = false;
    };
} // Stellar