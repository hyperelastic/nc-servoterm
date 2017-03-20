#include <stdio.h>
#include "hal_list.h"


char *hal_categories_list[] = {
    "adc",
    "cauto",
    "curpid",
    "dq",
    "en",
    "enc_cmd",
    "enc_fb",
    "encm",
    "encs",
    "fault",
    "hv",
    "hyper",
    "idq",
    "io",
    "pid",
    "pmsm",
    "pmsm_limits",
    "pmsm_t2c",
    "probe",
    "res",
    "rev",
    "sim",
    "sserial",
    "stp",
    "term",
    "vel",
    "vel_int",
    "yaskawa"
};

int n_hal_categories = sizeof(hal_categories_list)/
                        sizeof(hal_categories_list[0]);

/* make sure, that matches */
char **hal_pins_list[] = {
/*"adc",        */ (char *[])   { "adc0.pin1", "adc0.pin2", "adc0.pin3"},
/*"cauto",      */ (char *[])   { "cauto0.pin1", "cauto0.pin2", "cauto0.pin3"},
/*"curpid",     */ (char *[])   { "curpid0.pin1", "curpid0.pin2", "curpid0.pin3"},
/*"dq",         */ (char *[])   { "1", "2"},
/*"en",         */ (char *[])   { "1"},
/*"enc_cmd",    */ (char *[])   { "1"},
/*"enc_fb",     */ (char *[])   { "1"},
/*"encm",       */ (char *[])   { "1"},
/*"encs",       */ (char *[])   { "1"},
/*"fault",      */ (char *[])   { "1"},
/*"hv",         */ (char *[])   { "1"},
/*"hyper",      */ (char *[])   { "1"},
/*"idq",        */ (char *[])   { "1"},
/*"io",         */ (char *[])   { "1"},
/*"pid",        */ (char *[])   { "1"},
/*"pmsm",       */ (char *[])   { "1"},
/*"pmsm_limits",*/ (char *[])   { "1"},
/*"pmsm_t2c",   */ (char *[])   { "1"},
/*"probe",      */ (char *[])   { "1"},
/*"res",        */ (char *[])   { "1"},
/*"rev",        */ (char *[])   { "1"},
/*"sim",        */ (char *[])   { "1"},
/*"sserial",    */ (char *[])   { "1"},
/*"stp",        */ (char *[])   { "1"},
/*"term",       */ (char *[])   { "1"},
/*"vel",        */ (char *[])   { "1"},
/*"vel_int",    */ (char *[])   { "1"},
/*"yaskawa"     */ (char *[])   { "1"}
};

/* match number with "hal_pins_list"*/
int n_hal_pins[] = {
/*"adc",        */  3, 
/*"cauto",      */  3,
/*"curpid",     */  3,
/*"dq",         */  2,
/*"en",         */  1,
/*"enc_cmd",    */  1,
/*"enc_fb",     */  1,
/*"encm",       */  1,
/*"encs",       */  1,
/*"fault",      */  1,
/*"hv",         */  1,
/*"hyper",      */  1,
/*"idq",        */  1,
/*"io",         */  1,
/*"pid",        */  1,
/*"pmsm",       */  1,
/*"pmsm_limits",*/  1,
/*"pmsm_t2c",   */  1,
/*"probe",      */  1,
/*"res",        */  1,
/*"rev",        */  1,
/*"sim",        */  1,
/*"sserial",    */  1,
/*"stp",        */  1,
/*"term",       */  1,
/*"vel",        */  1,
/*"vel_int",    */  1,
/*"yaskawa"     */  1
};

void test_hal_categories() {
    int i;
    for(i = 0; i<n_hal_categories; ++i) {
        printf("%s\n", hal_categories_list[i]);
    }
}

void test_hal_pins() {
    int i, j;
    for(i = 0; i<n_hal_categories; ++i) {
        for (j=0; j<n_hal_pins[i]; ++j) {
            printf("%s\n", hal_pins_list[i][j]);
        }
    }
}

/*
int main() {
    test_hal_categories();
    test_hal_pins();
    return 0;
}
*/
