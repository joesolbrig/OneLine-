#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QHash>
#include <QSettings>

//extern QSettings* gSettings;

//#define tr(x) (QObject::tr(x))


#define APPLICATION_NAME "Oneline"
#define ORGANIZATION_NAME "Oneline"

#ifndef MAX
#define MAX(x,y) (((x) > (y))? (x):(y))
#endif
#ifndef MIN
#define MIN(x,y) (((x) < (y))? (x):(y))
//#define MIN(x,y, z) (MIN((x), MIN((y),(z))))
#endif
#define PARENT_INDEX (-2)
#define SELF_INDEX (-1)
#define CHILDSTART_INDEX (0)

#ifndef DEFAULTE_PAGE_COUNT
#define DEFAULTE_PAGE_COUNT 2000
#endif


#define PLUGIN_SETTINGS_ARRAY_KEY "plugins"
#define PLUGIN_SETTINGS_NAME_KEY "plugin_name"
#define PLUGIN_SETTINGS_LOAD_KEY "load"

#define PLUGIN_SETTING_LOAD_INTERVAL_KEY "plugin load interval"
#define PLUGIN_SETTING_DEFAULT_LOAD_INTERVAL (STANDARD_MOMENT*1000)
#define PLUGIN_SETTING_EXTEND_INTERVAL_KEY "plugin load interval"
#define PLUGIN_SETTING_DEFAULT_EXTEND_INTERVAL (STANDARD_MOMENT*10)

#define USING_ITEM_HASH_CLASS
#define USING_STRING_HASH

#define DEFAULT_APP_DIR "/.oneline/"
#define APP_HOME_KEY_DIRS "apphome"
#define SCRIPT_HOME_KEY_DIRS "scripthome"
#define LOCAL_DIR "localDir"
#ifndef STANDALONE_CMDLINE
#define USER_APP_DIR (gDirs->value( LOCAL_DIR )[0])
#else
#define USER_APP_DIR QString(QDir::homePath() + DEFAULT_APP_DIR)
#endif

#define SCRIPT_KEY "script"
#define USER_SCRIPT_DIR (gDirs->value( SCRIPT_KEY )[0])


//#define PUSER_APP_DIR (QString(PluginInterface::ms_homeDir.toAscii()))
#define PUSER_APP_DIR ((PluginInterface::getHomeDir()))
#define PSCRIPT_DIR ((PluginInterface::getHomeDir()))
#define USER_APP_INI (gDirs->value("config")[0])
#define BROWSER_OPTION_DIR "browser_view_codes/"
#define BROWSER_STYLE_SHEET "style.css"



#define CUSTOM_ACTION_ITEM_DIR "/custom_actions"
#define GENERATED_ITEM_DIR "/generated_items/"
#define FEED_CACHE_DIR "/cached_feeds"
#define FEED_CACHE_SUFFIX "cached_feed"

#define DEFAULT_KC_FILE "online_database.kc"

#define XLS_SUBDIR "/xsl_code/"
#define BOOKMARK_XSLT "/xbel.xsl"
#define OLD_STYLE_BOOKMARK_XSLT "/oorecent_items.xsl"
#define RSS_HEADER_XSLT "/header_parser.xsl"
#define ICON_SUBDIR_NAME "generated_icons/"
#define ITEM_CACHE_NAME "item_cache/"
#define RECOL_PREVIEW_SUBDIR "temp_for_preview/"


//---------- KEYS of various - String identifiers for parts of the system.
//There would normally never be a reason for a user to change these...

//File plugin
#define FILE_CATALOG_PLUGIN_STR "File Catalogue Plugin"
#define FILE_CATALOG_PLUGIN_ID qHash(QString("File Catalogue Plugin"))
#define FILECAT_SEARCH_DEPTH "filecat_search_depth"

#define FIREFOX_PLUGIN_NAME "FirefoxPlugin"
#define FIREFOX_PLUGIN_ID qHash(QString(FIREFOX_PLUGIN_NAME))

const QString DESKTOP_ITEM_LABEL("Desktop Item File");
#define DESKTOP_DIR_LABEL "Desktop Dir File"
#define CONTACT_LABEL "Contact Label"
#define EMAIL_MESSAGE_LABEL "Contact Label"

#define TAG_LABEL "TagPlugin"
#define TAGGED_ITEM_LABEL "tagged item"
const QString TAG_ACTION_ITEM_LABEL("tag item");
const QString TAG_ITEM_LABEL("tag");
const QString KEYWORKD_ITEM_LABEL("keyword");
#define TAG_ACTION_LABEL "tag action"
#define TAG_DEFAULT_TAG_ACTION_LABEL "Tag plugin default action"

#define DESKTOP_ITEM_EXTENSION ".desktop"
#define ONELINE_FILE_EXTENSION "oneline"
#define FILE_DIRECTORY_PLUGIN_STR "File Catalogue Directory"
#define FILE_CATALOG_MIME_PARENT "File Catalogue Mime Parent"
#define FILE_CATALOG_CUSTOM_VERB_SOURCE "File Catalogue Custom Verb"


#define TEMP_CATERGORY_LABEL "TEMP_CATERGORY_LABEL"

#define DESKTOP_ENTRY_KEY "Desktop Entry"
#define DESKTOPFILE_NAME_KEY "name"
#define DESKTOPFILE_COMMENT_KEY "Comment"
#define DESKTOPFILE_CATEGORY_KEY "Categories"
//#define SUBSTITUTE_COMMAND_LINE_KEY "Substitute_Commandline_Key"
#define EXTERNAL_COMMAND_SOURCE_KEY "external command source key"
#define EXTERNAL_EXTEND_COMMAND "external_extend_command"

#define SET_EMPTY_KEY "Set_Empty_Key"
#define LONG_DESCRIPTION_KEY "long_description_key"
#define ACTIVE_ICON_KEY "active icon key"

#define HTML_HEADER_PROCESSED_KEY "html_processed_key"

//#define COVERING_TOTAL_KEY "cover total key"
#define EXCLU_COVERING_TOTAL_KEY "Exclusive covering total key"
#define CATEGORY_PRESSINGNESS_KEY "CATEGORY_PRESSINGNESS_KEY"


