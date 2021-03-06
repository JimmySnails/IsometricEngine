#ifndef COLOR_HXX_
#define COLOR_HXX_

#include <cstdint>
#include <iostream>

class SDL_Color;

struct RGBAColor
{
public:
  constexpr RGBAColor(uint32_t color) : m_Color(color) { }
  constexpr RGBAColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : 
    m_Color((r << 24) | (g << 16) | (b << 8) | a)
  {
  }

  float hue() const noexcept;
  float lightness() const noexcept;
  float  saturation() const noexcept;
  uint8_t red() const noexcept;
  uint8_t green() const noexcept;
  uint8_t blue() const noexcept;
  uint8_t alpha() const noexcept;
  operator uint32_t() const noexcept;
  [[deprecated("Use sdl::toColor instead")]]
  SDL_Color to_SDL() const noexcept;

  /**
   * @returns   An RGBA representation of an HSLA color
   * @details   This operation is not invertible. This is due
   *            because HSL has less many possibilities than
   *            RGB.
   */
  static RGBAColor fromHSLA(float h, float s, float l, uint8_t a);

private:
  uint32_t m_Color;
  
  /**
   * @returns   Chroma of hue & angle
   */
  static float hsla_chroma(float hue, float angle);
  
  /**
   * @returns   RGBA value from lightness, range, and chroma
   */
  static float hsla_to_value(float lightness, float range, float chroma);
};

constexpr RGBAColor operator""_rgba(unsigned long long val) { return RGBAColor{static_cast<uint32_t>(val)}; }

std::ostream &operator<<(std::ostream &, const RGBAColor &);

namespace Palette
{
constexpr RGBAColor Gray = 0x858b8cFF_rgba;
constexpr RGBAColor Orange = 0xFF771EFF_rgba;
constexpr RGBAColor Black = 0x000000FF_rgba;
constexpr RGBAColor White = 0xFFFFFFFF_rgba;
}; // namespace Palette

#endif // COLOR_HXX_
