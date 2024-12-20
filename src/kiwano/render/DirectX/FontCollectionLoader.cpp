// Copyright (c) 2016-2018 Kiwano - Nomango
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// _Out_ OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <kiwano/render/DirectX/FontCollectionLoader.h>

namespace kiwano
{
namespace graphics
{
namespace directx
{
////////////////////////////////////////////////////////////////////////////////////////
//
// FontCollectionLoader
//
////////////////////////////////////////////////////////////////////////////////////////

class FontCollectionLoader : public IFontCollectionLoader
{
public:
    FontCollectionLoader()
        : refCount_(0)
    {
    }

    STDMETHOD(AddFilePaths)
    (const Vector<String>& filePaths, _Out_ LPVOID* pCollectionKey, _Out_ uint32_t* pCollectionKeySize);

    // IDWriteFontCollectionLoader methods
    virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(IDWriteFactory* pFactory, void const* collectionKey,
                                                              uint32_t                          collectionKeySize,
                                                              _Out_ IDWriteFontFileEnumerator** fontFileEnumerator);

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE   AddRef();
    virtual ULONG STDMETHODCALLTYPE   Release();

private:
    ULONG refCount_;

    typedef Vector<String> FileCollection;
    Vector<FileCollection> filePaths_;
    Vector<size_t>         collectionKeys_;
};

HRESULT IFontCollectionLoader::Create(_Out_ IFontCollectionLoader** ppCollectionLoader)
{
    HRESULT hr = S_OK;

    if (!ppCollectionLoader)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        FontCollectionLoader* pCollectionLoader = new (std::nothrow) FontCollectionLoader;
        hr                                      = pCollectionLoader ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*ppCollectionLoader);
            (*ppCollectionLoader) = DX::SafeAcquire(pCollectionLoader);
        }
    }
    return hr;
}

STDMETHODIMP FontCollectionLoader::AddFilePaths(const Vector<String>& filePaths, _Out_ LPVOID* pCollectionKey,
                                                _Out_ uint32_t* pCollectionKeySize)
{
    if (!pCollectionKey || !pCollectionKeySize)
    {
        return E_INVALIDARG;
    }

    try
    {
        size_t collectionKey = filePaths_.size();
        collectionKeys_.push_back(collectionKey);
        filePaths_.push_back(filePaths);

        *pCollectionKey     = reinterpret_cast<LPVOID>(&collectionKeys_.back());
        *pCollectionKeySize = sizeof(collectionKey);
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE FontCollectionLoader::CreateEnumeratorFromKey(
    IDWriteFactory* pFactory, void const* collectionKey, uint32_t collectionKeySize,
    _Out_ IDWriteFontFileEnumerator** fontFileEnumerator)
{
    HRESULT hr = S_OK;

    if (collectionKey == NULL || collectionKeySize % sizeof(uint32_t) != 0)
        hr = E_INVALIDARG;

    if (SUCCEEDED(hr))
    {
        IFontFileEnumerator* pEnumerator = NULL;
        hr                               = IFontFileEnumerator::Create(&pEnumerator, pFactory);

        if (SUCCEEDED(hr))
        {
            const uint32_t fileIndex = *static_cast<uint32_t const*>(collectionKey);
            hr                       = pEnumerator->SetFilePaths(filePaths_[fileIndex]);
        }

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*fontFileEnumerator);
            (*fontFileEnumerator) = pEnumerator;
        }
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE FontCollectionLoader::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontCollectionLoader))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE FontCollectionLoader::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE FontCollectionLoader::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// FontFileEnumerator
//
////////////////////////////////////////////////////////////////////////////////////////

class FontFileEnumerator : public IFontFileEnumerator
{
public:
    FontFileEnumerator();

    ~FontFileEnumerator()
    {
        DX::SafeRelease(currentFile_);
        DX::SafeRelease(pFactory_);
    }

    STDMETHOD(Initialize)(IDWriteFactory* pFactory);

