// C API for the wasm build of x2t.
//
// The other wasm modules in core (OfficeUtils/js, DesktopEditor/fontengine/js,
// DesktopEditor/graphics/pro/js, DesktopEditor/xmlsec/src/wasm/hash) all export
// a handful of C functions and are driven from JS; none of them goes through
// main(). x2t follows the same shape here.
//
// Going through main() does not work: converting a binary document back to
// docx hangs, while the identical conversion through this entry point returns
// normally.

#include <emscripten.h>
#include <string.h>
#include <string>

#include "../../src/cextracttools.h"
#include "../../src/ASCConverters.h"
#include "../../../DesktopEditor/fontengine/ApplicationFontsWorker.h"

static std::wstring from_utf8(const char* s)
{
    return NSFile::CUtf8Converter::GetUnicodeStringFromUTF8((BYTE*)s, (LONG)strlen(s));
}

extern "C" {

// Convert one file to another. The direction is taken from the extensions,
// the same way x2t's two-argument command line does.
// Returns 0 on success.
EMSCRIPTEN_KEEPALIVE
int x2t_convert(const char* input, const char* output)
{
    NExtractTools::InputParams params;
    params.m_sFileFrom = new std::wstring(from_utf8(input));
    params.m_sFileTo   = new std::wstring(from_utf8(output));
    return (int)NExtractTools::fromInputParams(params);
}

// Build AllFonts.js and the font cache that sdkjs needs. Fonts are read from
// fontsDir; the artifacts are written to outDir. System fonts are not visible
// to wasm, so the caller has to place the font files in MEMFS first.
EMSCRIPTEN_KEEPALIVE
int x2t_create_allfonts(const char* outDir, const char* fontsDir)
{
    CApplicationFontsWorker worker;
    worker.m_sDirectory        = from_utf8(outDir);
    worker.m_bIsUseSystemFonts = false;
    worker.m_bIsCleanDirectory = false;
    worker.m_arAdditionalFolders.push_back(from_utf8(fontsDir));

    // Font name previews for the editor's font dropdown.
    worker.m_bIsNeedThumbnails = true;
    worker.m_arThumbnailsScales.push_back(1.0);
    worker.m_arThumbnailsScales.push_back(1.25);
    worker.m_arThumbnailsScales.push_back(1.5);
    worker.m_arThumbnailsScales.push_back(1.75);
    worker.m_arThumbnailsScales.push_back(2.0);

    NSFonts::IApplicationFonts* pFonts = worker.Check();
    if (!pFonts) return -1;
    pFonts->Release();
    return 0;
}

}
