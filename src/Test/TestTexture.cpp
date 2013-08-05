/*
* Simd Library Tests.
*
* Copyright (c) 2011-2013 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/Test.h"

namespace Test
{
    namespace
    {
        struct Func1
        {
            typedef void (*FuncPtr)(const uchar * src, size_t srcStride, size_t width, size_t height, 
                uchar saturation, uchar boost, uchar * dx, size_t dxStride, uchar * dy, size_t dyStride);

            FuncPtr func;
            std::string description;

            Func1(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

            void Call(const View & src, uchar saturation, uchar boost, View &  dx, View & dy) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, saturation, boost, dx.data, dx.stride, dy.data, dy.stride);
            }
        };
    }
#define FUNC1(function) Func1(function, #function)

    bool TextureBoostedSaturatedGradientTest(int width, int height, int saturation, int boost, const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        std::cout << "Test " << f1.description << " & " << f2.description 
            << " [" << width << ", " << height << "] <" << saturation << ", " << boost << ">." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dx1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dx2(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dy2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, saturation, boost, dx1, dy1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, saturation, boost, dx2, dy2));

        result = result && Compare(dx1, dx2, 0, true, 32, 0, "dx");
        result = result && Compare(dy1, dy2, 0, true, 32, 0, "dy");

        return result;
    }

    bool TextureBoostedSaturatedGradientTest(int width, int height, const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientTest(width, height, 32, 3, f1, f2);
        result = result && TextureBoostedSaturatedGradientTest(width, height, 16, 4, f1, f2);
        result = result && TextureBoostedSaturatedGradientTest(width, height, 16, 5, f1, f2);

        return result;
    }

    bool TextureBoostedSaturatedGradientTest()
    {
        bool result = true;

        result = result && TextureBoostedSaturatedGradientTest(W, H, FUNC1(Simd::Base::TextureBoostedSaturatedGradient), FUNC1(Simd::TextureBoostedSaturatedGradient));
        result = result && TextureBoostedSaturatedGradientTest(W - 1, H + 1, FUNC1(Simd::Base::TextureBoostedSaturatedGradient), FUNC1(Simd::TextureBoostedSaturatedGradient));

#if defined(SIMD_SSE2_ENABLE) && defined(SIMD_AVX2_ENABLE)
        if(Simd::Sse2::Enable && Simd::Avx2::Enable)
        {
            result = result && TextureBoostedSaturatedGradientTest(W, H, FUNC1(Simd::Sse2::TextureBoostedSaturatedGradient), FUNC1(Simd::Avx2::TextureBoostedSaturatedGradient));
            result = result && TextureBoostedSaturatedGradientTest(W - 1, H + 1, FUNC1(Simd::Sse2::TextureBoostedSaturatedGradient), FUNC1(Simd::Avx2::TextureBoostedSaturatedGradient));
        }
#endif 

        return result;
    }

    namespace
    {
        struct Func2
        {
            typedef void (*FuncPtr)(const uchar * src, size_t srcStride, size_t width, size_t height, 
                uchar boost, uchar * dst, size_t dstStride);

            FuncPtr func;
            std::string description;

            Func2(const FuncPtr & f, const std::string & d) : func(f), description(d) {}

            void Call(const View & src, uchar boost, View &  dst) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, boost, dst.data, dst.stride);
            }
        };
    }
#define FUNC2(function) Func2(function, #function)

    bool TextureBoostedUvTest(int width, int height, int boost, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        std::cout << "Test " << f1.description << " & " << f2.description 
            << " [" << width << ", " << height << "] <" << boost << ">." << std::endl;

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        View dst1(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        View dst2(width, height, View::Gray8, NULL, TEST_ALIGN(width));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, boost, dst1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, boost, dst2));

        result = result && Compare(dst1, dst2, 0, true, 32, 0);

        return result;
    }

    bool TextureBoostedUvTest(int width, int height, const Func2 & f1, const Func2 & f2)
    {
        bool result = true;

        result = result && TextureBoostedUvTest(width, height, 3, f1, f2);
        result = result && TextureBoostedUvTest(width, height, 4, f1, f2);
        result = result && TextureBoostedUvTest(width, height, 5, f1, f2);

        return result;
    }

    bool TextureBoostedUvTest()
    {
        bool result = true;

        result = result && TextureBoostedUvTest(W, H, FUNC2(Simd::Base::TextureBoostedUv), FUNC2(Simd::TextureBoostedUv));
        result = result && TextureBoostedUvTest(W - 1, H + 1, FUNC2(Simd::Base::TextureBoostedUv), FUNC2(Simd::TextureBoostedUv));

#if defined(SIMD_SSE2_ENABLE) && defined(SIMD_AVX2_ENABLE)
        if(Simd::Sse2::Enable && Simd::Avx2::Enable)
        {
            result = result && TextureBoostedUvTest(W, H, FUNC2(Simd::Sse2::TextureBoostedUv), FUNC2(Simd::Avx2::TextureBoostedUv));
            result = result && TextureBoostedUvTest(W - 1, H + 1, FUNC2(Simd::Sse2::TextureBoostedUv), FUNC2(Simd::Avx2::TextureBoostedUv));
        }
#endif 

        return result;
    }
}
