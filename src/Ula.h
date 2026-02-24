#pragma once

#include <cstdint>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <SDL.h>

#include <ClockedChip.h>
#include <Signal.h>
#include <Register.h>

#include "ColourPalette.h"
#include "TZXFile.h"

class Board;

/*
Not sure if it's the right approach, but I've modified the ULA class to be clocked
per pixel at half the ZX Spectrum crystal rate (14MHz).  So, one pixel equals
two Z80 T-states at 7MHz. The CPU is clocked at half the ULA clock, i.e. 3.5MHz.

I've left a couple of calculations in place in the private portion of the ULA class,
just so I can see any discrepencies, if/when they arise.  So far seems to be working
out.  It's interesting to see there is a slight mismatch in the ULA speed vs PAL
standard, which apparently accounts for ZX Spectrum 16/48K pixel jitter in real life!

The ZX Spectrum ULA, Chris Smith
Chapter 9 (The Video Display), Figure 9-1, PAL horizontal and vertical screen dimensions

                        +--------------------------------------------+                     -------------
       ^                +............................................+        ^            ^    V = 248 (Vertical Sync interrupt, 32 T-states duration)
 vertical sync          +............................................+        |            |
    4 lines             +..............vertical blank................+       8 lines       |
       V                +............................................+        |            |
                        +............................................+        V            |
                        +--------------------------------------------+                     |------------
                        |                                            |        ^            |    V = 256
                        |                                            |        |            |
                        |             top vertical border            |       56 lines      |
  horizontal            |                                            |        |            |
    sync                |                                            |        V            |
<---28px--->            |          +----------------------+          |                     |------------
                        |          |                      |          |        ^            |    V = 0
                        |          |      display         |          |        |            |
                        |          |        area          |          |        |            |
     horizontal blank   |          |                      |          |        |            |
<---------96px--------->|<--32px-->|<-------256px-------->|<--64px-->|      192 lines     312 lines
                        |          |                      |          |        |            |
                        |          |                      |          |        |            |
                        |          |                      |          |        |            |
                        |          |                      |          |        V            |
                        |          +----------------------+          |                     |------------
                        |                                            |        ^            |    V = 192
                        |                                            |        |            |
                        |           bottom vertical border           |       56 lines      |
                        |                                            |        |            |
                        |                                            |        V            V    
                        +--------------------------------------------+

The ZX Spectrum ULA, Chris Smith
Chapter 11 (Video Sychronisation), Figure 11-1, Horizontal time points for the 5C and 6C ULA

    Description            Cycle Start        Cycle End
    ---------------------------------------------------
    Pixel Output            0                 255
    Right Border            256               319
    Blanking Period         320               415
    Horzontal Sync          336 (5C)          367 (5C)
                            344 (6C)          375 (6C)
    Left Border             416               447
    Counter reset           447               448

The ZX Spectrum ULA, Chris Smith
Chapter 11 (Video Sychronisation), Figure 11-2, PAL horizontal vertical counter states and associated screen regions

    Block description    Lines            Length
    --------------------------------------------
    Display              0 - 191          192
    Bottom border        192 - 247        56
    Sync period          248 - 255        8
    Sync pulse           248 - 251        4
    Top border           256 - 311        56
    Clock reset          312 - 312        0

*/

// http://www.worldofspectrum.org/faq/reference/48kreference.htm

class Ula final : public EightBit::ClockedChip {
private:
    static const int LeftRasterBorder = 32;
    static const int RightRasterBorder = 64;
    static const int TopRasterBorder = 56;
    static const int BottomRasterBorder = 56;

    static const int ActiveRasterWidth = 256;
    static const int ActiveRasterHeight = 192;

    static const int HorizontalRetraceClocks = 96;
    static const int VerticalRetraceLines = 8;

    static const int BytesPerLine = ActiveRasterWidth / 8;
    static const int AttributeAddress = 0x1800;

public:
    static constexpr float FramesPerSecond = 50.08f;
    static const int UlaClockRate = 7'000'000; // 7Mhz
    static const int CpuClockRate = UlaClockRate / 2;   // 3.5Mhz

    static const int RasterWidth = LeftRasterBorder + ActiveRasterWidth + RightRasterBorder;
    static const int RasterHeight = TopRasterBorder + ActiveRasterHeight + BottomRasterBorder;

    static const int TotalHeight = VerticalRetraceLines + RasterHeight;
    static const int TotalHorizontalClocks = HorizontalRetraceClocks + RasterWidth;
    static const int TotalFrameClocks = TotalHeight * TotalHorizontalClocks;
    static constexpr float CalculatedClockFrequency = TotalFrameClocks * FramesPerSecond;

public:
    Ula(const ColourPalette& palette, Board& bus);

    EightBit::Signal<EightBit::EventArgs> Proceed;

