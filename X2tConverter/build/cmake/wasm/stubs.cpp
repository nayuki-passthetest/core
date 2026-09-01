// Stubs for the modules that are excluded from the wasm build.
//
// ASCConverters.cpp dispatches over every format x2t knows, so these get
// referenced at link time even though the conversion never runs. The modules
// themselves cannot target wasm:
//
//   doctrenderer   needs v8
//   COOXMLSigner   needs the certificate half of OpenSSL; the wasm 3rd-party
//                  build only ships the hash half
//   CIWorkFile     pulls in the HTML stack, which drags in the network layer
//   CHtmlFile2     same, via katana/gumbo
//   CFb2File       reached through EpubFile
//
// None of them is on the docx/xlsx/pptx path. Each returns failure rather than
// succeeding silently.

#include "../../../../HtmlFile2/htmlfile2.h"
#include "../../../../Apple/IWork.h"
#include "../../../../Common/3dParty/md/md2html.h"
#include "../../../../DesktopEditor/xmlsec/src/include/OOXMLSigner.h"
#include "../../../../DesktopEditor/xmlsec/src/include/CertificateCommon.h"
#include "../../../../Fb2File/Fb2File.h"
#include "../../../../EpubFile/CEpubFile.h"
#include "../../../../Common/Network/FileTransporter/include/FileTransporter.h"
#include "../../../../DesktopEditor/doctrenderer/docbuilder.h"
#include "../../../../DesktopEditor/doctrenderer/doctrenderer.h"

// ---- Apple iWork ------------------------------------------------------
CIWorkFile::CIWorkFile() { m_internal = nullptr; }
CIWorkFile::~CIWorkFile() {}
IWorkFileType CIWorkFile::GetType(const std::wstring&) const { return IWorkFileType::None; }
int  CIWorkFile::Convert2Odf(const std::wstring&, const std::wstring&) const { return 1; }
void CIWorkFile::SetTmpDirectory(const std::wstring&) {}

// ---- Markdown ---------------------------------------------------------
namespace Md { bool ConvertMdFileToHtml(const std::wstring&, const std::wstring&) { return false; } }

// ---- OOXML signatures -------------------------------------------------
COOXMLSigner::COOXMLSigner(const std::wstring&, ICertificate*) { m_internal = nullptr; }
COOXMLSigner::~COOXMLSigner() {}
int COOXMLSigner::Sign() { return 1; }

namespace NSCertificate {
    ICertificate* FromFiles(const std::wstring&, const std::string&,
                            const std::wstring&, const std::string&) { return nullptr; }
}

// ---- FB2 --------------------------------------------------------------
CFb2File::CFb2File() {}
CFb2File::~CFb2File() {}
void CFb2File::SetTmpDirectory(const std::wstring&) {}
HRESULT CFb2File::Open(const std::wstring&, const std::wstring&, CFb2Params*) { return S_FALSE; }

// ---- EPUB -------------------------------------------------------------
CEpubFile::CEpubFile() {}
CEpubFile::~CEpubFile() {}
void CEpubFile::SetTempDirectory(const std::wstring&) {}
HRESULT CEpubFile::Convert(const std::wstring&, const std::wstring&, bool) { return S_FALSE; }
HRESULT CEpubFile::FromHtml(const std::wstring&, const std::wstring&, const std::wstring&) { return S_FALSE; }
HRESULT CFb2File::FromHtml(const std::wstring&, const std::wstring&, const std::wstring&) { return S_FALSE; }

// ---- Remote download --------------------------------------------------
// The network layer is an empty target in the wasm build (see Common/Network).
namespace NSNetwork { namespace NSFileTransport {
    CFileDownloader::CFileDownloader(std::wstring, bool) { m_pInternal = nullptr; }
    CFileDownloader::~CFileDownloader() {}
    bool CFileDownloader::DownloadSync() { return false; }
    std::wstring CFileDownloader::GetFilePath() { return L""; }
    // IFileTransporter is pure virtual, so every override has to exist for the vtable.
    void CFileDownloader::SetSession(CSession*) {}
    void CFileDownloader::Start(int) {}
    int  CFileDownloader::GetPriority() { return 0; }
    void CFileDownloader::Suspend() {}
    void CFileDownloader::Resume() {}
    void CFileDownloader::Stop() {}
    void CFileDownloader::StopNoJoin() {}
    void CFileDownloader::Cancel() {}
    int  CFileDownloader::IsRunned() { return 0; }
    void CFileDownloader::SetEvent_OnProgress(std::function<void(int)>) {}
    void CFileDownloader::SetEvent_OnComplete(std::function<void(int)>) {}
}}

// CEpubFile holds these by value, so their ctors/dtors are referenced even
// though the EPUB path never runs.
CBookInfo::CBookInfo() {}
CBookInfo::~CBookInfo() {}
CBookToc::CBookToc() {}
CBookToc::~CBookToc() {}
CBookItem::~CBookItem() {}
CBookContentItem::~CBookContentItem() {}

// ---- HTML / MHT -------------------------------------------------------
CHtmlFile2::CHtmlFile2() { m_pReader = nullptr; }
CHtmlFile2::~CHtmlFile2() {}
void CHtmlFile2::SetTempDirectory(const std::wstring&) {}
HRESULT CHtmlFile2::ConvertHTML2OOXML(const std::wstring&, const std::wstring&, HTML::THTMLParameters*) { return S_FALSE; }
HRESULT CHtmlFile2::ConvertMHT2OOXML(const std::wstring&, const std::wstring&, HTML::THTMLParameters*) { return S_FALSE; }
HRESULT CHtmlFile2::ConvertHTML2OOXML(const std::vector<std::wstring>&, const std::wstring&, HTML::THTMLParameters*) { return S_FALSE; }
HRESULT CHtmlFile2::ConvertHTML2Markdown(const std::wstring&, const std::wstring&, HTML::TMarkdownParameters*) { return S_FALSE; }

// ---- doctrenderer (v8) ------------------------------------------------
namespace NSDoctRenderer
{
    void CDocBuilder::Initialize(const wchar_t*) {}
    void CDocBuilder::Dispose() {}

    CDoctrenderer::CDoctrenderer(const std::wstring&) { m_pInternal = 0; }
    CDoctrenderer::~CDoctrenderer() {}
    void CDoctrenderer::LoadConfig(const std::wstring&, const std::wstring&) {}
    bool CDoctrenderer::Execute(const std::wstring&, std::wstring& sError)
    { sError = L"doctrenderer is not available in the wasm build"; return false; }
    std::vector<std::wstring> CDoctrenderer::GetImagesInChanges() { return std::vector<std::wstring>(); }
    void CDoctrenderer::CreateCache(const std::wstring&, const std::wstring&) {}
    void CDoctrenderer::CreateSnapshots() {}
    void CDoctrenderer::SetAdditionalParam(const AdditionalParamType&, void*) {}
}
