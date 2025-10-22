#include "daisy_seed.h"
#include "src/ili9341_ui_driver.hpp"
#include "src/menu.hpp"
#include "daisysp.h"
using namespace daisy;
using namespace daisysp;

ILI9341UiDriver driver;
DaisySeed  hw;
Oscillator lfo;
Random rdm;
Switch sw;
Led led;
Encoder enc;
Menu menu;

float lfo_out;
int xPos = 50; int yPos = 50; int moverWidth = 50;
bool myLedVal;
float lfoFreqs[8] = {0.01f, 0.05f, 0.1f, 0.2f, 0.5f, 1.f, 2.f, 4.f};
int lfoFreqIdx = 0;
int xVelo = 4; int yVelo  = 4;
char strbuff[128];
uint8_t buttonColor = COLOR_RED;
uint16_t dialX, dialY, dialGapX, dialGapY, dialRad, dialX2;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        lfo_out = lfo.Process();
    }
}

void draw() {
    driver.Fill(COLOR_BLACK);
    menu.Draw(&enc);
}

void GeneralSubmenuCallback() {
    driver.Fill(COLOR_DARK_BLUE);
    uint8_t numControls = menu.GetMenuItems()[menu.GetMenuCursorIndex()].GetNumControls();
    driver.FillRect(Rectangle(0, 0, MAX_X, 30), COLOR_BLACK);
    driver.WriteStringAligned(menu.GetMenuItems()[menu.GetMenuCursorIndex()].GetLabel(), Font_16x26, Rectangle(0, 0, MAX_X, 30), daisy::Alignment::centered, COLOR_WHITE);

    switch (numControls) {
        case 1:
            dialGapY = 0;
            dialGapX = 0;
            dialX = 160;
            dialY = 135;
            dialRad = 70;
            break;
        case 2:
             dialGapY = 0;
             dialGapX = 106;
             dialX = 107;
             dialY = 135;
             dialRad = 51;
            break;
        case 3:
            dialGapY = 0;
            dialGapX = 80;
            dialX = 80;
            dialY = 135;
            dialRad = 39;
            break;
        case 4:
            dialGapY = 70;
            dialGapX = 106;
            dialX = 107;
            dialY = 100;
            dialRad = 30;
            dialX2 = 107;
            break;
        case 5:
            dialGapY = 70;
            dialGapX = 80;
            dialX = 80;
            dialY = 100;
            dialRad = 35;
            dialX2 = 107;
            break;
        case 6:
            dialGapY = 70;
            dialGapX = 80;
            dialX = 80;
            dialY = 100;
            dialRad = 30;
            dialX2 = 107;
            break;
    }
    for (size_t i = 0; i < numControls; i++) {
        if (i == menu.GetMenuItems()[menu.GetMenuCursorIndex()].GetControlIndex()) {
            if (menu.GetMenuItems()[menu.GetMenuCursorIndex()].IsControlSelected()) {
                driver.FillRoundedRectangle(dialX - dialRad - 2, dialY - dialRad - 22, dialRad * 2 + 4, dialRad * 2 + 42, 4, COLOR_LIGHT_GRAY);
            }
            driver.DrawRoundedRectangle(dialX - dialRad - 2, dialY - dialRad - 22, dialRad * 2 + 4, dialRad * 2 + 42, 4, COLOR_WHITE);
        }
        const char* label = menu.GetMenuItems()[menu.GetMenuCursorIndex()].GetControlVec()[i].GetLabel();
        float radAsFloat = menu.GetMenuItems()[menu.GetMenuCursorIndex()].GetControlVec()[i].GetValue();
        float displayValue = menu.GetMenuItems()[menu.GetMenuCursorIndex()].GetControlVec()[i].GetDisplayValue();
        if (dialGapY == 0) {
            driver.DrawDial(dialX, dialY, dialRad, radAsFloat, COLOR_WHITE, COLOR_BLUE, true, displayValue, label);
        } else {
            if (i > 2) {
                dialY += dialGapY;
                dialX = dialX2;
                if (numControls == 5) {
                    dialGapX = 106;
                }
                driver.DrawDial(dialX, dialY, dialRad, radAsFloat, COLOR_WHITE, COLOR_BLUE, true, displayValue, label);
                dialGapY = 0;
            } else {
                driver.DrawDial(dialX, dialY, dialRad, radAsFloat, COLOR_WHITE, COLOR_BLUE, true, displayValue, label);
            }
        }
        dialX += dialGapX;
    }
}

