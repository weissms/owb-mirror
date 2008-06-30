LIST (APPEND PAGE_SRC
    page/AccessibilityListBoxOption.cpp
    page/AccessibilityListBox.cpp
    page/AccessibilityObject.cpp
    page/AccessibilityRenderObject.cpp
    page/AnimationController.cpp
    page/AXObjectCache.cpp
    page/BarInfo.cpp
    page/Chrome.cpp
    page/Console.cpp
    page/ContextMenuController.cpp
    page/DOMSelection.cpp
    page/DOMWindow.cpp
    page/DragController.cpp
    page/EventHandler.cpp
    page/JavaScriptProfile.cpp
    page/JavaScriptProfileNode.cpp
    page/JavaScriptCallFrame.cpp
    page/FocusController.cpp
    page/Frame.cpp
    page/FrameTree.cpp
    page/FrameView.cpp
    page/History.cpp
    page/InspectorController.cpp
    page/JavaScriptDebugServer.cpp
    page/Location.cpp
    page/MouseEventWithHitTestResults.cpp
    page/Navigator.cpp
    page/Page.cpp
    page/PageGroup.cpp
    page/PrintContext.cpp
    page/Screen.cpp
    page/Settings.cpp
    page/WindowFeatures.cpp
)

IF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
    LIST (APPEND PAGE_SRC
        page/gtk/AccessibilityObjectWrapperAtk.cpp
	page/gtk/AXObjectCacheAtk.cpp
    )
ENDIF (OWBAL_PLATFORM_GRAPHICS STREQUAL "GTK")
