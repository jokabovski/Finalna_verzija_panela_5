#ifndef UI_INPUT_HELPERS_H
#define UI_INPUT_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include <stdlib.h>
#include <stdio.h>
#include "ui_events.h"

// Process input from textarea
static inline void process_input(lv_obj_t* textarea, lv_obj_t* label, int* variable) {
    const char* text = lv_textarea_get_text(textarea);
    *variable = atoi(text);
    lv_label_set_text(label, text);
    lv_textarea_set_text(textarea, "");
}

// Update label from variable (when you change variable in code)
static inline void update_label_from_int(lv_obj_t* label, int value) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", value);
    lv_label_set_text(label, buffer);
}
/*
void update_all_labels() {
    update_label_from_int(ui_Label18, vreme_rada_motor1);
    update_label_from_int(ui_Label19, vreme_rada_motor2);
    update_label_from_int(ui_Label24, vreme_pauze);
    update_label_from_int(ui_Label7, zeljena_temp_radionice);
    update_label_from_int(ui_Label8, max_temp_kotla);
}
*/


#ifdef __cplusplus
}
#endif

#endif