void InitMenuItems(Menu &menu) {
    // Delay
    menu.AddMenuItem("Delay", GeneralSubmenuCallback);
    menu.GetMenuItems()[0].AddSubmenuControl("Dry/Wet", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[0].AddSubmenuControl("Time", 0.f, 1000.f, 100.f, 5.f);
    menu.GetMenuItems()[0].AddSubmenuControl("Feedback", 0.f, .99f, .5f, .01f);
    
    // Reverb
    menu.AddMenuItem("Reverb", GeneralSubmenuCallback);
    menu.GetMenuItems()[1].AddSubmenuControl("Mix", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[1].AddSubmenuControl("Decay", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[1].AddSubmenuControl("Damping", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[1].AddSubmenuControl("Size", 0.f, 1.f, .5f, .01f);

    // Distortion
    menu.AddMenuItem("Distortion", GeneralSubmenuCallback);
    menu.GetMenuItems()[2].AddSubmenuControl("Drive", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[2].AddSubmenuControl("Tone", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[2].AddSubmenuControl("Level", 0.f, 1.f, .5f, .01f);

    // Chorus
    menu.AddMenuItem("Chorus", GeneralSubmenuCallback);
    menu.GetMenuItems()[3].AddSubmenuControl("Mix", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[3].AddSubmenuControl("Rate", 0.1f, 10.f, 1.f, .1f);
    menu.GetMenuItems()[3].AddSubmenuControl("Depth", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[3].AddSubmenuControl("Feedback", 0.f, .99f, .2f, .01f);

    // Tremolo
    menu.AddMenuItem("Tremolo", GeneralSubmenuCallback);
    menu.GetMenuItems()[4].AddSubmenuControl("Depth", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[4].AddSubmenuControl("Rate", 0.1f, 20.f, 5.f, .1f);
    menu.GetMenuItems()[4].AddSubmenuControl("Wave", 0.f, 3.f, 0.f, 1.f); // 0=sine, 1=square, 2=ramp, 3=tri

    // Phaser
    menu.AddMenuItem("Phaser", GeneralSubmenuCallback);
    menu.GetMenuItems()[5].AddSubmenuControl("Rate", 0.1f, 10.f, 1.f, .1f);
    menu.GetMenuItems()[5].AddSubmenuControl("Depth", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[5].AddSubmenuControl("Feedback", 0.f, .99f, .7f, .01f);
    menu.GetMenuItems()[5].AddSubmenuControl("Mix", 0.f, 1.f, .5f, .01f);

    // Wah
    menu.AddMenuItem("Wah", GeneralSubmenuCallback);
    menu.GetMenuItems()[6].AddSubmenuControl("Position", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[6].AddSubmenuControl("Range", 0.f, 1.f, .5f, .01f);
    menu.GetMenuItems()[6].AddSubmenuControl("Q", 0.f, 1.f, .7f, .01f);

    // Equaliser
    menu.AddMenuItem("Equaliser", GeneralSubmenuCallback);
    menu.GetMenuItems()[7].AddSubmenuControl("Low", -12.f, 12.f, 0.f, .5f);
    menu.GetMenuItems()[7].AddSubmenuControl("Mid", -12.f, 12.f, 0.f, .5f);
    menu.GetMenuItems()[7].AddSubmenuControl("High", -12.f, 12.f, 0.f, .5f);
    menu.GetMenuItems()[7].AddSubmenuControl("Mid Freq", 200.f, 5000.f, 1000.f, 100.f);

    //menu.AddMenuItem("Looper", GeneralSubmenuCallback);
    //menu.AddMenuItem("Effects Chain", GeneralSubmenuCallback);

    menu.SetMenuColors(COLOR_WHITE, COLOR_WHITE, COLOR_DARK_BLUE, COLOR_WHITE, COLOR_LIGHT_GRAY);
}

int main(void)
{
    hw.Init(true);
    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    driver.Init();
    sw.Init(hw.GetPin(30), 1000);
    enc.Init(hw.GetPin(5), hw.GetPin(6), hw.GetPin(27), 1000.f);
    led.Init(hw.GetPin(28), false, 1000.f);

    lfo.Init(hw.AudioSampleRate());
    lfo.SetWaveform(lfo.WAVE_TRI);
    lfo.SetFreq(1.0f);
    lfo.SetAmp(1.0f);

    // Here all the drawing happening in the memory buffer, so no drawing happening at this point.
    //driver.Fill(COLOR_BLACK);
    menu.Init(&driver, true);
    InitMenuItems(menu);

    hw.StartAudio(AudioCallback);

    for(;;)
    {
        sw.Debounce();
        enc.Debounce();
        // IsRender() checks if DMA is idle (i.e. done transmitting the buffer), Update() initiates the DMA transfer
        if(driver.IsRender())
        {   
            draw();
            driver.Update();
        }
        if (!menu.IsMenuItemSelected()){ // Main Menu
            if (enc.Increment() > 0) {
                menu.IncrementMenuCursor();
            }
            if (enc.Increment() < 0) {
                menu.DecrementMenuCursor();
            }
            if (enc.RisingEdge()) {
                menu.SelectCurrentMenuItem();
            }
        } else { // In Submenu
            if (enc.RisingEdge()) {
                 if (menu.GetMenuItems()[menu.GetMenuCursorIndex()].IsControlSelected()) {
                     menu.GetMenuItems()[menu.GetMenuCursorIndex()].DeselectCurrentControl();
                 } else {
                     menu.GetMenuItems()[menu.GetMenuCursorIndex()].SelectCurrentControl();
                 }
             }
             if (!menu.GetMenuItems()[menu.GetMenuCursorIndex()].IsControlSelected()) {
                 if (enc.Increment() > 0) {
                     menu.GetMenuItems()[menu.GetMenuCursorIndex()].IncrementControlIndex();
                 }
                 if (enc.Increment() < 0) {
                     menu.GetMenuItems()[menu.GetMenuCursorIndex()].DecrementControlIndex();
                 }
             } else {
                 if (enc.Increment() > 0) {
                     menu.GetMenuItems()[menu.GetMenuCursorIndex()].IncrementControl();
                 }
                 if (enc.Increment() < 0) {
                     menu.GetMenuItems()[menu.GetMenuCursorIndex()].DecrementControl();
                 }
             }
            if (!menu.GetMenuItems()[menu.GetMenuCursorIndex()].IsControlSelected()) {
                if (enc.TimeHeldMs() > 1200.f) {
                    menu.ReturnToMenuScreen();
                }
            }
        }

        if (sw.RisingEdge()) {
            lfoFreqIdx = (lfoFreqIdx + 1) % 7;
            lfo.SetFreq(lfoFreqs[lfoFreqIdx]);
            menu.ReturnToMenuScreen();
        }
        if (myLedVal) {
            hw.SetLed(true);
        } else {
            hw.SetLed(false);
        }
        led.Set(abs(lfo_out));
        led.Update();
    }
}