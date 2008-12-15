list(APPEND WEBCORE_SRC
    rendering/style/Animation.cpp
    rendering/style/AnimationList.cpp
    rendering/style/BindingURI.cpp
    rendering/style/ContentData.cpp
    rendering/style/CounterDirectives.cpp
    rendering/style/FillLayer.cpp
    rendering/style/KeyframeList.cpp
    rendering/style/MatrixTransformOperation.cpp
    rendering/style/NinePieceImage.cpp
    rendering/style/RenderStyle.cpp
    rendering/style/RotateTransformOperation.cpp
    rendering/style/ScaleTransformOperation.cpp
    rendering/style/ShadowData.cpp
    rendering/style/SkewTransformOperation.cpp
    rendering/style/StyleBackgroundData.cpp
    rendering/style/StyleBoxData.cpp
    rendering/style/StyleCachedImage.cpp
    rendering/style/StyleFlexibleBoxData.cpp
    rendering/style/StyleGeneratedImage.cpp
    rendering/style/StyleInheritedData.cpp
    rendering/style/StyleMarqueeData.cpp
    rendering/style/StyleMultiColData.cpp
    rendering/style/StyleRareInheritedData.cpp
    rendering/style/StyleRareNonInheritedData.cpp
    rendering/style/StyleSurroundData.cpp
    rendering/style/StyleTransformData.cpp
    rendering/style/StyleVisualData.cpp
    rendering/style/TransformOperations.cpp
    rendering/style/TranslateTransformOperation.cpp
)

if(ENABLE_SVG)
    list(APPEND WEBCORE_SRC
        rendering/style/SVGRenderStyle.cpp
        rendering/style/SVGRenderStyleDefs.cpp
    )
endif(ENABLE_SVG)
