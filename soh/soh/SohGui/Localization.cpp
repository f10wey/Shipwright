#include "Localization.h"

#include <unordered_map>

namespace SohLoc {

// English menu text -> Russian. Keys must match the strings used in the SohMenu*.cpp files exactly.
static const std::unordered_map<std::string, const char*> sRussian = {
    // ---- Header / sidebar ----
    { "Settings", "Настройки" },
    { "Enhancements", "Улучшения" },
    { "Randomizer", "Рандомайзер" },
    { "Network", "Сеть" },
    { "Dev Tools", "Разработчику" },
    { "General", "Общие" },
    { "Audio", "Звук" },
    { "Graphics", "Графика" },
    { "Controls", "Управление" },
    { "Notifications", "Уведомления" },
    { "Mod Menu", "Моды" },
    { "Search", "Поиск" },
    { "Quality of Life", "Удобство" },
    { "Skips & Speed-ups", "Пропуски и ускорения" },
    { "Items", "Предметы" },
    { "Fixes", "Исправления" },
    { "Difficulty", "Сложность" },
    { "Minigames", "Мини-игры" },
    { "Extra Modes", "Доп. режимы" },
    { "Cheats", "Читы" },
    { "Cosmetics Editor", "Внешний вид" },
    { "Audio Editor", "Редактор звука" },
    { "Gameplay Stats", "Статистика" },
    { "Time Splits", "Сплиты" },
    { "Timers", "Таймеры" },
    { "Quit SoH", "Выйти из игры" },
    { "Reset", "Перезапуск" },
    { "Close Menu (Esc)", "Закрыть меню (Esc)" },
    { "Clear Search", "Очистить поиск" },

    // ---- Common combobox values ----
    { "Small", "Маленький" },
    { "Normal", "Обычный" },
    { "Large", "Большой" },
    { "X-Large", "Очень большой" },
    { "Red", "Красная" },
    { "Dark Red", "Тёмно-красная" },
    { "Orange", "Оранжевая" },
    { "Green", "Зелёная" },
    { "Dark Green", "Тёмно-зелёная" },
    { "Light Blue", "Голубая" },
    { "Blue", "Синяя" },
    { "Dark Blue", "Тёмно-синяя" },
    { "Indigo", "Индиго" },
    { "Violet", "Фиолетовая" },
    { "Purple", "Пурпурная" },
    { "Brown", "Коричневая" },
    { "Gray", "Серая" },
    { "Dark Gray", "Тёмно-серая" },
    { "Three-Point", "Трёхточечный" },
    { "Linear", "Линейный" },
    { "None", "Нет" },
    { "Top Left", "Слева вверху" },
    { "Top Right", "Справа вверху" },
    { "Bottom Left", "Слева внизу" },
    { "Bottom Right", "Справа внизу" },
    { "Hidden", "Скрыто" },
    { "Default", "По умолчанию" },
    { "Authentic", "Как в оригинале" },
    { "File Select", "Выбор файла" },
    { "Debug Warp Screen", "Отладочный телепорт" },
    { "Warp Point", "Точка телепорта" },
    { "Always", "Всегда" },
    { "Once", "Один раз" },
    { "Never", "Никогда" },
    { "Off", "Выкл." },
    { "Only in Rando", "Только в рандомайзере" },

    // ---- Settings -> General ----
    { "Menu Settings", "Меню" },
    { "Menu Theme", "Цвет меню" },
    { "Changes the Theme of the Menu Widgets.", "Меняет цветовую тему элементов меню." },
    { "Menu Controller Navigation", "Управление меню с геймпада" },
    { "Allows controller navigation of the port menu (Settings, Enhancements,...)\nCAUTION: This will disable game "
      "inputs while the menu is visible.\n\nD-pad to move between items, A to select, B to move up in scope.",
      "Позволяет управлять меню (Настройки, Улучшения...) с геймпада.\nВНИМАНИЕ: пока меню открыто, игра не "
      "реагирует на кнопки.\n\nКрестовина — перемещение, A — выбор, B — назад." },
    { "Allow background inputs", "Ввод в фоновом режиме" },
    { "Allows controller inputs to be picked up by the game even when the game window isn't the focused window.",
      "Игра принимает нажатия геймпада, даже когда её окно не активно." },
    { "Menu Background Opacity", "Непрозрачность фона меню" },
    { "Sets the opacity of the background of the port menu.", "Насколько затемняется фон под меню." },
    { "General Settings", "Общие настройки" },
    { "Cursor Always Visible", "Всегда показывать курсор" },
    { "Makes the cursor always visible, even in full screen.", "Курсор мыши виден всегда, даже в полноэкранном режиме." },
    { "Search In Sidebar", "Поиск в боковой панели" },
    { "Displays the Search menu as a sidebar entry in Settings instead of in the header.",
      "Показывает поиск пунктом в боковой панели настроек, а не в верхней строке." },
    { "Search Input Autofocus", "Автофокус на поиске" },
    { "Search input box gets autofocus when visible. Does not affect using other widgets.",
      "Поле поиска сразу активно, когда появляется. На остальные элементы не влияет." },
    { "Reset Button Combination:", "Сочетание кнопок для перезапуска:" },
    { "Open App Files Folder", "Открыть папку игры" },
    { "Opens the folder that contains the save and mods folders, etc.",
      "Открывает папку с сохранениями, модами и прочими файлами." },
    { "Boot", "Запуск" },
    { "Boot Sequence", "Порядок запуска" },
    { "Configure what happens when starting or resetting the game.\n\nDefault: LUS logo -> N64 logo\nAuthentic: N64 "
      "logo only\nFile Select: Skip to file select menu\nDebug Warp Screen: Skip to the debug warp screen\nWarp Point: "
      "Skip to active warp point (if set), see Dev Tools -> General",
      "Что происходит при запуске и перезапуске игры.\n\nПо умолчанию: логотип LUS -> логотип N64\nКак в оригинале: "
      "только логотип N64\nВыбор файла: сразу к выбору сохранения\nОтладочный телепорт: сразу к отладочному "
      "экрану телепорта\nТочка телепорта: сразу к заданной точке (если есть), см. Разработчику -> Общие" },
    { "Languages", "Языки" },
    { "Translate Title Screen", "Переводить титульный экран" },
    { "Language", "Язык" },
    { "Accessibility", "Специальные возможности" },
    { "Text to Speech", "Озвучка текста" },
    { "Enables text to speech for in-game dialogue", "Синтезатор речи зачитывает диалоги в игре." },
    { "Disable Idle Camera Re-Centering", "Не центрировать камеру в покое" },
    { "Disables the automatic re-centering of the camera when idle.",
      "Камера не возвращается за спину Линка сама, когда он стоит на месте." },
    { "Disable Screen Flash for Finishing Blow", "Без вспышки при добивании" },
    { "Disables the white screen flash on enemy kill.", "Убирает белую вспышку экрана при убийстве врага." },
    { "Disable Jabu Wobble", "Без качки внутри Джабу-Джабу" },
    { "Disable the geometry wobble and camera distortion inside Jabu.",
      "Убирает колыхание стен и искажение камеры внутри Джабу-Джабу." },
    { "Disable Heat Haze", "Без марева от жара" },
    { "Disable the heat haze distortion effect in Death Mountain / Fire Temple.",
      "Убирает дрожание воздуха на Горе Смерти и в Храме Огня." },
    { "EXPERIMENTAL", "ЭКСПЕРИМЕНТАЛЬНОЕ" },
    { "ImGui Menu Scaling", "Масштаб меню" },
    { "Changes the scaling of the ImGui menu elements.", "Меняет размер элементов меню." },
    { "About", "О программе" },
    { "Branch: ", "Ветка: " },
    { "Commit: ", "Коммит: " },

    // ---- Settings -> Audio ----
    { "Audio API (Needs reload)", "Звуковой API (нужен перезапуск)" },
    { "Sets the audio API used by the game. Requires a relaunch to take effect.",
      "Какой звуковой API использует игра. Применится после перезапуска." },
    { "Only one audio API is available on this platform.", "На этой системе доступен только один звуковой API." },

    // ---- Settings -> Graphics ----
    { "Graphics Options", "Параметры графики" },
    { "Toggle Fullscreen", "Полный экран" },
    { "Toggles Fullscreen On/Off.", "Включает и выключает полноэкранный режим." },
    { "Internal Resolution", "Внутреннее разрешение" },
    { "Multiplies your output resolution by the value inputted, as a more intensive but effective form of "
      "anti-aliasing.",
      "Умножает разрешение картинки на выбранное значение. Хорошо сглаживает края, но сильнее нагружает видеокарту." },
    { "Anti-aliasing (MSAA)", "Сглаживание (MSAA)" },
    { "Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered geometry.\nHigher "
      "sample count will result in smoother edges on models, but may reduce performance.",
      "Включает сглаживание MSAA от 2x до 8x, чтобы края моделей были ровнее.\nЧем больше значение, тем глаже края, "
      "но может упасть производительность." },
    { "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely visual and does "
      "not impact game logic, execution of glitches, etc.\n\nA higher target FPS than your monitor's refresh rate will "
      "waste resources, and might give a worse result.",
      "Дорисовывает промежуточные кадры, чтобы картинка была плавнее. Влияет только на изображение: логика игры и "
      "баги не меняются.\n\nFPS выше частоты монитора только зря нагружает компьютер и может выглядеть хуже." },
    { "Current FPS", "Текущий FPS" },
    { "Match Refresh Rate", "По частоте монитора" },
    { "Matches interpolation value to the refresh rate of your display.",
      "Подстраивает FPS под частоту обновления монитора." },
    { "Renderer API (Needs reload)", "Графический API (нужен перезапуск)" },
    { "Sets the renderer API used by the game.", "Какой графический API использует игра." },
    { "Only one renderer API is available on this platform.", "На этой системе доступен только один графический API." },
    { "Enable Vsync", "Вертикальная синхронизация" },
    { "Removes tearing, but clamps your max FPS to your display's refresh rate.",
      "Убирает разрывы картинки, но ограничивает FPS частотой монитора." },
    { "Windowed Fullscreen", "Оконный полноэкранный" },
    { "Enables Windowed Fullscreen Mode.", "Окно без рамки на весь экран." },
    { "Allow multi-windows", "Несколько окон" },
    { "Allows multiple windows to be opened at once. Requires a reload to take effect.",
      "Можно открывать несколько окон сразу. Применится после перезапуска." },
    { "Texture Filter (Needs reload)", "Фильтр текстур (нужен перезапуск)" },
    { "Sets the applied Texture Filtering.", "Как сглаживаются текстуры." },
    { "Advanced Graphics Options", "Расширенные параметры графики" },

    // ---- Settings -> Controls ----
    { "Clear Devices", "Сбросить устройства" },
    { "Clear Config", "Сбросить настройки" },
    { "This will completely erase the controls config, including registered devices.\nContinue?",
      "Настройки управления будут полностью удалены, включая подключённые устройства.\nПродолжить?" },
    { "Clear", "Очистить" },
    { "Cancel", "Отмена" },
    { "Controller Bindings", "Назначение кнопок" },
    { "Popout Bindings Window", "Открыть в отдельном окне" },
    { "Configure Controller", "Настроить управление" },
    { "Enables the separate Bindings Window.", "Открывает отдельное окно назначения кнопок." },
    { "Input Viewer", "Отображение ввода" },
    { "Toggle Input Viewer", "Показывать нажатия" },
    { "Toggles the Input Viewer.", "Показывает на экране, какие кнопки нажаты." },
    { "Input Viewer Settings", "Настройки отображения ввода" },
    { "Popout Input Viewer Settings", "Открыть в отдельном окне" },
    { "Enables the separate Input Viewer Settings Window.", "Открывает отдельное окно настроек отображения ввода." },

    // ---- Settings -> Notifications ----
    { "Position", "Положение" },
    { "Which corner of the screen notifications appear in.", "В каком углу экрана появляются уведомления." },
    { "Duration (seconds):", "Длительность (сек.):" },
    { "How long notifications are displayed for.", "Сколько времени показывается уведомление." },
    { "Background Opacity", "Непрозрачность фона" },
    { "How opaque the background of notifications is.", "Насколько непрозрачен фон уведомлений." },
    { "Size:", "Размер:" },
    { "How large notifications are.", "Размер уведомлений." },
    { "Test Notification", "Тестовое уведомление" },
    { "Displays a test notification.", "Показывает пробное уведомление." },
    { "Mute Notification Sound", "Без звука уведомлений" },
    { "Prevent notifications from playing a sound.", "Уведомления появляются без звука." },

    // ---- Settings -> Mod Menu ----
    { "Popout Mod Menu Window", "Открыть в отдельном окне" },
    { "Enables the separate Mod Menu Window.", "Открывает отдельное окно управления модами." },

    // ---- Enhancements -> Quality of Life ----
    { "Saving", "Сохранение" },
    { "Autosave", "Автосохранение" },
    { "Save the game automatically on a 3 minute interval and when soft-resetting the game. The interval autosave will "
      "wait if the game is paused in any way (dialogue, pause screen up, cutscenes, etc.).\n\nThe soft-reset save will "
      "*not* trigger in cutscene maps like the Chamber of Sages!",
      "Игра сохраняется сама каждые 3 минуты и при перезапуске. Если игра на паузе (диалог, меню паузы, ролик и т. "
      "п.), автосохранение подождёт.\n\nПри перезапуске сохранение *не* сработает в локациях-роликах вроде Зала "
      "Мудрецов!" },
    { "Notification on Autosave", "Уведомлять об автосохранении" },
    { "Show a notification when the game is autosaved.", "Показывать уведомление, когда игра сохранилась сама." },
    { "Remember Save Location", "Запоминать место сохранения" },
    { "When loading a save, places Link at the last entrance he went through.\nThis doesn't work if the save was made "
      "in grottos, fairy fountains, or dungeons.",
      "При загрузке Линк появляется у последнего входа, через который прошёл.\nНе работает, если сохранялся в "
      "пещерах, фонтанах фей или подземельях." },
    { "Better Save Menu", "Улучшенное меню сохранения" },
    { "Replaces the authentic save menu with a textbox that asks Yes or No for saving,\nthen asks if you want to "
      "Continue, Reset, or Reset to Spawn.",
      "Вместо оригинального меню появляется окно «Сохранить? Да/Нет»,\nа затем выбор: продолжить, перезапустить или "
      "вернуться к точке появления." },
    { "Containers Match Contents", "Сундуки по содержимому" },
    { "Toggle to change container textures to match their contents in randomizer games.\nCategories: Major items, "
      "Lesser items, Junk items, Small keys, Boss keys, Skulltula Tokens.",
      "В рандомайзере вид сундуков и других контейнеров зависит от того, что внутри.\nКатегории: важные предметы, "
      "второстепенные, мусор, малые ключи, ключи босса, жетоны скуллтул." },
    { "Containers of Agony", "Только с Камнем Агонии" },
    { "Only change the texture of containers if you have the Stone of Agony.",
      "Вид контейнеров меняется, только если у тебя есть Камень Агонии." },
    { "Time of Day", "Время суток" },
    { "Nighttime GS Always Spawn", "Ночные скуллтулы всегда" },
    { "Nighttime Skulltulas will spawn during both day and night.",
      "Золотые скуллтулы, которые обычно бывают только ночью, появляются и днём." },
    { "Pull Grave During the Day", "Двигать могилы днём" },
    { "Allows graves to be pulled when child during the day.", "Ребёнком можно двигать могилы и днём." },
    { "Dampe Appears All Night", "Дампе всю ночь" },
    { "Makes Dampe appear anytime during the night, not just his usual working hours.",
      "Могильщик Дампе появляется в любое время ночи, а не только в свои рабочие часы." },
    { "Exit Market at Night", "Выход с рынка ночью" },
    { "Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking to the guard next to the gate.",
      "Ночью можно выйти с рынка в Хайрульское поле, поговорив со стражником у ворот." },
    { "Shops and Games Always Open", "Магазины и игры открыты всегда" },
    { "Shops and Minigames are open both day and night. Requires a scene reload to take effect.",
      "Магазины и мини-игры работают и днём, и ночью. Применится после смены локации." },
    { "This is not compatible with the Locked Overworld Doors Randomizer option.",
      "Несовместимо с опцией рандомайзера «Запертые двери»." },
    { "Pause Menu", "Меню паузы" },
    { "Allow the Cursor to be on Any Slot", "Курсор на любой ячейке" },
    { "Allows the cursor on the pause menu to be over any slot. Sometimes required in Randomizer to select certain "
      "items.",
      "Курсор в меню паузы можно ставить на любую ячейку. Иногда нужно в рандомайзере, чтобы выбрать некоторые "
      "предметы." },
    { "Pause Warp", "Телепорт из паузы" },
    { "Selection of warp song in pause menu initiates a warp. Disables song playback.",
      "Выбор песни-телепорта в меню паузы сразу переносит в нужное место. Песня при этом не играет." },
    { "Answer Navi Prompt with L Button", "Отвечать Нави кнопкой L" },
    { "Speak to Navi with L but enter First-Person Camera with C-Up.",
      "Говорить с Нави на L, а вид от первого лица — на C-вверх." },
    { "Don't Require Input for Credits Sequence", "Титры без нажатий" },
    { "Removes the Input Requirement on Text boxes after defeating Ganon, allowing the Credits Sequence to continue to "
      "progress.",
      "После победы над Ганоном титры идут сами, не нужно нажимать кнопки в окнах с текстом." },
    { "Include Held Inputs at the Start of Pause Buffer Input Window", "Учитывать зажатые кнопки после паузы" },
    { "Typically, inputs that are held prior to the buffer window are not included in the buffer. This setting changes "
      "that behavior to include them. This may cause some inputs to be re-triggered undesireably, for instance "
      "Z-Targeting something you might not want to.",
      "Обычно кнопки, зажатые до окна буфера, не учитываются. Эта опция их учитывает. Из-за этого некоторые действия "
      "могут сработать ещё раз, например, нежелательный Z-таргетинг." },
    { "Adds back in a delay after unpausing before the game resumes playing again, where inputs can be held "
      "prematurely to be input immediately after the game resumes. This essentially brings back behaviour from console "
      "releases which are lost on default because SoH isn't limited to N64 hardware.",
      "Возвращает задержку после снятия паузы, как на консоли: зажатые заранее кнопки сработают сразу, как игра "
      "продолжится. По умолчанию её нет, потому что порт не ограничен железом N64." },
    { "Buffers your inputs to be executed a specified amount of frames later.",
      "Нажатия срабатывают с задержкой на указанное число кадров." },
    { "Reworked Targeting", "Переделанный таргетинг" },
    { "Reworks targeting functionality\n- Press Z while locked always untargets (in Toggle mode)\n- Use the configured "
      "button combo to switch between targets",
      "Меняет работу прицеливания\n- Z при захвате всегда снимает цель (в режиме переключения)\n- Заданное сочетание "
      "кнопок переключает цели" },
    { "Target Switch Button Combination:", "Сочетание для смены цели:" },
    { "Buttons to activate target switching.", "Кнопки, которыми переключаются цели." },
    { "Item Count Messages", "Счётчик предметов" },
    { "Gold Skulltula Tokens", "Жетоны золотых скуллтул" },
    { "Pieces of Heart", "Кусочки сердца" },
    { "Heart Containers", "Контейнеры сердца" },
    { "Misc", "Разное" },
    { "Disable Crit Wiggle", "Без тряски при малом здоровье" },
    { "Disable Random Camera Wiggle at Low Health.", "Камера не трясётся, когда мало здоровья." },
    { "Better Owl", "Удобная сова" },
    { "The default response to Kaepora Gaebora is always that you understood what he said.",
      "Ответ сове Кэпоре Гэборе по умолчанию всегда «я понял», чтобы не слушать её снова." },
    { "Easy Butterfly Fairies", "Феи из бабочек легко" },
    { "Butterflies will transform into a fairy as soon as you approach them with a Deku Stick, skipping the need to "
      "stand still and let the butterfly land on your stick.",
      "Бабочка превращается в фею, как только подходишь к ней с палкой Деку. Не нужно ждать, пока она сядет на "
      "палку." },
    { "Allow Dropping Throw-Only Objects", "Можно опускать предметы" },
    { "Allows normally throw-only objects (such as Cuccos, pots, grass, and small rocks) to be dropped by pressing A "
      "while standing still. Can be toggled while holding an object.",
      "Кур, горшки, траву и мелкие камни, которые обычно можно только бросить, можно опустить: стой на месте и нажми "
      "A. Можно переключать, держа предмет." },
    { "Convenience", "Удобства" },
    { "Quit Fishing at Door", "Закончить рыбалку у двери" },
    { "Fisherman asks if you want to quit at the door if you try to leave the Fishing Pond while still holding the "
      "Fishing Rod.",
      "Если пытаешься уйти с пруда с удочкой в руках, рыбак у двери спросит, хочешь ли ты закончить." },
    { "Instant Putaway", "Мгновенно убирать оружие" },
    { "Allow Link to put items away without having to wait around.", "Линк убирает предметы сразу, без ожидания." },
    { "Navi Timer Resets on Scene Change", "Таймер Нави при смене локации" },
    { "Resets the Navi timer on scene change. If you have already talked to her, she will try and talk to you again, "
      "instead of needing a save warp or death.",
      "Таймер подсказок Нави сбрасывается при смене локации. Если ты уже говорил с ней, она снова позовёт, без "
      "перезагрузки или смерти." },
    { "Link's Cow in Both Time Periods", "Корова Линка в обоих временах" },
    { "Allows the Lon Lon Ranch Obstacle Course reward to be shared across time periods.",
      "Награда за полосу препятствий на ранчо Лон Лон есть и в прошлом, и в будущем." },
    { "Play Zelda's Lullaby to Open Sleeping Waterfall", "Колыбельная Зельды открывает водопад" },
    { "Always: Link must always play Zelda's Lullaby to open the waterfall entrance to Zora's Domain.\nOnce: Link only "
      "needs to play Zelda's Lullaby once to open the waterfall; after that, it stays open permanently.\nNever: Link "
      "never needs to play Zelda's Lullaby to open the waterfall. He only needs to have learned it and have an Ocarina.",
      "Всегда: чтобы открыть водопад у входа во Владения Зора, каждый раз нужно играть Колыбельную Зельды.\nОдин "
      "раз: сыграть нужно один раз, дальше водопад открыт навсегда.\nНикогда: играть не нужно, достаточно знать "
      "песню и иметь окарину." },
    { "Skip Feeding Jabu-Jabu", "Не кормить Джабу-Джабу" },
    { "Allow Link to enter Jabu-Jabu without feeding him a fish.", "Можно войти в Джабу-Джабу, не кормя его рыбой." },

    // ---- Settings -> Audio (slider labels keep their %d %% format) ----
    { "Master Volume: %d %%", "Общая громкость: %d %%" },
    { "Main Music Volume: %d %%", "Музыка: %d %%" },
    { "Sub Music Volume: %d %%", "Доп. музыка: %d %%" },
    { "Fanfare Volume: %d %%", "Фанфары: %d %%" },
    { "Sound Effects Volume: %d %%", "Звуковые эффекты: %d %%" },
    { "Original (%d)", "Оригинал (%d)" },

    // ---- Settings -> Language ----
    { "English", "Английский" },
    { "German", "Немецкий" },
    { "French", "Французский" },
    { "Japanese", "Японский" },
    { "Select Language:", "Выберите язык:" },

    // ---- Settings -> Presets ----
    { "Presets", "Пресеты" },
    { "No presets with rando options. Make some in Settings -> Presets",
      "Нет пресетов с настройками рандомайзера. Создай их в Настройки -> Пресеты" },
    { "Name", "Название" },
    { "Apply", "Применить" },
    { "Apply Preset", "Применить пресет" },
    { "Edit", "Изменить" },
    { "Delete", "Удалить" },
    { "No presets found.", "Пресетов нет." },
    { "New Preset", "Новый пресет" },
    { "Hide built-in presets", "Скрыть встроенные пресеты" },
    { "Preset Name", "Название пресета" },
    { "Preset name already exists", "Пресет с таким названием уже есть" },
    { "Preset name is empty", "Название пресета пустое" },
    { "No sections selected", "Не выбран ни один раздел" },
    { "Save", "Сохранить" },
    { "Disabled because of race lockout", "Отключено из-за режима гонки" },
    { "Cosmetics", "Внешний вид" },
    { "Rando Settings", "Настройки рандомайзера" },
    { "Trackers", "Трекеры" },
    { "Check Tracker", "Трекер проверок" },
    { "Entrance Tracker", "Трекер входов" },

    // ---- Settings -> Mod Menu ----
    { "Enable Mods", "Включить моды" },
    { "Toggle mods. For graphics mods, this means toggling between default and mod graphics.",
      "Включает и выключает моды. Для графических модов — переключение между обычной графикой и графикой мода." },
    { "Mods Tab Hotkey", "Переключать моды клавишей Tab" },
    { "Allows pressing the Tab key to toggle mods", "Клавиша Tab включает и выключает моды." },
    { "Mods are currently not reloaded at runtime. Close and re-open Ship for the changes to take effect.\nDrag "
      "ordering for the enabled list is available.\nMod priority is top to bottom. They override mods listed below "
      "them.",
      "Моды не перезагружаются на ходу: закрой и снова открой игру, чтобы изменения применились.\nПорядок включённых "
      "модов можно менять перетаскиванием.\nПриоритет сверху вниз: верхние моды перекрывают те, что ниже." },
    { "Update", "Обновить" },
    { "Currently editing...", "Идёт редактирование..." },
    { "Already editing...", "Уже редактируется..." },
    { "Clear List", "Очистить список" },
    { "Clear the current mod list and force a rebuild on next boot.\nClick Apply & Close to save this change.",
      "Очищает список модов, при следующем запуске он соберётся заново.\nНажми «Применить и закрыть», чтобы "
      "сохранить." },
    { "Apply & Close", "Применить и закрыть" },
    { "Application currently requires a restart. Save the mod info and close SoH?",
      "Нужен перезапуск игры. Сохранить список модов и закрыть игру?" },
    { "Close", "Закрыть" },
    { "Temporarily disabled while editing mods list.", "Недоступно, пока редактируется список модов." },
    { "Enabled Mods", "Включённые моды" },
    { "Disabled Mods", "Выключенные моды" },
    { "Move %s", "Переместить %s" },
    { "Move %zu mods", "Переместить модов: %zu" },
    { "Custom Music: %zu song(s) loaded from .ootrs files", "Своя музыка: загружено треков из файлов .ootrs: %zu" },
    { "Custom soundbanks are not supported yet:", "Свои звуковые банки пока не поддерживаются:" },

    // ---- Android: background scene preload ----
    { "Preload Scene Assets", "Подгружать локацию заранее" },
    { "Loads the textures and models of the whole area in the background as soon as you enter it, so HD texture packs "
      "stutter less right after a loading screen. Uses more memory.",
      "Сразу при входе в локацию загружает в фоне все её текстуры и модели, чтобы HD-текстуры меньше тормозили "
      "после загрузки. Расходует больше памяти." },
    { "Use Alternate Assets", "Альтернативные ресурсы (моды)" },
    { "Mods", "Моды" },
};

const char* Text(const char* english) {
    if (english == nullptr) {
        return english;
    }
    auto it = sRussian.find(english);
    return it != sRussian.end() ? it->second : english;
}

const char* Text(const std::string& english) {
    auto it = sRussian.find(english);
    return it != sRussian.end() ? it->second : english.c_str();
}

std::string Label(const std::string& english) {
    // Translate only the visible part (before "##"); keep the whole original label after "##" so the ImGui ID
    // stays unique and stable.
    size_t hashPos = english.find("##");
    std::string visible = hashPos == std::string::npos ? english : english.substr(0, hashPos);
    auto it = sRussian.find(visible);
    if (it == sRussian.end()) {
        return english;
    }
    return std::string(it->second) + "##" + english;
}

} // namespace SohLoc
