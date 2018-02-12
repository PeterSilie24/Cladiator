HEADERS += ../../source/utils/math.hpp \
    ../../source/utils/icons.hpp \
    ../../source/utils/reg-exp.hpp \
    ../../source/utils/translator.hpp \
    ../../source/utils/xml.hpp \
    ../../source/utils/action.hpp \
    ../../source/cladiator/cladiator.hpp \
    ../../source/document/document.hpp \
    ../../source/dialogs/about.hpp \
    ../../source/dialogs/license.hpp \
    ../../source/dialogs/preferences.hpp \
    ../../source/dialogs/export-as-image.hpp \
    ../../source/dialogs/attribute-editor.hpp \
    ../../source/dialogs/class-editor.hpp \
    ../../source/dialogs/method-editor.hpp \
    ../../source/dialogs/parameter-editor.hpp \
    ../../source/dialogs/association-editor.hpp \
    ../../source/dialogs/comment-editor.hpp \
    ../../source/content/association.hpp \
    ../../source/content/class.hpp \
    ../../source/content/comment.hpp \
    ../../source/content/connection.hpp \
    ../../source/content/content.hpp \
    ../../source/content/generalization.hpp \
    ../../source/content/navigability.hpp \
    ../../source/content/realization.hpp \
    ../../source/content/visibility.hpp \
    ../../source/items/aggregation-item.hpp \
    ../../source/items/association-item.hpp \
    ../../source/items/class-item.hpp \
    ../../source/items/comment-item.hpp \
    ../../source/items/composition-item.hpp \
    ../../source/items/connection-item.hpp \
    ../../source/items/generalization-item.hpp \
    ../../source/items/item.hpp \
    ../../source/items/realization-item.hpp \
    ../../source/item-scene/identifier.hpp \
    ../../source/item-scene/item-dragger.hpp \
    ../../source/item-scene/item-scene.hpp \
    ../../source/item-scene/undo-redo.hpp \
    ../../source/config/info.hpp \
    ../../source/config/settings.hpp \
    ../../source/class-diagram/class-diagram.hpp

SOURCES += ../../source/main.cpp \
    ../../source/document/document.cpp \
    ../../source/cladiator/cladiator.cpp \
    ../../source/utils/action.cpp \
    ../../source/utils/icons.cpp \
    ../../source/utils/math.cpp \
    ../../source/utils/reg-exp.cpp \
    ../../source/utils/translator.cpp \
    ../../source/utils/xml.cpp \
    ../../source/dialogs/about.cpp \
    ../../source/dialogs/association-editor.cpp \
    ../../source/dialogs/attribute-editor.cpp \
    ../../source/dialogs/class-editor.cpp \
    ../../source/dialogs/comment-editor.cpp \
    ../../source/dialogs/export-as-image.cpp \
    ../../source/dialogs/license.cpp \
    ../../source/dialogs/method-editor.cpp \
    ../../source/dialogs/parameter-editor.cpp \
    ../../source/dialogs/preferences.cpp \
    ../../source/content/association.cpp \
    ../../source/content/class.cpp \
    ../../source/content/comment.cpp \
    ../../source/content/connection.cpp \
    ../../source/content/content.cpp \
    ../../source/content/generalization.cpp \
    ../../source/content/navigability.cpp \
    ../../source/content/realization.cpp \
    ../../source/content/visibility.cpp \
    ../../source/items/aggregation-item.cpp \
    ../../source/items/association-item.cpp \
    ../../source/items/class-item.cpp \
    ../../source/items/comment-item.cpp \
    ../../source/items/composition-item.cpp \
    ../../source/items/connection-item.cpp \
    ../../source/items/generalization-item.cpp \
    ../../source/items/item.cpp \
    ../../source/items/realization-item.cpp \
    ../../source/item-scene/identifier.cpp \
    ../../source/item-scene/item-dragger.cpp \
    ../../source/item-scene/item-scene.cpp \
    ../../source/item-scene/undo-redo.cpp \
    ../../source/config/info.cpp \
    ../../source/config/settings.cpp \
    ../../source/class-diagram/class-diagram.cpp

FORMS += ../../source/cladiator/cladiator.ui \
    ../../source/dialogs/about.ui \
    ../../source/dialogs/license.ui \
    ../../source/dialogs/preferences.ui \
    ../../source/dialogs/export-as-image.ui \
    ../../source/dialogs/attribute-editor.ui \
    ../../source/dialogs/class-editor.ui \
    ../../source/dialogs/method-editor.ui \
    ../../source/dialogs/parameter-editor.ui \
    ../../source/dialogs/association-editor.ui \
    ../../source/dialogs/comment-editor.ui

TRANSLATIONS += ../../translations/cladiator_de.ts \
    ../../translations/cladiator_en.ts

RESOURCES += ../../source/resources/cladiator.qrc

win32:RC_FILE = ../../source/resources/cladiator.rc
