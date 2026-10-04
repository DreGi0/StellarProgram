# Stellar Program — Roadmap por pasos

Estado al 4 de octubre de 2026. Repo: `DreGi0/StellarProgram`, rama `main` (último commit: `c04b32f`, bump del manifest a 0.3.0). Releases: **`v0.1.0-alpha`** (base del motor), **`v0.2.0-alpha`** (mecánica orbital, fase 4), **`v0.2.1-alpha`** (refactor y limpieza, fase 4.5), **`v0.3.0-alpha`** (esqueleto del juego, fase 5). En curso: **fase 6**, paso 21.

## Stack actual

- C++20, CMake 3.28 + `CMakePresets.json` (preset `debug`, Ninja), CLion en Arch Linux
- vcpkg en manifest mode (`builtin-baseline e0a6b857…`): glfw3, glad (GLAD 1, gl-api-46), glm 1.0.3, catch2 v3, imgui 1.92 (glfw + opengl3 bindings), miniaudio
- Targets: `StellarEngine` (librería estática, todo `src/` menos `main.cpp`), `StellarProgram` (ejecutable), `StellarTests` (Catch2 + CTest)
- Warnings: `-Wall -Wextra -Wpedantic` en un target `INTERFACE` (`stellar_warnings`) enlazado `PRIVATE` a los tres targets
- CI: GitHub Actions (`.github/workflows/ci.yml`) en ubuntu-latest, vcpkg vía `lukka/run-vcpkg` + caché binaria propia (`VCPKG_BINARY_SOURCES` con `files` + `actions/cache`, porque vcpkg eliminó `x-gha`), `ctest` en cada PR. Ruleset en `main`: PR obligatorio + check `build-and-test` requerido
- Flujo: rama corta por paso → PR → merge a `main` con CI en verde (al hacer squash, editar el mensaje para que siga Conventional Commits)

## Estructura de `src/`

| Carpeta | Qué hay | Depende de |
|---|---|---|
| `core/` | `Application` (orquestador: dueño de todo, loop de paso fijo), `Window`, `Input` (acciones de teclado + gamepad), `paths` | todo |
| `audio/` | `Audio` (miniaudio, sonido del motor) | miniaudio |
| `game/` | `World` (cuerpo central, `Vessel`, reloj de misión, time warp, quemado) | solo `physics/` — sin OpenGL/GLFW/ImGui, testeable sin ventana |
| `physics/` | `orbit`: integradores, elementos orbitales, Kepler (funciones libres) | solo GLM |
| `graphics/` | `GraphicsContext`, `Shader`, `Mesh`, `Camera`, `OrbitCamera`, `gl_debug`; `primitives` (formas generadas como datos, sin OpenGL) | OpenGL |
| `ui/` | `DebugOverlay` (RAII de ImGui + `draw(World&, cameraPosition)` con el panel de debug) | ImGui, `game/` |

## Convenciones

| Qué | Estilo |
|---|---|
| Tipos | `PascalCase` |
| Funciones, métodos, variables | `camelCase` |
| Miembros | `m_camelCase` |
| Constantes globales | `UPPER_SNAKE` |
| Archivos | `snake_case` |

- Commits: Conventional Commits (`feat:`, `fix:`, `refactor:`, `build:`, `test:`, `ci:`, `style:`, `docs:`)
- Documentación: Doxygen en los headers (`@brief`, `@param`, `@return`, `///<` para miembros); el `.cpp` solo lleva el `@brief` de cabecera
- Todo OpenGL pasa por GLAD (`GLFW_INCLUDE_NONE` global; nunca `<GL/gl.h>`)
- Posiciones de mundo en `glm::dvec3`; a `float` solo después de restar la posición de la cámara
- Física en `src/physics/`: funciones libres, sin dependencias de OpenGL/GLFW/ImGui (testeables sin ventana). Lo mismo para `src/game/`
- Marco orbital como en los libros: plano de referencia XY, +Z "arriba", Ω medido desde +X
- Input: `Application` pide `Action`s (`m_input.axis(Action::…)`, `wasPressed(Action::…)`), nunca teclas. Las teclas y ejes del mando viven solo en la tabla `BINDINGS` de `input.cpp`

### Versionado (SemVer, semver.org) — release por hito

