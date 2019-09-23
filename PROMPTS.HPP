/**** Language Constants ****************************************************/

#define S_ENTER_YOUR_NAME                                                  pb_lang[   0 ].s
#define S_SCANNING_USER_FILE                                               pb_lang[   1 ].s
#define S_ENTER_PASSWORD                                                   pb_lang[   2 ].s
#define S_MORE                                                             pb_lang[   3 ].s
#define S_YES                                                              pb_lang[   4 ].s
#define S_NO                                                               pb_lang[   5 ].s
#define _S_PRESS_ENTER_TO_CONTINUE                                         pb_lang[   6 ].s
#define S_EVENT_HAS_TO_RUN                                                 pb_lang[   7 ].s
#define S_UPCOMING_EVENT                                                   pb_lang[   8 ].s
#define S_CHECK_MAIL_PROMPT                                                pb_lang[   9 ].s
#define S_ASK_YES_NO                                                       pb_lang[  10 ].s
#define S_ASK_NO_YES                                                       pb_lang[  11 ].s
#define S_ENTER_SYSTEM_PASSWORD                                            pb_lang[  12 ].s
#define S_WRONG_PASSWORD                                                   pb_lang[  13 ].s
#define S_ILLEGAL_NAME                                                     pb_lang[  14 ].s
#define S_WRONG_NAME_FORMAT                                                pb_lang[  15 ].s
#define S_ARE_YOU_A_NEW_USER(p1)                                parse_lang(pb_lang[  16 ].s , (p1))
#define S_NO_ACCESS_ON_THIS_NODE                                           pb_lang[  17 ].s
#define S_LEAVE_MESSAGE_TO_SYSOP                                           pb_lang[  18 ].s
#define S_HANGING_UP                                                       pb_lang[  19 ].s
#define S_LEVEL_EXPIRED                                                    pb_lang[  20 ].s
#define S_LEVEL_WILL_EXPIRE(p1)                                 parse_lang(pb_lang[  21 ].s , (p1))
#define S_LOCKED_OUT                                                       pb_lang[  22 ].s
#define S_PRIVATE_SYSTEM                                                   pb_lang[  23 ].s
#define S_NOT_FOR_NEW_USERS                                                pb_lang[  24 ].s
#define S_ASK_ANSI                                                         pb_lang[  25 ].s
#define S_ASK_AVATAR                                                       pb_lang[  26 ].s
#define S_ASK_AVATAR_PLUS                                                  pb_lang[  27 ].s
#define S_ASK_FSED                                                         pb_lang[  28 ].s
#define S_ASK_CLS                                                          pb_lang[  29 ].s
#define S_ASK_PAUSE                                                        pb_lang[  30 ].s
#define S_ASK_SCREENLENGTH(p1)                                  parse_lang(pb_lang[  31 ].s , (p1))
#define S_ASK_CITY                                                         pb_lang[  32 ].s
#define S_ASK_STATE                                                        pb_lang[  33 ].s
#define S_ASK_VOICE_PHONE                                                  pb_lang[  34 ].s
#define S_ASK_DATA_PHONE                                                   pb_lang[  35 ].s
#define S_ASK_BIRTHDATE(p1)                                     parse_lang(pb_lang[  36 ].s , (p1))
#define S_ASK_ALIAS                                                        pb_lang[  37 ].s
#define S_ALIAS_BEING_USED                                                 pb_lang[  38 ].s
#define S_ASK_PASSWORD(p1,p2)                                   parse_lang(pb_lang[  39 ].s , (p1) , (p2))
#define S_ILLEGAL_PASSWORD                                                 pb_lang[  40 ].s
#define S_ASK_PASSWORD_AGAIN                                               pb_lang[  41 ].s
#define S_ASK_EVERYTHING_CORRECT                                           pb_lang[  42 ].s
#define S_WRITE_MESSAGE_TITLE(p1)                               parse_lang(pb_lang[  43 ].s , (p1))
#define S_NOT_ALLOWED_TO_WRITE_IN_AREA                                     pb_lang[  44 ].s
#define S_ASK_WRITE_MSG_USING_YOUR_ALIAS                                   pb_lang[  45 ].s
#define S_ASK_WRITE_MSG_USING_AN_ALIAS                                     pb_lang[  46 ].s
#define S_ENTER_ALIAS_TO_USE                                               pb_lang[  47 ].s
#define S_ENTER_ALL_IF_TO_EVERYONE                                         pb_lang[  48 ].s
#define S_WRITE_MSG_TO                                                     pb_lang[  49 ].s
#define S_CONFIRM_WRITE_MESSAGE_TO(p1)                          parse_lang(pb_lang[  50 ].s,(p1))
#define S_FUZZY_SEARCH_LIST_HEADER                                         pb_lang[  51 ].s
#define S_FUZZY_SEARCH_LIST_FOOTER                                         pb_lang[  52 ].s
#define S_USER_NOT_LOCATED                                                 pb_lang[  53 ].s
#define S_ENTER_NETMAIL_ADDRESS_HEADER                                     pb_lang[  54 ].s
#define S_ENTER_NETMAIL_ADDRESS                                            pb_lang[  55 ].s
#define S_CONFIRM_NETMAIL_ADDRESS(p1,p2,p3)                     parse_lang(pb_lang[  56 ].s , (p1) , (p2) , (p3))
#define S_NOT_ENOUGH_NETMAIL_CREDIT(p1)                         parse_lang(pb_lang[  57 ].s , (p1))
#define S_ASK_SEND_CRASH                                                   pb_lang[  58 ].s
#define S_ASK_ATTACH_FILE                                                  pb_lang[  59 ].s
#define S_FILE_TO_ATTACH                                                   pb_lang[  60 ].s
#define S_SHOW_MESSAGE_SUBJECT(p1)                              parse_lang(pb_lang[  61 ].s , (p1))
#define S_ENTER_MESSAGE_SUBJECT                                            pb_lang[  62 ].s
#define S_ASK_PRIVATE_MESSAGE                                              pb_lang[  63 ].s
#define S_SAVING_MESSAGE                                                   pb_lang[  64 ].s
#define S_SAVING_MESSAGE_ERROR                                             pb_lang[  65 ].s
#define S_MESSAGE_ABORTED                                                  pb_lang[  66 ].s
#define S_USER_REQUESTS_CHAT(p1,p2)                             parse_lang(pb_lang[  67 ].s,(p1),(p2))
#define S_MULTILINE_CHAT_STARTED                                           pb_lang[  68 ].s
#define S_MULTILINE_CHAT_ENDED                                             pb_lang[  69 ].s
#define S_FATAL_ERROR                                                      pb_lang[  70 ].s
#define S_ACCESS_DENIED                                                    pb_lang[  71 ].s
#define S_THIS_MESSAGE_AREA_IS_LOCKED                                      pb_lang[  72 ].s
#define S_THERE_ARE_X_MESSAGES_IN_THIS_AREA(p1)                 parse_lang(pb_lang[  73 ].s,(p1))
#define S_THERE_ARE_NO_MESSAGES_IN_THIS_AREA                               pb_lang[  74 ].s
#define S_READ_MESSAGES_ORDER_SELECTION                                    pb_lang[  75 ].s
#define S_READMSG_FORWARD                                                  pb_lang[  76 ].s
#define S_READMSG_REVERSE                                                  pb_lang[  77 ].s
#define S_READMSG_ALL                                                      pb_lang[  78 ].s
#define S_READMSG_NEW                                                      pb_lang[  79 ].s
#define S_READMSG_SELECTED                                                 pb_lang[  80 ].s
#define S_READMSG_SELECTED_SEARCH_FOR                                      pb_lang[  81 ].s
#define S_READMSG_INDIVIDUAL                                               pb_lang[  82 ].s
#define S_READMSG_MARKED                                                   pb_lang[  83 ].s
#define S_READMSG_ALL_FORWARD_OR_REVERSE                                   pb_lang[  84 ].s
#define S_READMSG_START_AT_MESSAGE                                         pb_lang[  85 ].s
#define S_READMSG_SEARCHING_NEW_MESSAGES                                   pb_lang[  86 ].s
#define S_READMSG_NO_NEW_MESSAGES_FOUND                                    pb_lang[  87 ].s
#define S_READMSG_X_NEW_MESSAGES_FOUND(p1)                      parse_lang(pb_lang[  88 ].s , (p1))
#define S_READMSG_NO_MESSAGES_FOUND                                        pb_lang[  89 ].s
#define S_READMSG_WAIT_AFTER_EACH_MSG                                      pb_lang[  90 ].s
#define S_PRESS_S_TO_STOP_OR_P_TO_PAUSE                                    pb_lang[  91 ].s
#define S_READMSG_INDIV_ASK_MSGNUM                                         pb_lang[  92 ].s
#define S_READMSG_INDIV_MSG_NOT_FOUND                                      pb_lang[  93 ].s
#define S_ASK_DELETE_MSG                                                   pb_lang[  94 ].s
#define S_MESSAGE_MARKED                                                   pb_lang[  95 ].s
#define S_EXPORT_MESSAGE_TO_FILE                                           pb_lang[  96 ].s
#define S_THERE_IS_NO_PREVIOUS_MSG                                         pb_lang[  97 ].s
#define S_NO_ACCESS_TO_REPLIES                                             pb_lang[  98 ].s
#define S_END_OF_MESSAGES                                                  pb_lang[  99 ].s
#define S_QSCAN_MESSAGES_ORDER_SELECTION                                   pb_lang[ 100 ].s
#define S_QSCAN_MSG_HEADER                                                 pb_lang[ 101 ].s
#define S_QSCAN_MSG_DATALINE(p1,p2,p3,p4,p5)                    parse_lang(pb_lang[ 102 ].s , (p1) , (p2) , (p3) , (p4) , (p5) )
#define S_SCAN_MESSAGES_ORDER_SELECTION                                    pb_lang[ 103 ].s
#define S_SCAN_MESSAGES_SHOWMSG(p1,p2,p3,p4,p5)                 parse_lang(pb_lang[ 104 ].s , (p1) , (p2) , (p3) , (p4) , (p5) )
#define S_SCAN_MESSAGES_PROMPT                                             pb_lang[ 105 ].s
#define S_ENTER_MENU_PASSWORD                                              pb_lang[ 106 ].s
#define S_DOWNLOAD_TITLE                                                   pb_lang[ 107 ].s
#define S_DOWNLOAD_DELAY(p1)                                    parse_lang(pb_lang[ 108 ].s , (p1))
#define S_DOWNLOAD_NOT_NOW                                                 pb_lang[ 109 ].s
#define S_ENTER_FILES_TO_DOWNLOAD                                          pb_lang[ 110 ].s
#define S_ENTER_DOWNLOAD_FILENAME(p1)                           parse_lang(pb_lang[ 111 ].s , (p1))
#define S_SEARCHING_FILE_DATABASE                                          pb_lang[ 112 ].s
#define S_NO_PERSONAL_FILES_FOUND                                          pb_lang[ 113 ].s
#define S_DOWNLOAD_FILES_FOUND_HEADER                                      pb_lang[ 114 ].s
#define S_DOWNLOAD_FILE_FOUND_ENTRY(p1,p2,p3)                   parse_lang(pb_lang[ 115 ].s , (p1),(p2),(p3))
#define S_FILE_EXCEEDS_LIMIT(p1)                                parse_lang(pb_lang[ 116 ].s , (p1))
#define S_UPLOAD_REQUIRED(p1)                                   parse_lang(pb_lang[ 117 ].s , (p1))
#define S_FILE_FOUND_ACTION_PROMPT                                         pb_lang[ 118 ].s
#define S_ACTION_DOWNLOAD                                                  pb_lang[ 119 ].s
#define S_FREE_FILE                                                        pb_lang[ 120 ].s
#define S_ACTION_FILE_SKIPPED                                              pb_lang[ 121 ].s
#define S_ACTION_FILE_QUIT                                                 pb_lang[ 122 ].s
#define S_NO_FILES_FOUND                                                   pb_lang[ 123 ].s
#define S_X_FILES_SELECTED(p1,p2,p3)                            parse_lang(pb_lang[ 124 ].s , (p1),(p2),(p3))
#define S_NOT_ENOUGH_TIME_LEFT_FOR_DOWNLOAD                                pb_lang[ 125 ].s
#define S_START_DOWNLOAD_PROMPT                                            pb_lang[ 126 ].s
#define S_DOWNLOAD_ACTION_GOODBYE                                          pb_lang[ 127 ].s
#define S_ACTIVATING_PROTOCOL                                              pb_lang[ 128 ].s
#define S_COPYING_FROM_CDROM(p1)                                parse_lang(pb_lang[ 129 ].s , (p1))
#define S_ERROR_COPYING_FROM_CDROM                                         pb_lang[ 130 ].s
#define S_X_FILES_DOWNLOADED_SUCCESSFULLY(p1,p2)                parse_lang(pb_lang[ 131 ].s , (p1),(p2))
#define S_X_FILES_NOT_DOWNLOADED(p1)                            parse_lang(pb_lang[ 132 ].s , (p1))
#define S_HANGUP_AFTER_DOWNLOAD_PROMPT                                     pb_lang[ 133 ].s
#define S_MORE_TAG                                                         pb_lang[ 134 ].s
#define S_ENTER_FILE_TAGS                                                  pb_lang[ 135 ].s
#define S_X_FILES_TAGGED(p1)                                    parse_lang(pb_lang[ 136 ].s , (p1))
#define S_FILELIST_PRESS_S_TO_STOP                                         pb_lang[ 137 ].s
#define S_END_OF_FILE_LISTING                                              pb_lang[ 138 ].s
#define S_KEYWORD_SEARCH_TITLE_AND_PROMPT                                  pb_lang[ 139 ].s
#define S_SEARCHING_FOR_KEYWORD(p1)                             parse_lang(pb_lang[ 140 ].s , (p1))
#define S_END_OF_FILE_SEARCH                                               pb_lang[ 141 ].s
#define S_NEW_FILES_SEARCH_TITLE_AND_PROMPT(p1)                 parse_lang(pb_lang[ 142 ].s  ,(p1))
#define S_SEARCHING_FOR_NEW_FILES(p1)                           parse_lang(pb_lang[ 143 ].s  ,(p1))
#define S_FILENAME_SEARCH_TITLE_AND_PROMPT                                 pb_lang[ 144 ].s
#define S_SEARCHING_FOR_FILENAME(p1)                            parse_lang(pb_lang[ 145 ].s  ,(p1))
#define S_CHANGE_CITY_TITLE_AND_PROMPT(p1)                      parse_lang(pb_lang[ 146 ].s  ,(p1))
#define S_CHANGE_VOICE_PHONE_TITLE_AND_PROMPT(p1)               parse_lang(pb_lang[ 147 ].s  ,(p1))
#define S_CHANGE_DATA_PHONE_TITLE_AND_PROMPT(p1)                parse_lang(pb_lang[ 148 ].s  ,(p1))
#define S_CHANGE_STATE_TITLE_AND_PROMPT(p1)                     parse_lang(pb_lang[ 149 ].s  ,(p1))
#define S_CHANGE_ALIAS_TITLE_AND_PROMPT(p1)                     parse_lang(pb_lang[ 150 ].s  ,(p1))
#define S_CHANGE_SCREENLENGTH_TITLE_AND_PROMPT(p1)              parse_lang(pb_lang[ 151 ].s  ,(p1))
#define S_ANSI_ENABLED                                                     pb_lang[ 152 ].s
#define S_ANSI_DISABLED                                                    pb_lang[ 153 ].s
#define S_AVATAR_ENABLED                                                   pb_lang[ 154 ].s
#define S_AVATAR_DISABLED                                                  pb_lang[ 155 ].s
#define S_AVTPLUS_ENABLED                                                  pb_lang[ 156 ].s
#define S_AVTPLUS_DISABLED                                                 pb_lang[ 157 ].s
#define S_FSED_ENABLED                                                     pb_lang[ 158 ].s
#define S_FSED_DISABLED                                                    pb_lang[ 159 ].s
#define S_CLS_ENABLED                                                      pb_lang[ 160 ].s
#define S_CLS_DISABLED                                                     pb_lang[ 161 ].s
#define S_PAUSE_ENABLED                                                    pb_lang[ 162 ].s
#define S_PAUSE_DISABLED                                                   pb_lang[ 163 ].s
#define S_IBMCHARS_ENABLED                                                 pb_lang[ 164 ].s
#define S_IBMCHARS_DISABLED                                                pb_lang[ 165 ].s
#define S_HOTKEYS_ENABLED                                                  pb_lang[ 166 ].s
#define S_HOTKEYS_DISABLED                                                 pb_lang[ 167 ].s
#define S_RIP_ENABLED                                                      pb_lang[ 168 ].s
#define S_RIP_DISABLED                                                     pb_lang[ 169 ].s
#define S_ASK_HOTKEYS                                                      pb_lang[ 170 ].s
#define S_ASK_IBMCHARS                                                     pb_lang[ 171 ].s
#define S_ASK_RIP                                                          pb_lang[ 172 ].s
#define S_ASK_RIP_FONT                                                     pb_lang[ 173 ].s
#define S_LARGE_RIP_FONT_SELECTED                                          pb_lang[ 174 ].s
#define S_SMALL_RIP_FONT_SELECTED                                          pb_lang[ 175 ].s
#define S_CHANGE_PASSWORD_TITLE                                            pb_lang[ 176 ].s
#define S_ENTER_NEW_PASSWORD(p1)                                parse_lang(pb_lang[ 177 ].s  ,(p1))
#define S_PASSWORD_CHANGED_SUCCESSFULLY                                    pb_lang[ 178 ].s
#define S_LIST_USERS_TITLE_AND_PROMPT                                      pb_lang[ 179 ].s
#define S_END_OF_USER_LIST                                                 pb_lang[ 180 ].s
#define S_UPLOAD_TITLE                                                     pb_lang[ 181 ].s
#define S_NOT_ENOUGH_FREE_DISK_SPACE                                       pb_lang[ 182 ].s
#define S_NO_UPLOAD_DUE_TO_EVENT                                           pb_lang[ 183 ].s
#define S_FILE_TO_UPLOAD                                                   pb_lang[ 184 ].s
#define S_UPLOAD_START_PROMPT                                              pb_lang[ 185 ].s
#define S_READY_TO_UPLOAD_FILES                                            pb_lang[ 186 ].s
#define S_NO_FILES_RECEIVED                                                pb_lang[ 187 ].s
#define S_CHECKING_UPLOADS                                                 pb_lang[ 188 ].s
#define S_FILE_RECEIVED(p1)                                     parse_lang(pb_lang[ 189 ].s , (p1))
#define S_DUPLICATE_UPLOAD_REMOVED                                         pb_lang[ 190 ].s
#define S_DUPLICATE_UPLOAD                                                 pb_lang[ 191 ].s
#define S_X_FILES_UPLOADED(p1,p2)                               parse_lang(pb_lang[ 192 ].s , (p1),(p2))
#define S_NO_FILES_UPLOADED                                                pb_lang[ 193 ].s
#define S_PLEASE_ENTER_UPLOAD_DESCRIPTIONS                                 pb_lang[ 194 ].s
#define S_ENTER_USERNAME_FOR_PERSONAL_FILE(p1)                  parse_lang(pb_lang[ 195 ].s , (p1))
#define S_ENTER_DESCRIPTION_LINE_1(p1)                          parse_lang(pb_lang[ 196 ].s , (p1))
#define S_ENTER_DESCRIPTION_LINE_X                                         pb_lang[ 197 ].s
#define S_USER_LIST_HEADER                                                 pb_lang[ 198 ].s
#define S_UNMARKED_ALL_MESSAGES                                            pb_lang[ 199 ].s
#define S_PAGED_TOO_MANY_TIMES                                             pb_lang[ 200 ].s
#define S_PAGING_NOT_ALLOWED_RIGHT_NOW                                     pb_lang[ 201 ].s
#define S_ASK_CHAT_REASON                                                  pb_lang[ 202 ].s
#define S_CHAT_REASON_TOO_SHORT(p1)                             parse_lang(pb_lang[ 203 ].s , (p1))
#define S_SYSOP_NOT_RESPONDING                                             pb_lang[ 204 ].s
#define S_MSGED_HEADER                                                     pb_lang[ 205 ].s
#define S_MSGED_COMMAND_LIST                                               pb_lang[ 206 ].s
#define S_MSGED_LIST                                                       pb_lang[ 207 ].s
#define S_MSGED_SAVE                                                       pb_lang[ 208 ].s
#define S_MSGED_ASK_ABORT                                                  pb_lang[ 209 ].s
#define S_MSGED_CONTINUE                                                   pb_lang[ 210 ].s
#define S_MSGED_EDIT_LINE(p1)                                   parse_lang(pb_lang[ 211 ].s , (p1))
#define S_MSGED_DELETE_LINE                                                pb_lang[ 212 ].s
#define S_MSGED_DELETE_FROM_LINE(p1)                            parse_lang(pb_lang[ 213 ].s , (p1))
#define S_MSGED_DELETE_TO_LINE(p1,p2)                           parse_lang(pb_lang[ 214 ].s , (p1),(p2))
#define S_MSGED_X_LINES_DELETED(p1)                             parse_lang(pb_lang[ 215 ].s , (p1))
#define S_MSGED_INSERT_LINE(p1)                                 parse_lang(pb_lang[ 216 ].s , (p1))
#define S_MSGED_REPLACE_TEXT(p1)                                parse_lang(pb_lang[ 217 ].s , (p1))
#define S_MSGED_ENTER_TEXT_TO_REPLACE                                      pb_lang[ 218 ].s
#define S_MSGED_TEXT_NOT_FOUND                                             pb_lang[ 219 ].s
#define S_MSGED_REPLACE_BY                                                 pb_lang[ 220 ].s
#define S_MSGED_LINE_NOW_READS                                             pb_lang[ 221 ].s
#define S_VIEW_ARCHIVE_TITLE_AND_PROMPT                                    pb_lang[ 222 ].s
#define S_ARCHIVE_FILE_NOT_FOUND                                           pb_lang[ 223 ].s
#define S_ARCHIVE_HEADER_FILENAME                                          pb_lang[ 224 ].s
#define S_ARCHIVE_HEADER_DATE                                              pb_lang[ 225 ].s
#define S_ARCHIVE_HEADER_TIME                                              pb_lang[ 226 ].s
#define S_ARCHIVE_HEADER_ORIGSIZE                                          pb_lang[ 227 ].s
#define S_ARCHIVE_HEADER_COMPSIZE                                          pb_lang[ 228 ].s
#define S_TIME_LIMIT_EXCEEDED                                              pb_lang[ 229 ].s
#define S_TIME_LIMIT_WARNING(p1)                                parse_lang(pb_lang[ 230 ].s , (p1))
#define S_INACTIVITY_TIMEOUT(p1)                                parse_lang(pb_lang[ 231 ].s , (p1))
#define S_INACTIVITY_WARNING(p1)                                parse_lang(pb_lang[ 232 ].s , (p1))
#define S_MCHAT_TITLE                                                      pb_lang[ 233 ].s
#define S_MCHAT_ENTER_NODE_NUMBER                                          pb_lang[ 234 ].s
#define S_MCHAT_CALLING_OTHER_NODE                                         pb_lang[ 235 ].s
#define S_MCHAT_NODE_IS_NOT_AVAILABLE                                      pb_lang[ 236 ].s
#define S_MCHAT_STARTED                                                    pb_lang[ 237 ].s
#define S_MCHAT_OTHER_NODE_HUNG_UP                                         pb_lang[ 238 ].s
#define S_SELECT_AREAS_TO_SCAN_TITLE                                       pb_lang[ 239 ].s
#define S_SELECT_COMBINED_AREAS_TITLE                                      pb_lang[ 240 ].s
#define S_SELECT_AREAS_TO_TOGGLE                                           pb_lang[ 241 ].s
#define S_ENABLE_OR_DISABLE_ALL_AREAS                                      pb_lang[ 242 ].s
#define S_ALL_AREAS_ENABLED                                                pb_lang[ 243 ].s
#define S_ALL_AREAS_DISABLED                                               pb_lang[ 244 ].s
#define S_EDIT_TAGGED_TITLE                                                pb_lang[ 245 ].s
#define S_EDIT_TAGGED_PROMPT                                               pb_lang[ 246 ].s
#define S_ALL_FILE_TAGS_CLEARED                                            pb_lang[ 247 ].s
#define S_ENTER_FILE_TO_UNTAG                                              pb_lang[ 248 ].s
#define S_MAIL_FOUND_TITLE                                                 pb_lang[ 249 ].s
#define S_MAIL_FOUND_ACTION_PROMPT                                         pb_lang[ 250 ].s
#define S_CHECKING_MAILBOX                                                 pb_lang[ 251 ].s
#define S_MAILBOX_EMPTY                                                    pb_lang[ 252 ].s
#define S_MESSAGES_HAVE_BEEN_MARKED                                        pb_lang[ 253 ].s
#define S_CHECKING_FOR_PERSONAL_FILES                                      pb_lang[ 254 ].s
#define S_PERSONAL_FILES_FOUND                                             pb_lang[ 255 ].s
#define S_PERSONAL_FILE_FROM(p1)                                parse_lang(pb_lang[ 256 ].s , (p1))
#define S_PROTOCOL_MENU_TITLE                                              pb_lang[ 257 ].s
#define S_PROTOCOL_MENU_GOBACK                                             pb_lang[ 258 ].s
#define S_PROTOCOL_MENU_PROMPT                                             pb_lang[ 259 ].s
#define S_ENTER_MESSAGE_AREA_NUMBER                                        pb_lang[ 260 ].s
#define S_PRESS_ENTER_TO_LIST                                              pb_lang[ 261 ].s
#define S_SELECT_MESSAGE_AREA_TITLE                                        pb_lang[ 262 ].s
#define S_UNKNOWN_MESSAGE_AREA                                             pb_lang[ 263 ].s
#define S_ENTER_FILE_AREA_NUMBER                                           pb_lang[ 264 ].s
#define S_SELECT_FILE_AREA_TITLE                                           pb_lang[ 265 ].s
#define S_UNKNOWN_FILE_AREA                                                pb_lang[ 266 ].s
#define S_ZONELIST_HEADER                                                  pb_lang[ 267 ].s
#define S_NETLIST_HEADER(p1)                                    parse_lang(pb_lang[ 268 ].s , (p1))
#define S_NODELIST_HEADER(p1)                                   parse_lang(pb_lang[ 269 ].s , (p1))
#define S_NODE_NUMBER_NOT_FOUND                                            pb_lang[ 270 ].s
#define S_ENTER_FILENAME_TO_DISPLAY                                        pb_lang[ 271 ].s
#define S_REPLY_TO_MSG_USING_YOUR_ALIAS                                    pb_lang[ 272 ].s
#define S_REPLY_TO_MSG_USING_AN_ALIAS                                      pb_lang[ 273 ].s
#define S_DO_YOU_WANT_TO_CHANGE_THE_SUBJECT                                pb_lang[ 274 ].s
#define S_QUOTING_MESSAGE                                                  pb_lang[ 275 ].s
#define S_SHOW_GIF_INFO_TITLE_AND_PROMPT                                   pb_lang[ 276 ].s
#define S_SHOW_GIF_FILENAME                                                pb_lang[ 277 ].s
#define S_SHOW_GIF_RESOLUTION                                              pb_lang[ 278 ].s
#define S_SHOW_GIF_COLORS                                                  pb_lang[ 279 ].s
#define S_ENTER_BULLETIN                                                   pb_lang[ 280 ].s
#define S_FILE_NOT_FOUND                                                   pb_lang[ 281 ].s
#define S_SELECT_MSG_AREA_TO_MOVE_MESSAGE_TO                               pb_lang[ 282 ].s
#define S_MOVING_MESSAGE                                                   pb_lang[ 283 ].s
#define S_FORWARD_IN_SAME_AREA                                             pb_lang[ 284 ].s
#define S_SELECT_MSG_AREA_TO_FORWARD_MESSAGE_IN                            pb_lang[ 285 ].s
#define S_FORWARD_MESSAGE_TO                                               pb_lang[ 286 ].s
#define S_FORWARDING_MESSAGE                                               pb_lang[ 287 ].s
#define S_TIME_STATS_TITLE                                                 pb_lang[ 288 ].s
#define S_STATS_TIME_ONLINE(p1)                                 parse_lang(pb_lang[ 289 ].s , (p1))
#define S_STATS_TIME_LEFT(p1)                                   parse_lang(pb_lang[ 290 ].s , (p1))
#define S_STATS_FREE_TIME(p1)                                   parse_lang(pb_lang[ 291 ].s , (p1))
#define S_STATS_TIME_TODAY(p1)                                  parse_lang(pb_lang[ 292 ].s , (p1))
#define S_STATS_DAILY_TIME_LIMIT(p1)                            parse_lang(pb_lang[ 293 ].s , (p1))
#define S_STATS_DAILY_DL_LIMIT(p1)                              parse_lang(pb_lang[ 294 ].s , (p1))
#define S_STATS_DOWNLOAD_LEFT(p1)                               parse_lang(pb_lang[ 295 ].s , (p1))
#define S_STATS_KB_DOWNLOAD(p1)                                 parse_lang(pb_lang[ 296 ].s , (p1))
#define S_STATS_KB_UPLOADED(p1)                                 parse_lang(pb_lang[ 297 ].s , (p1))
#define S_STATS_KB_UPLOAD_NEEDED(p1)                            parse_lang(pb_lang[ 298 ].s , (p1))
#define S_STATS_FREE_DOWNLOAD(p1)                               parse_lang(pb_lang[ 299 ].s , (p1))
#define S_STATS_LAST_CALLED(p1,p2)                              parse_lang(pb_lang[ 300 ].s , (p1),(p2))
#define S_STATS_TOTAL_CALLS(p1)                                 parse_lang(pb_lang[ 301 ].s , (p1))
#define S_TOPS_TOP_CALLERS                                                 pb_lang[ 302 ].s
#define S_TOPS_TOP_CALLERS_UNIT                                            pb_lang[ 303 ].s
#define S_TOPS_TOP_DOWNLOADERS_KB                                          pb_lang[ 304 ].s
#define S_TOPS_TOP_DOWNLOADERS_KB_UNIT                                     pb_lang[ 305 ].s
#define S_TOPS_TOP_DOWNLOADERS_TIMES                                       pb_lang[ 306 ].s
#define S_TOPS_TOP_DOWNLOADERS_TIMES_UNIT                                  pb_lang[ 307 ].s
#define S_TOPS_TOP_UPLOADERS_KB                                            pb_lang[ 308 ].s
#define S_TOPS_TOP_UPLOADERS_KB_UNIT                                       pb_lang[ 309 ].s
#define S_TOPS_TOP_UPLOADERS_TIMES                                         pb_lang[ 310 ].s
#define S_TOPS_TOP_UPLOADERS_TIMES_UNIT                                    pb_lang[ 311 ].s
#define S_TOPS_TOP_MSGWRITERS                                              pb_lang[ 312 ].s
#define S_TOPS_TOP_MSGWRITERS_UNIT                                         pb_lang[ 313 ].s
#define S_TOPS_TOP_ONLINE                                                  pb_lang[ 314 ].s
#define S_TOPS_TOP_ONLINE_UNIT                                             pb_lang[ 315 ].s
#define S_SHOW_USERS_ONLINE_HEADER                                         pb_lang[ 316 ].s
#define S_USER_ONLINE_STATUS_BROWSING                                      pb_lang[ 317 ].s
#define S_USER_ONLINE_STATUS_CHATTING                                      pb_lang[ 318 ].s
#define S_USER_ONLINE_STATUS_ONLINE                                        pb_lang[ 319 ].s
#define S_USER_ONLINE_STATUS_INSHELL                                       pb_lang[ 320 ].s
#define S_USER_ONLINE_STATUS_BUSY                                          pb_lang[ 321 ].s
#define S_USER_ONLINE_NO_USER_ONLINE                                       pb_lang[ 322 ].s
#define S_DIRECTORY_TO_SHOW                                                pb_lang[ 323 ].s
#define S_ONE_MOMENT_PLEASE                    ((char *)replace_stringvars(pb_lang[ 324 ].s))
#define S_LOADING                              ((char *)replace_stringvars(pb_lang[ 325 ].s))
#define S_SYSOP_SHELLING                       ((char *)replace_stringvars(pb_lang[ 326 ].s))
#define S_SYSOP_RETURNED_FROM_DOS              ((char *)replace_stringvars(pb_lang[ 327 ].s))
#define S_SYSOP_CHAT_START                     ((char *)replace_stringvars(pb_lang[ 328 ].s))
#define S_SYSOP_CHAT_ENDED                     ((char *)replace_stringvars(pb_lang[ 329 ].s))
#define S_SELECT_LANGUAGE_TITLE                                            pb_lang[ 330 ].s
#define S_SELECT_LANGUAGE                                                  pb_lang[ 331 ].s
#define S_READING_LANGUAGE_FILE                                            pb_lang[ 332 ].s
#define S_INVALID_ENTRY                                                    pb_lang[ 333 ].s
#define S_ASK_FAX_PHONE                                                    pb_lang[ 334 ].s
#define S_ASK_MAILING_ADDRESS                                              pb_lang[ 335 ].s
#define S_ASK_SEX                                                          pb_lang[ 336 ].s
#define S_SELECT_DATE_FORMAT_TITLE                                         pb_lang[ 337 ].s
#define S_SELECT_DATE_FORMAT_PROMPT                                        pb_lang[ 338 ].s
#define S_CHANGE_FAX_PHONE_TITLE_AND_PROMPT(p1)                 parse_lang(pb_lang[ 339 ].s  ,(p1))
#define S_CHANGE_MAILING_ADDRESS_TITLE_AND_PROMPT(p1)           parse_lang(pb_lang[ 340 ].s  ,(p1))
#define S_CHANGE_DATE_FORMAT_TITLE(p1)                          parse_lang(pb_lang[ 341 ].s  ,(p1))
#define S_MALE                                                             pb_lang[ 342 ].s
#define S_FEMALE                                                           pb_lang[ 343 ].s
#define S_UNKNOWN                                                          pb_lang[ 344 ].s
#define S_ASK_COUNTRY                                                      pb_lang[ 345 ].s
#define S_CHANGE_COUNTRY_TITLE_AND_PROMPT(p1)                   parse_lang(pb_lang[ 346 ].s  ,(p1))
#define S_END_OF_LIST_TAG                                                  pb_lang[ 347 ].s
#define S_SELECT_FILE_AREA_GROUP_TITLE                                     pb_lang[ 348 ].s
#define S_SELECT_MSG_AREA_GROUP_TITLE                                      pb_lang[ 349 ].s
#define S_ENTER_GROUP_NUMBER                                               pb_lang[ 350 ].s
#define S_UNKNOWN_GROUP                                                    pb_lang[ 351 ].s
#define S_DOWNLOAD_NOW                                                     pb_lang[ 352 ].s
#define S_SCANNING_UPLOADS                                                 pb_lang[ 353 ].s
#define S_FOLLOWING_FILES_REJECTED                                         pb_lang[ 354 ].s
#define S_MSGHEADER_MESSAGENUM(p1,p2)                           parse_lang(pb_lang[ 355 ].s  ,(p1),(p2))
#define S_MSGHEADER_PRIVATE                                                pb_lang[ 356 ].s
#define S_MSGHEADER_CRASH                                                  pb_lang[ 357 ].s
#define S_MSGHEADER_KILL                                                   pb_lang[ 358 ].s
#define S_MSGHEADER_FILE                                                   pb_lang[ 359 ].s
#define S_MSGHEADER_SENT                                                   pb_lang[ 360 ].s
#define S_MSGHEADER_FROM(p1,p2,p3)                              parse_lang(pb_lang[ 361 ].s , (p1),(p2),(p3))
#define S_MSGHEADER_TO(p1)                                      parse_lang(pb_lang[ 362 ].s , (p1))
#define S_MSGHEADER_RECEIVED                                               pb_lang[ 363 ].s
#define S_MSGHEADER_RECEIVED_ON(p1,p2)                          parse_lang(pb_lang[ 364 ].s  ,(p1),(p2))
#define S_MSGHEADER_SUBJECT(p1)                                 parse_lang(pb_lang[ 365 ].s  ,(p1))
#define S_MSGHEADER_FILEATTACHED(p1)                            parse_lang(pb_lang[ 366 ].s  ,(p1))
#define S_MSGPROMPT_NEXT                                                   pb_lang[ 367 ].s
#define S_MSGPROMPT_PREV                                                   pb_lang[ 368 ].s
#define S_MSGPROMPT_AGAIN                                                  pb_lang[ 369 ].s
#define S_MSGPROMPT_STOP                                                   pb_lang[ 370 ].s
#define S_MSGPROMPT_MARK                                                   pb_lang[ 371 ].s
#define S_MSGPROMPT_REPLY                                                  pb_lang[ 372 ].s
#define S_MSGPROMPT_UNREAD                                                 pb_lang[ 373 ].s
#define S_MSGPROMPT_DEL                                                    pb_lang[ 374 ].s
#define S_MSGPROMPT_ORIG                                                   pb_lang[ 375 ].s
#define S_MSGPROMPT_SYSOPFUNC                                              pb_lang[ 376 ].s
#define S_MSGHELP_TITLE                                                    pb_lang[ 377 ].s
#define S_MSGHELP_NEXT                                                     pb_lang[ 378 ].s
#define S_MSGHELP_PREV                                                     pb_lang[ 379 ].s
#define S_MSGHELP_AGAIN                                                    pb_lang[ 380 ].s
#define S_MSGHELP_STOP                                                     pb_lang[ 381 ].s
#define S_MSGHELP_MARK                                                     pb_lang[ 382 ].s
#define S_MSGHELP_REPLY                                                    pb_lang[ 383 ].s
#define S_MSGHELP_DEL                                                      pb_lang[ 384 ].s
#define S_MSGHELP_UNREAD                                                   pb_lang[ 385 ].s
#define S_MSGHELP_PLUS                                                     pb_lang[ 386 ].s
#define S_MSGHELP_MINUS                                                    pb_lang[ 387 ].s
#define S_MSGHELP_ORIG                                                     pb_lang[ 388 ].s
#define S_MSGHELP_FORWARD                                                  pb_lang[ 389 ].s
#define S_MSGHELP_PRIVATE                                                  pb_lang[ 390 ].s
#define S_MSGHELP_MOVE                                                     pb_lang[ 391 ].s
#define S_MSGHELP_EXPORT                                                   pb_lang[ 392 ].s
#define S_MSGHELP_NOTE                                                     pb_lang[ 393 ].s
#define S_CHANGE_PROTOCOL_TITLE(p1)                             parse_lang(pb_lang[ 394 ].s , (p1))
#define S_NO_DEFAULT_PROTOCOL                                              pb_lang[ 395 ].s
#define S_PROTOCOL_SELECTED(p1)                                 parse_lang(pb_lang[ 396 ].s , (p1))
#define S_ENTER_SELECTION_PASSWORD                                         pb_lang[ 397 ].s
#define S_SYSTEM_GOING_DOWN                                                pb_lang[ 398 ].s
#define S_UNREG_NO_NEW_ACCOUNTS                                            pb_lang[ 399 ].s

