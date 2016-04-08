#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#include "../Base/FontDataFile.h"
#include "../Base/StringUtils.h"

//This is the code responsible for creating FontFiles.  It uses Direct2D and DirectWrite to generate an 8bit alpha texture of the font.
//It also stores information about individual glyph LSB and RSB to try and preserve decent spacing for overlapping characters in fonts.
class FontBuilder : public IDWriteTextRenderer
{
public:
	FontBuilder::FontBuilder()
	{
		m_fCurrentX = 0.f;
		m_fCurrentY = 0.f;
		m_fBaseline = 0.f;

		m_mGlyphs.clear();

		//Just in case no one else called it.
		HRESULT hr = CoInitialize(NULL);

		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pWICFactory);
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	}

	FontBuilder::~FontBuilder()
	{
		//Clean Up
		pBrush->Release();
		pRT->Release();
		pD2DFactory->Release();

		pFont->Release();
		pFontFamily->Release();
		pFontCollection->Release();
		pTextFormat->Release();
		pDWriteFactory->Release();

		pWICBitmap->Release();
		pWICFactory->Release();
	}

	void Build(std::wstring& in_FontName, float in_FontSize, int in_TextureSize)
	{
		m_TextureSize = in_TextureSize;

		//Init the string we'll use for creating the FontFileTexture
		wchar_t wszTextureText[512] = {};
		for (int i = 1; i <= 255; i++)
			wszTextureText[i - 1] = i;
		UINT32 cTextureTextLength = (UINT32)wcslen(wszTextureText);

		//Create the WIC Bitmap
		HRESULT hr = pWICFactory->CreateBitmap(in_TextureSize, in_TextureSize, GUID_WICPixelFormat8bppAlpha, WICBitmapCacheOnLoad, &pWICBitmap);

		//Create the requested FontFormat
		hr = pDWriteFactory->CreateTextFormat(in_FontName.c_str(), NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, in_FontSize * (96.f / 72.f), L"en-us", &pTextFormat);
		hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		
		//Get IDWriteFont* in order to be able to use ->HasCharacter() when building the FontFile
		pTextFormat->GetFontCollection(&pFontCollection);
		UINT32 index;
		BOOL exists;
		hr = pFontCollection->FindFamilyName(in_FontName.c_str(), &index, &exists);
		
		hr = pFontCollection->GetFontFamily(index, &pFontFamily);
		hr = pFontFamily->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL, &pFont);

		//D2D1 RenderTarget creation linking with our WICBitmap
		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
		hr = pD2DFactory->CreateWicBitmapRenderTarget(pWICBitmap, rtProps, &pRT);
		pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

		//Create the brush used to draw the Text
		hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBrush);

		//Start rendering the FontTexture
		pRT->BeginDraw();
		pRT->SetTransform(D2D1::IdentityMatrix());
		pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.f));

		//Render characters one at a time in order to avoid : unwanted line breaks, kerning and other spacing oddities.
		for (unsigned int i = 0; i < cTextureTextLength; i++)
		{
			IDWriteTextLayout* pTextLayout = NULL;
			hr = pDWriteFactory->CreateTextLayout(wszTextureText + i, 1, pTextFormat, (FLOAT)in_TextureSize, (FLOAT)in_TextureSize, (IDWriteTextLayout**)&pTextLayout);
			pTextLayout->Draw(NULL, this, this->m_fCurrentX, this->m_fCurrentY);
			pTextLayout->Release();
		}

		hr = pRT->EndDraw();

		//Lock the WICBitmap and use it to save the font file
		WICRect lock = { 0, 0, in_TextureSize, in_TextureSize };
		IWICBitmapLock* pBitmapLock;
		hr = pWICBitmap->Lock(&lock, WICBitmapLockRead, &pBitmapLock);
		unsigned char* pointer;
		UINT size;
		pBitmapLock->GetDataPointer(&size, &pointer);
		
		Save(Format(L"%s_%dpt.dat", in_FontName.c_str(), (int)in_FontSize), in_FontName, in_FontSize, in_TextureSize, pointer);
		pBitmapLock->Release();
	}

