
#ifndef COMPLEX_HLSLI
#define COMPLEX_HLSLI


// 虚数数を表す構造体
struct Complex
{
    float real;
    float imag;

    Complex Add(Complex other)
    {
        Complex c =
        {
            real + other.real,
            imag + other.imag
        };
        return c;
    }

    Complex Sub(Complex other)
    {
        Complex c =
        {
            real - other.real,
            imag - other.imag
        };
        return c;
    }
    Complex Mul(Complex other)
    {
        Complex c =
        {
            real * other.real - imag * other.imag,
            real * other.imag + imag * other.real
        };
        return c;
    }

    float Abs()
    {
        return length(float2(real, imag));
    }

};

#endif // COMPLEX_HLSLI
