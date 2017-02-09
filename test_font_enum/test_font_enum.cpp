// test_font_enum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <cstdint>

#include <dwrite.h>
#include <Dwrite_1.h>
#include <iostream>

wchar_t  textString   [] = L"`1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./";
uint32_t codePoints   [_countof(textString)]{};
uint16_t glyphIndieces[_countof(textString)]{};

int main()
{
    for (size_t i     = 0; i < _countof(textString) - 1; ++i)
        codePoints[i] = textString[i];

    CComPtr<IDWriteFactory> pDWriteFactory;
    auto hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&pDWriteFactory));

    CComPtr<IDWriteFontCollection> fontCollection;
    // Get the system font collection.
    if (SUCCEEDED(hr))
    {
        hr = pDWriteFactory->GetSystemFontCollection(&fontCollection);
    }

    UINT32 familyCount = 0;
    // Get the number of font families in the collection.
    if (SUCCEEDED(hr))
    {
        familyCount = fontCollection->GetFontFamilyCount();
    }
    const auto start = ::GetTickCount64();
    for (uint32_t i = 0; i < familyCount; ++i)
    {
        CComPtr<IDWriteFontFamily> fontFamily;
        hr = fontCollection->GetFontFamily(i, &fontFamily);
        if (FAILED(hr))
        {
            assert(false);
            continue;
        }
        CComPtr<IDWriteLocalizedStrings> names;
        hr = fontFamily->GetFamilyNames(&names);

        uint32_t familyNameLength = 0;
        hr                        = names->GetStringLength(0, &familyNameLength);
        if (FAILED(hr))
        {
            assert(false);
            continue;
        }

        std::wstring familyBuffer(familyNameLength + 1, L'\0');

        hr                   = names->GetString(0, &familyBuffer[0], static_cast<uint32_t>(familyBuffer.size()));
        const auto fontCount = fontFamily->GetFontCount();
        for (uint32_t familyIndex = 0; familyIndex < fontCount; ++familyIndex)
        {
            CComPtr<IDWriteFont> font;
            hr = fontFamily->GetFont(familyIndex, &font);

            CComPtr<IDWriteFontFace> baseFontFace;
            hr = font->CreateFontFace(&baseFontFace);

            CComPtr<IDWriteFontFace1> fontFace;
            hr = baseFontFace.QueryInterface(&fontFace);
            if (FAILED(hr))
            {
                assert(false);
                continue;
            }
            // Only try to load true-type fonts ??
            const auto type      = fontFace->GetType();
            const auto weight    = font->GetWeight();
            const auto stretch   = font->GetStretch();
            const auto stely     = font->GetStyle();
            const auto monospace = fontFace->IsMonospacedFont();

            DWRITE_FONT_METRICS metrix{};
            font->GetMetrics(&metrix);

            hr = (baseFontFace->GetGlyphIndices)(codePoints, _countof(codePoints) - 1, glyphIndieces);
            if (FAILED(hr))
            {
                assert(false);
                continue;
            }
        }
    }
    const auto finish = ::GetTickCount64() - start;
    std::cout << finish;

    return 0;
}
