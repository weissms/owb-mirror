##################################################
# Widgets support part.                          #
##################################################

set(WIDGETS_DIR ${BAL_DIR}/Widgets/WebCore)

create_include_link(${WIDGETS_DIR}/WK/BCHostWindowWK.h BAL/HostWindow.h)
create_include_link(${WIDGETS_DIR}/WK/BCPopupMenuClientWK.h BAL/PopupMenuClient.h)
create_include_link(${WIDGETS_DIR}/WK/BCPopupMenuStyleWK.h BAL/PopupMenuStyle.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarWK.h BAL/Scrollbar.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarClientWK.h BAL/ScrollbarClient.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarThemeWK.h BAL/ScrollbarTheme.h)
create_include_link(${WIDGETS_DIR}/WK/BCScrollbarThemeCompositeWK.h BAL/ScrollbarThemeComposite.h)
create_include_link(${WIDGETS_DIR}/WK/BCThemeWK.h BAL/Theme.h)
create_include_link(${WIDGETS_DIR}/WK/BCThemeTypesWK.h BAL/ThemeTypes.h)
create_include_link(${WIDGETS_DIR}/WK/BCWidgetWK.h BAL/Widget.h)

if(USE_GRAPHICS_GTK)
    create_include_link(${WIDGETS_DIR}/WK/BCContextMenuWK.h BAL/ContextMenu.h)

    create_include_link(${WIDGETS_DIR}/Gtk/BCClipboardGtk.h BAL/ClipboardGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCContextMenuItemGtk.h BAL/ContextMenuItem.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCCursorGtk.h BAL/Cursor.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCgtkdrawingGtk.h BAL/gtkdrawing.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCPasteboardGtk.h BAL/Pasteboard.h)
    create_include_link(${WIDGETS_DIR}/Gtk/PasteboardHelperGtk.h BAL/PasteboardHelperGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCPasteboardHelperGtk.h BAL/PasteboardHelper.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCPlatformMenuDescriptionGtk.h BAL/PlatformMenuDescription.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCPlatformScreenGtk.h BAL/PlatformScreen.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCPopupMenuGtk.h BAL/PopupMenu.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCRenderThemeGtk.h BAL/RenderThemeGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCScrollbarGtk.h BAL/ScrollbarGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCScrollbarThemeGtk.h BAL/ScrollbarThemeGtk.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCScrollViewGtk.h BAL/ScrollView.h)
    create_include_link(${WIDGETS_DIR}/Gtk/BCSearchPopupMenuGtk.h BAL/SearchPopupMenu.h)
    
    aux_source_directory(${WIDGETS_DIR}/Gtk WEBCORE_SRC)
endif(USE_GRAPHICS_GTK)

if(USE_GRAPHICS_SDL)
    create_include_link(${WIDGETS_DIR}/SDL/BCContextMenuSDL.h BAL/ContextMenu.h)

    create_include_link(${WIDGETS_DIR}/SDL/BCClipboardSDL.h BAL/ClipboardSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCContextMenuItemSDL.h BAL/ContextMenuItem.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCCursorSDL.h BAL/Cursor.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPasteboardSDL.h BAL/Pasteboard.h)
    create_include_link(${WIDGETS_DIR}/SDL/PasteboardHelperSDL.h BAL/PasteboardHelperSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPasteboardHelperSDL.h BAL/PasteboardHelper.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPlatformMenuDescriptionSDL.h BAL/PlatformMenuDescription.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPlatformScreenSDL.h BAL/PlatformScreen.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCPopupMenuSDL.h BAL/PopupMenu.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCRenderThemeSDL.h BAL/RenderThemeSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCScrollbarThemeSDL.h BAL/ScrollbarThemeSDL.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCScrollViewSDL.h BAL/ScrollView.h)
    create_include_link(${WIDGETS_DIR}/SDL/BCSearchPopupMenuSDL.h BAL/SearchPopupMenu.h)

    aux_source_directory(${WIDGETS_DIR}/SDL WEBCORE_SRC)
endif(USE_GRAPHICS_SDL)

aux_source_directory(${WIDGETS_DIR}/WK WEBCORE_SRC)
