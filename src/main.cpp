#include <cstdio>
#include <cstdlib>
#include <exception>

#include "core/application.h"

/**
 * @brief Entry point. Calls the app main loop.
 * @return EXIT_SUCCESS on clean shutdown, EXIT_FAILURE if initialization throws.
 */
int main() {
    try {
        Stellar::Application app;
        app.run();
    } catch (const std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
