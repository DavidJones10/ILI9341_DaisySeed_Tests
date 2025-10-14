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
    xPos += xVelo;
    yPos += yVelo;
    if (xPos + moverWidth > MAX_X) {
        xPos = MAX_X - moverWidth;
        xVelo *= -1;
    }
    if (xPos < MIN_XY) {
        xPos = 0;
        xVelo *= -1;
    }
    if (yPos + moverWidth > MAX_Y) {
        yPos = MAX_Y - moverWidth;
        yVelo *= -1;
    }
    if (yPos < MIN_XY) {
        yPos = 0;
        yVelo *= -1;
    }
    driver.FillRect(Rectangle(xPos, yPos, 50, 50), COLOR_RED);
    driver.FillRoundedRectangle(100, 100, 100, 50, 10, buttonColor);
    driver.DrawRoundedTextRect("Delay", 10, 10, 150, 30, 5, COLOR_WHITE, COLOR_CYAN, COLOR_BLACK, Font_16x26);
    sprintf(strbuff, "LFO: %.2f Hz", lfo_out);
    driver.DrawLine(0, 50, 320, 50, COLOR_WHITE);
    driver.WriteStringAligned("lol", Font_16x26, Rectangle(200, 30, 50, 50), Alignment::centered, COLOR_WHITE);
    driver.WriteString(strbuff, 100, 20, Font_7x10, COLOR_WHITE);
    driver.WriteString("Underlined", 20, 100, Font_16x26, COLOR_RED);
    driver.FillCircle(100, 200, 30, COLOR_WHITE);
}

void randomizeValues() {
    if (rdm.IsReady()) {
        xPos = int(rdm.GetFloat(0.f, MAX_X - moverWidth));
        yPos = int(rdm.GetFloat(0.f, MAX_Y - moverWidth));
        float negx = rdm.GetFloat(0.f, 2.f);
        if (negx > 1.f) {
            xVelo *= -1;
        }
        float neg = rdm.GetFloat(0.f, 2.f);
        if (neg > 1.f) {
            yVelo *= -1;
        }
    }
}

int main(void)
{
    hw.Init(true);
    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    driver.Init();
    sw.Init(hw.GetPin(30), 1000);
    led.Init(hw.GetPin(28), false, 1000.f);

    lfo.Init(hw.AudioSampleRate());
    lfo.SetWaveform(lfo.WAVE_TRI);
    lfo.SetFreq(1.0f);
    lfo.SetAmp(1.0f);

    // Here all the drawing happening in the memory buffer, so no drawing happening at this point.
    //driver.Fill(COLOR_BLACK);
    randomizeValues();

    hw.StartAudio(AudioCallback);

    for(;;)
    {
        sw.Debounce();
        // IsRender() checks if DMA is idle (i.e. done transmitting the buffer), Update() initiates the DMA transfer
        if(driver.IsRender())
        {   
            draw();
            driver.Update();
        }

        if (sw.RisingEdge()) {
            randomizeValues();
            myLedVal = !myLedVal;
            lfoFreqIdx = (lfoFreqIdx + 1) % 7;
            lfo.SetFreq(lfoFreqs[lfoFreqIdx]);
            buttonColor = myLedVal ? COLOR_CYAN : COLOR_LIGHT_GREEN;
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