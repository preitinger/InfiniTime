#pragma once

// #include "prButtonTexts.hpp"
#include <concepts>
#include <string_view>
#include <cstdint>

namespace pr {

    template <typename T>
    concept Mode = requires(T t, const char* btnText) {
        { t.onShortClick(btnText) } -> std::same_as<void>;
        { t.onLongClick(btnText) } -> std::same_as<void>;
    };

    class GoMode {
      public:
        void onShortClick(const char* btnText);
        void onLongClick(const char* btnText);
    };

    static_assert(Mode<GoMode>);

    class ModeBase {
      public:
        const uint8_t id;
        const char* const* lvglMap;

        ModeBase(const char* const* map) : id(generateNextId()), lvglMap(map) {
            registerMode(this);
        }

        // --- DIE NEUEN EVENTS ---
        // 'virtual' erlaubt es den erbenden Klassen, diese Methoden zu überschreiben.
        virtual void onShortClick(const char* /* btnText */) {
        }

        virtual void onLongClick(const char* /* btnText */) {
        }

        static ModeBase* getFirst() {
            return firstMode;
        }

        ModeBase* getNext() const {
            return next;
        }

      private:
        ModeBase* next = nullptr;
        inline static ModeBase* firstMode = nullptr;
        inline static ModeBase* lastMode = nullptr;
        inline static uint8_t idCounter = 0;

        static uint8_t generateNextId() {
            return idCounter++;
        }

        static void registerMode(ModeBase* newMode) {
            if (firstMode == nullptr) {
                firstMode = newMode;
                lastMode = newMode;
            } else {
                lastMode->next = newMode;
                lastMode = newMode;
            }
        }
    };

    namespace {
        using namespace pr;
        static const char* mapGo[] = {tBw, tMod, tFw, ""};

        // --- MODUS GO ---
        class ModeGo : public ModeBase {
          public:
            ModeGo() : ModeBase(mapGo) {
            }

            // ModeGo reagiert nur auf den kurzen Klick
            void onShortClick(const char* btnText) override {
                if (std::string_view(btnText) == "Vorwaerts") {
                    // Motor vorwärts starten...
                }
            }
        };

        ModeGo modeGo __attribute__((used));

    }

} // namespace pr
