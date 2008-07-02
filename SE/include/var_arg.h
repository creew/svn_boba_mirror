extern int __var_arg_book;
extern int __var_arg_htext;
extern int __var_arg_call_back_ok;
extern int __var_arg_call_back_unk1;
extern int __var_arg_call_back_long_back;
extern int __var_arg_strinp_mode;
extern int __var_arg_strinp_fixed_text;
extern int __var_arg_strinp_text;
extern int __var_arg_strinp_new_line;
extern int __var_arg_strinp_empty_str_en;
extern int __var_arg_strinp_max_len;
extern int __var_arg_strinp_min_len;
extern int __var_arg_strinp_unk1;
extern int __var_arg_strinp_unk2;
extern int __var_arg_strinp_unk3;

extern int __var_arg_yesno_pre_question;
extern int __var_arg_yesno_question;
extern int __var_arg_yesno_yes_action;
extern int __var_arg_yesno_no_action;
extern int __var_arg_perinp_row_numbers;
extern int __var_arg_perinp_cur_percent;

#define VAR_BOOK(__param__) &__var_arg_book,__param__
#define VAR_HEADER_TEXT(__param__) &__var_arg_htext,__param__
#define VAR_OK_PROC(__param__) &__var_arg_call_back_ok,__param__
#define VAR_UNK1_PROC(__param__) &__var_arg_call_back_unk1,__param__
#define VAR_LONG_BACK_PROC(__param__) &__var_arg_call_back_long_back,__param__

#define VAR_STRINP_MODE(__param__) &__var_arg_strinp_mode,__param__
#define VAR_STRINP_FIXED_TEXT(__param__) &__var_arg_strinp_fixed_text,__param__
#define VAR_STRINP_TEXT(__param__) &__var_arg_strinp_text,__param__
#define VAR_STRINP_NEW_LINE(__param__) &__var_arg_strinp_new_line,__param__
#define VAR_STRINP_ENABLE_EMPTY_STR(__param__) &__var_arg_strinp_empty_str_en,__param__
#define VAR_STRINP_MAX_LEN(__param__) &__var_arg_strinp_max_len,__param__
#define VAR_STRINP_MIN_LEN(__param__) &__var_arg_strinp_min_len,__param__
#define VAR_STRINP_UNK1(__param__) &__var_arg_strinp_unk1,__param__
#define VAR_STRINP_UNK2(__param__) &__var_arg_strinp_unk2,__param__
#define VAR_STRINP_UNK3(__param__) &__var_arg_strinp_unk3,__param__

#define VAR_YESNO_PRE_QUESTION(__param__) &__var_arg_yesno_pre_question,__param__
#define VAR_YESNO_QUESTION(__param__) &__var_arg_yesno_question,__param__
#define VAR_YESNO_YES_ACTION(__param__) &__var_arg_yesno_yes_action,__param__
#define VAR_YESNO_NO_ACTION(__param__) &__var_arg_yesno_no_action,__param__

#define VAR_PERINP_ROWNUM(__param__) &__var_arg_perinp_row_numbers,__param__
#define VAR_PERINP_CURPERC(__param__) &__var_arg_perinp_cur_percent,__param__