private:
	unsigned int m_TextureSize;

	float m_fCurrentX;
	float m_fCurrentY;

	float m_fBaseline;

	std::map<wchar_t, FontDataFile::GlyphInfo> m_mGlyphs;

	IWICImagingFactory* pWICFactory = NULL;
	IWICBitmap *pWICBitmap = NULL;

	ID2D1Factory* pD2DFactory = NULL;
	ID2D1RenderTarget* pRT = NULL;
	ID2D1SolidColorBrush* pBrush = NULL;

	IDWriteFactory* pDWriteFactory = NULL;
	IDWriteTextFormat* pTextFormat = NULL;
	IDWriteFont* pFont = NULL;
	IDWriteFontCollection* pFontCollection = NULL;
	IDWriteFontFamily* pFontFamily = NULL;
	
	//IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) { return S_OK; };
	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; };
	ULONG STDMETHODCALLTYPE Release(void) { return 1; };

	void Save(std::wstring& in_Filename, const std::wstring& in_FontName, float in_FontSize, unsigned int in_TextureSize, unsigned char* pFontData)
	{
		FontDataFile* pFontDataFile = new FontDataFile(in_FontName, in_FontSize, in_TextureSize, m_mGlyphs, pFontData);
		pFontDataFile->Save(in_Filename);
		delete pFontDataFile;
	}

	//IDWriteTextRenderer
	STDMETHOD(DrawGlyphRun)(_In_opt_ void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode, _In_ DWRITE_GLYPH_RUN const* glyphRun, _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription, _In_opt_ IUnknown* clientDrawingEffect)
	{
		HRESULT hr = S_OK;

		DWRITE_FONT_METRICS fontMetrics;
		glyphRun->fontFace->GetMetrics(&fontMetrics);

		FLOAT designUnitsToPixels = 1.f / fontMetrics.designUnitsPerEm * glyphRun->fontEmSize;
		FLOAT lineHeight = ceilf(FLOAT(fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap) * designUnitsToPixels);

		BOOL exists = FALSE;
		hr = pFont->HasCharacter(glyphRunDescription->string[0], &exists);

		//If this characters mapped to index 0, ignore it.  The character is not supported by this font.
		if (!exists || glyphRun->glyphIndices[0] == 0)
			return S_OK;

		//This is a bit of a hack.. For some reason the baseline offset jumps around from line to line otherwise.
		if (m_fBaseline == 0.f)
			m_fBaseline = baselineOriginY;

		DWRITE_GLYPH_METRICS metrics;
		hr = glyphRun->fontFace->GetDesignGlyphMetrics(&glyphRun->glyphIndices[0], 1, &metrics);

		FontDataFile::GlyphInfo info;
		info.advance = FLOAT(metrics.advanceWidth) * designUnitsToPixels;
		info.lsb = FLOAT(metrics.leftSideBearing) * designUnitsToPixels;
		info.rsb = FLOAT(metrics.rightSideBearing) * designUnitsToPixels;

		float boxWidth = info.advance;
		if (info.lsb < 0)
			boxWidth -= info.lsb;

		if (info.rsb < 0)
			boxWidth -= info.rsb;

		if (m_fCurrentX + boxWidth > m_TextureSize)
		{
			m_fCurrentX = 0.f;
			m_fCurrentY += lineHeight;
		}

		info.boxX = m_fCurrentX / m_TextureSize;
		info.boxY = m_fCurrentY / m_TextureSize;
		info.boxWidth = (m_fCurrentX + boxWidth) / m_TextureSize;
		info.boxHeight = (m_fCurrentY + lineHeight) / m_TextureSize;

		m_mGlyphs[glyphRunDescription->string[0]] = info;

		D2D1_RECT_F rc = { m_fCurrentX, m_fCurrentY, 1024, 1024 };
		if (info.lsb < 0)
			rc.left -= info.lsb;

		pRT->DrawTextW(glyphRunDescription->string, 1, pTextFormat, rc, pBrush);

		m_fCurrentX += boxWidth;

		return S_OK;
	}

	STDMETHOD(DrawUnderline)(_In_opt_ void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, _In_ DWRITE_UNDERLINE const* underline, _In_opt_ IUnknown* clientDrawingEffect)
	{
		return S_OK;
	}

	STDMETHOD(DrawStrikethrough)(_In_opt_ void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, _In_ DWRITE_STRIKETHROUGH const* strikethrough, _In_opt_ IUnknown* clientDrawingEffect)
	{
		return S_OK;
	}

	STDMETHOD(DrawInlineObject)(_In_opt_ void* clientDrawingContext, FLOAT originX, FLOAT originY, _In_ IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, _In_opt_ IUnknown* clientDrawingEffect)
	{
		return S_OK;
	}

	STDMETHOD(IsPixelSnappingDisabled)(_In_opt_ void* clientDrawingContext, _Out_ BOOL* isDisabled)
	{
		*isDisabled = FALSE;
		return S_OK;
	}

	STDMETHOD(GetCurrentTransform)(_In_opt_ void* clientDrawingContext, _Out_ DWRITE_MATRIX* transform)
	{
		pRT->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
		return S_OK;
	}

	STDMETHOD(GetPixelsPerDip)(_In_opt_ void* clientDrawingContext, _Out_ FLOAT* pixelsPerDip)
	{
		*pixelsPerDip = 1.f;
		return S_OK;
	}
};

//Create Font from a system installed Font
void BuildFont(std::wstring in_FontName, float in_fontSize, unsigned int textureSize)
{
	FontBuilder* fontBuilder = new FontBuilder;
	fontBuilder->Build(in_FontName, in_fontSize, textureSize);
	delete fontBuilder;
}