//const QString SET_EMPTY_KEY_STR("SET_EMPTY_KEY_STR");
const QString SET_FOR_DELETE_KEY_STR("SET_FOR_DELETE_KEY_STR");
const QString SET_FOR_DBINSERT_KEY_STR("SET_FOR_DBINSERT_KEY_STR");
const QString TAG_LEVEL_KEY_STR("TAG_LEVEL_KEY_STR");
const QString USE_DESCRIPTION_KEY_STR("USE_DESCRIPTION_KEY_STR");
const QString USE_DESCRIPTION_LONG_KEY_STR("USE_DESCRIPTION_LONG_KEY_STR");
const QString USE_TYPE_DESCRIPTION_KEY_STR("USE_TYPE_DESCRIPTION_KEY_STR");
const QString USE_LONG_NAME_KEY_STR("USE_LONG_NAME_KEY_STR");
const QString TAKES_ANY_KEYS_KEY_STR("TAKES_ANY_KEYS_KEY_STR");
const QString IS_TEMP_KEY_STR("IS_TEMP_KEY_STR");
const QString IS_DEPRICATED_KEY_STR("IS_DEPRICATED_KEY_STR");
const QString DISTINGUISH_KEY_STR("IS_TEMP_KEY_STR");
const QString MATCH_TYPE_KEY_STR("MATCH_TYPE_KEY_STR");
const QString USAGE_KEY_STR("USAGE_KEY_STR");
const QString EXTERNAL_WEIGHT_KEY_STR("EXTERNAL_WEIGHT_KEY_STR");
const QString RELEVANCE_WEIGHT_KEY_STR("RELEVANCE_WEIGHT_KEY_STR");
const QString TOTAL_WEIGHT_KEY_STR("TOTAL_WEIGHT_KEY_STR");
const QString SOURCE_WEIGHT_KEY_STR("SOURCE_WEIGHT_KEY_STR");
const QString WEIGHT_SECTION_KEY_STR("WEIGHT_SECTION_KEY_STR");
const QString CHOSENNESS_KEY_STR("CHOSENNESS_KEY_STR");
const QString VISIBILITY_KEY_STR("VISIBILITY_KEY_STR");
const QString ITEM_TYPE_KEY_STR("ITEM_TYPE_KEY_STR");
const QString DESCRIPTION_KEY_STR("DESCRIPTION_KEY_STR");
const QString ALT_DESCRIPTION_KEY_STR("ALT_DESCRIPTION_KEY_STR");
const QString LONG_TEXT_KEY_STR("LONG_TEXT_KEY_STR");
const QString NAME_KEY_STR("Item_Name");
const QString ICON_KEY_STR("ICON_KEY_STR");
const QString ICON_OVERRIDE_KEY_STR("ICON_OVERRIDE_KEY_STR");
const QString PLUGIN_ID_KEY_STR("PLUGIN_ID_KEY_STR");
const QString GET_WEIGHT_FROM_PLUGIN_KEY_STR("GET_WEIGHT_FROM_PLUGIN_KEY_STR");
const QString IS_ACTION_KEY_STR("IS_ACTION_KEY_STR");
const QString MATCHINDEX_KEY_STR("MATCHINDEX_KEY_STR");
const QString IS_FOR_LATER_KEY_STR("IS_FOR_LATER_KEY_STR");
const QString TEMPORARY_DESCRIPTION_KEY_STR("TEMPORARY_DESCRIPTION_KEY_STR");
const QString TEMPORARY_LONG_HTML_KEY_STR("TEMPORARY_LONG_HTML_KEY_STR");
const QString TEMPORARY_NAME_KEY_STR("TEMPORARY_NAME_KEY_STR");
const QString PREFIX_NAME_KEY_STR("PREFIX_NAME_KEY_STR");
const QString TEMP_LOW_PATH_KEY_STR("TEMP_LOW_NAME_KEY_STR");
const QString TEMP_NAME_KEY_STR("TEMP_NAME_KEY_STR");
const QString TEMPORARY_PRIORITY_KEY_STR("TEMPORARY_PRIORITY_KEY_STR");
const QString SET_PRIORITY_KEY_STR("SET_PRIORITY_KEY_STR");
const QString RESET_FILTER_ICONS_KEY("RESET_FILTER_ICONS_KEY");

const QString IS_TIMELY_ITEM_KEY_STR("IS_TIMELY_ITEM_KEY_STR");
const QString IS_INTERMEDIATE_SOURCE_KEY("is_intermediate_source");
const QString IS_CATEGORING_SOURCE_KEY("is_categorizing_source");
const QString ITEM_CREATION_TIME_KEY_STR("ITEM_CREATION_TIME_KEY_STR");
const QString ARG_TYPE_KEY_STR("ARG_TYPE_KEY_STR");
const QString ARG_VALUE_KEY_STR("ARG_VALUE_KEY_STR");
const QString IS_VERB_ARG_KEY_STR("IS_VERB_ARG_KEY_STR");
const QString IS_REQUIRED_ARG_KEY_STR("IS_REQUIRED_ARG_KEY_STR");
const QString CUSTOM_VERB_DISCRIPTION_KEY_STR("Item_Description_Generator");
const QString FIELD_NAME_KEY("FIELD_NAME_KEY");

const QString POSITIVE_WEIGHT_KEY_STR("POSITIVE_WEIGHT_KEY_STR");
const QString PATH_KEY_STR("Item_Path");
const QString ARG_PATH_KEY_STR("ARG_PATH_KEY_STR");
const QString USER_DESCRIPTION_KEY_STR("USER_DESCRIPTION_KEY_STR");
const QString PREVIEW_IMAGE_KEY("PREVIEW_IMAGE_KEY");
const QString PREVIEW_URL_KEY("PREVIEW_URL_KEY");
//const QString THUMBNAIL_KEY_STR("THUMBNAIL_KEY_STR");
const QString LAST_UPDATE_TYPE_KEY_STR("LAST_UPDATE_TYPE_KEY_STR");
const QString LAST_UPDATE_TIME_KEY_STR("LAST_UPDATE_TIME_KEY_STR");
const QString LAST_UPDATE_CHILD_PATH_KEY_STR("LAST_UPDATE_CHILD_PATH_KEY_STR");
const QString MAX_UPDATE_CHILD_PASSES_KEY_STR("MAX_UPDATE_CHILD_PASSES_KEY_STR");
const QString LAST_UPDATE_DEPTH_PASSES_KEY_STR("LAST_UPDATE_DEPTH_PASSES_KEY_STR");
const QString AUTHOR_KEY_STR("AUTHOR_KEY_STR");
const QString EMAIL_ADDR_KEY_STR("EMAIL_ADDR_KEY_STR");
const QString NAME_IS_TITLE_KEY_STR("NAME_IS_TITLE_KEY_STR");
const QString CHILD_ITEMS_HEIGHT_KEY("NAME_IS_TITLE_KEY_STR");
const QString ICON_URL_KEY("ICON_URL_KEY");
const QString TYPE_FILTERING_KEY("TYPE_FILTERING_KEY");
const QString LABEL_FILTERING_KEY("LABEL_FILTERING_KEY");
const QString EXTENSION_COMMAND_KEY("EXTENSION_COMMAND_KEY");
const QString EXTENSION_SUBSTITUTE_PATH_KEY("path_to_index");

const QString IS_FILTER_ITEM("IS_FILTER_ITEM");

const QString IS_STUB_KEY_STR("IS_STUB_KEY_STR");
const QString IS_OPERATION_KEY_STR("IS_OPERATION_KEY_STR");
const QString IS_SOURCE_REFRESH("IS_SOURCE_REFRESH");

const QString TIME_EXPLICITELY_SET_KEY_STR("TIME_EXPLICITELY_SET_KEY_STR");
const QString MODIFICATION_TIME_KEY("MODIFICATION_TIME_KEY");
const QString CHILD_SCAN_TIME_KEY("CHILD_SCAN_TIME_KEY");
const QString GEN_KEY_SETTINGS("childFillinData");

const QString AUTHENTICATION_STR_INHERITED_KEY("authenticate_str");


//-----------
const QString FILLIN_GEN_FILE_KEY("FILLIN_GEN_FILE_KEY");
const QString FILLIN_GEN_SUBKEY_KEY("Stream_key");
const QString EXTERN_SOURCE_KEY("EXTERN_SOURCE_KEY");
const QString EXTERN_SOURCE_ARG_PATH("sourceArg");
const QString EXTERN_SOURCE_ARG_NAME("sourceArgName");
const QString EXTERN_SOURCE_API_PATH("variableDownloadPath");
const QString FACEBOOK_ITEM_GENERATOR("facebook_item");

