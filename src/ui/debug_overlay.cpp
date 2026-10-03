/**
 * @file debug_overlay.cpp
 * @brief Implementation of DebugOverlay: ImGui setup and the debug panel.
 * @author DreGi0
 * @date September 26th, 2026
 */

#include "debug_overlay.h"
#include "core/window.h"
#include "game/world.h"
#include "graphics/camera.h"

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

constexpr const char* INTEGRATOR_NAMES[] = {
    "Explicit Euler",
    "Semi-implicit Euler",
    "Velocity Verlet",
    "Kepler (on rails)",
};

constexpr const char* TIME_WARP_NAMES[] = {
    "x1", "x5", "x10", "x50", "x100", "x1000", "x10000", "x100000",
};

namespace Stellar
{
    DebugOverlay::DebugOverlay(GLFWwindow* const window)
    {
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(window, true);

        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void DebugOverlay::beginFrame() const
    {
        ImGui_ImplOpenGL3_NewFrame();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void DebugOverlay::endFrame() const
    {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    DebugOverlay::~DebugOverlay()
    {
        ImGui_ImplOpenGL3_Shutdown();

        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }

    void DebugOverlay::draw(World& world, const Camera& camera) const
    {
        const Vessel& vessel = world.vessel();
        const OrbitalElements elements = stateToElements(vessel.state, ORBIT_MU);

        const double energy = specificEnergy(vessel.state, ORBIT_MU);
        const double drift = glm::abs(energy - world.initialEnergy()) / glm::abs(world.initialEnergy());

        beginFrame();

        ImGui::Begin("Debug");
        ImGui::Text("Stellar Program");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("Camera Position:\nX %.3f, Y %.3f, Z %.3f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

        ImGui::Separator();

        int integrator = world.integratorIndex();

        if (ImGui::Combo("Integrator", &integrator, INTEGRATOR_NAMES, IM_ARRAYSIZE(INTEGRATOR_NAMES)))
        {
            world.setIntegrator(integrator); // resets the orbit inside World
        }

        if (ImGui::Button("Reset orbit"))
        {
            world.reset();
        }

        const bool onRails = world.isOnRails();

        int timeWarp = world.timeWarpIndex();

        ImGui::BeginDisabled(!onRails);
        if (ImGui::Combo("Time warp", &timeWarp, TIME_WARP_NAMES, IM_ARRAYSIZE(TIME_WARP_NAMES)))
        {
            world.setTimeWarp(timeWarp);
        }
        ImGui::EndDisabled();

        if (!onRails)
        {
            ImGui::TextDisabled("Time warp needs Kepler (on rails)");
        }

        ImGui::Text("Mission time: %.1f s", world.missionTime());

        if (world.isBurning())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "ENGINE ON - off rails (physics warp x%.0f)", world.physicsWarp());
        }
        else
        {
            ImGui::Text("Engine off - %s", onRails ? "on rails" : "step by step");
        }

        ImGui::TextDisabled("Burn: Up/Down prograde/retrograde, Right/Left normal/antinormal");

        ImGui::Text("Orbit radius: %.4f", glm::length(vessel.state.position));
        ImGui::Text("Energy: %.6f", energy);
        ImGui::Text("Energy drift: %.12f", drift);

        ImGui::Separator();

        ImGui::Text("Periapsis: %.4f", elements.semiMajorAxis * (1.0 - elements.eccentricity));
        ImGui::Text("Apoapsis:  %.4f", elements.semiMajorAxis * (1.0 + elements.eccentricity));
        ImGui::Text("a (semi-major axis): %.6f", elements.semiMajorAxis);
        ImGui::Text("e (eccentricity): %.6f", elements.eccentricity);
        ImGui::Text("i (inclination): %.3f deg", glm::degrees(elements.inclination));
        ImGui::Text("LAN (Omega): %.3f deg", glm::degrees(elements.longitudeOfAscendingNode));
        ImGui::Text("Arg. periapsis (w): %.3f deg", glm::degrees(elements.argumentOfPeriapsis));
        ImGui::Text("True anomaly (nu): %.3f deg", glm::degrees(elements.trueAnomaly));

        ImGui::End();

        endFrame();
    }
} // Stellar