    STDMETHOD(SetFilePaths)(const Vector<String>& filePaths);

    // IDWriteFontFileEnumerator methods
    virtual HRESULT STDMETHODCALLTYPE MoveNext(_Out_ BOOL* hasCurrentFile);
    virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(_Out_ IDWriteFontFile** fontFile);

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, _Out_ void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE   AddRef();
    virtual ULONG STDMETHODCALLTYPE   Release();

private:
    ULONG refCount_;

    IDWriteFactory*  pFactory_;
    IDWriteFontFile* currentFile_;
    Vector<String>   filePaths_;
    uint32_t         nextIndex_;
};

HRESULT IFontFileEnumerator::Create(_Out_ IFontFileEnumerator** ppEnumerator, IDWriteFactory* pFactory)
{
    HRESULT hr = S_OK;

    if (!ppEnumerator)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        FontFileEnumerator* pEnumerator = new (std::nothrow) FontFileEnumerator;
        hr                              = pEnumerator ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            hr = pEnumerator->Initialize(pFactory);
        }

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*ppEnumerator);
            (*ppEnumerator) = DX::SafeAcquire(pEnumerator);
        }
    }
    return hr;
}

FontFileEnumerator::FontFileEnumerator()
    : refCount_(0)
    , pFactory_(NULL)
    , currentFile_(NULL)
    , nextIndex_(0)
{
}

STDMETHODIMP FontFileEnumerator::Initialize(IDWriteFactory* pFactory)
{
    if (pFactory)
    {
        pFactory_ = DX::SafeAcquire(pFactory);
        return S_OK;
    }
    return E_INVALIDARG;
}