#define K_MORE                                          pb_lang[   3 ].hotkeys
#define K_ASK_YES_NO                                    pb_lang[  10 ].hotkeys
#define K_ASK_NO_YES                                    pb_lang[  11 ].hotkeys
#define K_READ_MESSAGES_ORDER_SELECTION                 pb_lang[  75 ].hotkeys
#define K_READMSG_SELECTED                              pb_lang[  80 ].hotkeys
#define K_READMSG_ALL_FORWARD_OR_REVERSE                pb_lang[  84 ].hotkeys
#define K_QSCAN_MESSAGES_ORDER_SELECTION                pb_lang[ 100 ].hotkeys
#define K_SCAN_MESSAGES_ORDER_SELECTION                 pb_lang[ 103 ].hotkeys
#define K_SCAN_MESSAGES_PROMPT                          pb_lang[ 105 ].hotkeys
#define K_FILE_FOUND_ACTION_PROMPT                      pb_lang[ 118 ].hotkeys
#define K_START_DOWNLOAD_PROMPT                         pb_lang[ 126 ].hotkeys
#define K_HANGUP_AFTER_DOWNLOAD_PROMPT                  pb_lang[ 133 ].hotkeys
#define K_MORE_TAG                                      pb_lang[ 134 ].hotkeys
#define K_UPLOAD_START_PROMPT                           pb_lang[ 185 ].hotkeys
#define K_MSGED_COMMAND_LIST                            pb_lang[ 206 ].hotkeys
#define K_ENABLE_OR_DISABLE_ALL_AREAS                   pb_lang[ 242 ].hotkeys
#define K_EDIT_TAGGED_PROMPT                            pb_lang[ 246 ].hotkeys
#define K_MAIL_FOUND_ACTION_PROMPT                      pb_lang[ 250 ].hotkeys
#define K_ASK_SEX                                       pb_lang[ 336 ].hotkeys
#define K_END_OF_LIST_TAG                               pb_lang[ 347 ].hotkeys
#define K_MSGPROMPT_NEXT                                pb_lang[ 367 ].hotkeys
#define K_MSGPROMPT_PREV                                pb_lang[ 368 ].hotkeys
#define K_MSGPROMPT_AGAIN                               pb_lang[ 369 ].hotkeys
#define K_MSGPROMPT_STOP                                pb_lang[ 370 ].hotkeys
#define K_MSGPROMPT_MARK                                pb_lang[ 371 ].hotkeys
#define K_MSGPROMPT_REPLY                               pb_lang[ 372 ].hotkeys
#define K_MSGPROMPT_UNREAD                              pb_lang[ 373 ].hotkeys
#define K_MSGPROMPT_DEL                                 pb_lang[ 374 ].hotkeys
#define K_MSGPROMPT_ORIG                                pb_lang[ 375 ].hotkeys
#define K_MSGPROMPT_SYSOPFUNC                           pb_lang[ 376 ].hotkeys
