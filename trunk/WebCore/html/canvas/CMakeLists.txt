list(APPEND WEBCORE_SRC
    html/canvas/CanvasGradient.cpp
    html/canvas/CanvasNumberArray.cpp
    html/canvas/CanvasObject.cpp
    html/canvas/CanvasPattern.cpp
    html/canvas/CanvasPixelArray.cpp
    html/canvas/CanvasStyle.cpp
    html/canvas/CanvasRenderingContext.cpp
    html/canvas/CanvasRenderingContext2D.cpp
    html/canvas/WebGLArray.cpp
    html/canvas/WebGLArrayBuffer.cpp
    html/canvas/WebGLByteArray.cpp
    html/canvas/WebGLFloatArray.cpp
    html/canvas/WebGLFramebuffer.cpp
    html/canvas/WebGLIntArray.cpp
    html/canvas/WebGLProgram.cpp
    html/canvas/WebGLRenderbuffer.cpp
    html/canvas/WebGLShader.cpp
    html/canvas/WebGLShortArray.cpp
    html/canvas/WebGLTexture.cpp
    html/canvas/WebGLUnsignedByteArray.cpp
    html/canvas/WebGLUnsignedIntArray.cpp
    html/canvas/WebGLUnsignedShortArray.cpp
) 

list(APPEND IDL_SRC
    html/canvas/CanvasGradient.idl
    html/canvas/CanvasNumberArray.idl
    html/canvas/CanvasPattern.idl
    html/canvas/CanvasRenderingContext2D.idl
    html/canvas/CanvasRenderingContext.idl
    html/canvas/WebGLArray.idl
    html/canvas/WebGLArrayBuffer.idl
    html/canvas/WebGLByteArray.idl
    html/canvas/WebGLFloatArray.idl
    html/canvas/WebGLFramebuffer.idl
    html/canvas/WebGLIntArray.idl
    html/canvas/WebGLProgram.idl
    html/canvas/WebGLRenderbuffer.idl
    html/canvas/WebGLRenderingContext.idl
    html/canvas/WebGLShader.idl
    html/canvas/WebGLShortArray.idl
    html/canvas/WebGLTexture.idl
    html/canvas/WebGLUnsignedByteArray.idl
    html/canvas/WebGLUnsignedIntArray.idl
    html/canvas/WebGLUnsignedShortArray.idl
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