STDMETHODIMP FontFileEnumerator::SetFilePaths(const Vector<String>& filePaths)
{
    try
    {
        filePaths_.assign(filePaths.begin(), filePaths.end());
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE FontFileEnumerator::MoveNext(_Out_ BOOL* hasCurrentFile)
{
    HRESULT hr = S_OK;

    *hasCurrentFile = FALSE;
    DX::SafeRelease(currentFile_);

    if (nextIndex_ < filePaths_.size())
    {
        WideString file_name = strings::NarrowToWide(filePaths_[nextIndex_]);

        hr = pFactory_->CreateFontFileReference(file_name.c_str(), NULL, &currentFile_);

        if (SUCCEEDED(hr))
        {
            *hasCurrentFile = TRUE;

            ++nextIndex_;
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE FontFileEnumerator::GetCurrentFontFile(_Out_ IDWriteFontFile** fontFile)
{
    *fontFile = DX::SafeAcquire(currentFile_);

    return (currentFile_ != NULL) ? S_OK : E_FAIL;
}

HRESULT STDMETHODCALLTYPE FontFileEnumerator::QueryInterface(REFIID iid, _Out_ void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileEnumerator))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE FontFileEnumerator::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE FontFileEnumerator::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ResourceFontCollectionLoader
//
////////////////////////////////////////////////////////////////////////////////////////

class ResourceFontCollectionLoader : public IResourceFontCollectionLoader
{
public:
    ResourceFontCollectionLoader(IDWriteFontFileLoader* pFileLoader)
        : refCount_(0)
        , pFileLoader_(pFileLoader)
    {
    }

    STDMETHOD(AddResources)
    (const Vector<BinaryData>& data, _Out_ LPVOID* pCollectionKey, _Out_ uint32_t* pCollectionKeySize);

    // IDWriteFontCollectionLoader methods
    virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(IDWriteFactory* pFactory, void const* collectionKey,
                                                              uint32_t                          collectionKeySize,
                                                              _Out_ IDWriteFontFileEnumerator** fontFileEnumerator);

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE   AddRef();
    virtual ULONG STDMETHODCALLTYPE   Release();

private:
    ULONG                  refCount_;
    IDWriteFontFileLoader* pFileLoader_;

    typedef Vector<BinaryData> ResourceCollection;
    Vector<ResourceCollection> resources_;
    Vector<size_t>             collectionKeys_;
};

HRESULT IResourceFontCollectionLoader::Create(_Out_ IResourceFontCollectionLoader** ppCollectionLoader,
                                              IDWriteFontFileLoader*                pFileLoader)
{
    HRESULT hr = S_OK;

    if (!ppCollectionLoader)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        ResourceFontCollectionLoader* pCollectionLoader = new (std::nothrow) ResourceFontCollectionLoader(pFileLoader);
        hr                                              = pCollectionLoader ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*ppCollectionLoader);
            (*ppCollectionLoader) = DX::SafeAcquire(pCollectionLoader);
        }
    }
    return hr;
}

STDMETHODIMP ResourceFontCollectionLoader::AddResources(const Vector<BinaryData>& data, _Out_ LPVOID* pCollectionKey,
                                                        _Out_ uint32_t* pCollectionKeySize)
{
    if (!pCollectionKey || !pCollectionKeySize)
    {
        return E_INVALIDARG;
    }

    try
    {
        size_t collectionKey = resources_.size();
        collectionKeys_.push_back(collectionKey);
        resources_.push_back(data);

        *pCollectionKey     = reinterpret_cast<LPVOID>(&collectionKeys_.back());
        *pCollectionKeySize = sizeof(collectionKey);
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ResourceFontCollectionLoader::CreateEnumeratorFromKey(
    IDWriteFactory* pFactory, void const* collectionKey, uint32_t collectionKeySize,
    _Out_ IDWriteFontFileEnumerator** fontFileEnumerator)
{
    HRESULT hr = S_OK;

    if (collectionKey == NULL || collectionKeySize % sizeof(BinaryData*) != 0)
        hr = E_INVALIDARG;

    if (SUCCEEDED(hr))
    {
        IResourceFontFileEnumerator* pEnumerator = NULL;
        hr = IResourceFontFileEnumerator::Create(&pEnumerator, pFactory, pFileLoader_);

        if (SUCCEEDED(hr))
        {
            const uint32_t resourceIndex = *static_cast<const uint32_t*>(collectionKey);

            hr = pEnumerator->SetResources(resources_[resourceIndex]);
        }

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*fontFileEnumerator);
            (*fontFileEnumerator) = pEnumerator;
        }
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE ResourceFontCollectionLoader::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontCollectionLoader))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE ResourceFontCollectionLoader::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE ResourceFontCollectionLoader::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ResourceFontFileLoader
//
////////////////////////////////////////////////////////////////////////////////////////

class ResourceFontFileLoader : public IResourceFontFileLoader
{
public:
    ResourceFontFileLoader()
        : refCount_(0)
    {
    }

    // IDWriteFontFileLoader methods
    virtual HRESULT STDMETHODCALLTYPE CreateStreamFromKey(void const*                   fontFileReferenceKey,
                                                          uint32_t                      fontFileReferenceKeySize,
                                                          _Out_ IDWriteFontFileStream** fontFileStream);

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE   AddRef();
    virtual ULONG STDMETHODCALLTYPE   Release();

private:
    ULONG refCount_;
};

HRESULT IResourceFontFileLoader::Create(_Out_ IResourceFontFileLoader** ppFileLoader)
{
    HRESULT hr = S_OK;

    if (!ppFileLoader)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        ResourceFontFileLoader* pFileLoader = new (std::nothrow) ResourceFontFileLoader;
        hr                                  = pFileLoader ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            (*ppFileLoader) = DX::SafeAcquire(pFileLoader);
        }
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileLoader::CreateStreamFromKey(void const* fontFileReferenceKey,
                                                                      uint32_t    fontFileReferenceKeySize,
                                                                      _Out_ IDWriteFontFileStream** fontFileStream)
{
    HRESULT hr = S_OK;

    // Make sure the key is the right size.
    if (fontFileReferenceKeySize != sizeof(BinaryData))
        hr = E_INVALIDARG;

    if (SUCCEEDED(hr))
    {
        // Create the pFileStream object.
        IResourceFontFileStream* pFileStream = NULL;
        BinaryData               resource    = *static_cast<BinaryData const*>(fontFileReferenceKey);

        hr = IResourceFontFileStream::Create(&pFileStream, resource);

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*fontFileStream);
            (*fontFileStream) = pFileStream;
        }
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileLoader::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileLoader))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE ResourceFontFileLoader::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE ResourceFontFileLoader::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ResourceFontFileEnumerator
//
////////////////////////////////////////////////////////////////////////////////////////

