#ifndef CONSTANTS_UI_H
#define CONSTANTS_UI_H


//Macros
#define DEF_INT1(x, key) (gStyleSettings->value("global/" #key,x).toInt())
#define DEF_STR(x, key) (gStyleSettings->value("global/" #key,x).toString())

#define DEF_INT1(x, key, section) (gStyleSettings->value(#section "/" #key,x).toInt())
#define DEF_STR(x, key, section) (gStyleSettings->value(#section "/"  #key,x).toString())

#define FOO DEF_INT1(FOO, 1)


//App Style
#define STYLE_DIR "/styles/"
#define STYLE_DIR DEF_STR(STYLE_DIR, "/styles/")

//#define STYLE_INI_FILE (gSettings->value("style/style_sheet", "default.ini").toString())
#define STYLE_INI_FILE DEF_STR(style_sheet, "default.ini")
//Global Rect Options
#define UI_RECT_XRADIUS (gStyleSettings->value("global/x_rect_radius",3).toInt())
#define UI_RECT_YRADIUS (gStyleSettings->value("global/y_rect_radius",3).toInt())

#define SLOT_OVERLAP_RATIO (1.5)
#define MIN_USEFUL_SUBCHILD (3)

#define UI_USE_PRIMARY_SCRN true
#define UI_SCRN_NUMBER 0

#define UI_RANGEOVER_ENTIRE_SCREEN true

const QString relatedChildColor("blue");


//Fields by characters and controls by line-height
const int DESCRIPTION_MAX = 120;
const int MAX_EDIT_LINES = 4;
const int EDIT_MAIN_BOX_MARGIN =2;
const int UI_FIELD_DISPLAY_LIMIT = 20;


//Widths and heights
const int UI_DEFAULT_LIST_WIDTH=10;
const int UI_DEFAULT_ITEM_HEIGHT=32;
const int UI_POSTMASSAGE_ITEM_HEIGHT=48;

const int UI_DEFAULT_ICONBAR_SIDE_PADDING = 4;
const int UI_DEFAULT_ICONBAR_TOP_PADDING = 1;

const int UI_APP_MARGIN_WIN_VERT = 3;
const int UI_APP_MARGIN_WIN_HORIZ = 3;

const int UI_PREVIEW_ARROW_LEFT_RATIO = .66;
const int UI_PREVIEW_ARROW_RIGHT_RATIO = .66;

const QString UI_RECT_INNER_WIDTH_KEY("inner_width");
const QString UI_RECT_OUTTER_WIDTH_KEY("outter_width");
const QString UI_RECT_OFFSET_KEY("offset");
const QString UI_MINI_BAR_FONT("Roboto-Black");

const QString UI_RECT_COLOR_KEY("color");
const QString UI_RECT_BITMAP_KEY("bitmap");
const QString UI_RECT_PREFIX("rect_");
const QString UI_RECT_GRADIANT_ANGLE_KEY("angle");
const QString UI_RECT_GRADIANT_STOP_KEY("stop");
const QString RECT_GRADIANT_PREFIX("gradiant_");
const QString IMAGE_RECT_PREFIX("bitmap:");

//Padding and borders
const int UI_BORDER_WINDOW_LEFT_MARGIN=2;
const int UI_BORDER_WINDOW_RIGHT_MARGIN =2;
const int UI_BORDER_WINDOW_TOP_MARGIN =2;
const int UI_BORDER_WINDOW_BOTTOM_MARGIN =1;
const int UI_PREVIEW_RIGHT_OFFSET=6;

const int UI_DEFAULT_MINI_ICON_PADDING = 8;
const int UI_DEFAULT_MINI_ICONBAR_PADDING = 8;
const int UI_DEFAULT_ICON_PADDING = 8;
const int UI_DEFAULT_ICONBAR_PADDING = 8;
const int UI_DEFAULT_TEXT_ITEM_H_PADDING = 2;
const int UI_DEFAULT_TEXT_ITEM_V_PADDING = 1;

