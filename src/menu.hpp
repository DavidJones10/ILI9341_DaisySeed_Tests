#include <vector>
#include <functional>
#include <string>
#include "ili9341_ui_driver.hpp"

#define MAX_X 320
#define MAX_Y 240
#define MIN_XY 0

class MenuItem;
typedef std::function<void()> SubmenuCallback;

class MenuItem {
public:
    SubmenuCallback submenuCallback;
    MenuItem(): submenuCallback(nullptr){}

    void Init(ILI9341UiDriver* driver_, char* label_){
        driver = static_cast<ILI9341UiDriver*>(driver_);
        label = label_;
    }
    void DrawItem(uint16_t x, 
                  uint16_t y,
                  uint16_t w,
                  uint16_t h,
                  uint8_t borderColor,
                  uint8_t fillColor,
                  uint8_t textColor)
    {
        driver->DrawRoundedTextRect(label, x, y, w, h, 5, borderColor, fillColor, textColor, Font_7x10);
    }
    void DrawSubmenu(){
        if (submenuCallback) {
            submenuCallback();
        }
    }
    void SetSubmenuCallback(SubmenuCallback func) {
        submenuCallback = func;
    } 
    char* label;
private:
    ILI9341UiDriver* driver;
};

class Menu {
public:

    void Init(ILI9341UiDriver* driver_, bool wrapIncDec_ = false){
        driver = driver_;
        wrapIncDec = wrapIncDec_;
    }
    void SetMenuColors(uint8_t textColor_,
                       uint8_t primaryBorderColor_, 
                       uint8_t primaryFillColor_, 
                       uint8_t secondaryBorderColor_, 
                       uint8_t secondaryFillColor_)
    {
        textColor = textColor_;
        primaryBorderColor = primaryBorderColor_;
        primaryFillColor = primaryFillColor_;
        secondaryBorderColor = secondaryBorderColor_;
        secondaryFillColor = secondaryFillColor_;
    }
    void AddMenuItem(char* label, void* UICallback) {
        MenuItem item;
        item.Init(driver, label);
    }
    void SelectCurrentMenuItem(){
        drawSubmenu = true;
    }
    void ReturnToMenuScreen(){
        drawSubmenu = false;
    }
    char* GetMenuItemSelected(){
        char* selected = menuItems[menuCursorIdx].label;
        if (selected != nullptr) {
            return selected;
        } else {
            return "";
        }
    }
    bool IsMenuItemSelected() {
        return drawSubmenu;
    }
    uint8_t GetMenuCursorIndex(){
        return menuCursorIdx;
    }
    void IncrementMenuCursor(){
        if (wrapIncDec) {
            menuCursorIdx = (menuCursorIdx + 1) % menuItems.size();
        } else {
            menuCursorIdx += 1;
            if (menuCursorIdx > menuItems.size()) {
                menuCursorIdx = menuItems.size() - 1;
            }
        }
    }
    void DecrementMenuCursor(){
        size_t newCursorId = menuCursorIdx - 1;
        if (wrapIncDec) {
            menuCursorIdx = (newCursorId < 0) ? menuItems.size() - 1 : newCursorId;
        } else {
            menuCursorIdx = (newCursorId < 0) ? 0 : newCursorId;
        }
    }
    void Draw() {
        size_t writeX = 10, writeY = 10;

        if (drawSubmenu) {
            menuItems[menuCursorIdx].DrawSubmenu();
        } else {
            for (size_t i = 0; i < menuItems.size(); i++) {
                uint8_t borderColor = (i == menuCursorIdx) ? secondaryBorderColor : primaryBorderColor;
                uint8_t fillColor = (i == menuCursorIdx) ? secondaryFillColor : primaryFillColor;
                menuItems[i].DrawItem(writeX, writeY, itemWidth, itemHeight, borderColor, fillColor, textColor);
                writeY += itemHeight + itemGap;
            }
        }
    }
private:
    ILI9341UiDriver* driver;
    uint8_t textColor, primaryBorderColor, primaryFillColor, secondaryBorderColor, secondaryFillColor;
    size_t menuCursorIdx = 0;
    bool drawSubmenu = false;
    bool wrapIncDec;
    std::vector<MenuItem> menuItems;
    uint16_t itemWidth = 150, itemHeight = 30, itemGap = 10;
};