class ResourceFontFileEnumerator : public IResourceFontFileEnumerator
{
public:
    ResourceFontFileEnumerator();

    ~ResourceFontFileEnumerator()
    {
        DX::SafeRelease(currentFile_);
        DX::SafeRelease(pFactory_);
    }

    STDMETHOD(Initialize)(IDWriteFactory* pFactory, IDWriteFontFileLoader* pLoader);

    STDMETHOD(SetResources)(const Vector<BinaryData>& data);

    // IDWriteFontFileEnumerator methods
    virtual HRESULT STDMETHODCALLTYPE MoveNext(_Out_ BOOL* hasCurrentFile);
    virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(_Out_ IDWriteFontFile** fontFile);

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, _Out_ void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE   AddRef();
    virtual ULONG STDMETHODCALLTYPE   Release();

private:
    ULONG refCount_;

    IDWriteFactory*        pFactory_;
    IDWriteFontFile*       currentFile_;
    IDWriteFontFileLoader* pLoader_;
    Vector<BinaryData>     resources_;
    uint32_t               nextIndex_;
};

HRESULT IResourceFontFileEnumerator::Create(_Out_ IResourceFontFileEnumerator** ppEnumerator, IDWriteFactory* pFactory,
                                            IDWriteFontFileLoader* pFileLoader)
{
    HRESULT hr = S_OK;

    if (!ppEnumerator)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        ResourceFontFileEnumerator* pEnumerator = new (std::nothrow) ResourceFontFileEnumerator;
        hr                                      = pEnumerator ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            hr = pEnumerator->Initialize(pFactory, pFileLoader);
        }

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*ppEnumerator);
            (*ppEnumerator) = DX::SafeAcquire(pEnumerator);
        }
    }
    return hr;
}

ResourceFontFileEnumerator::ResourceFontFileEnumerator()
    : refCount_(0)
    , pFactory_(NULL)
    , currentFile_(NULL)
    , nextIndex_(0)
    , pLoader_(NULL)
{
}

STDMETHODIMP ResourceFontFileEnumerator::Initialize(IDWriteFactory* pFactory, IDWriteFontFileLoader* pLoader)
{
    if (pFactory && pLoader)
    {
        pFactory_ = DX::SafeAcquire(pFactory);
        pLoader_  = DX::SafeAcquire(pLoader);
        return S_OK;
    }
    return E_INVALIDARG;
}