- **Release por hito, no por fase**: una o varias fases se agrupan en un release cuando juntas forman algo que se siente como avance
- **Minor** = release con funcionalidad nueva, sin importar su tamaño
- **Patch** = solo correcciones o cambios internos sin funcionalidad nueva (p. ej. fase 4.5 → `v0.2.1-alpha`)
- Los números no son decimales: después de `0.9` viene `0.10`, no `1.0`
- La versión mide lo que cambió en el código, no qué tan "completo" se siente el juego

**Checklist de release** (aprendido en `v0.3.0-alpha`, donde el bump del manifest entró después del tag): bump de `version` en `vcpkg.json` (o quitar el campo, es opcional) → README al día (status, stack, controles) → merge con CI en verde → tag → GitHub Release.

| Release | Fases | Hito |
|---|---|---|
| `v0.1.0-alpha` ✅ | 1–3 | Base del motor |
| `v0.2.0-alpha` ✅ | 4 | Mecánica orbital |
| `v0.2.1-alpha` ✅ | 4.5 | Refactor y limpieza |
| `v0.3.0-alpha` ✅ | 5 | Esqueleto del juego |
| `v0.4.0-alpha` | 6 + 7 | Una Tierra con base; objetos que se apoyan y chocan |
| `v0.5.0-alpha` | 8 + 9 | Despegar a través de la atmósfera |
| `v0.6.0-alpha` | 10 + 11 | Naves reales: combustible, staging, prefabs |
| `v0.7.0-alpha` | 12 | La Luna |

---

## Decisiones de diseño

### Un solo planeta hasta la fase 11
Toda la física (suelo, despegue, atmósfera, combustible, staging) se construye y valida en la Tierra. La fase 12 extiende a más cuerpos.

Principio para que la extensión sea barata: **un cuerpo celeste es datos, no una subclase.** La Tierra y la Luna se comportan igual (gravedad, radio, rotación, atmósfera opcional); cambian sus números. Una sola clase `CelestialBody` con sus parámetros, sin herencia ni interfaz, salvo que aparezca un comportamiento realmente distinto.

### Escala: parámetro de datos; 1:10 para desarrollar, decisión final tras probar
Referencia KSP: Kerbin mide 600 km de radio (Tierra: 6371 km, ~1:10.6), pero conserva g = 9.81 m/s² en superficie → el planeta es mucho más denso que la Tierra (μ = g·R²). Consecuencias en KSP: velocidad orbital baja ~2.3 km/s (vs. ~7.8), Δv a órbita ~3.4 km/s (vs. ~9.4). Las distancias orbitales en KSP están comprimidas mucho más que 1:10 (la Mun está a 12 000 km; la Luna real a 384 400 km, ~1:32).

Fijo:
- g de superficie real (el peso se siente real); μ = g·R² por cuerpo
- Unidades SI internamente (metros, segundos, kg)
- **La escala es un parámetro de datos, no algo cableado en el código**
- Los datos reales (NASA/JPL) se usan como base y se les aplica el factor de escala

Por decidir — **decisión final de escala tras probar en la fase 8, antes de balancear piezas en la fase 10**:
- Se desarrolla en **1:10** (fases 7 y 8)
- En la fase 8, ya con despegue, probar 1:10, ~1:3 (rango de mods de reescala populares como JNSQ, ~2.5×–3.2× KSP) y 1:1, y decidir por cómo se siente
- Debe quedar fijada antes de la fase 10: los números de las piezas (empuje, masa, combustible) solo tienen sentido para una escala concreta; cambiarla después obliga a rebalancearlas todas
- Trade-offs: escala mayor = ascensos más largos (~3–4 min en KSP vs. ~8–9 min en la Tierra real), más dependencia del time warp, piezas con proporciones realistas se ven "correctas", logística entre cuerpos más cara (a favor de la visión de recursos/hábitats), más superficie procedural que llenar. Escala mayor hace el juego más *largo*, no necesariamente más *interesante*; otras palancas de dificultad: Isp, masa seca de tanques, aerodinámica, recursos limitados
- Intuición inicial: ~1:3

### Nave como un solo cuerpo rígido
Para evitar el tambaleo/rotura de KSP en physics warp: la nave es un cuerpo rígido cuya masa, empuje y arrastre salen de sumar sus piezas; sin uniones físicas entre piezas.

### `World` es la simulación concreta, no una "escena"
`World` = estado vivo de la simulación (qué existe, dónde está, cómo avanza el tiempo), al estilo del `UWorld` de Unreal, no una escena de Unity (unidad de carga de contenido). Hoy contiene un cuerpo central (constantes `ORBIT_MU` / `CENTRAL_BODY_POSITION`, reemplazadas por `CelestialBody` en la fase 7) y un `Vessel`. En la fase 6 pasa a tener una lista de `GameObject` (la nave es uno de ellos). Si aparecen estados de juego (hangar, vuelo), el estado "vuelo" contendría un `World`, no al revés.

