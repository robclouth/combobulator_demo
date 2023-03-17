#pragma once

#include <emmintrin.h>
#include <juce_dsp/juce_dsp.h>

class MathUtils
{
public:
  static double dbToLinear (double dB)
  {
    return pow (10.0, (0.05 * dB));
  }

  static double linearToDB (double linear)
  {
    return 20.0 * log10 (linear);
  }

  static float radToDeg (float rad)
  {
    return 180 * rad / juce::float_Pi;
  }

  static void carToPol (float real, float imag, float& mag, float& phase)
  {
    mag = hypot (real, imag);
    phase = -atan2 (imag, real);
  }

  static void polToCar (float mag, float phase, float& real, float& imag)
  {
    real = mag * fastcos (phase);
    imag = -mag * fastsin (phase);
  }

  static inline double fastPow (double a, double b)
  {
    union {
      double d;
      int x[2];
    } u = { a };
    u.x[1] = (int) (b * (u.x[1] - 1072632447) + 1072632447);
    u.x[0] = 0;
    return u.d;
  }

  static inline float fastcos (float x) noexcept
  {
    const float tp = 1. / (2. * juce::float_Pi);
    x *= tp;
    x -= float (.25) + floor (x + float (.25));
    x *= float (16.) * (std::abs (x) - float (.5));
#if EXTRA_PRECISION
    x += T (.225) * x * (std::abs (x) - T (1.));
#endif
    return x;
  }

  static inline float fastsin (float x) noexcept
  {
    return fastcos (x - juce::float_Pi / 2);
  }

  static inline float minss (float a, float b)
  {
    // Branchless SSE min.
    _mm_store_ss (&a, _mm_min_ss (_mm_set_ss (a), _mm_set_ss (b)));
    return a;
  }

  static inline float maxss (float a, float b)
  {
    // Branchless SSE max.
    _mm_store_ss (&a, _mm_max_ss (_mm_set_ss (a), _mm_set_ss (b)));
    return a;
  }

  static inline float fastclamp (float val, float minval, float maxval)
  {
    // Branchless SSE clamp.
    // return minss( maxss(val,minval), maxval );

    _mm_store_ss (&val, _mm_min_ss (_mm_max_ss (_mm_set_ss (val), _mm_set_ss (minval)), _mm_set_ss (maxval)));
    return val;
  }

  static inline float fastmod1 (float val)
  {
    return val - ((long) val);
  }
};
