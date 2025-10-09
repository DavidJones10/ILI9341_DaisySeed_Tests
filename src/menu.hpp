#include <vector>
#include <functional>
#include <string>
#include "src/ili9341_ui_driver.hpp"

class Menu {
public:

    void Init(ILI9341UiDriver* driver_){
        driver = driver_;
    }
    void SetMenuColors(uint8_t primaryBorderColor, uint8_t primaryFillColor, uint8_t secondaryBorderColor, uint8_t secondaryFillColor) {

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
        menuCursorIdx = (menuCursorIdx + 1) % menuItems.size();
    }
    void DecrementMenuCursor(){
        uint8_t newCursorId = menuCursorIdx - 1;
        menuCursorIdx = (newCursorId < 0) ? 0 : newCursorId;
    }
    void Draw() {
        if (drawSubmenu) {
            menuItems[menuCursorIdx].DrawSubmenu();
        } else {

        }
    }
private:
    ILI9341UiDriver* driver;
    uint8_t menuCursorIdx = 0;
    bool drawSubmenu = false;
    std::vector<MenuItem> menuItems;
};
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
    void DrawItem(){

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