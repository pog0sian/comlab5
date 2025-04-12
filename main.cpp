#include <cassert>
#include <iostream>
#include "objbase.h"
#include <unknwn.h>


interface IX : public IUnknown {
    virtual void __stdcall Fx() = 0;
};

interface IY : public IUnknown {
    virtual void __stdcall Fy() = 0;
};

interface IZ : public IUnknown {
    virtual void __stdcall Fz() = 0;
};

class CA : public IX, public IY {
public:

    CA() : m_cRef(0) {}

    ~CA() { std::cout << "CA: destroyed..." << std::endl; };

    virtual ULONG __stdcall AddRef() {
        std::cout << "CA: AddRef = " << m_cRef + 1 << std::endl;
        return ++m_cRef;
    }

    virtual ULONG __stdcall Release() {
        std::cout << "CA: Release = " << m_cRef - 1 << std::endl;
        if (--m_cRef == 0) {
            delete this;
            return 0;
        }
        return m_cRef;
    }

    virtual HRESULT QueryInterface(const IID& iid, void** ppv) {
        if (iid == IID_IUnknown1) {
            std::cout << "QueryInterface: return pointer to IUnknown" << std::endl;
            *ppv = static_cast<IX*>(this);
        } else if (iid == IID_IX) {
            std::cout << "QueryInterface: return pointer to IX" << std::endl;
            *ppv = static_cast<IX*>(this);
        } else if (iid == IID_IY) {
            std::cout << "QueryInterface: return pointer to IY" << std::endl;
            *ppv = static_cast<IY*>(this);
        } else {
            std::cout << "Interface not supported" << std::endl;
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    virtual void Fx() { std::cout << "CA::Fx" << std::endl; }

    virtual void Fy() { std::cout << "CA::Fy" << std::endl; }

private:
    long m_cRef;
};

IUnknown* CreateInstance() {
    IUnknown* pI = static_cast<IX*>(new CA);
    pI->AddRef();
    return pI;
}

BOOL SameComponents(IX* pIX, IY* pIY) {
    IUnknown* pI1 = NULL;
    IUnknown* pI2 = NULL;

    pIX->QueryInterface(IID_IUnknown1, (void**)&pI1);

    pIY->QueryInterface(IID_IUnknown1, (void**)&pI2);

    BOOL result = (pI1 == pI2);

    pI1->Release();
    pI2->Release();

    return result;
}

void f(IX* pIX) {
    IX* pIX2 = NULL;

    HRESULT hr = pIX->QueryInterface(IID_IX, (void**)&pIX2);

    assert(SUCCEEDED(hr));
    if (pIX2) pIX2->Release();
}

void f2(IX* pIX) {
    HRESULT hr;

    IX* pIX2 = NULL;
    IY* pIY = NULL;

    hr = pIX->QueryInterface(IID_IY, (void**)&pIY);

    if (SUCCEEDED(hr)) {
        hr = pIY->QueryInterface(IID_IX, (void**)&pIX2);

        assert(SUCCEEDED(hr));
        if (pIX2) pIX2->Release();
        pIY->Release();
    }
}

void f3(IX* pIX) {
    HRESULT hr;

    IY* pIY = NULL;

    hr = pIX->QueryInterface(IID_IY, (void**)&pIY);

    if (SUCCEEDED(hr)) {
        IZ* pIZ = NULL;

        hr = pIY->QueryInterface(IID_IZ, (void**)&pIZ);

        if (SUCCEEDED(hr)) {
            pIZ->Release();

            hr = pIX->QueryInterface(IID_IZ, (void**)&pIZ);

            if (SUCCEEDED(hr)) {
                pIZ->Release();
            }
        }
        pIY->Release();
    }
}

int main() {

    std::cout << "Hello, World!" << std::endl;

    HRESULT hr;

    std::cout << "Client: get pointer to IUnknown" << std::endl;
    IUnknown* pIUnknown = CreateInstance();

    std::cout << "\nClient: get pointer to IX" << std::endl;
    IX* pIX = NULL;
    hr = pIUnknown->QueryInterface(IID_IX, (void**)&pIX);
    if (SUCCEEDED(hr)) {
        std::cout << "Client: IX received successfully" << std::endl;
        pIX->Fx();
    }

    std::cout << "\nClient: get pointer to IY" << std::endl;
    IY* pIY = NULL;
    hr = pIUnknown->QueryInterface(IID_IY, (void**)&pIY);
    if (SUCCEEDED(hr)) {
        std::cout << "Client: IY received successfully" << std::endl;
        pIY->Fy();
    }

    std::cout << "\nClient: Get unsupported interface" << std::endl;
    IZ* pIZ = NULL;
    hr = pIUnknown->QueryInterface(IID_IZ, (void**)&pIZ);
    if (SUCCEEDED(hr)) {
        std::cout << "Client: interface IZ get successfully" << std::endl;
        pIZ->Release();
    } else {
        std::cout << "Client: Can not get interface IZ" << std::endl;
    }

    std::cout << "\nClient: Get pointer to IY from IX" << std::endl;
    IY* pIYfromIx = NULL;
    hr = pIX->QueryInterface(IID_IY, (void**)&pIYfromIx);
    if (SUCCEEDED(hr)) {
        std::cout << "Client: IY received successfully" << std::endl;
        pIYfromIx->Fy();
        pIYfromIx->Release();
    } else {
        std::cout << "Client: Can not get IY from IX" << std::endl;
    }

    std::cout << "\nClient: Get pointer to IUnknown from IY" << std::endl;
    IUnknown* pIUnknownFromIY = NULL;
    hr = pIY->QueryInterface(IID_IUnknown1, (void**)&pIUnknownFromIY);
    if (SUCCEEDED(hr)) {
        std::cout << "Two pointers are equal?" << std::endl;
        if (pIUnknownFromIY == pIUnknown) {
            std::cout << "YES" << std::endl;
        } else {
            std::cout << "NO" << std::endl;
        }
        pIUnknownFromIY->Release();
    }

    std::cout << "\nTest f()" << std::endl;
    try {
        if (pIX) f(pIX);
        std::cout << "f() successfully completed" << std::endl;
    }
    catch (...) {
        std::cout << "Error in f()" << std::endl;
    }

    std::cout << "\nTest f2()" << std::endl;
    try {
        if (pIX) f2(pIX);
        std::cout << "f2() successfully completed" << std::endl;
    }
    catch (...) {
        std::cout << "Error in f2()" << std::endl;
    }

    std::cout << "\nTest f3()" << std::endl;
    try {
        if (pIX) f3(pIX);
        std::cout << "f3() successfully completed" << std::endl;
    }
    catch (...) {
        std::cout << "Error in f3()" << std::endl;
    }

    std::cout << "\nTest SameComponents()" << std::endl;
    bool result = SameComponents(pIX, pIY);
    std::cout << "Result of comparison: " << (result ? "pointers match" : "pointers are different") << std::endl;

    if (pIX != NULL) pIX->Release();
    if (pIY != NULL) pIY->Release();

    pIUnknown->Release();

    return 0;
}