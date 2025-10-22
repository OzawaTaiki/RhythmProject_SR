
#ifndef COMPLEX_HLSLI
#define COMPLEX_HLSLI


// 虚数数を表す構造体
struct Complex
{
    float real;
    float imag;

    Complex Add(Complex other)
    {
        return (Complex) (real + other.real, imag + other.imag);
    }

    Complex Sub(Complex other)
    {
        return (Complex) (real - other.real, imag - other.imag);
    }
    Complex Mul(Complex other)
    {
        return (Complex) (real * other.real - imag * other.imag,
                          real * other.imag + imag * other.real);
    }

    float Abs()
    {
        return length(float2(real, imag));
    }

};

#endif // COMPLEX_HLSLI
