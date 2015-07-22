// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/WrapCanvasContext.h"

#if !defined(DISABLE_CANVAS)
#include "bindings/core/v8/V8CanvasRenderingContext2D.h"
#include "bindings/core/v8/V8WebGLRenderingContext.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/inspector/InspectorCanvasInstrumentation.h"
#endif

namespace blink {

#if !defined(DISABLE_CANVAS)
ScriptValue wrapCanvasContext(ScriptState* scriptState, HTMLCanvasElement* canvas, PassRefPtrWillBeRawPtr<CanvasRenderingContext2D> value)
{
    v8::Handle<v8::Value> v8Result = toV8(value, scriptState->context()->Global(), scriptState->isolate());
    ScriptValue context(scriptState, v8Result);
    if (InspectorInstrumentation::canvasAgentEnabled(&canvas->document())) {
        ScriptValue wrapped = InspectorInstrumentation::wrapCanvas2DRenderingContextForInstrumentation(&canvas->document(), context);
        if (!wrapped.isEmpty()) {
            return wrapped;
        }
    }
    return context;
}

ScriptValue wrapCanvasContext(ScriptState* scriptState, HTMLCanvasElement* canvas, PassRefPtrWillBeRawPtr<WebGLRenderingContext> value)
{
    v8::Handle<v8::Value> v8Result = toV8(value, scriptState->context()->Global(), scriptState->isolate());
    ScriptValue context(scriptState, v8Result);
    if (InspectorInstrumentation::canvasAgentEnabled(&canvas->document())) {
        ScriptValue wrapped = InspectorInstrumentation::wrapWebGLRenderingContextForInstrumentation(&canvas->document(), context);
        if (!wrapped.isEmpty()) {
            return wrapped;
        }
    }
    return context;
}
#endif

} // namespace blink
