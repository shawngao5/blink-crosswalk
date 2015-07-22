// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "config.h"
#include "core/dom/DOMDataView.h"

#if defined(DISABLE_CANVAS)
#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "platform/CheckedInt.h"
#include "wtf/CPU.h"

namespace {

template<typename T>
union Value {
    T data;
    char bytes[sizeof(T)];
};

}

namespace blink {

PassRefPtr<DataView> DataView::create(PassRefPtr<ArrayBuffer> buffer, unsigned byteOffset, unsigned byteLength)
{
    RELEASE_ASSERT(byteOffset <= buffer->byteLength());
    CheckedInt<uint32_t> checkedOffset(byteOffset);
    CheckedInt<uint32_t> checkedLength(byteLength);
    CheckedInt<uint32_t> checkedMax = checkedOffset + checkedLength;
    RELEASE_ASSERT(checkedMax.isValid());
    RELEASE_ASSERT(checkedMax.value() <= buffer->byteLength());
    return adoptRef(new DataView(buffer, byteOffset, byteLength));
}

DataView::DataView(PassRefPtr<ArrayBuffer> buffer, unsigned byteOffset, unsigned byteLength)
    : ArrayBufferView(buffer, byteOffset)
    , m_byteLength(byteLength)
{
}

static bool needToFlipBytes(bool littleEndian)
{
#if CPU(BIG_ENDIAN)
    return littleEndian;
#else
    return !littleEndian;
#endif
}

inline void swapBytes(char* p, char* q)
{
    char temp = *p;
    *p = *q;
    *q = temp;
}

static void flipBytesFor16Bits(char* p)
{
    swapBytes(p, p + 1);
}

static void flipBytesFor32Bits(char* p)
{
    swapBytes(p, p + 3);
    swapBytes(p + 1, p + 2);
}

static void flipBytesFor64Bits(char* p)
{
    swapBytes(p, p + 7);
    swapBytes(p + 1, p + 6);
    swapBytes(p + 2, p + 5);
    swapBytes(p + 3, p + 4);
}

static void flipBytesIfNeeded(char* value, size_t size, bool littleEndian)
{
    if (!needToFlipBytes(littleEndian))
        return;

    switch (size) {
    case 1:
        // Nothing to do.
        break;
    case 2:
        flipBytesFor16Bits(value);
        break;
    case 4:
        flipBytesFor32Bits(value);
        break;
    case 8:
        flipBytesFor64Bits(value);
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

template<typename T>
T DataView::getData(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState) const
{
    if (beyondRange<T>(byteOffset)) {
        exceptionState.throwDOMException(IndexSizeError, "The provided offset (" + String::number(byteOffset) + ") is outside the allowed range.");
        return 0;
    }

    // We do not want to load the data directly since it would cause a bus error on architectures that don't support unaligned loads.
    Value<T> value;
    memcpy(value.bytes, static_cast<const char*>(m_baseAddress) + byteOffset, sizeof(T));
    flipBytesIfNeeded(value.bytes, sizeof(T), littleEndian);
    return value.data;
}

template<typename T>
void DataView::setData(unsigned byteOffset, T value, bool littleEndian, ExceptionState& exceptionState)
{
    if (beyondRange<T>(byteOffset)) {
        exceptionState.throwDOMException(IndexSizeError, "The provided offset (" + String::number(byteOffset) + ") is outside the allowed range.");
        return;
    }

    // We do not want to store the data directly since it would cause a bus error on architectures that don't support unaligned stores.
    Value<T> tempValue;
    tempValue.data = value;
    flipBytesIfNeeded(tempValue.bytes, sizeof(T), littleEndian);
    memcpy(static_cast<char*>(m_baseAddress) + byteOffset, tempValue.bytes, sizeof(T));
}

int16_t DataView::getInt16(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState)
{
    return getData<int16_t>(byteOffset, littleEndian, exceptionState);
}

uint16_t DataView::getUint16(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState)
{
    return getData<uint16_t>(byteOffset, littleEndian, exceptionState);
}

int32_t DataView::getInt32(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState)
{
    return getData<int32_t>(byteOffset, littleEndian, exceptionState);
}

uint32_t DataView::getUint32(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState)
{
    return getData<uint32_t>(byteOffset, littleEndian, exceptionState);
}

float DataView::getFloat32(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState)
{
    return getData<float>(byteOffset, littleEndian, exceptionState);
}

double DataView::getFloat64(unsigned byteOffset, bool littleEndian, ExceptionState& exceptionState)
{
    return getData<double>(byteOffset, littleEndian, exceptionState);
}

void DataView::setInt16(unsigned byteOffset, short value, bool littleEndian, ExceptionState& exceptionState)
{
    setData<int16_t>(byteOffset, value, littleEndian, exceptionState);
}

void DataView::setUint16(unsigned byteOffset, uint16_t value, bool littleEndian, ExceptionState& exceptionState)
{
    setData<uint16_t>(byteOffset, value, littleEndian, exceptionState);
}

void DataView::setInt32(unsigned byteOffset, int32_t value, bool littleEndian, ExceptionState& exceptionState)
{
    setData<int32_t>(byteOffset, value, littleEndian, exceptionState);
}

void DataView::setUint32(unsigned byteOffset, uint32_t value, bool littleEndian, ExceptionState& exceptionState)
{
    setData<uint32_t>(byteOffset, value, littleEndian, exceptionState);
}

void DataView::setFloat32(unsigned byteOffset, float value, bool littleEndian, ExceptionState& exceptionState)
{
    setData<float>(byteOffset, value, littleEndian, exceptionState);
}

void DataView::setFloat64(unsigned byteOffset, double value, bool littleEndian, ExceptionState& exceptionState)
{
    setData<double>(byteOffset, value, littleEndian, exceptionState);
}

void DataView::neuter()
{
    ArrayBufferView::neuter();
    m_byteLength = 0;
}

} // namespace blink
#endif

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8DOMWrapper.h"

namespace blink {

PassRefPtr<DOMDataView> DOMDataView::create(PassRefPtr<DOMArrayBuffer> prpBuffer, unsigned byteOffset, unsigned byteLength)
{
    RefPtr<DOMArrayBuffer> buffer = prpBuffer;
    RefPtr<DataView> dataView = DataView::create(buffer->buffer(), byteOffset, byteLength);
    return adoptRef(new DOMDataView(dataView.release(), buffer.release()));
}

v8::Handle<v8::Object> DOMDataView::wrap(v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    // It's possible that no one except for the new wrapper owns this object at
    // this moment, so we have to prevent GC to collect this object until the
    // object gets associated with the wrapper.
    RefPtr<DOMDataView> protect(this);

    ASSERT(!DOMDataStore::containsWrapper(this, isolate));

    const WrapperTypeInfo* wrapperTypeInfo = this->wrapperTypeInfo();
    v8::Local<v8::Value> v8Buffer = toV8(buffer(), creationContext, isolate);
    ASSERT(v8Buffer->IsArrayBuffer());

    v8::Handle<v8::Object> wrapper = v8::DataView::New(v8Buffer.As<v8::ArrayBuffer>(), byteOffset(), byteLength());

    return associateWithWrapper(isolate, wrapperTypeInfo, wrapper);
}

v8::Handle<v8::Object> DOMDataView::associateWithWrapper(v8::Isolate* isolate, const WrapperTypeInfo* wrapperTypeInfo, v8::Handle<v8::Object> wrapper)
{
    return V8DOMWrapper::associateObjectWithWrapper(isolate, this, wrapperTypeInfo, wrapper);
}

} // namespace blink