const int UI_DEFAULT_MINI_ICON_SIZE = 16;
const int UI_DEFAULT_MINI_ACTION_ICON_SIZE = 16;
const int UI_LEFT_MARGIN(UI_BORDER_WINDOW_LEFT_MARGIN);
const int UI_TOP_MARGIN=(UI_BORDER_WINDOW_TOP_MARGIN);

//Data entry padding and border
const int CUSTOM_FORM_TOPMARGIN=6;
const int CUSTOM_FORM_BOTTOMMARGIN=2;
const int CUSTOM_VERB_RIGHTMARGIN=1;
const int CUSTOM_LABEL_TOPMARGIN=3;
const int CUSTOM_LABEL_RIGHTMARGIN=4;
const int CUSTOM_LABEL_LEFTMARGIN=4;
const int CUSTOM_LABEL_HEIGHT=12;
const int CUSTOM_FIELD_STANDARDHEIGHT=32;
const int CUSTOM_FIELD_BOTTOMPADDING=2;
const int CUSTOM_FIELD_CHOICEHIEGHT=100;
const int CUSTOM_FIELD_PLACE_HOLDER_HIEGHT=3;
#define CUSTOM_FIELD_SELECT_COLOR Qt::darkYellow

const int CUSTOM_LABEL_SELECT_RIGHTMARGIN=2;
const int CUSTOM_LABEL_SELECT_LEFTMARGIN=2;

//Fonts and sizes
#define NAME_FONT_FAMILY_DEFAULT_NAME "Roboto-Black"
const QString NAME_FONT_FAMILY_DEFAULT = NAME_FONT_FAMILY_DEFAULT_NAME;
const QString NAME_BOLD_FONT_FAMILY_DEFAULT = NAME_FONT_FAMILY_DEFAULT;
const int NAME_FONT_SIZE_DEFAULT = 8;
const int NAME_FONT_ARG_SIZE = 10;
const int FONT_SIZE_DEFAULT = 8;
const int FONT_WEIGHT_DEFAULT = 0;
const bool FONT_ITALIC_DEFAULT = false;
const int CAT_BAR_DISPLAY_MAX_DEFAULT = 5;
#define BOLD_LINE_FONT QFont(NAME_FONT_FAMILY_DEFAULT, FONT_SIZE_DEFAULT, QFont::Bold)


#define UI_LIST_LINE_DEFAULT_STYLE  \
    "<style> body {  font-weight: bold; font-family: " \
    NAME_FONT_FAMILY_DEFAULT_NAME \
    ", Sans, Helvetica, sans-serif; font-size: 10px; "\
    "background-color: #88FF66;"\
    "color: #FAACAA;} <style>"

#define UI_MINIBAR_DEFAULT_STYLE \
    "<style> body {  font-weight: bold; font-family: " \
    NAME_FONT_FAMILY_DEFAULT_NAME \
    ", Sans, Helvetica, sans-serif; font-size: 10px; "\
    "background-color: #030103;} <style>"\
    "color: #FAFCFA;} <style>"

#define UI_MINIBAR_SUBTOPIC_STYLE \
    "<style> body {  font-family: " \
    NAME_FONT_FAMILY_DEFAULT_NAME \
    ", Sans, Helvetica, sans-serif; font-size: 9px; "\
    "background-color: #030103;} <style>"\
    "color: #FAFCFA;} <style>"


const int UI_INPUT_LABEL_MIN_SIZE=10;
const int UI_INPUT_LABEL_MAX_SIZE=16;

const int UI_LIST_INNER_MARGIN=2;

const float FIELD_NAME_RIGHTSPACE_RATIO = 0.4;
const int FIELD_NAME_FONT_STANDARDSIZE=6;
const int UI_FIELD_FONT_MARGIN=2;

const QString TIME_FONT_FAMILY_DEFAULT = "Verdana";
const int TIME_FONT_SIZE_DEFAULT = 7;


//Icons
const int UI_STANDARD_ICON_WIDTH = 32;
const int UI_ACTIVE_ICON_SIZE = 48;
const int UI_STANDARD_ICON_BORDER = 16;
const int UI_MAIN_ICON_SIZE=48;
#define UI_TEXT_PLACEHOLDER_ICON "text-x-generic"
#define UI_NEXTACTION_PLACEHOLDER_ICON "system-run"
#define TAG_ICON_NAME "bookmark-new"
#define KEYWORD_ICON_NAME "folder"

