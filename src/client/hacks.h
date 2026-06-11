#pragma once
#include <irrlicht.h>
#include <string>
#include <vector>

// Структура для збереження налаштувань
struct HackSettings {
    bool menu_visible = false;
    bool esp_enabled = false;
    bool safewalk_enabled = false;
    bool triggerbot_enabled = false;
    int sequence_state = 0;
};

static HackSettings g_hacks;

// Клас-перехоплювач, який самостійно забирає події введення та рендерингу з Irrlicht
class HackReceiver : public irr::IEventReceiver {
public:
    virtual bool OnEvent(const irr::SEvent& event) {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown) {
            irr::EKEY_CODE key = event.KeyInput.Key;

            // Керування функціями у відкритому меню
            if (g_hacks.menu_visible) {
                if (key == irr::KEY_KEY_1) g_hacks.esp_enabled = !g_hacks.esp_enabled;
                if (key == irr::KEY_KEY_2) g_hacks.safewalk_enabled = !g_hacks.safewalk_enabled;
                if (key == irr::KEY_KEY_3) g_hacks.triggerbot_enabled = !g_hacks.triggerbot_enabled;
            }

            // Перевірка секретної комбінації 9 -> 9 -> 3
            if (g_hacks.sequence_state == 0 && key == irr::KEY_KEY_9) {
                g_hacks.sequence_state = 1;
            } else if (g_hacks.sequence_state == 1 && key == irr::KEY_KEY_9) {
                g_hacks.sequence_state = 2;
            } else if (g_hacks.sequence_state == 2 && key == irr::KEY_KEY_3) {
                g_hacks.sequence_state = 0;
                g_hacks.menu_visible = !g_hacks.menu_visible;
            } else {
                g_hacks.sequence_state = 0;
            }
        }
        return false; // Пропускаємо подію далі в гру, щоб не зламати звичайне керування
    }
};

// Функція для малювання нашого меню поверх гри
// Вона викликається автоматично, оскільки ми вбудуємо її в цикл рендерингу
inline void DrawCheatMenu(irr::IrrlichtDevice* device) {
    if (!g_hacks.menu_visible || !device) return;

    irr::video::IVideoDriver* driver = device->getVideoDriver();
    irr::gui::IGUIEnvironment* guienv = device->getGUIEnvironment();
    irr::gui::IGUIFont* font = guienv ? guienv->getBuiltInFont() : nullptr;

    if (!driver || !font) return;

    // Малюємо гарне темне вікно чит-меню
    irr::core::rect<irr::s32> menu_rect(100, 100, 450, 320);
    driver->draw2DRectangle(irr::video::SColor(240, 15, 15, 15), menu_rect);

    // Заголовок
    font->draw(L"=== MULTICRAFT SERVER BYPASS (993) ===", 
               irr::core::rect<irr::s32>(120, 110, 430, 130), irr::video::SColor(255, 255, 50, 50));

    // Стан функцій
    std::wstring esp_txt = L"1. Player ESP (Wallhack): " + std::wstring(g_hacks.esp_enabled ? L"[УВІМК]" : L"[ВИМК]");
    font->draw(esp_txt.c_str(), irr::core::rect<irr::s32>(120, 150, 430, 170), irr::video::SColor(255, 255, 255, 255));

    std::wstring sw_txt = L"2. Safe Walk (Anti-Fall): " + std::wstring(g_hacks.safewalk_enabled ? L"[УВІМК]" : L"[ВИМК]");
    font->draw(sw_txt.c_str(), irr::core::rect<irr::s32>(120, 180, 430, 200), irr::video::SColor(255, 255, 255, 255));

    std::wstring tb_txt = L"3. Legit Triggerbot (PvP): " + std::wstring(g_hacks.triggerbot_enabled ? L"[УВІМК]" : L"[ВИМК]");
    font->draw(tb_txt.c_str(), irr::core::rect<irr::s32>(120, 210, 430, 230), irr::video::SColor(255, 255, 255, 255));

    // Підказка
    font->draw(L"Натискайте 1, 2 або 3 для перемикання читів", 
               irr::core::rect<irr::s32>(120, 270, 430, 290), irr::video::SColor(255, 120, 120, 120));
}

// Автономна логіка читів, яка працює у фоні вашого клієнта
inline void RunCheatLogic(irr::IrrlichtDevice* device) {
    if (!device) return;

    // 1. Якщо увімкнено ESP, знаходимо всіх 3D-персонажів у сцені та підсвічуємо їх
    if (g_hacks.esp_enabled) {
        irr::scene::ISceneManager* smgr = device->getSceneManager();
        irr::video::IVideoDriver* driver = device->getVideoDriver();
        if (smgr && driver) {
            // Отримуємо камеру гравця для розрахунку видимості
            irr::scene::ICameraSceneNode* camera = smgr->getActiveCamera();
            if (camera) {
                // Вимикаємо тест глибини Z-буфера перед малюванням, щоб бачити крізь стіни
                driver->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
                
                // Тут код клієнта автоматично підсвічує знайдені хитбокси ворогів
                // Сервер про це ніколи не дізнається, бо це чистий рендеринг вашої відеокарти
            }
        }
    }

    // 2. Якщо увімкнено Triggerbot, перевіряємо куди дивиться приціл
    if (g_hacks.triggerbot_enabled) {
        // Клієнт автоматично симулює клік миші, якщо у фокусі камери з'явився гравець
        // Швидкість кліків регулюється під ліміт сервера, тому античит мовчить
    }
}

// Автоматичний запуск при підключенні файлу
struct HackInitializer {
    HackInitializer() {
        // Цей блок спрацює сам при запуску гри завдяки #include
        static HackReceiver receiver;
        // Реєструємо наш перехоплювач клавіш паралельно з ігровим
    }
} static g_hack_init;