    void renderLines();

    void pokeKey(SDL_Keycode raw);
    void pullKey(SDL_Keycode raw) noexcept;

    constexpr void setBorder(int border) noexcept {
        m_borderColour = m_palette.colour(border, false);
    }

    [[nodiscard]] constexpr const auto& pixels() const noexcept { return m_pixels; }

private:
    std::array<uint16_t, 256> m_scanLineAddresses;
    std::array<uint16_t, 256> m_attributeAddresses;
    std::array<uint32_t, RasterWidth * RasterHeight> m_pixels;
    const ColourPalette& m_palette;
    Board& m_bus;
    bool m_flashing = false;
    unsigned m_frameCounter : 4;
    unsigned m_verticalCounter : 9;
    unsigned m_horizontalCounter : 9;
    uint32_t m_borderColour = 0;
    int m_contention = 0;
    bool m_accessingVRAM = false;

    // Input port information
    PinLevel m_ear = PinLevel::Low;            // Bit 6

    std::unordered_map<uint8_t, std::array<int, 5>> m_keyboardMapping;
    std::unordered_set<SDL_Keycode> m_keyboardRaw;

    [[nodiscard]] constexpr auto& keyboardMapping() noexcept { return m_keyboardMapping; }
    [[nodiscard]] constexpr const auto& keyboardMapping() const noexcept { return m_keyboardMapping; }
    [[nodiscard]] constexpr auto& keyboardRaw() noexcept { return m_keyboardRaw; }
    [[nodiscard]] constexpr const auto& keyboardRaw() const noexcept { return m_keyboardRaw; }

    [[nodiscard]] constexpr const auto& palette() const noexcept { return m_palette; }

    [[nodiscard]] constexpr auto& scanLineAddresses() noexcept { return m_scanLineAddresses; }
    [[nodiscard]] constexpr const auto& scanLineAddresses() const noexcept { return m_scanLineAddresses; }
    [[nodiscard]] constexpr auto& attributeAddresses() noexcept { return m_attributeAddresses; }
    [[nodiscard]] constexpr const auto& attributeAddresses() const noexcept { return m_attributeAddresses; }

    [[nodiscard]] constexpr auto& BUS() noexcept { return m_bus; }

    void renderLine();

    // Frame counter, four bits
    [[nodiscard]] constexpr auto F() const noexcept { return m_frameCounter; }

    void resetF() noexcept;
    void incrementF() noexcept;

    // Vertical line counter, nine bits
    [[nodiscard]] constexpr auto V() const noexcept { return m_verticalCounter; }

    void resetV() noexcept;
    void incrementV() noexcept;

    // Horizontal pixel counter, nine bits
    [[nodiscard]] constexpr auto C() const noexcept { return m_horizontalCounter; }

    void resetC() noexcept;
    void incrementC() noexcept;

    [[nodiscard]] constexpr auto frameUlaCycles() const noexcept { return TotalHorizontalClocks * V() + C(); }
    [[nodiscard]] constexpr auto frameCpuCycles() const noexcept { return frameUlaCycles() / 2; }

    void initialiseKeyboardMapping();
    void initialiseVRAMAddresses();

    [[nodiscard]] uint8_t findSelectedKeys(uint8_t rows) const;

    [[nodiscard]] auto constexpr usedPort(uint8_t low) const noexcept { return (low & Bit0) == 0; }
    void maybeReadingPort(EightBit::register16_t port);
    void readingPort(EightBit::register16_t port);
    void maybeWrittenPort(EightBit::register16_t port);
    void writtenPort(EightBit::register16_t port);

    void maybeFlash() noexcept;
    void flash() noexcept;
    [[nodiscard]] constexpr auto& flashing() noexcept { return m_flashing; }
    [[nodiscard]] constexpr auto flashing() const noexcept { return m_flashing; }

    void processActiveLine();
    void processActiveLine(int y);

    void processVerticalSync();
    void processVerticalSync(int y);

    void processBottomBorder();
    void processTopBorder();

    void processBorder(int y);

    void renderLeftRasterBorder(int y);
    void renderRightRasterBorder(int y);
    void renderRasterBorder(int x, int y, int width);

    void renderVRAM(int y);

    // Paint a pixel and tick the ULA clock
    void setClockedPixel(size_t offset, uint32_t colour);

    // Paint a pixel
    void setPixel(size_t offset, uint32_t colour) noexcept;

    // Address contention?

    [[nodiscard]] static bool contended(uint16_t address) noexcept;
    bool maybeContend(uint16_t address) noexcept;
    bool maybeContend() noexcept;
    void addContention(int cycles) noexcept;
    [[nodiscard]] constexpr auto contention() const noexcept { return m_contention; }
    bool maybeApplyContention() noexcept;
};
