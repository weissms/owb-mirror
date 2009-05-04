list(APPEND WEBCORE_SRC
    page/AccessibilityImageMapLink.cpp
    page/AccessibilityList.cpp
    page/AccessibilityListBox.cpp
    page/AccessibilityListBoxOption.cpp
    page/AccessibilityObject.cpp
    page/AccessibilityRenderObject.cpp
    page/AccessibilityTable.cpp
    page/AccessibilityTableCell.cpp
    page/AccessibilityTableColumn.cpp
    page/AccessibilityTableHeaderContainer.cpp
    page/AccessibilityTableRow.cpp
    page/animation/AnimationBase.cpp
    page/animation/AnimationController.cpp
    page/animation/CompositeAnimation.cpp
    page/animation/ImplicitAnimation.cpp
    page/animation/KeyframeAnimation.cpp
    page/AXObjectCache.cpp
    page/BarInfo.cpp
    page/Chrome.cpp
    page/ContextMenuController.cpp
    page/Coordinates.cpp
    page/DOMSelection.cpp
    page/DOMTimer.cpp
    page/DOMWindow.cpp
    page/DragController.cpp
    page/EventHandler.cpp
    page/FocusController.cpp
    page/Frame.cpp
    page/FrameTree.cpp
    page/FrameView.cpp
    page/Geoposition.cpp
    page/Geolocation.cpp
    page/History.cpp
    page/Location.cpp
    page/MouseEventWithHitTestResults.cpp
    page/Navigator.cpp
    page/NavigatorBase.cpp
    page/Page.cpp
    page/PageGroup.cpp
    page/PageGroupLoadDeferrer.cpp
    page/PrintContext.cpp
    page/Screen.cpp
    page/SecurityOrigin.cpp
    page/Settings.cpp
    page/WindowFeatures.cpp
)
list(APPEND IDL_SRC
    page/BarInfo.idl
    page/Coordinates.idl
    page/DOMSelection.idl
    page/DOMWindow.idl
    page/Geoposition.idl
    page/Geolocation.idl
    page/History.idl
    page/Location.idl
    page/Navigator.idl
    page/PositionError.idl
    page/Screen.idl
    page/WebKitPoint.idl
)

if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        page/WorkerNavigator.cpp
    )
    list(APPEND IDL_SRC
        page/WorkerNavigator.idl
    )
endif(ENABLE_WORKERS)

if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        page/Console.cpp
    )
    list(APPEND IDL_SRC
        page/Console.idl
    )    
endif(ENABLE_INSPECTOR)
