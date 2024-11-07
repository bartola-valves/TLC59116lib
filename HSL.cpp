#include <cmath> // for fabs and fmod

// define the HSL colour structure
struct HSL
{
    float hue;
    float saturation;
    float lightness;
};

// define the HSL colour for light orange
HSL lightOrangeHSL = {30.0f, 1.0f, 0.7f};
// define the HSL colour for dark orange
HSL darkOrangeHSL = {30.0f, 1.0f, 0.5f};

// Define the HSL to RGB conversion function
void hsl_to_rgb(float h, float s, float l, uint8_t &r, uint8_t &g, uint8_t &b)
{
    float c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;
    float r1, g1, b1;

    if (h >= 0.0f && h < 60.0f)
    {
        r1 = c;
        g1 = x;
        b1 = 0.0f;
    }
    else if (h >= 60.0f && h < 120.0f)
    {
        r1 = x;
        g1 = c;
        b1 = 0.0f;
    }
    else if (h >= 120.0f && h < 180.0f)
    {
        r1 = 0.0f;
        g1 = c;
        b1 = x;
    }
    else if (h >= 180.0f && h < 240.0f)
    {
        r1 = 0.0f;
        g1 = x;
        b1 = c;
    }
    else if (h >= 240.0f && h < 300.0f)
    {
        r1 = x;
        g1 = 0.0f;
        b1 = c;
    }
    else
    {
        r1 = c;
        g1 = 0.0f;
        b1 = x;
    }

    r = static_cast<uint8_t>((r1 + m) * 255.0f);
    g = static_cast<uint8_t>((g1 + m) * 255.0f);
    b = static_cast<uint8_t>((b1 + m) * 255.0f);

    // Print intermediate values for debugging
    // printf("HSL: (%f, %f, %f)\n", h, s, l);
    // printf("Intermediate RGB: (%f, %f, %f)\n", r1, g1, b1);
    // printf("RGB: (%d, %d, %d)\n", r, g, b);
}