const QString MIME_TYPE_NAME_KEY("MIME_TYPE_NAME_KEY");

const QString ITEM_EXTENSION_LEVEL_KEY("ITEM_EXTENSION_LEVEL_KEY");

const QString NEW_ITEM_OF_TYPE_KEY("NEW_ITEM_OF_TYPE_KEY");
const QString SHOULD_CACHE_KEY("SHOULD_CACHE_KEY");
const QString ORGANIZING_TYPE_KEY("ORGANIZING_TYPE_KEY");
const QString SORTING_TYPE_KEY("SORTING_TYPE_KEY");
const QString ORGANIZING_CHARACTERISTIC_KEY("ORGANIZING_CHARACTERISTIC_KEY");
const QString LOCALLY_STORED_AT_KEY("LOCALLY_STORED_AT_KEY");
const QString UPDATEABLE_SOURCE_KEY("UPDATEABLE_SOURCE_KEY");
const QString EXTERNAL_GEN_SOURCE_KEY("EXTERNAL_GEN_SOURCE_KEY");
const QString IS_CHILD_STUB_KEY("IS_CHILD_STUB_KEY");
const QString CHILD_COUNT_STUBBED_KEY("CHILD_COUNT_STUBBED_KEY");
const QString SOURCE_UPDATE_TIME_KEY("UPDATEABLE_SOURCE_KEY");
const QString SUPPORT_SOURCE_UPDATE_TIME_KEY("SUPPORT_SOURCE_UPDATE_TIME_KEY");

//const QString GENERATING_SETTINGS_FILE_KEY("GENERATING_SETTINGS_FILE_KEY");
const QString REAL_MIMETYPE_KEY("REAL_MIMETYPE_KEY");
const QString REAL_MIMEDESCRIPT_KEY("REAL_MIMEDESCRIPT_KEY");
const QString HTML_PREVIEW_KEY("HTML_PREVIEW_KEY");
const QString HTML_PREVIEW_INLINED_KEY("HTML_PREVIEW_SHOWN_KEY");
const QString ITEM_HIDDEN_KEY("ITEM_HIDDEN_KEY");
const QString ITEM_SEEN_KEY("ITEM_SEEN_KEY");
const QString ITEM_PIN_KEY("ITEM_PINNED_KEY");
const QString ITEM_NOTPINABLE_KEY("ITEM_NOTPINABLE_KEY");
const QString ITEM_PINREMOVE_KEY("ITEM_PINREMOVE_KEY");
const QString ITEM_NAME_MARGIN_OVERIDE_KEY("custom_name_margin");

const QString UNSEEN_CHILDREN_KEY("UNSEEN_CHILDREN_KEY");
const QString SOURCE_MATCH_KEY("SOURCE_MATCH_KEY");

const QString CHILD_TYPE_OP_TAKES("CHILD_TYPE_OP_TAKES");

const QString ITEM_ACCOUNT_PATH_KEY("ITEM_ACCOUNT_PATH_KEY");
const QString TYPE_PARENT_KEY("TYPE_PARENT_KEY");



const QString DONT_KEY_INDEX("DONT_KEY_INDEX");

const QString EXECUTE_IF_FULLFILLED_KEY("EXECUTE_IF_FULLFILLED");

const QString SPECIFICALLY_CHOSEN_KEY="SPECIFICALLY_CHOSEN_KEY";
const QString EXECUTED_KEY="EXECUTED_KEY";

const QString PLACE_HOLDER_ITEM_KEY="PLACE_HOLDER_ITEM_KEY";

const QString OVERRIDE_FORMATTED_NAME_KEY="OVERRIDE_FORMATTED_NAME_KEY";
const QString RELATION_NAME_KEY="RELATION_NAME_KEY";

const QString IS_SUBORDINATE_KEY="IS_SUBORDINATE_KEY";
const QString BEST_MATCH_TYPE="BEST_MATCH_TYPE";

const QString FILTER_ROLE_KEY="FILTER_ROLE_KEY";
const QString ACTIVE_FILTER_ITEM_KEY="ACTIVE_FILTER_ITEM_KEY";
const QString CLOSABLE_ORGANIZING_SOURCE_KEY("CLOSABLE_ORGANIZING_SOURCE_KEY");

//--------------------

const QString ATTRIB_PRIORITY_POSTFIX("_ATTRIB_PRIORITY");

const QString ARRAY_CHILD_GEN_PREFIX("array_child_gen_");

const QString IS_PARENT_KEY("IsParent");
const QString TAKE_CHILD_KEY("TakesChildrenOf");
const QString MERGE_W_PARENT_KEY("MergeWithParent");
const QString CHILDREN_REPLACE_PARENT_KEY("ChildrenReplaceParents");
const QString GEN_FROM_PARENT_KEY("CreatedFromParentAttribs");
const QString ITEM_ARRAY_LIST_KEY("AddValueArray");

const QString CHAR_GEN_KEY_PREFIX("char_gen_");
const QString CHILD_GEN_PREFIX("child_gen_");

const QString LIST_BEGIN_DUMMY_TOKEN("list_dummy");

const QString ORDER_BY_SIBLING_KEY("order_by_sibling");
const QString SIBLING_ORDER_KEY("sibling_order");
const QString SIBLING_ORDER_PARENT_KEY("sibling_parent_path");



//const QString ITEM_PATH_CONFIG_FIEILD("Item_Path");

const QString ATTRIBUTE_GENERATOR_GROUP("ATTRIBUTE_GENERATOR_GROUP");
const QString DIRECTORIES_GROUP("directories");
const QString ITEM_CHILD_GEN_GROUP("ITEM_CHILD_GEN_GROUP");
const QString ITEM_CHARACTERISTIC_GEN_GROUP("ITEM_CHILD_GEN_GROUP");

const QString MAIN_MENU_APP_STATE("MAIN_MENU_APP_STATE");
const QString TASKBAR_APP_STATE("TASKBAR_APP_STATE");

const QString PARENT_NAME_TO_CHILD_SEARCH("PARENT_NAME_TO_CHILD_SEARCH");
const QString PARENT_PATH_TO_CHILD_SEARCH("PARENT_PATH_TO_CHILD_SEARCH");

const QString MESSAGE_SOURCE_TYPE("Message_Source");
const QString PERSON_POSTING_TYPE("Person_Posting");

//General network
#define ONELINE_HEADER_STRING "Oneline Relation Browser 0.1"
#define ONELINE_AGENT_STR "Oneline Associative Browser 0.1"

//Stream Data
const QString STREAM_FILTER_SOURCE("Xlst Filter Source key");
const QString STREAM_FILTER_FILESOURCE("Xlst_Filter_Source_Path");

//const QString STREAM_PATH("Stream_Path");
const QString STREAM_SOURCE_PATH("Stream_Source_Path");
const QString STREAM_TARGET_PATH("StreamPlugin Target Path Key");
const QString STREAM_LOGIN_PATH("StreamPlugin Login Path Key");
const QString STREAM_PASSWORD_PATH("StreamPlugin Password Path Key");
const QString STREAM_USERNAME_PATH("StreamPlugin Username Path Key");
#define MAX_WAIT 1000*60*2