### `World` guarda datos, no mallas
`src/game/` no depende de OpenGL, así que un `GameObject` no puede tener una `Mesh`. Guarda datos (forma y tamaño del collider); `Application` decide con qué malla dibujarlo.

### Colisiones: propias y simples, sin respuesta rotacional
Fase 6: esfera–planeta, esfera–esfera y esfera–caja estática orientada. Fuera: caja–caja (OBB vs OBB con SAT) y que los objetos giren o se vuelquen al chocar (tensor de inercia, torque) — la nave es un solo cuerpo rígido aproximado por una esfera, y ese nivel de física es terreno de Jolt (mucho después). Lo difícil de la fase no son las formas sino el **contacto en reposo**: quieto sobre la superficie sin temblar, hundirse ni rebotar.

---

## ✅ Hecho

### Fase 1 — Base
- Ventana GLFW + contexto OpenGL 4.6 core (`Window`, RAII)
- `Shader` (compilación, cache de uniforms, move-only), `assetPath()` relativo al ejecutable
- Rewrite limpio del proyecto viejo (`apogee`) → `StellarProgram` con vcpkg

### Fase 2 — Rendering 3D
| PR | Rama | Qué |
|---|---|---|
| #1 | `feat/glm-mvp` | GLM + pipeline Model-View-Projection, fix de aspect ratio al minimizar |
| #2, #3 | `feat/cube-depth` | Cubo 3D, depth testing, debug output de OpenGL (`STELLAR_DEBUG` en Debug), `Mesh` con EBO (24 vértices + 36 índices) |
| #4 | `feat/camera` | Clase `Camera` free-look: WASD, mouse (yaw/pitch, pitch limitado a ±89°), movimiento con delta time |

### Fase 3 — Núcleo del motor → `v0.1.0-alpha`
| PR | Rama | Qué |
|---|---|---|
| #5 | `refactor/application` | `GraphicsContext` (carga de GLAD), clase `Application` con miembros en orden RAII, loop dividido en `processInput` / `render` |
| #6 | `style/naming-convention` | Convención de nombres unificada |
| #7 | `feat/fixed-timestep` | Loop de simulación a paso fijo (1/60 s) con acumulador, límite anti "espiral de la muerte" (0.25 s), interpolación de render |
| #8 | `build/testing` | `StellarEngine` como librería, Catch2 + CTest, tests de `Camera`, CI en GitHub Actions |
| #9 | `feat/double-precision` | Posiciones en `dvec3`, render relativo a la cámara (View solo con rotación), test de precisión a 10⁷ unidades |
| #10 | `feat/imgui-overlay` | `DebugOverlay` (RAII sobre Dear ImGui), FPS / posición de cámara / ángulo, Tab libera/captura el cursor |

### Fase 4 — Mecánica orbital → `v0.2.0-alpha`
Inicio: 26 sep 2026. Desde aquí se construye el juego en sí.

