#ifndef ITEM_WHEEL_H
#define ITEM_WHEEL_H

// Quick item wheel: a button opens a wheel with the owned items and equipment while the game is paused.
// Tap an item, then press a C button to put it there; tap equipment to put it on.

#ifdef __cplusplus
#include <libultraship/libultraship.h>

class ItemWheelWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override{};
    void DrawElement() override{};
    void Draw() override;
    void UpdateElement() override{};
};

extern "C" {
#endif

struct PlayState;

// Called at the start of every game frame. Returns non-zero while the wheel is open,
// in which case the game logic must not run this frame (the game is paused).
int ItemWheel_Update(struct PlayState* play);

#ifdef __cplusplus
}
#endif

#endif // ITEM_WHEEL_H