const QString JSON_SOURCE_LABEL("JSON_SOURCE_LABEL");

const QString VERB_TAKES_NO_ARGS_KEY("VERB_TAKES_NO_ARGS_KEY");
const QString VERB_TAKES_URIS("VERB_TAKES_URIS_KEY");

#define SOURCE_TAG_SIG "tagSig_key"
#define SOURCE_TIME_SIG "timeSig_key"
#define SOURCE_KEY_SIG "keysSig_key"
#define SOURCE_CONTACT_SIG "contactSig_key"
#define SOURCE_LOGIN_SIG "loginSig_key"

//Duplicates..
const QString PARENT_SOURCE_ITEM_LABEL("Parent_Source");

const QString XSLT_SOURCE_STR("XSLT_SOURCE_STR");

const QString XSLT_ITEM_STR("Xlst Plugin Item");
const QString XLST_SOURCE_NAME("XlstSourceItem");
//#define XLSTPLUGIN_NAME "XsltPlugin"
#define STREAMPLUGIN_NAME "StreamPlugin"

#define XBEL_HISTORY_NAME1 "/.recently-used.xbel"
#define XBEL_HISTORY_NAME2 "/.recently-used"

#define FACEBOOK_PLUGIN_SOURCE_STR "Facebook_Source"
#define FACEBOOK_PLUGIN_SOURCE_PARENT_STR "Facebook_Source_Parent"
#define FACEBOOK_ACCOUNT_STR "Facebook_Account_Key"
#define TWITTER_SOURCE_STR "Twitter Plugin File Source String"
#define TWITTER_CONSUMER_KEY_KEY "TwitterConsumerkey"
#define TWITTER_CONSUMER_SECRET_KEY "TwitterConsumerSecret"

#define JSON_ACCESS_TOKEN "JSON_ACCESS_TOKEN"


//#define EMAIL_BODY_KEY "Email Body"

#define DEFAULT_HTML_MIME_TYPE "text/html"
#define DEFAULT_TEXT_MIME_TYPE "text/plain"
#define MIME_EXECUTABLE "application/x-executable"
#define MIME_SCRIPT "application/x-shellscript"

#define ERROR_TYPE "ERROR_TYPE"
//Settings Keys

const QString IS_USER_SPECIFIC_KEY("is_user_specific_key");

#define LAST_REFRESH_KEY "GenOps/last_Catalog_Refresh"

#define FILE_SETTINGS_KEY "FileCatalog"
#define GET_MENU_ITEMS_KEY "getMenuItems"
#define ADD_RECENT_ITEMS_SOURCE_KEY "addRecentItemsSource"

#define FULL_TEXT_PLUGIN_MIMEPARENT_FOUND "FULL_TEXT_PLUGIN_MIMEPARENT_FOUND"

const QString FULL_TEXT_PLUGIN_NAME("fulltext_plugin");
const QString IS_FULL_TEXT_INDEXED("IS_FULL_TEXT_INDEXED");
const QString FULL_TEXT_PLUGIN_KEY("FULL_TEXT_PLUGIN_KEY");
const int FULLTEXT_EXTENDWITHIN_CYCLES(10);

const int HASH_FILE_CATALOG = (qHash(QString(FILE_CATALOG_PLUGIN_STR)));
const QString HISTORY_INTERNAL_TIME_KEY("History Internal Time Key");
const QString HISTORY_EVENT_TYPE_KEY("History Event Type Key");
const QString HISTORY_USER_INPUT_KEY("History USer Input Key");

const QString ATTRIBUTE_GENERATOR_KEY("ATTRIBUTE_GENERATOR_KEY");

const int HISTORY_INTERNAL_TIME_HASH = qHash("History Internal Time Entry");

const QString EQUAL_FUNCTION("use");
const QString QUOTE_FUNCTION("quote");
const QString DIR_FUNCTION("dir");
const QString SHORT_QUOTE_PREFIX("Re: ");
const QString LONG_QUOTE_PREFIX("\n---------[You Wrote]--------------\n");

const QString META_ITEM_SEP("::");
#define TYPE_LIST_SEP "/&#&/"
#define TYPE_ITEM_SEP "/&*&/"
#define PATH_SEP "/"
#define CHAR_PATH_SEP '/'
#define FORMATTED_PATH_SEP "/"
#define HOME_ABREV "~"
#define ARG_SEPERATOR "|"

#define WHITE_SPACE_EXPR " "
#define EMAIL_EXPR "@"

#define CONTEXT_KEY ':'
const QString COLON_SEP(": ");

//const QString STR_SEP1("#");
//const QString STR_SEP2("$");
#define STR_SEP1 ('#')
#define STR_SEP2 ('$')

#define SYSTEM_ITEM_ID (0)

//local files should have no prefix but..
const QString SEPERATOR_REGEX("[/\\s]+");
const QString PREFIX_REGEX("://");
const QString FILE_PREFIX("file://");
const QString HTTP_NAME("http");
const QString HTTP_PREFIX("http://");
const QString HTTPS_NAME("https");
const QString HTTPS_PREFIX("https://");

const QString HTTP_EXTENSION1(".html");
const QString HTTP_EXTENSION2(".htm");

//specific to this App...
const QString HISTORY_PREFIX("history://");
const QString PLUGIN_PATH_PREFIX("plugin://");
const QString COMPOUND_ITEM_PREFIX("compound://");
const QString COMPOUND_FILE_PREFIX("compound_file://");
const QString EMAIL_ITEM_PREFIX("email://");
const QString EMAIL_ACCOUNT_PREFIX("email_account://");
const QString XSLT_SOURCE_PREFIX("xslt_source://");
//const QString FACEBOOK_SOURCE_PREFIX("facebook_source://");
const QString DESKTOPDIR_SOURCE_PREFIX("desktop_dir://");
//const QString DESKTOPITEM_SOURCE_PREFIX("desktop_item://");

#define BUILTIN "builtin"
#define ACTION_TYPE_DEF "action_type"

const QString TAGITEM_NAME("tag");
const QString OPERATION_NAME("operation");
const QString TAGITEM_PREFIX("tag://");
const QString KEYWORD_NAME("keyword");
const QString KEYWORD_PREFIX("keyword://");
const QString OPERATION_PREFIX("operation://");
const QString ACTION_NAME(ACTION_TYPE_DEF);
const QString ACTION_TYPE_PREFIX(ACTION_TYPE_DEF "://");
//const QString TYPE_PREFIX("type://");
const QString TYPE_PREFIX(ACTION_TYPE_DEF "://");
const QString PERSON_PREFIX("person://");
const QString SYNONYM_PREFIX("synonym://");

const QString TASK_PREFIX("task://");

//const QString CUSTOM_SOURCE_PREFIX("custom_source://");

const QString FORMAL_XML_BODY_NAME("Formal_Xml_BodyText_name");
const QString FORMAL_XML_DOC_NAME("Formal_Xml_Doc_name");

//RFC-whatever EMAIL constants
#define SUBJECT_STR "title"
const QString FROM_STR = "author";
const QString TO_STR ="recipient";
#define BODY_STR "body"

//Field names...
//const QString ANNONYMOUS_USER_FIELD("ANNONYMOUS_USER_FIELD");

//Field Types...

const QString BUILTIN_PREFIX(BUILTIN "://");
const QString VALUE_PREFIX("value://");
const QString USERTEXT_PREFIX("user_text://");
const QString XSLT_PREFIX("xslt_source://");