| Paso | Rama | Qué |
|---|---|---|
| 10 | `feat/integrators` (PR #11) | Gravedad de un cuerpo central; Euler explícito vs. semi-implícito vs. Verlet; test de deriva de energía |
| 11 | `feat/orbital-elements` (PR #12) | Vector de estado ↔ elementos orbitales (a, e, i, Ω, ω, ν); test de ida y vuelta (incl. Curtis Ex. 4.3) |
| 12 | `feat/kepler-propagation` | Propagación "on-rails" resolviendo la ecuación de Kepler; Kepler en el selector de integrador |
| 13 | `feat/orbit-rendering` | `orbitEllipseMatrix`, `Mesh` con primitivas de línea, línea de la órbita |
| 14 | `feat/time-warp` | Time warp sobre Kepler + reloj de misión |
| 15 | `feat/vessel-thrust` | Empuje y dirección de quemado en la física; flechas para quemar (prograde/retrograde, normal/antinormal) saliendo y volviendo a los rieles; physics warp con sub-stepping mientras se quema |

Medido en paso 10 (μ = 1000, r = 10, dt = 1/60, 10 vueltas): Euler explícito ~49 % de deriva; semi-implícito ~2.8·10⁻⁴; Verlet ~1.9·10⁻⁸. En paso 11 con Verlet ω precesaba ~−0.03°/vuelta → motivó Kepler.

Nota: los controles de quemado del paso 15 son **andamiaje de prueba**; los controles reales estilo KSP van en la fase 8.

### Fase 4.5 — Refactor y limpieza → `v0.2.1-alpha`
27 sep – 3 oct 2026. **Regla cumplida: ningún comportamiento nuevo.**

| Paso | Rama / PR | Qué |
|---|---|---|
| 4.5.5 | `chore/infra-cleanup` (rebase) | Docker eliminado; warnings compartidos vía `stellar_warnings` (ahora también en tests); caché binaria de vcpkg en CI; CI obligatorio en `main`. La rama `feat/vessel-thrust` ya estaba borrada |
| 4.5.1 | `refactor/input` (#18) | Clase `Input`: `isDown`, `axis(positiva, negativa)` → −1/0/1, `wasPressed` genérico, `mouseDelta` (Y invertida dentro), `setCursorCaptured` con resincronización del mouse. `processInput` bajó de ~60 a ~15 líneas |
| 4.5.2 | `refactor/world` (#19) | Clase `World` + `struct Vessel` en `src/game/`; `Application` queda como orquestador. Primer test de algo no-físico: `world_tests.cpp` (estado inicial, warp limitado al quemar, reset al cambiar de integrador) |
| 4.5.3 | `refactor/overlay` (#20) | El panel de ImGui sale de `render()` a `DebugOverlay::draw(World&, const Camera&)`; `beginFrame`/`endFrame` privados; `application.cpp` ya no incluye ImGui |
| 4.5.4 | `docs/briefs` (#21) | Doxygen en `Input`, `World`, `orbit.h`, `Application`, `Camera`, `GraphicsContext`; ningún `@brief` vacío |

Límites conocidos que quedaron documentados en el código:
- ~~`Input::wasPressed` solo funciona llamado una vez por frame por tecla~~ → resuelto en el paso 16: `Input::update()` toma una foto por frame y `wasPressed` compara contra la anterior
- El selector de integrador en `World` es **herramienta de laboratorio** del paso 10; se va cuando Kepler/Verlet sean el único camino (fase 8)

### Fase 5 — Esqueleto del juego → `v0.3.0-alpha`
3–4 oct 2026. Tag `v0.3.0-alpha` sobre `c1d2555`; después, `c04b32f` (#26) subió `vcpkg.json` a 0.3.0 (el tag quedó con `0.1.0`; no se mueve un tag publicado → checklist de release).

| Paso | Qué | Notas |
|---|---|---|
| 16 ✅ | Gamepad en `Input` (`feat/gamepad-input`, #22) | `enum class Action` + tabla `BINDINGS` en `input.cpp` (tecla positiva/negativa, eje del mando, signo). `Input::update()` una vez por frame en `run()`: lee teclado y `glfwGetGamepadState(GLFW_JOYSTICK_1)`, se queda con el valor de mayor magnitud; zona muerta 0.15. Stick izquierdo = quemado (Y con signo −1 porque GLFW da +1 abajo). Sin callback de conexión (el polling ya cubre conectar/desconectar), solo el primer mando, la cámara free-look no usa el mando. **No probado con un mando físico** (teclado y ratón verificados). Con el stick a medias se quema a fondo igual (`burnDirection` normaliza) hasta el throttle de la fase 8 |
| 17 ✅ | Esfera generada por código + iluminación (`feat/sphere-lighting`, #23) | `src/graphics/primitives.{h,cpp}`: `MeshData` (vértices + índices, sin OpenGL → testeable) con `unitCircle(segments)` y `unitSphere(stacks, slices)` (esfera UV radio 1, polos en ±Z, costura duplicada para texturas futuras); `Mesh(const MeshData&, primitive)`; `primitives_tests.cpp`. Shader `lit` (Blinn-Phong: ambiente 0.05 + difusa + especular 32) para las esferas; `triangle` queda para la línea de órbita. **Atajos conocidos:** la normal sale de `aPos` (solo vale para la esfera unidad, escala uniforme) — atributo de normal cuando lleguen modelos `.gltf`; luz direccional fija `uLightDir = (1, 0.5, 0.3)` hasta que exista el Sol; sin face culling |
| ~~18~~ | ~~Profundidad a escala planetaria~~ | **Movido a la fase 7**: hoy la escena mide ~20 unidades y no hay forma de ver ni probar el problema |
| 19 ✅ | Cámara orbital alrededor de la nave (`feat/orbit-camera`, #24) | `src/graphics/orbit_camera.{h,cpp}`: distancia + yaw/pitch (pitch ±89°, distancia 1.5–500), no guarda el objetivo (`offset()` se suma a la posición de la nave cada frame). `setFrame(posición, velocidad)`: marco local estilo modo *Orbital* de KSP. **C** alterna con la free-look (debug). Ratón / stick derecho giran, W/S hacen zoom (`exp`). `DebugOverlay::draw` recibe la posición de la cámara activa. El mensaje del squash quedó como `eat:` (typo; no vale reescribir `main`) |
| 20 ✅ | Audio con **miniaudio** (`feat/engine-audio`, #25) | `src/audio/audio.{h,cpp}`: clase `Audio` con pimpl (`miniaudio.h` solo en `audio.cpp`). Ruido **rosa** generado (`ma_noise`; el browniano se cortaba en bocinas de laptop). Sin espacialización, fade de 150 ms (`setEngineBurning`). Sin dispositivo de audio → sigue sin sonido. CMake: `find_path` + include `SYSTEM` + `${CMAKE_DL_LIBS}` |

Pendientes menores detectados al cerrar (hacer cuando se toque el archivo): README desactualizado (status, stack sin GLM/ImGui/miniaudio, controles); `application.cpp`: `glm::dvec3(10'000'000.0, 0.0f, 30.0f)` mezcla literales `float`, comentario `// MODE - Satellite`.

---

## 🔨 Fase 6 — GameObjects y colliders → (release con la fase 7)
Inicio: 4 oct 2026.

**Termina cuando:** hay varios objetos en el mundo que chocan entre sí sin atravesarse y se apoyan en el planeta y sobre cajas estáticas, y la nave se queda quieta en el suelo hasta que su empuje supera su peso.

- *Objeto* = algo que existe en el mundo; *sistema* = código que actúa sobre todos (física, colisiones, render)
- Sin ECS completo (EnTT): resuelve problemas de miles de entidades que no existen aquí
- Ver *Decisiones de diseño*: `World` guarda datos, no mallas; colisiones sin respuesta rotacional

| Paso | Rama | Qué |
|---|---|---|
| 21 | `refactor/game-objects` | `struct GameObject` en `world.h` (estado, posición previa, `radius`) y `std::vector<GameObject>` dentro de `World`; la nave es `m_objects[0]` y el `burn` pasa a `World`. `render()` dibuja todos en un loop. **Sin comportamiento nuevo** |
| 22 | `feat/sphere-collisions` | Collider esfera; detección objeto–planeta y objeto–objeto; resolución: separar + anular la velocidad relativa hacia adentro. Un segundo objeto de prueba |
| 23 | `feat/ground-contact` | Contacto en reposo (estado `landed`, sin temblar ni hundirse); quemado radial "arriba"; protección para v = 0 en `burnDirection` (hoy `normalize(0)` → NaN); slider de empuje en el `DebugOverlay` (con unidades de juguete g = μ/R² = 1000 y el empuje vale 1 → nunca despegaría). Despega solo si empuje > g |
| 24 | `feat/box-collider` | Orientación en `GameObject` (entra aquí porque es cuando algo la usa); caja orientada estática; esfera–caja. Prueba: una caja sobre el planeta y la nave aterrizando encima |

## Fase 7 — La Tierra y la base → release `v0.4.0-alpha` (fases 6 + 7)
**Termina cuando:** hay un planeta a escala con una plataforma de lanzamiento y un hangar, y la nave está parada en el suelo.

- Escala de desarrollo 1:10 (ver *Decisiones de diseño*): unidades SI, g real, factor de escala como dato
- `CelestialBody` como datos (radio, μ, rotación); reemplaza `ORBIT_MU` / `CENTRAL_BODY_POSITION` de `world.h`
- Base: plataforma de lanzamiento y hangar como cajas estáticas del mundo (los colliders ya existen desde el paso 24; aquí solo se colocan)
- **Profundidad a escala planetaria** (ex paso 18): con el planeta a 1:10 el far plane de `100.0f` deja de alcanzar. Opción simple: profundidad logarítmica (escribir `gl_FragDepth` en los shaders, funciona con el depth buffer por defecto). Alternativa: reverse-Z con `glClipControl` + depth `GL_DEPTH_COMPONENT32F` en un FBO propio
- Pendiente de pensar: rotación del planeta y marco de referencia de la superficie

## Fase 8 — Nave controlable → (release con la fase 9)
**Termina cuando:** despegas desde la plataforma.

Referencia: los controles básicos de KSP.
- **Orientación de la nave** (rotación, cuaterniones): el empuje sale a lo largo del eje de la nave. Un "SAS" con bloqueo a prograde/normal reproduciría los controles de prueba actuales
- **Throttle persistente** (0–100 %), como una barra de volumen y no un pedal: subir/bajar gradual + atajos a máximo / cortar. `Input` da "subir/bajar"; el valor del throttle vive en la simulación
- Masa total **fija** (necesaria desde ya: a = F/m para empuje y, luego, drag)
- Quitar el selector de integrador (andamiaje del paso 10)
- **Prueba de escala**: con despegue funcionando, probar 1:10, ~1:3 y 1:1 y fijar la escala final

## Fase 9 — Atmósfera → release `v0.5.0-alpha` (fases 8 + 9)
**Termina cuando:** el aire frena la nave de forma distinta según la altura.

- Densidad del aire en función de la altitud
- Drag (arrastre)
- Opcionales: viento (KSP no lo tiene), calentamiento por reentrada

## Fase 10 — Masa por pieza y combustible → (release con la fase 11)
**Termina cuando:** la nave se aligera mientras quema.

**Requisito: escala final ya fijada** (el balance de piezas depende de ella).

- Masa total = suma de las piezas (sigue siendo un solo cuerpo rígido)
- Combustible que se consume; ecuación de Tsiolkovsky
- Piezas con propiedades (masa, arrastre, empuje, capacidad de combustible)
- **Motores con y sin throttle**: líquidos regulables y apagables; sólidos (SRB) a empuje fijo que no se apagan una vez encendidos

## Fase 11 — Staging, prefabs y dev mode → release `v0.6.0-alpha` (fases 10 + 11)
**Termina cuando:** puedes elegir una nave predefinida en el hangar, lanzarla y separar etapas.

- **Staging** con la barra espaciadora (encender la siguiente etapa, separar) — una `Action` nueva + `Input::wasPressed`; en el mando hará falta un binding de **botón** (hoy `BINDINGS` solo tiene ejes)
- **Prefabs** de naves definidos como datos (un JSON por nave); el hangar sirve para *elegir* uno antes de lanzar
- Dev mode para probar cosas específicas de cada pieza
- El futuro editor de naves solo tendrá que escribir ese mismo archivo → extensión natural, no un sistema aparte

## Fase 12 — La Luna y el sistema solar → release `v0.7.0-alpha`
- Varios cuerpos (misma clase `CelestialBody`, otros datos)
- Esferas de influencia (patched conics, estilo KSP)
- Los planetas también van "on rails" alrededor del Sol (el Kepler de la fase 4)
- Validar precisión a escala de sistema solar con un test como el de 10⁷

---

## Visión: lo que KSP no tiene (sin ordenar)

Jugabilidad que depende de que existan naves, piezas y varios cuerpos:
- Satélites y estaciones funcionales (redes de comunicación, etc.)
- Hábitats
- Recolección de recursos
- Elementos de supervivencia
- Superficies procedurales (Perlin) sobre datos orbitales reales

## Después (sin ordenar)

- Editor de naves (construir sobre los prefabs en JSON de la fase 11)
- Texturas; carga de modelos `.gltf` (cgltf / tinygltf) para piezas; imágenes con stb_image
- Texto en pantalla (FreeType) si ImGui deja de alcanzar como HUD
- Guardar / cargar, menús, estados del juego (vuelo, construcción)
- Face culling (los triángulos del cubo ya están en sentido antihorario)
- Logger con `std::format` en lugar de `fprintf` — diferido hasta que moleste
- `POST_BUILD` que copia `assets/` solo corre si el ejecutable se re-enlaza
- Input: remapeo de controles, varios mandos, callback de conexión (`glfwSetJoystickCallback`) — cuando haga falta

### Librerías de referencia (cuándo hagan falta)

| Área | Opción |
|---|---|
| Ventana + input + gamepad | GLFW (actual). Alternativa todo-en-uno: SDL3 — no vale la pena cambiar ahora |
| Audio | miniaudio / OpenAL Soft |
| Imágenes | stb_image |
| Modelos | cgltf / tinygltf |
| JSON (prefabs) | nlohmann-json |
| Texto | FreeType |
| Colisiones entre piezas | Jolt (mucho después; la física orbital es propia) |
