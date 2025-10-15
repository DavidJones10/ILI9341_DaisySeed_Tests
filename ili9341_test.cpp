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
Switch sw2;
Led led;
Menu menu;

float lfo_out;
int xPos = 50; int yPos = 50; int moverWidth = 50;
bool myLedVal;
float lfoFreqs[8] = {0.01f, 0.05f, 0.1f, 0.2f, 0.5f, 1.f, 2.f, 4.f};
int lfoFreqIdx = 0;
int xVelo = 4; int yVelo  = 4;
char strbuff[128];
uint8_t buttonColor = COLOR_RED;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        lfo_out = lfo.Process();
    }
}

void draw() {
    driver.Fill(COLOR_BLACK);
    menu.Draw();
}


void GeneralSubmenuCallback() {
    driver.DrawCircle(50, 50, 10, COLOR_BLUE);
    driver.DrawCircle(80, 50, 10, COLOR_RED);
    driver.DrawCircle(50, 80, 10, COLOR_WHITE);
}

int main(void)
{
    hw.Init(true);
    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    driver.Init();
    sw.Init(hw.GetPin(30), 1000);
    sw2.Init(hw.GetPin(27), 1000.f);
    led.Init(hw.GetPin(28), false, 1000.f);

    lfo.Init(hw.AudioSampleRate());
    lfo.SetWaveform(lfo.WAVE_TRI);
    lfo.SetFreq(1.0f);
    lfo.SetAmp(1.0f);

    // Here all the drawing happening in the memory buffer, so no drawing happening at this point.
    //driver.Fill(COLOR_BLACK);
    menu.Init(&driver, true);
    menu.AddMenuItem("Delay", GeneralSubmenuCallback);
    menu.AddMenuItem("Reverb", GeneralSubmenuCallback);
    menu.AddMenuItem("Distortion", GeneralSubmenuCallback);
    menu.SetMenuColors(COLOR_WHITE, COLOR_WHITE, COLOR_DARK_BLUE, COLOR_WHITE, COLOR_LIGHT_GRAY);

    hw.StartAudio(AudioCallback);

    for(;;)
    {
        sw.Debounce();
        sw2.Debounce();
        // IsRender() checks if DMA is idle (i.e. done transmitting the buffer), Update() initiates the DMA transfer
        if(driver.IsRender())
        {   
            draw();
            driver.Update();
        }

        if (sw.RisingEdge()) {
            myLedVal = !myLedVal;
            lfoFreqIdx = (lfoFreqIdx + 1) % 7;
            lfo.SetFreq(lfoFreqs[lfoFreqIdx]);
            menu.IncrementMenuCursor();
        }
        if (sw2.RisingEdge()) {
            if (menu.IsMenuItemSelected()) {
                menu.ReturnToMenuScreen();
            } else {
                menu.SelectCurrentMenuItem();
            }
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