const QString LONG_TEXT_NAME("longtext");
const QString SHORT_TEXT_NAME("shorttext");
const QString EMAIL_ADDRESS_NAME("emailaddress");
const QString YOUR_EMAIL_ADDRESS_NAME("your_emailaddress");

const QString RFC_EMAIL_MESSAGE_NAME("message/rfc822");
const QString MESSAGE_NAME("message");
const QString WEB_ADDRESS_NAME("webaddress");
const QString NUMBER_NAME("number");
const QString FILE_NAME("file");
const QString FOLDER_NAME("filefolder");
const QString URL_ENCODE_NAME("urlencode");
const QString ANY_ITEM_NAME("genericitem");

const QString TWITTER_VERIFY_FIELD(ACTION_TYPE_DEF "://TwitterId");
const QString FACEBOOK_VERIFY_FIELD(ACTION_TYPE_DEF "://FacebookId");

const QString OAUTHKEY_KEY("oathkey");
const QString FACEBOOK_USERNAME_FIELD("user_name");

const int LONG_TEXT_LENGTH = 20;
const int SHORT_TEXT_LENGTH = 15;

const QString LONG_TEXT_PATH(ACTION_TYPE_DEF "://longtext");
#define LONG_TEXT_ITEM (CatItem(LONG_TEXT_PATH))
const QString SHORT_TEXT_PATH(ACTION_TYPE_DEF "://shorttext");
#define SHORT_TEXT_ITEM (CatItem(SHORT_TEXT_PATH))
const QString EMAIL_ADDRESS_PATH(ACTION_TYPE_DEF "://emailaddress");
#define EMAIL_ITEM (CatItem(EMAIL_ADDRESS_PATH))

const QString NUMBER_PATH(ACTION_TYPE_DEF "://number");
#define NUMBER_TYPE_ITEM (CatItem(NUMBER_PATH))
const QString WEBADDRESS_PATH(ACTION_TYPE_DEF "://webaddress");
#define WEBADDRESS_TYPE_ITEM (CatItem(WEBADDRESS_PATH))

const QString ITEM_ARG_PATH(ACTION_TYPE_DEF "://item");
#define ITEM_ARG_ITEM (CatItem(ITEM_ARG_PATH))

const QString VERB_TYPE_PATH(ACTION_TYPE_DEF "://verb");
const QString VERB_LABEL("Application");
#define VERB_TYPE_ITEM CatItem(VERB_TYPE_PATH)

const QString FILE_TYPE_PATH(ACTION_TYPE_DEF "://file");
#define FILE_TYPE_ITEM CatItem(FILE_TYPE_PATH)
const QString FOLDER_TYPE_PATH(ACTION_TYPE_DEF "://folder");
#define FOLDER_TYPE_ITEM CatItem(FOLDER_TYPE_PATH)
const QString SOURCE_TYPE_PATH(ACTION_TYPE_DEF "://source_cat");
#define SOURCE_TYPE_ITEM CatItem(SOURCE_TYPE_PATH)
const QString CATEGORY_TYPE_PATH(ACTION_TYPE_DEF "://categories");

//Compound types
const QString LOCAL_TYPE_PATH(ACTION_TYPE_DEF "://local");
#define LOCAL_TYPE_ITEM CatItem(LOCAL_TYPE_PATH)



const QString TIME_SORT_CATEGORY_ITEM_PATH(BUILTIN "://time_sort_pseudo_item");
const QString MIMETYPE_SORT_CATEGORY_ITEM_PATH(BUILTIN "://mime_type_sort_pseudo_item");
const QString SIZE_SORT_CATEGORY_ITEM_PATH(BUILTIN "://size_sort_pseudo_item");
const QString NAME_SORT_CATEGORY_ITEM_PATH(BUILTIN "://name_sort_pseudo_item");
const QString DEFAULT_SORT_CATEGORY_ITEM_PATH(BUILTIN "://default_sort_pseudo_item");
const QString UNSEEN_MSG_ITEMS_PATH(BUILTIN "://unseen_msg_pseudo_source");
const QString UNSEEN_ITEMS_PATH(BUILTIN "://unseen_item_pseudo_source");
const QString PEOPLE_PATH(BUILTIN "://person");
const QString RUN_PSEUDO_ITEM_PATH(BUILTIN "://run_pseudo_item");

const QString HISTORY_SRC_PATH(BUILTIN "://history-source");
const QString BASE_SRCOBJECT_PATH(BUILTIN "://base_type");
const QString BASE_TASKSRC_PATH(BUILTIN "://tasks");
const QString BASE_GNOME_MENU_PATH(BUILTIN "://gnome_menus");
const QString GNOME_MENUS_NAME("Apps");

const QString FACEBOOK_FRIEND_PATH(BUILTIN "://facebook_friends_source");
const QString PLACEHOLDER_PATH(BUILTIN "://placeholder");

const QString SOURCE_W_ARG_PATH(BUILTIN "://source_taking_arg");

const QString CUSTOM_VERB_PREFIX("custom_verb://");
const QString SHELL_VERB_PREFIX("shell_verb://");

const QString IS_REQUIRED_ARG_KEY("required_arg_key");
const QString CHARACTERISTIC_VALUE_KEY("CHARACTERISTIC_VALUE_KEY");
const QString CHARACTERISTIC_NAME_KEY("CHARACTERISTIC_NAME_KEY");
const QString TEMP_CHARACTERISTIC_VALUE_KEY("TEMP_CHARACTERISTIC_VALUE_KEY");
const QString TAG_CONSTANT_VALUE("TAG_CONSTANT_VALUE");

//Recent items source
const QString XMEL_BOOKMARKS_PATH1("xslt_source://~/recently-used.xmel");
const QString XMEL_BOOKMARKS_PATH2("xslt_source://~/recently-used");



const QString FILE_SIZE_CHARACTERISTIC_KEY("File_Size");

const QString MAILPLUGIN_NAME("MailPlugin");

//const QString ITEM_PATH_CONFIG_FIEILD("Item_Path");
const QString ITEM_DESCRIPTION_CONFIG_FIELD("Item_Description");
const QString ITEM_DESCRIPTION_GEN_FIELD("Item_Explanation_Generator");
const QString ITEM_NAME_CONFIG_FIEILD("Item_Name");
const QString ITEM_TYPE_CONFIG_FIEILD("Item_Type");
const QString ITEM_SYNONYM_PATH("Item_Synonym_Path");
const QString ITEM_PARENT_CONFIG_FIELD ("Item_Parent_Path");
const QString ITEM_CHILD_CONFIG_FIELD ("Item_Child_Path");
const QString ITEM_SECTION_CHILD_CONFIG_FIELD ("Item_Child_Section");
const QString ITEM_WEIGHT_CONFIG_FIELD("Item_Weight");
const QString FILTER_SUBDIR("xslt_sources");
const QString ITEM_TEMPLATE_DIR("item_templates");
const QString TEMPLATE_PARENT_STR("TEMParent_");
const QString TEMPLATE_CHILD_STR("TEMChild_");
const QString TEMPLATE_INT_STR("_INT");
const QString CHARACTERISTIC_CACHE_PREFIX("charac_cache_");

const QString DEFAULT_APP_ACTION_MIXIN("_DEFAULT_APP_ACTION");