STDMETHODIMP ResourceFontFileEnumerator::SetResources(const Vector<BinaryData>& data)
{
    try
    {
        resources_.assign(data.begin(), data.end());
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileEnumerator::MoveNext(_Out_ BOOL* hasCurrentFile)
{
    HRESULT hr = S_OK;

    *hasCurrentFile = FALSE;
    DX::SafeRelease(currentFile_);

    if (nextIndex_ < resources_.size())
    {
        hr = pFactory_->CreateCustomFontFileReference(&resources_[nextIndex_], sizeof(BinaryData), pLoader_,
                                                      &currentFile_);

        if (SUCCEEDED(hr))
        {
            *hasCurrentFile = TRUE;

            ++nextIndex_;
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileEnumerator::GetCurrentFontFile(_Out_ IDWriteFontFile** fontFile)
{
    *fontFile = DX::SafeAcquire(currentFile_);

    return (currentFile_ != NULL) ? S_OK : E_FAIL;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileEnumerator::QueryInterface(REFIID iid, _Out_ void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileEnumerator))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE ResourceFontFileEnumerator::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE ResourceFontFileEnumerator::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ResourceFontFileStream
//
////////////////////////////////////////////////////////////////////////////////////////

class ResourceFontFileStream : public IResourceFontFileStream
{
public:
    ResourceFontFileStream();

    STDMETHOD(Initialize)(const BinaryData& data);

    // IDWriteFontFileStream methods
    virtual HRESULT STDMETHODCALLTYPE ReadFileFragment(void const** fragmentStart, UINT64 fileOffset,
                                                       UINT64 fragmentSize, _Out_ void** fragmentContext);

    virtual void STDMETHODCALLTYPE ReleaseFileFragment(void* fragmentContext);

    virtual HRESULT STDMETHODCALLTYPE GetFileSize(_Out_ UINT64* fileSize);

    virtual HRESULT STDMETHODCALLTYPE GetLastWriteTime(_Out_ UINT64* lastWriteTime);

    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE   AddRef();
    virtual ULONG STDMETHODCALLTYPE   Release();

private:
    ULONG  refCount_;
    LPVOID resourcePtr_;
    DWORD  resourceSize_;
};

HRESULT IResourceFontFileStream::Create(_Out_ IResourceFontFileStream** ppStream, const BinaryData& data)
{
    HRESULT hr = S_OK;

    if (!ppStream)
    {
        hr = E_POINTER;
    }

    if (SUCCEEDED(hr))
    {
        ResourceFontFileStream* pFileStream = new (std::nothrow) ResourceFontFileStream;
        hr                                  = pFileStream ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            hr = pFileStream->Initialize(data);
        }

        if (SUCCEEDED(hr))
        {
            DX::SafeRelease(*ppStream);
            (*ppStream) = DX::SafeAcquire(pFileStream);
        }
    }
    return hr;
}

ResourceFontFileStream::ResourceFontFileStream()
    : refCount_(0)
    , resourcePtr_(NULL)
    , resourceSize_(0)
{
}

STDMETHODIMP ResourceFontFileStream::Initialize(const BinaryData& data)
{
    HRESULT hr = data.IsValid() ? S_OK : E_FAIL;

    if (SUCCEEDED(hr))
    {
        resourcePtr_  = data.buffer;
        resourceSize_ = data.size;
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileStream::ReadFileFragment(void const** fragmentStart, UINT64 fileOffset,
                                                                   UINT64 fragmentSize, _Out_ void** fragmentContext)
{
    // The pLoader is responsible for doing a bounds check.
    if (fileOffset <= resourceSize_ && fragmentSize <= resourceSize_ - fileOffset)
    {
        *fragmentStart   = static_cast<BYTE const*>(resourcePtr_) + static_cast<uint32_t>(fileOffset);
        *fragmentContext = NULL;
        return S_OK;
    }
    else
    {
        *fragmentStart   = NULL;
        *fragmentContext = NULL;
        return E_FAIL;
    }
}

void STDMETHODCALLTYPE ResourceFontFileStream::ReleaseFileFragment(void* fragmentContext) {}

HRESULT STDMETHODCALLTYPE ResourceFontFileStream::GetFileSize(_Out_ UINT64* fileSize)
{
    *fileSize = resourceSize_;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ResourceFontFileStream::GetLastWriteTime(_Out_ UINT64* lastWriteTime)
{
    *lastWriteTime = 0;
    return E_NOTIMPL;
}

// IUnknown methods
HRESULT STDMETHODCALLTYPE ResourceFontFileStream::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileStream))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE ResourceFontFileStream::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE ResourceFontFileStream::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

}  // namespace directx
}  // namespace graphics
}  // namespace kiwano
