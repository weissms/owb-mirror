create_lut_jsc(JSC_SRC JavaScriptCore/runtime/ArrayPrototype.cpp generated_sources/JavaScriptCore/ArrayPrototype.lut.h JavaScriptCore/runtime/ArrayPrototype.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/DatePrototype.cpp generated_sources/JavaScriptCore/DatePrototype.lut.h JavaScriptCore/runtime/DatePrototype.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/JSONObject.cpp generated_sources/JavaScriptCore/JSONObject.lut.h JavaScriptCore/runtime/JSONObject.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/MathObject.cpp generated_sources/JavaScriptCore/MathObject.lut.h JavaScriptCore/runtime/MathObject.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/NumberConstructor.cpp generated_sources/JavaScriptCore/NumberConstructor.lut.h JavaScriptCore/runtime/NumberConstructor.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/RegExpConstructor.cpp generated_sources/JavaScriptCore/RegExpConstructor.lut.h JavaScriptCore/runtime/RegExpConstructor.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/RegExpObject.cpp generated_sources/JavaScriptCore/RegExpObject.lut.h JavaScriptCore/runtime/RegExpObject.cpp)
create_lut_jsc(JSC_SRC JavaScriptCore/runtime/StringPrototype.cpp generated_sources/JavaScriptCore/StringPrototype.lut.h JavaScriptCore/runtime/StringPrototype.cpp)

list(APPEND JSC_SRC
    runtime/ExceptionHelpers.cpp
    runtime/Executable.cpp
    runtime/InitializeThreading.cpp
    runtime/JSActivation.cpp
    runtime/JSByteArray.cpp
    runtime/JSGlobalData.cpp
    runtime/JSNotAnObject.cpp
    runtime/JSONObject.cpp
    runtime/JSPropertyNameIterator.cpp
    runtime/LiteralParser.cpp
    runtime/MarkStack.cpp
    runtime/MarkStackPosix.cpp
    runtime/PropertyDescriptor.cpp
    runtime/SmallStrings.cpp
    runtime/Structure.cpp
    runtime/StructureChain.cpp
    runtime/TimeoutChecker.cpp
)

if(ENABLE_DEBUG)
    list(APPEND JSC_SRC
        runtime/ArgList.cpp
        runtime/Arguments.cpp
        runtime/ArrayConstructor.cpp
        runtime/ArrayPrototype.cpp
        runtime/BooleanConstructor.cpp
        runtime/BooleanObject.cpp
        runtime/BooleanPrototype.cpp
        runtime/CallData.cpp
        runtime/Collector.cpp
        runtime/CommonIdentifiers.cpp
        runtime/Completion.cpp
        runtime/ConstructData.cpp
        runtime/DateConstructor.cpp
        runtime/DateConversion.cpp
        runtime/DateInstance.cpp
        runtime/DatePrototype.cpp
        runtime/Error.cpp
        runtime/ErrorConstructor.cpp
        runtime/ErrorInstance.cpp
        runtime/ErrorPrototype.cpp
        runtime/FunctionConstructor.cpp
        runtime/FunctionPrototype.cpp
        runtime/GetterSetter.cpp
        runtime/GlobalEvalFunction.cpp
        runtime/Identifier.cpp
        runtime/InternalFunction.cpp
        runtime/JSAPIValueWrapper.cpp
        runtime/JSArray.cpp
        runtime/JSCell.cpp
        runtime/JSFunction.cpp
        runtime/JSGlobalObject.cpp
        runtime/JSGlobalObjectFunctions.cpp
        runtime/JSImmediate.cpp
        runtime/JSLock.cpp
        runtime/JSNumberCell.cpp
        runtime/JSObject.cpp
        runtime/JSStaticScopeObject.cpp
        runtime/JSString.cpp
        runtime/JSValue.cpp
        runtime/JSVariableObject.cpp
        runtime/JSWrapperObject.cpp
        runtime/Lookup.cpp
        runtime/MathObject.cpp
        runtime/NativeErrorConstructor.cpp
        runtime/NativeErrorPrototype.cpp
        runtime/NumberConstructor.cpp
        runtime/NumberObject.cpp
        runtime/NumberPrototype.cpp
        runtime/ObjectConstructor.cpp
        runtime/ObjectPrototype.cpp
        runtime/Operations.cpp
        runtime/PropertyNameArray.cpp
        runtime/PropertySlot.cpp
        runtime/PrototypeFunction.cpp
        runtime/RegExp.cpp
        runtime/RegExpConstructor.cpp
        runtime/RegExpObject.cpp
        runtime/RegExpPrototype.cpp
        runtime/ScopeChain.cpp
        runtime/StringConstructor.cpp
        runtime/StringObject.cpp
        runtime/StringPrototype.cpp
        runtime/UString.cpp
    )
else(ENABLE_DEBUG)
    list(APPEND JSC_SRC
        AllInOneFile.cpp
    )
endif(ENABLE_DEBUG)