const QString XSLT_PLUGIN_END_TAG("xslt_plugin_end_tag");

const unsigned int DOUBLE_SCALE_FACTOR(1000);
const int MAX_FULL_WEIGHT(DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR);
const int MAX_MAX_FULL_WEIGHT(DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR*10);
const unsigned int WEIGHT_TICS(10);
const qint32 STANDARD_TICS(3);
const int ITEM_TYPE_LEVEL(WEIGHT_TICS-2);
const int MAX_EXTRA_ORGANIZING_SOURCES(2);
const int TOP_ITEMS_TO_UPDATE(100);

//Total weight scale items by their sources
const unsigned int RELATIVE_WEIGHT_ABSOLUTE_FRECENCY_FACTORE(10);
const int PINNED_MIN_ORDER(20);

const QString LIST_ORGANIZING_CATEGORY_KEY("List Organizing Category key");

const int DEFAULT_RESTORE_DEPTH=1;

const int MAX_PATH_LENGTH(1024);
const int MAX_EXTERNAL_WEIGHT_BINARY_EXPONENT = 14;
const int MAX_MAX_EXTERNAL_WEIGHT = 20000;
#define ERROR_EXTERNAL_WEIGHT (MAX_MAX_EXTERNAL_WEIGHT)
const int MAX_EXTERNAL_WEIGHT = 5120;
const int VERY_HIGHT_EXTERNAL_WEIGHT = 640;
const int HIGH_EXTERNAL_WEIGHT = 160;
const int MEDIUM_EXTERNAL_WEIGHT = 40;
const int MINIMUM_EXTERNAL_WEIGHT = 10;
const qreal ITEM_SEEN_REDUCTION_RATIO(0.9);

#define DEPRICATED_DEFAULT_WEIGHT MINIMUM_EXTERNAL_WEIGHT

const qint32 MAX_TOTAL_WEIGHT = (DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR)-1;
const float VISIBILITY_DISCOUNT_FACTOR=0.25;

const int APP_FOLDER_WEIGHT_DECREASE_FACTOR = 4;

const int DEFAULT_SORT_SOURCE_INTIAL_PRESSINGNESS = 10;

const int INITIAL_MAX_EDIT_LINES = 3;

const int MAX_CHILDTYPE_COUNT=30;

const int VERB_PRELOAD_COUNT=100;

const int MIN_SUBSTR_MATCH_LEN=3;

const int GENERIC_KEYS_LIMIT=5;

//Command syntax
const char WORD_DIVIDER = ' ';
const char SOURCE_INDICATOR = ':';
const char LIST_INDICATOR = '%';

const char FIELD_TYPE_SEPERATOR = '.';


//------ RESPONSE CONFIG type options
//ratios and values setting Oneline's responsiveness to user input and such...

//#define STANDARD_MOMENT (gSettings->value("GenOps/LengthOfAMoment", 6*MILLISECS_IN_SECOND).toInt())
const int ONE_DAY_IN_SECONDS = 60*60*24;
const int MILLISECS_IN_SECOND = 1000;


#define UNSEEN_MSG_CUTOFF_INTERVAL (5*ONE_DAY_IN_SECONDS)
#define SEEN_MSG_CUTOFF_INTERVAL ONE_DAY_IN_SECONDS
#define NEW_ITEM_CUTOFF_INTERVAL ONE_DAY_IN_SECONDS

#define STANDARD_MOMENT (4) //Our unit is SECONDS!!! not, millisecs, no" ...*MILLISECS_IN_SECOND"
const uint TIMEOUT_MOMENTS=10;

#define UPDATE_PERIOD ((STANDARD_MOMENT)*100)
#define TAG_DELAY_PERIOD ((STANDARD_MOMENT)*100)
#define STUB_UPDATE_PERIOD (1)
#define BASE_TIMER_TIME (0.5)
#define ITEM_SEEN_PERIOD (STANDARD_MOMENT*2)
#define FORGROUND_SEARCH_DELAY (0)
#define BACKGROUND_SEARCH_DELAY (STANDARD_MOMENT)
#define SHOW_PREVIEW_DELAY (STANDARD_MOMENT*5)
#define VISIBLE_SOURCES_EXTEND_DELAY (STANDARD_MOMENT)
#define UI_PREVIEW_DELAY (STANDARD_MOMENT)
#define REFRESH_EXTEND_DELAY (STANDARD_MOMENT*100)
#define SHOW_LIST_DELAY (STANDARD_MOMENT*10)
#define TRY_HIDE_MINIICONS_DELAY (STANDARD_MOMENT/2)
#define DELAYED_UI_EFFECT_TIC (STANDARD_MOMENT/2)

#define LIST_HOVER_INTERVAL (2500)
#define LIST_HOVER_START_INTERVAL (500)
#define STANDARD_ANIMATION_DURACTION ((1000*STANDARD_MOMENT)/SIGNAL_MOMENT_RATIO)
#define PREVIEW_FLOAT_SECS (5)
#define UPDATE_DELAY_SECS (1)

const int MAX_WORDS_FOR_SEARCH(5);


//Update/extend interval management;
const int CATEGORY_CONTAINMENT_RATIO = 2;
const int INTERVAL_ADJ_PROP=10;
const int EXTENDING_PROP = 4;

const int USE_CATEGORY_FOR_FILTER_THRESH = 10;
const int CATEGORY_FILTER_PERCENTAGE = (2/3);

//const int SIGNAL_MOMENT_RATIO = 10;

//Animation speed
#define SIGNAL_MOMENT_RATIO 15

const int NEW_ITEM_CUFFOFF = ONE_DAY_IN_SECONDS*3;


const int NEW_ITEM_WEIGHT_CUTOFF = HIGH_EXTERNAL_WEIGHT;

const int DEFAULT_SOURCE_WEIGHT=200;
const int MAX_CHILD_SCAN_DEPTH = 5;
const int MAX_WORDS_IN_KEY_SEARCH=4;
const int KEY_SKIP_SWITCH_LIMIT = 3;
const int MAX_CHAR_LEN_FOR_KEY_MATCH = 7;

const int DEFAULT_BASE_SEARCH_DEPTH = 2;
const int MAX_DIR_SEARCH_DEPTH = 3;

const int MAX_TAG_DEPTH = 4;
const int SHORT_TEXT_CUTOFF = 20;
const int TEXT_CUTOFF_BOUNDARY = 10;

const int MAX_KEYS = 12;

const int MIN_NODE_DEFICIT_FOR_EXPAND=3;
const float MIN_NODE_RATIOS_FOR_CONDENSE=2.5;
const float MIN_RATIO_FOR_SPOTLIGHT_HIGHEST=5;


const int FIRE_FOX_FRECENCY = 6000;
const int PRD_TIME_SCALE =1000000;

const QString CUSTOM_MSG_PADDING ="custom_message_padding";
const QString CUSTOM_MSG_BAR_PADDING ="custom_message_bar_padding";
const QString CUSTOM_ICON_SIZE ="custom_icon_size";

const QString CUSTOM_MIN_ICON_COUNT_KEY = "Custom Min Icon Count Key";

const int MIN_ICON_VALUE = 5;

