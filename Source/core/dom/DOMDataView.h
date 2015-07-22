// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMDataView_h
#define DOMDataView_h

#include "core/dom/DOMArrayBufferView.h"
#if !defined(DISABLE_CANVAS)
#include "core/html/canvas/DataView.h"
#else
#include "wtf/ArrayBufferView.h"

namespace blink {

class ExceptionState;

class DataView final : public ArrayBufferView {
public:
    static PassRefPtr<DataView> create(PassRefPtr<ArrayBuffer>, unsigned byteOffset, unsigned byteLength);

    virtual unsigned byteLength() const override { return m_byteLength; }
    virtual ViewType type() const override { return TypeDataView; }

    int16_t getInt16(unsigned byteOffset, bool littleEndian, ExceptionState&);
    uint16_t getUint16(unsigned byteOffset, bool littleEndian, ExceptionState&);
    int32_t getInt32(unsigned byteOffset, bool littleEndian, ExceptionState&);
    uint32_t getUint32(unsigned byteOffset, bool littleEndian, ExceptionState&);
    float getFloat32(unsigned byteOffset, bool littleEndian, ExceptionState&);
    double getFloat64(unsigned byteOffset, bool littleEndian, ExceptionState&);

    void setInt16(unsigned byteOffset, int16_t value, bool littleEndian, ExceptionState&);
    void setUint16(unsigned byteOffset, uint16_t value, bool littleEndian, ExceptionState&);
    void setInt32(unsigned byteOffset, int32_t value, bool littleEndian, ExceptionState&);
    void setUint32(unsigned byteOffset, uint32_t value, bool littleEndian, ExceptionState&);
    void setFloat32(unsigned byteOffset, float value, bool littleEndian, ExceptionState&);
    void setFloat64(unsigned byteOffset, double value, bool littleEndian, ExceptionState&);

protected:
    virtual void neuter() override;

private:
    DataView(PassRefPtr<ArrayBuffer>, unsigned byteOffset, unsigned byteLength);

    template<typename T>
    bool beyondRange(unsigned byteOffset) const { return byteOffset >= m_byteLength || byteOffset + sizeof(T) > m_byteLength; }

    template<typename T>
    T getData(unsigned byteOffset, bool littleEndian, ExceptionState&) const;

    template<typename T>
    void setData(unsigned byteOffset, T value, bool littleEndian, ExceptionState&);

    unsigned m_byteLength;
};

} // namespace blink

#endif

namespace blink {

class DOMDataView final : public DOMArrayBufferView {
    DEFINE_WRAPPERTYPEINFO();
public:
    typedef char ValueType;

    static PassRefPtr<DOMDataView> create(PassRefPtr<DOMArrayBuffer>, unsigned byteOffset, unsigned byteLength);

    const DataView* view() const { return static_cast<const DataView*>(DOMArrayBufferView::view()); }
    DataView* view() { return static_cast<DataView*>(DOMArrayBufferView::view()); }

    virtual v8::Handle<v8::Object> wrap(v8::Handle<v8::Object> creationContext, v8::Isolate*) override;
    virtual v8::Handle<v8::Object> associateWithWrapper(v8::Isolate*, const WrapperTypeInfo*, v8::Handle<v8::Object> wrapper) override;

private:
    DOMDataView(PassRefPtr<DataView> dataView, PassRefPtr<DOMArrayBuffer> domArrayBuffer)
        : DOMArrayBufferView(dataView, domArrayBuffer) { }
};

} // namespace blink

#endif // DOMDataView_h