//Stock Icon
#define ARROW_ICON_NAME "media-playback-start"
#define PLUS_ICON_NAME "add"
#define PIN_ICON_NAME "stock_pin"
#define DEPRICATE_ICON_NAME "stock_close"

#define FOLDER_ICON_NAME "folder"
#define FILE_MISSING_ICON_NAME "image-missing"
//#define GENERIC_DOCUMENT_ICON_NAME "text-x-generic"
#define GENERIC_DOCUMENT_ICON_NAME "text-x-preview"

//For mini-icons, among other thing
#define FILE_TYPE_ICON "text-x-preview"
#define TAG_TYPE_ICON "folder"
#define MESSAGE_TYPE_ICON "mail_new"
#define PERSON_TYPE_ICON "stock_person"
#define FEED_TYPE_ICON "text-x-preview"
#define SITE_TYPE_ICON "text-x-html"


//Cuts and section sizes
const int UI_DEFAULT_TASKBAR_CUT = 10;
const int UI_DEFAULT_SECTION_WIDTH = 10;

const int UI_IMAGE_MIDSECTION_WIDTH=6;
const int UI_BACKGROUND_CUT_HEIGHT=50;
const QString UI_SKIN_DIR ="./skins/Default/";
const QString UI_IMAGE_FILE =UI_SKIN_DIR + "DefaultBox.png";
const QString TASK_IMAGE_FILE =UI_SKIN_DIR + "DefaultBox.png";
const QString UI_MINI_BAR_BACKGROUND =UI_SKIN_DIR + "DarkBar.png";
const QString UI_LIST_BACKGROUND =UI_SKIN_DIR + "lightTexture.png";

const QString UI_BORDER_TEXTURE_FILE =UI_SKIN_DIR +  "defaultbackground.png";
#define UI_TASKBAR_LABEL TASK_IMAGE_FILE
#define UI_LIST_RESIZE_TIME  ((1000*(STANDARD_MOMENT))/SIGNAL_MOMENT_RATIO)
#define UI_ICONSWITCH_TIME  ((1000*(STANDARD_MOMENT))/SIGNAL_MOMENT_RATIO)
#define UI_FADEIN_TIME  ((1000*(STANDARD_MOMENT))/SIGNAL_MOMENT_RATIO)
#define UI_TEXTEDIT_FADE_TIME  ((1000*(STANDARD_MOMENT))/SIGNAL_MOMENT_RATIO)
const int UI_BASE_FONT_SIZE = 13;
const int UI_EXPLANATION_FONT_SIZE = 8;
const int UI_MIN_TEXT_EDIT_WIDTH = 3;
const int UI_MIN_TEXT_EDIT_LENGTH = 20;
const int UI_MAX_MESSAGE_DESCRIPTION_LEN = 20;
const int UI_LIST_OFFSET=5;
const float UI_TASKBAR_SCREEN_RATIO = 0.70;

#define UI_VERTICAL_ORIENTATION gSettings->value("Display/verticalOrientation", 2).toInt()
#define UI_HORIZONTAL_ORIENTATION gSettings->value("Display/HorizontalOrientation", 1).toInt()

#define SET_UI_VERTICAL_ORIENTATION(x) (gSettings->setValue("Display/verticalOrientation", (x)))
#define SET_UI_HORIZONTAL_ORIENTATION(x) (gSettings->setValue("Display/HorizontalOrientation", (x)))

#define UI_BUTTON_WIDTH 14
#define UI_BUTTON_HEIGH 14

#define OPT_BUTTON_RATIO .75
#define OPT_BUTTON_WIDTH UI_BUTTON_WIDTH
#define OPT_BUTTON_HEIGHT UI_BUTTON_HEIGH

#define CLOSE_BUTTON_RATIO .85
#define CLOSE_BUTTON_WIDTH UI_BUTTON_WIDTH
#define COSE_BUTTON_HEIGHT UI_BUTTON_HEIGH