const QString REAL_ICON_KEY = "Real Icon Info Key";
const QString LIST_WIDGET_POSTFIX = "_list_widget";
const QString TAG_OPERATION_NAME("tag_item");
const QString TAG_OP_REMOVE_NAME("remove_tag");
const QString PIN_OPERATION_NAME("pin_item");
const QString DEPRICATE_OPERATION_NAME("depricate_item");
const QString SELECTION_OPERATION_NAME("select_item");
const QString GOTO_CHILD_OPERATION("go to child");
const QString GOTO_PARENT_OPERATION("go to parent");
const QString SET_PRIORIT_OPERATION("set priority op");
const QString ACTIVATE_OPTION_ITEM("ActivateOptionItem");
const QString TAG_ARG_STR("");

#define OP_REMOVE_PATH addPrefix(OPERATION_PREFIX,TAG_OP_REMOVE_NAME)

const QString FONT_FAMILY_KEY = "Default Display Font Key";


const QString FONT_SIZE_KEY = "default display font size key";
const QString FONT_WEIGHT_KEY = "font weight key";
const QString FONT_ITALIC_KEY = "Item display itallic default key";
const QString CUSTOM_WIDGET_GREETING_KEY = "Custom Widget Greeting Key";
const QString CUSTOM_SKIN_DIR_KEY = "Custom Skin Dir Key";
const QString CAT_BAR_DISPLAY_MAX_KEY = "cat bar display max key";

const QString ICONIZED_CHILD_KEY = "Display Iconize child key";
const QString CUSTOM_X_WINDOW_POS_KEY="Custom x window position key";
const QString CUSTOM_Y_WINDOW_POS_KEY="Custom y window position key";
const QString REPLACE_CHILDREN_TEMP_KEY="REPLACE CHILDREN TEMP KEY";

const QString VERB_COMMAND_STRING_KEY = "command";
const QString ARG_DESCRIPTION_KEY="Arg Description key";
const QString ARG_TYPE_KEY="Arg Type key";
const QString ORDER_OF_ARG="Order Of Argument";
const QString NAME_OF_ARG="NAME_OF_ARG";
//const QString MIN_ARG_NUM_KEY= "min_args";
const QString CUSTOM_ACTION_KEY="Custom Action key";
const QString OP_HELP_PHRASE_KEY = "operation_help_phrase";



const QString FILLIN_GENERATOR_KEY="SourceTemplate";


const QString IS_TIMELY_ITEM_KEY = "Is Time Item Key";
const QString LAST_UPDATETIME_KEY = "Last Up Date Time Key";

const QString ARG_SPEC_STRING="arg spec string";
const QString ARG_DEFAULT_STRING="arg default spec string";
const QString ARG_URL_PREFIX="urlForArg_";

const QString STATIC_ARG_KEY="STATIC_ARG_KEY";
const QString STATIC_FILE_KEY="STATIC_FILE_KEY";

const QString LIST_OFFSET_KEY="LIST_OFFSET_KEY";

const QString FIELD_TYPE_LABEL="field value label";
const QString CHARACTERISTIC_SOURCE_LABEL="CHARACTERISTIC_SOURCE_LABEL";

const QString FIELD_VALUE_KEY="field_value_key";
const QString ITEM_GEN_PATH_KEY="item gen path key";
const QString ITEM_GEN_FIELDS_KEY="item gen fields key";
const QString ITEM_GEN_FILE_KEY="Item_File";

const QString SIBLINGS_AS_CHILDREN_KEY = "sibling_as_children_key";




const QString SCALE_VALUE_MODIFIER="SCALE_VALUE_MODIFIER";


const int CUSTOM_VERB_DISCRIPTION_KEY = qHash(CUSTOM_VERB_DISCRIPTION_KEY_STR);

const QString NO_MATCH_ARGS_FOR_MSG="Can't %1 with your input";

//how deep do you look for items while filtering...
const int DEFAULT_NODE_SEARCH_DEPTH=5;

const int MAX_ARGS = 20;

const int CUSTOM_VERB_ARGPATH_INDEX = 0;
const int CUSTOM_VERB_ARGDESCRIPT_INDEX = 1;
const int CUSTOM_VERB_ARGSPEC_INDEX = 2;
const int CUSTOM_VERB_ARGDEFAULT_INDEX = 3;

#define VERB_ARG "verb_arg"

const int RECUR_DOWNLOAD_LIMIT=4;

const float TOO_HIGH_COALATE_RATIO = 0.6;
const float TOO_LOW_COALATE_RATIO = 0.25;

const int TIME_SORT_EXTRA_WEIGHT = 100;
const int MAX_ICON_CACHE_SIZE = 5000;

const float HEIGHT_DEFLECTION_RATIO= 0.3;

const int DEFAULT_ITEMS_PER_CELL = 4;
const int DEFAULT_DELAY_FACTOR= 20;
const int DEFAUL_KEYSTROKES_FOR_INTERNAL_TYPE=6;
const int BROAD_KEYSEARCH_MAXLEN=1;

const int MAX_SOURCE_COUNT=60;
const int ALL_SOURCE_LIMIT=500;

const int CHAR_ENCODE_LEN=5;
const int CHAR_ENCODE_NUMBER=28;

const float DB_ITEM_CATEGORY_RATIO= 0.5;
const float PATH_AS_NAME_CONTRACTION= 0.5;

const int DEFAULT_FULLTEXT_SUBFILE_PASSES=9;
const int DEFAULT_SUBDIR_SCAN_PASSES=500;


const int NUMBER_SOURCES_TO_BALANCE=10;
const int DEFAULT_USAGE=0;

const QString FACEBOOK_FRIENDS_QUERY_STR = "select name,pic_big, status,birthday_date, "
                                           "timezone from user where uid in (select uid2 from "
                                           "friend where uid1==%1)";

const QString FACEBOOK_SOURCE_QUERRY = "facebook_source_query";
const QString OAUTH2_TOKEN_KEY = "OAUTH2_TOKEN_KEY";
const QString EXTEND_IMMEDIATELY_EXTERNALLY_KEY = "EXTEND_IMMEDIATELY_EXTERNALLY_KEY";


const QString TWITTER_SOURCE_ITEM_COUNT_DESIRED_KEY = "Items desired from twitter key";
const QString TWITTER_LAST_ID = "Items desired from twitter key";
const QString PIN_MESSAGE("Bookmark Item (set max priority)");
const QString PIN_ITEM_NAME("Pin Item");
const QString DEPRICATE_MESSAGE("Hide Item (set min priority)");
const QString DEPRICATE_ITEM_NAME("Hide Item");
const QString CREATE_TAG_NAME("tag");
const QString REMOVE_TAG_NAME("remove tag");
const QString GOTO_CHILDREN_MESSAGE("expand item");


#define AUTOHIDE_BAR false
#define ALWAYS_SHOW_WINDOW true

#define ONELINE_ALWAY_VISIBLE gSettings->value("GenOps/hideiflostfocus", false).toBool()

const QString JAVA_FILTER_FILESOURCE("Webview_js_filter_path");

const QString CSS_FILTER_PATH_KEY("CSS_FILTER_PATH_KEY");

#ifndef STANDALONE_CMDLINE
    #define SCRIPT_DEFAULT_PATH "/usr/lib/oneline/script/"
#else
    #define SCRIPT_DEFAULT_PATH "../../script"
#endif

#define PREVIEW_CSS_DEFAULT_PATH SCRIPT_DEFAULT_PATH "/js/reader.css"
#define JAVA_SCRIPT_DEFAULT_PATH SCRIPT_DEFAULT_PATH "/js/reader.js"



