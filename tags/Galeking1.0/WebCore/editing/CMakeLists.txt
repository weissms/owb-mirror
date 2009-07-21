list(APPEND WEBCORE_SRC
    editing/AppendNodeCommand.cpp
    editing/ApplyStyleCommand.cpp
    editing/BreakBlockquoteCommand.cpp
    editing/CompositeEditCommand.cpp
    editing/CreateLinkCommand.cpp
    editing/DeleteButtonController.cpp
    editing/DeleteButton.cpp
    editing/DeleteFromTextNodeCommand.cpp
    editing/DeleteSelectionCommand.cpp
    editing/EditCommand.cpp
    editing/Editor.cpp
    editing/EditorCommand.cpp
    editing/FormatBlockCommand.cpp
    editing/htmlediting.cpp
    editing/HTMLInterchange.cpp
    editing/IndentOutdentCommand.cpp
    editing/InsertIntoTextNodeCommand.cpp
    editing/InsertLineBreakCommand.cpp
    editing/InsertListCommand.cpp
    editing/InsertNodeBeforeCommand.cpp
    editing/InsertParagraphSeparatorCommand.cpp
    editing/InsertTextCommand.cpp
    editing/JoinTextNodesCommand.cpp
    editing/markup.cpp
    editing/MergeIdenticalElementsCommand.cpp
    editing/ModifySelectionListLevel.cpp
    editing/MoveSelectionCommand.cpp
    editing/RemoveCSSPropertyCommand.cpp
    editing/RemoveFormatCommand.cpp
    editing/RemoveNodeCommand.cpp
    editing/RemoveNodePreservingChildrenCommand.cpp
    editing/ReplaceNodeWithSpanCommand.cpp
    editing/ReplaceSelectionCommand.cpp
    editing/SelectionController.cpp
    editing/SetNodeAttributeCommand.cpp
    editing/SmartReplace.cpp
    editing/SplitElementCommand.cpp
    editing/SplitTextNodeCommand.cpp
    editing/SplitTextNodeContainingElementCommand.cpp
    editing/TextIterator.cpp
    editing/TypingCommand.cpp
    editing/UnlinkCommand.cpp
    editing/VisiblePosition.cpp
    editing/VisibleSelection.cpp
    editing/visible_units.cpp
    editing/WrapContentsInDummySpanCommand.cpp
)

if(USE_I18N_ICU OR USE_I18N_GLIB)
    list(APPEND WEBCORE_SRC
        editing/SmartReplaceICU.cpp
    )
endif(USE_I18N_ICU OR USE_I18N_GLIB)

if(USE_I18N_QT)
    list(APPEND WEBCORE_SRC
        editing/qt/EditorQt.cpp
    )
endif(USE_I18N_QT)

if(USE_GRAPHICS_GTK)
    list(APPEND WEBCORE_SRC
       editing/gtk/SelectionControllerGtk.cpp
    )
endif(USE_GRAPHICS_GTK)