#define UI_LISTNAME_BOLD true

const QString UI_LISTNAME_STYLE = "";
const QString UI_LISTPATH_STYLE = "";
const QString UI_LISTDESCRIPTION_STYLE = "";

const float UI_LISTITEM_INNER_MARGIN_RATIO = 0.4;
const float UI_LISTITEM_FONT_ABJ_FACTOR = 0.2;

const int UI_LIST_PIN_RECT_TOP_MARGIN=1;
const int UI_LIST_PIN_RECT_HEIGHT=16;
const int UI_LIST_PIN_RECT_WIDTH=16;
const int UI_LIST_PIN_RECT_RIGHT_MARGIN=20;
const int UI_LIST_PIN_DEPRICATE_MARGIN=6;

const int UI_MAINBOX_BTM_EXPL_TEXT_OFFSET = 40;
const int UI_MAINBOX_BTM_EXPL_TEXT_MARGIN = 4;
const int UI_MAINBOX_LEFT_MARGIN = 10;
const int UI_ICON_WINDOW_RIGHT_MARGIN = 5;

const int UI_LONG_TEXT_LIMIT=30;
const int UI_LISTMESSAGE_BAR_WIDTH=100;
const int UI_LISTMESSAGE_BAR_HEIGHT=30;
const int UI_MINI_ICON_BAR_HEIGHT=32;
const bool UI_MINI_ICON_ON_MAINFRM=true;


const QString UI_MORE_STR = "More";

#define DOTS_CHARS "..."
#define DOTS_CHARS "..."
#define EXTENSION_STRING "..."
const QString ELIPSIS_STRING_CHARS(EXTENSION_STRING);
#define KEY_COLOR "yellow"
#define MULTIKEY_COLOR "yellow"
#define UI_ICON_KEY_COLOR "yellow"
#define NEXT_KEY_FORMAT "dark red"
#define EMPHESIS_COLOR "dark blue"
#define UI_SUBACTIVE_FONT "dark green"
#define UI_FIELD_FORMAT_STR "dark blue"
#define UI_FIELD_TYPE_FORMAT_STR "gray"
#define UI_FADEIN_COLOR "dark blue"
#define UI_ITEM_ARROW_GLOW_COLOR QColor(229,229,150)

#define UI_ICONBAR_ITEM_COLOR QColor("#9F937A")
#define UI_ICONBAR_MESSAGE_COLOR QColor("#FFF3FA")
#define UI_ICONBAR_ACTIVE_ITEM_COLOR QColor("#23105")


#define UI_ILLUSTRATIVE_POLYGON_COLOR Qt::blue

#define UI_CUSTOM_VERB_COLOR Qt::yellow
#define UI_LIST_TOOL_COLOR Qt::black
#define UI_CUSTOM_VERB_ARG_COLOR Qt::lightGray
#define UI_CUSTOM_HIGH_VALUE_ITEM Qt::yellow
#define UI_SELECTED_MINI_ICON_BACKGROUND Qt::lightGray
#define UI_SELECTED_MINI_ICON_PENWIDTH 3

#define UI_BORDER_WINDOW_OPACITY 0.3
#define UI_BORDER_WINDOW_GRADIENT_STOP 0.3
#define UI_BORDER_WINDOW_GRADIENT_ANGLE 30
#define UI_BORDER_WINDOW_GRADIENT_COLOR1 Qt::darkBlue
#define UI_BORDER_WINDOW_GRADIENT_COLOR2 QColor(75, 175, 230, 120)
#define UI_BORDER_WINDOW_GRADIENT_COLOR3 QColor(0, 99,88, 120)

#define UI_BORDE


//A unicode arrow character
#define UI_SEPARATOR_STRING QString(QChar(0x25B8))

//GUI messages...
#define NO_OP_ON_OP_ERR "No further actions available"
#define NO_OP_CHOSEN_ERR "Must chose an action"

#define STANDARD_GNOMEDIR_DESCRIPTION "Gnome Menus Directory"

const QString UI_MISSING_FIELD_INTICATOR("???");

#define LIST_SCREEN_RATIO 3




#endif // CONSTANTS_UI_H