//Item constants
#define TAG_LEVEL_DEFAULT CatItem::ATOMIC_ELEMENT
const bool USE_DESCRIPTION_DEFAULT =false;
const bool TAKES_ANY_KEYS_DEFAULT=false;
const bool IS_TEMP_DEFAULT=false;
#define MATCH_TYPE_DEFAULT CatItem::USER_KEYS
const int USAGE_DEFAULT=0;
const qint32 RELEVANCE_WEIGHT_DEFAULT=MEDIUM_EXTERNAL_WEIGHT;;
const qint32 TOTAL_WEIGHT_DEFAULT=MAX_TOTAL_WEIGHT;
const qint32 SOURCE_WEIGHT_DEFAULT=MINIMUM_EXTERNAL_WEIGHT;

const qint32 CHOSENNESS_DEFAULT=0;
const qint32 VISIBILITY_DEFAULT=0;
#define ITEM_TYPE_DEFAULT CatItem::LOCAL_DATA_DOCUMENT
const QString DESCRIPTION_DEFAULT="";
const QString ICON_DEFAULT="";
const int PLUGIN_ID_DEFAULT=0;
const bool GET_WEIGHT_FROM_PLUGIN_DEFAULT=false;
const bool IS_ACTION_DEFAULT=false;

const int PREV_SIBLING_INCLUSION=20;


//User Message strings------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

const QString NAME_NAME("Name");
const QString SIZE_NAME("Size");
const QString MIME_TYPE_NAME("type");
const QString TIME_NAME("Time");

const QString PLUGIN_NOT_FOUND_ERR("sorry, the plugin for this item wasn't not found");
const QString PLUGIN_NOT_LOADED_ERR("sorry, the plugin for this item isn't loaded");

const QString TAG_PHRASE(" tag ");
#define AS_PHRASE " as "
#define TO_PHRASE " to "
#define WITH_PHRASE " with "
#define OPEN_PHRASE " open "
#define USE_PHRASE " use "

#define CANNOT_RUN_ERR " Cannot execute item "

#define NO_ACTION_MESSAGE " No action to take "

#define ILLEGAL_ORDER_MESSAGE "Cannot understand this sequence "
#define ARGUMENT_IS_MISSING_POSTFIX_MESSAGE " is missing "
const QString NO_ASSOCIATED_APP_MESSAGE(" I don't know what to do with this ");

#define ASSOCIATED_APPS_PSUEDO_FOLD_NAME "choose more programs"
#define EXTRA_ACTIONS_PSUEDO_FOLD_NAME "more custom actions"
#define ASSOCIATED_APPS_PSUEDO_FOLD_ICON "document-open"
#define INTERNAL_PSUEDO_FOLD_ICON "document-open"

#define OPERATION_PSUEDO_FOLD_NAME "choose more programs"

#define UI_HIGHEST_RATED_MESSAGE "Highest Weighted Items"

#define RUN_PSEUDO_ITEM_NAME "Run..."

#define EXTRA_OPS_PSUEDO_FOLD_NAME "more operations"

#define HTTP_CONTENT_LOADING_MSG "<html><head></head><body><h1>loading...</h1></body></html>"

const QString CUSTOM_FIELD_DESCRIPTION_PREFIX("Enter ");

#define UI_STYLE_DIR gSettings->value("GenOps/defSkin", gDirs["defSkin"][0]).toString()

#define RECOL_DB_DIR (settings->value("FullTextPlugin/recol_db_dir", QDir::homePath() + "/.recoll").toString())

const QString VERB_NAME("Apps");
const QString FILES_NAME("Files");
const QString WEBPAGE_NAME("Web Sites");
const QString MESSAGES_NAME("Messages");
const QString PEOPLE_NAME("People");
const QString NEW_POSTS_NAME("Feed");
const QString CATEGORY_NAME("Categories");
const QString TAGS_NAME("Tags");
const QString OPERATIONS_NAME("Operations");
const QString FOLDERS_NAME("Folders");

const QString LOCAL_NAME("Local");
const QString FEED_NAME("Feed");

#define MAX_ITEMS (gSettings->value("GenOps/numresults", 30).toInt())
#define MAX_SEPERATE_VERBS (2)
#define ITEM_COMPRESS_RATIO (6)

const QString TWITTER_PIN_ERROR = "cannot decode PIN somehow - press re-enter and press tab";
const QString ITEM_MISSING_ERROR = "Need more information here first";
const QString NO_MORE_ARGUMENTS = "Nothing more to add";

const QString FILE_DOESNT_EXIST_ERROR = "File Not Found";
const QString URL_LOAD_ERROR = "Error Loading...";
const QString URL_REDIRECT_MSG = "Redirected...";
const QString MINUTES_AGO_PHRASE = "minutes ago";
const QString FROM_PERSON__PHRASE= " from ";

const QString EMAIL_ACCOUNT_NAME_DESCRIPTION = "Email Account Of ";

const QString USE_CTRL_RETURN_MSG = "Press ctrl-return to finish ";
const QString USE_CTRL_TAB_MSG = "Press ctrl-tab to continue ";

#define UI_MAIN_GREETING "Type to Begin"

//--> UI_* = Appearance-specific values ---------------------------------------------------------------
//Element Names
#define MAIN_LIST_NAME "main_list"
#define MAIN_EDIT_NAME "main_edit"
#define BORDER_WIND_NAME "border_window"
#define CENTER_PREVIEW_NAME "center_preview"
#define SIDE_PREVIEW_NAME "side_preview"


//App Style
#define STYLE_DIR "/styles/"
#define STYLE_INI_FILE (gSettings->value("style/style_sheet", "default.ini").toString())
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
const int UI_BORDER_WINDOW_LEFT_MARGIN=4;
const int UI_BORDER_WINDOW_RIGHT_MARGIN =4;
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
const int CUSTOM_FORM_TOPMARGIN=2;
const int CUSTOM_FORM_BOTTOMMARGIN=2;
const int CUSTOM_VERB_RIGHTMARGIN=1;
const int CUSTOM_LABEL_TOPMARGIN=3;
const int CUSTOM_LABEL_RIGHTMARGIN=4;
const int CUSTOM_LABEL_LEFTMARGIN=10;
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
const int NAME_FONT_SIZE_DEFAULT = 9;
const int NAME_FONT_ARG_SIZE = 10;
const int FONT_SIZE_DEFAULT = 9;
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

#define DOWN_ARROW "down"
#define UP_ARROW "up"
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

//const QString UI_SKIN_DIR ="./skins/Default/";
//#define UI_SKIN_DIR (gDirs->value( "skins" )[0])
#define UI_SKIN_DIR (gDirs->value( "defSkin" )[0])
#define UI_IMAGE_FILE (UI_SKIN_DIR + "/DefaultBox.png")
#define TASK_IMAGE_FILE (UI_SKIN_DIR + "/DefaultBox.png")
#define UI_MINI_BAR_BACKGROUND  (UI_SKIN_DIR + "/DarkBar.png")
#define UI_LIST_BACKGROUND  (UI_SKIN_DIR + "/lightTexture.png")
#define UI_BORDER_TEXTURE_FILE (UI_SKIN_DIR +  "/defaultbackground.png")

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
const int UI_MINI_COUNT_LIMIT=4;
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


#endif // CONSTANTS_H
