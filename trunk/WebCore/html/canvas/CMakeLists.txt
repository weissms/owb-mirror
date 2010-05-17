list(APPEND WEBCORE_SRC
    html/canvas/ArrayBuffer.cpp
    html/canvas/ArrayBufferView.cpp
    html/canvas/CanvasContextAttributes.cpp
    html/canvas/CanvasGradient.cpp
    html/canvas/CanvasNumberArray.cpp
    html/canvas/CanvasObject.cpp
    html/canvas/CanvasPattern.cpp
    html/canvas/CanvasPixelArray.cpp
    html/canvas/CanvasStyle.cpp
    html/canvas/CanvasRenderingContext.cpp
    html/canvas/CanvasRenderingContext2D.cpp
    html/canvas/FloatArray.cpp
    html/canvas/Int16Array.cpp
    html/canvas/Int32Array.cpp
    html/canvas/Int8Array.cpp
    html/canvas/Uint16Array.cpp
    html/canvas/Uint32Array.cpp
    html/canvas/Uint8Array.cpp
    html/canvas/WebGLContextAttributes.cpp
    html/canvas/WebGLFramebuffer.cpp
    html/canvas/WebGLProgram.cpp
    html/canvas/WebGLRenderbuffer.cpp
    html/canvas/WebGLShader.cpp
    html/canvas/WebGLTexture.cpp
) 

list(APPEND IDL_SRC
    html/canvas/ArrayBuffer.idl
    html/canvas/ArrayBufferView.idl
    html/canvas/CanvasGradient.idl
    html/canvas/CanvasNumberArray.idl
    html/canvas/CanvasPattern.idl
    html/canvas/CanvasRenderingContext2D.idl
    html/canvas/CanvasRenderingContext.idl
    html/canvas/FloatArray.idl
    html/canvas/Int16Array.idl
    html/canvas/Int32Array.idl
    html/canvas/Int8Array.idl
    html/canvas/Uint16Array.idl
    html/canvas/Uint32Array.idl
    html/canvas/Uint8Array.idl
    html/canvas/WebGLContextAttributes.idl
    html/canvas/WebGLFramebuffer.idl
    html/canvas/WebGLProgram.idl
    html/canvas/WebGLRenderbuffer.idl
    html/canvas/WebGLRenderingContext.idl
    html/canvas/WebGLShader.idl
    html/canvas/WebGLTexture.idl
)

if(ENABLE_3D_CANVAS)
    list(APPEND WEBCORE_SRC
        html/canvas/WebGLBuffer.cpp
        html/canvas/WebGLGetInfo.cpp
        html/canvas/WebGLRenderingContext.cpp
        html/canvas/WebGLUniformLocation.cpp
    )
    list(APPEND IDL_SRC
        html/canvas/WebGLActiveInfo.idl
        html/canvas/WebGLBuffer.idl
        html/canvas/WebGLUniformLocation.idl
    )
endif(ENABLE_3D_CANVAS)

