#include <vector>
#include <functional>
#include <string>
#include "ili9341_ui_driver.hpp"

#define MAX_X 320
#define MAX_Y 240
#define MIN_XY 0

class MenuItem;
class SubmenuControl;
typedef std::function<void()> SubmenuCallback;

class SubmenuControl {
    public:
        void Init(const char* name, float minValue, float maxValue, float initial, float step) {
            label = name;
            minVal = minValue;
            maxVal = maxValue;
            range = maxVal - minVal;
            stepVal = step / range;
            if (initial < minVal) initial = minVal;
            if (initial > maxVal) initial = maxVal;
            value = (initial - minVal) / range;
        }
        float GetDisplayValue() {
            return minVal + (value * range);
        }
        void IncrementValue() {
            float newVal = value + stepVal;
            value = (newVal >= 1.f) ? 1.f : newVal;
        }
        void DecrementValue() {
            float newVal = value - stepVal;
            value = (newVal <= 0.f) ? 0.f : newVal;
        }
        float GetValue() {
            return value;
        }
        const char* GetLabel() {
            return label;
        }
    private:
        const char* label;
        float minVal, maxVal, range, stepVal, value;
};

class MenuItem {
public:
    SubmenuCallback submenuCallback;
    MenuItem(): submenuCallback(nullptr){}

    void Init(ILI9341UiDriver* driver_, const char* label_){
        driver = static_cast<ILI9341UiDriver*>(driver_);
        label = label_;
        ctlIdx = 0;
    }
    void DrawItem(uint16_t x, 
                  uint16_t y,
                  uint16_t w,
                  uint16_t h,
                  uint8_t borderColor,
                  uint8_t fillColor,
                  uint8_t textColor)
    {
        driver->DrawRoundedTextRect(label, x, y, w, h, 5, borderColor, fillColor, textColor, Font_11x18);
    }
    void DrawSubmenu(){
        if (submenuCallback) {
            submenuCallback();
        }
    }
    void SetSubmenuCallback(SubmenuCallback func) {
        submenuCallback = func;
    }
    const char* GetLabel() {
        return label;
    }
    void AddSubmenuControl(const char* name, float minValue, float maxValue, float initial, float step) {
        SubmenuControl newControl;
        newControl.Init(name, minValue, maxValue, initial, step);
        controls.push_back(newControl);
    }
    void IncrementControlIndex() {
        ctlIdx = ctlIdx + 1 % controls.size();
    }
    void DecrementControlIndex() {
        uint8_t newIdx = ctlIdx - 1;
        ctlIdx = (newIdx < 0) ? controls.size() - 1 : newIdx;
    }
    void SelectCurrentControl(){
        ctlSelected = true;
    }
    void DeselectCurrentControl() {
        ctlSelected = false;
    }
    bool IsControlSelected() {
        return ctlSelected;
    }
    void IncrementControl() {
        controls[ctlIdx].IncrementValue();
    }
    void DecrementControl() {
        controls[ctlIdx].DecrementValue();
    }
    uint8_t GetControlIndex() {
        return ctlIdx;
    }
    // Returns a pointer to the vector of Controls
    SubmenuControl* GetControlVec() {
        return controls.data();
    }
    SubmenuControl* GetControl(uint8_t idx) {
        if (idx < this->GetNumControls())
            return &controls[idx];
    }
    uint8_t GetNumControls() {
        return controls.size();
    }

private:
    const char* label;
    ILI9341UiDriver* driver;
    uint8_t ctlIdx;
    bool ctlSelected;
    std::vector<SubmenuControl> controls;
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
    void AddMenuItem(const char* label, SubmenuCallback UICallback) {
        MenuItem item;
        item.Init(driver, label);
        item.SetSubmenuCallback(UICallback);
        menuItems.push_back(item);
    }
    void SelectCurrentMenuItem(){
        drawSubmenu = true;
    }
    void ReturnToMenuScreen(){
        drawSubmenu = false;
    }
    const char* GetMenuItemSelected(){
        const char* selected = menuItems[menuCursorIdx].GetLabel();
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
            menuCursorIdx = (newCursorId < 0) ? menuItems.size() - 2 : newCursorId;
        } else {
            menuCursorIdx = (newCursorId < 0) ? 0 : newCursorId;
        }
    }
    void Draw(Encoder *enc) {
        size_t writeX = 10, writeY = 40;
        
        if (drawSubmenu) {
            menuItems[menuCursorIdx].DrawSubmenu();
        } else {
            for (size_t i = 0; i < menuItems.size(); i++) {
                if (writeY + itemHeight >= 240) {
                    writeY = 40;
                    writeX += itemWidth + itemGap;
                }
                uint8_t borderColor = (i == menuCursorIdx) ? secondaryBorderColor : primaryBorderColor;
                uint8_t fillColor = (i == menuCursorIdx) ? secondaryFillColor : primaryFillColor;
                menuItems[i].DrawItem(writeX, writeY, itemWidth, itemHeight, borderColor, fillColor, textColor);
                writeY += itemHeight + itemGap;
            }
        }
    }
    MenuItem* GetMenuItems() {
        return menuItems.data();
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
