/*
 * file: hal_list.c
 *
 * Update workflow:
 *     1 find categories in stmbl/src/comps
 *     2 list pins in the classic servoterm, sending category names to stmbl
 *     3 commented unused categories in hal_categories_list and hal_pins_list
 *     4 count the number of pins per category and modify into n_hal_pins
 *     5 uncomment main (end of file), compile gcc -o test hal_list.c
 *         hal_list.h
 *     6 ./test and check for error
 *     7 comment main, remove ./test
 *
 */


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
//    "encs",
    "fault",
    "hv",
    "hyper",
    "idq",
    "io",
    "pid",
    "pmsm",
    "pmsm_limits",
//    "pmsm_t2c",
//    "probe",
    "res",
    "rev",
    "sim",
    "sserial",
    "stp",
    "term",
    "vel",
    "vel_int"
//    "yaskawa"
};

int n_hal_categories = sizeof(hal_categories_list)/
                        sizeof(hal_categories_list[0]);


/* make sure, this matches with "n_hal_pins" */
char **hal_pins_list[] = {
    (char *[])   {
        /*"adc",        */
        "adc0.rt_calc_time",
        "adc0.frt_calc_time",
        "adc0.rt_prio",
        "adc0.frt_prio",
        "adc0.sin",
        "adc0.cos",
        "adc0.sin3",
        "adc0.cos3",
        "adc0.quad",
        "adc0.res_en",
        "adc0.sin_gain ",
        "adc0.cos_gain ",
        "adc0.sin_offset ",
        "adc0.cos_offset "
    },

    (char *[])   {
        /*"cauto",      */
        "cauto0.rt_calc_time",
        "cauto0.frt_calc_time",
        "cauto0.rt_prio",
        "cauto0.frt_prio",
        "cauto0.i_q",
        "cauto0.i_d",
        "cauto0.pos",
        "cauto0.start ",
        "cauto0.ready",
        "cauto0.mag_offset ",
        "cauto0.start_offset ",
        "cauto0.mo",
        "cauto0.moffset",
        "cauto0.fb_in ",
        "cauto0.fb_out",
        "cauto0.cur ",
        "cauto0.time "
    },

    (char *[])   {
        /*"curpid",     */
        "curpid0.rt_calc_time",
        "curpid0.frt_calc_time",
        "curpid0.rt_prio",
        "curpid0.frt_prio",
        "curpid0.id_cmd ",
        "curpid0.iq_cmd ",
        "curpid0.id_fb ",
        "curpid0.iq_fb ",
        "curpid0.indd_fb ",
        "curpid0.indq_fb ",
        "curpid0.ud",
        "curpid0.uq",
        "curpid0.ac_volt ",
        "curpid0.dc_volt ",
        "curpid0.max_cur ",
        "curpid0.rd ",
        "curpid0.rq ",
        "curpid0.ld ",
        "curpid0.lq ",
        "curpid0.ff ",
        "curpid0.kp ",
        "curpid0.ki ",
        "curpid0.kind ",
        "curpid0.id_error",
        "curpid0.iq_error",
        "curpid0.res",
        "curpid0.volt_res"
    },

    (char *[])   {
        /*"dq",         */
        "dq0.rt_calc_time",
        "dq0.frt_calc_time",
        "dq0.rt_prio",
        "dq0.frt_prio",
        "dq0.u",
        "dq0.v",
        "dq0.w",
        "dq0.pos",
        "dq0.polecount",
        "dq0.a",
        "dq0.b",
        "dq0.c",
        "dq0.d",
        "dq0.q"
    },

    (char *[])   {
        /*"en",         */
        "en0.rt_calc_time",
        "en0.frt_calc_time",
        "en0.rt_prio",
        "en0.frt_prio",
        "en0.en",
        "en0.txen"
    },

    (char *[])   {
        /*"enc_cmd",    */
        "enc_cmd0.rt_calc_time",
        "enc_cmd0.frt_calc_time",
        "enc_cmd0.rt_prio",
        "enc_cmd0.frt_prio",
        "enc_cmd0.res",
        "enc_cmd0.pos",
        "enc_cmd0.index",
        "enc_cmd0.a",
        "enc_cmd0.b"
    },

    (char *[])   {
        /*"enc_fb",     */
        "enc_fb0.rt_calc_time",
        "enc_fb0.frt_calc_time",
        "enc_fb0.rt_prio",
        "enc_fb0.frt_prio",
        "enc_fb0.res",
        "enc_fb0.ires",
        "enc_fb0.pos",
        "enc_fb0.index",
        "enc_fb0.a",
        "enc_fb0.b",
        "enc_fb0.ipos",
        "enc_fb0.sin",
        "enc_fb0.cos",
        "enc_fb0.quad",
        "enc_fb0.oquad",
        "enc_fb0.oquadoff",
        "enc_fb0.qdiff",
        "enc_fb0.error",
        "enc_fb0.amp",
        "enc_fb0.s",
        "enc_fb0.c"
    },

    (char *[])   {
        /*"encm",       */
        "encm0.rt_calc_time",
        "encm0.frt_calc_time",
        "encm0.rt_prio",
        "encm0.frt_prio",
        "encm0.pos",
        "encm0.error",
    },

//    (char *[])   {
//        /*"encs",       */
//
//    },

    (char *[])   {
        /*"fault",      */
        "fault0.rt_calc_time",
        "fault0.frt_calc_time",
        "fault0.rt_prio",
        "fault0.frt_prio",
        "fault0.max_dc_cur ",
        "fault0.max_ac_cur ",
        "fault0.max_dc_volt ",
        "fault0.max_hv_temp ",
        "fault0.max_core_temp ",
        "fault0.max_motor_temp ",
        "fault0.max_pos_error ",
        "fault0.max_sat ",
        "fault0.high_dc_volt ",
        "fault0.high_hv_temp ",
        "fault0.high_motor_temp ",
        "fault0.low_dc_volt ",
        "fault0.fan_hv_temp ",
        "fault0.fan_core_temp ",
        "fault0.fan_motor_temp ",
        "fault0.enable ",
        "fault0.reset",
        "fault0.phase_on_start ",
        "fault0.phase_start",
        "fault0.phase_ready ",
        "fault0.start_offset",
        "fault0.hv_temp ",
        "fault0.core_temp0 ",
        "fault0.core_temp1 ",
        "fault0.motor_temp ",
        "fault0.dc_volt ",
        "fault0.dc_cur ",
        "fault0.ac_cur",
        "fault0.pos_error ",
        "fault0.fb_ready",
        "fault0.fb_error ",
        "fault0.cmd_ready",
        "fault0.cmd_error",
        "fault0.hv_no_comms ",
        "fault0.hv_error ",
        "fault0.sat ",
        "fault0.cmd ",
        "fault0.fb ",
        "fault0.cur",
        "fault0.brake_resistor",
        "fault0.brake",
        "fault0.brake_release",
        "fault0.hv_fan",
        "fault0.core_fan",
        "fault0.motor_fan",
        "fault0.fan",
        "fault0.enable_out",
        "fault0.enable_pid",
        "fault0.fault",
        "fault0.fault_not",
        "fault0.scale",
        "fault0.led_green",
        "fault0.led_red",
        "fault0.state"
    },

    (char *[])   {
        /*"hv",         */
        "hv0.rt_calc_time",
        "hv0.frt_calc_time",
        "hv0.rt_prio",
        "hv0.frt_prio",
        "hv0.a ",
        "hv0.b ",
        "hv0.mode",
        "hv0.enable ",
        "hv0.error",
        "hv0.over_cur",
        "hv0.over_temp",
        "hv0.hv_fault",
        "hv0.dc_cur",
        "hv0.dc_volt",
        "hv0.pwm_volt",
        "hv0.hv_temp",
        "hv0.power",
        "hv0.dc_cur_sim",
        "hv0.ac_cur_sim",
        "hv0.iq ",
        "hv0.rev "
    },

    (char *[])   {
        /*"hyper",      */
        "hyper0.rt_calc_time",
        "hyper0.frt_calc_time",
        "hyper0.rt_prio",
        "hyper0.frt_prio",
        "hyper0.pos",
        "hyper0.error",
        "hyper0.status",
        "hyper0.read_status"
    },

    (char *[])   {
        /*"idq",        */
        "idq0.rt_calc_time",
        "idq0.frt_calc_time",
        "idq0.rt_prio",
        "idq0.frt_prio",
        "idq0.d ",
        "idq0.q ",
        "idq0.pos ",
        "idq0.polecount ",
        "idq0.a",
        "idq0.b"
    },

    (char *[])   {
        /*"io",         */
        "io0.rt_calc_time",
        "io0.frt_calc_time",
        "io0.rt_prio",
        "io0.frt_prio",
        "io0.fan ",
        "io0.red ",
        "io0.green ",
        "io0.brake "
    },

    (char *[])   {
        /*"pid",        */
        "pid0.rt_calc_time",
        "pid0.frt_calc_time",
        "pid0.rt_prio",
        "pid0.frt_prio",
        "pid0.pos_ext_cmd ",
        "pid0.pos_fb ",
        "pid0.pos_error",
        "pid0.vel_ext_cmd ",
        "pid0.vel_fb ",
        "pid0.vel_cmd",
        "pid0.vel_error",
        "pid0.vel_min",
        "pid0.acc_ext_cmd",
        "pid0.acc_cmd",
        "pid0.torque_ext_cmd",
        "pid0.torque_cmd",
        "pid0.torque_cor_cmd",
        "pid0.friction",
        "pid0.damping",
        "pid0.load",
        "pid0.enable ",
        "pid0.pos_en",
        "pid0.vel_en",
        "pid0.acc_en",
        "pid0.p ",
        "pid0.pos_p ",
        "pid0.pos_lp",
        "pid0.vel_p ",
        "pid0.vel_lp",
        "pid0.vel_ff",
        "pid0.acc_p ",
        "pid0.acc_lp",
        "pid0.acc_pi ",
        "pid0.acc_ff",
        "pid0.torque_ff",
        "pid0.friction_ff",
        "pid0.damping_ff",
        "pid0.load_ff",
        "pid0.j ",
        "pid0.max_vel ",
        "pid0.max_torque ",
        "pid0.min_torque ",
        "pid0.max_usr_vel ",
        "pid0.max_usr_acc ",
        "pid0.max_usr_torque ",
        "pid0.vel_sat",
        "pid0.acc_sat",
        "pid0.torque_sat",
        "pid0.saturated"
    },

    (char *[])   {
        /*"pmsm",       */
        "pmsm0.rt_calc_time",
        "pmsm0.frt_calc_time",
        "pmsm0.rt_prio",
        "pmsm0.frt_prio",
        "pmsm0.psi ",
        "pmsm0.r ",
        "pmsm0.ld ",
        "pmsm0.lq ",
        "pmsm0.polecount ",
        "pmsm0.vel ",
        "pmsm0.ud ",
        "pmsm0.uq ",
        "pmsm0.indd",
        "pmsm0.indq",
        "pmsm0.id",
        "pmsm0.iq",
        "pmsm0.psi_d",
        "pmsm0.psi_q",
        "pmsm0.torque",
        "pmsm0.drop_q",
        "pmsm0.drop_d",
        "pmsm0.drop_v",
        "pmsm0.drop_exp"
    },

    (char *[])   {
        /*"pmsm_limits",*/
        "pmsm_limits0.rt_calc_time",
        "pmsm_limits0.frt_calc_time",
        "pmsm_limits0.rt_prio",
        "pmsm_limits0.frt_prio",
        "pmsm_limits0.psi ",
        "pmsm_limits0.r ",
        "pmsm_limits0.ld ",
        "pmsm_limits0.lq ",
        "pmsm_limits0.polecount ",
        "pmsm_limits0.ac_volt ",
        "pmsm_limits0.next_max_cur",
        "pmsm_limits0.next_max_torque",
        "pmsm_limits0.next_min_cur",
        "pmsm_limits0.next_min_torque",
        "pmsm_limits0.max_cur",
        "pmsm_limits0.max_torque",
        "pmsm_limits0.min_cur",
        "pmsm_limits0.min_torque",
        "pmsm_limits0.abs_max_cur",
        "pmsm_limits0.abs_max_torque",
        "pmsm_limits0.abs_max_vel",
        "pmsm_limits0.iq ",
        "pmsm_limits0.indq "
    },

//    (char *[])   {
//        /*"pmsm_t2c",   */
//    },

//    (char *[])   {
//        /*"probe",      */
//    },

    (char *[])   {
        /*"res",        */
        "res0.rt_calc_time",
        "res0.frt_calc_time",
        "res0.rt_prio",
        "res0.frt_prio",
        "res0.pos",
        "res0.amp",
        "res0.quad ",
        "res0.poles ",
        "res0.sin ",
        "res0.cos ",
        "res0.enable",
        "res0.error"
    },

    (char *[])   {
        /*"rev",        */
        "rev0.rt_calc_time",
        "rev0.frt_calc_time",
        "rev0.rt_prio",
        "rev0.frt_prio",
        "rev0.in",
        "rev0.out",
        "rev0.rev ",
        "rev1.rt_calc_time",
        "rev1.frt_calc_time",
        "rev1.rt_prio",
        "rev1.frt_prio",
        "rev1.in ",
        "rev1.out",
        "rev1.rev "
    },

    (char *[])   {
        /*"sim",        */
        "sim0.rt_calc_time",
        "sim0.frt_calc_time",
        "sim0.rt_prio",
        "sim0.frt_prio",
        "sim0.amp",
        "sim0.freq",
        "sim0.sin",
        "sim0.msin",
        "sim0.sin2",
        "sim0.msin2",
        "sim0.sin3",
        "sim0.msin3",
        "sim0.square",
        "sim0.vel",
        "sim0.res ",
        "sim0.offset"
    },

    (char *[])   {
        /*"sserial",    */
        "sserial0.rt_calc_time",
        "sserial0.frt_calc_time",
        "sserial0.rt_prio",
        "sserial0.frt_prio",
        "sserial0.dump_pd_vals",
        "sserial0.error",
        "sserial0.crc_error",
        "sserial0.connected",
        "sserial0.timeout",
        "sserial0.pos_cmd",
        "sserial0.pos_cmd_d",
        "sserial0.pos_fb",
        "sserial0.in0",
        "sserial0.in1",
        "sserial0.in2",
        "sserial0.in3",
        "sserial0.fault",
        "sserial0.out0",
        "sserial0.out1",
        "sserial0.out2",
        "sserial0.out3",
        "sserial0.enable"
    },

    (char *[])   {
        /*"stp",        */
        "stp0.rt_calc_time",
        "stp0.frt_calc_time",
        "stp0.rt_prio",
        "stp0.frt_prio",
        "stp0.target",
        "stp0.pos",
        "stp0.pos_in",
        "stp0.pos_out",
        "stp0.vel_in",
        "stp0.vel_out",
        "stp0.acc_out",
        "stp0.max_vel ",
        "stp0.max_acc ",
        "stp0.dtg"
    },

    (char *[])   {
        /*"term",       */
        "term0.rt_calc_time",
        "term0.frt_calc_time",
        "term0.rt_prio",
        "term0.frt_prio",
        "term0.wave0 ",
        "term0.gain0",
        "term0.offset0",
        "term0.wave1 ",
        "term0.gain1",
        "term0.offset1",
        "term0.wave2 ",
        "term0.gain2",
        "term0.offset2",
        "term0.wave3 ",
        "term0.gain3",
        "term0.offset3",
        "term0.wave4",
        "term0.gain4",
        "term0.offset4",
        "term0.wave5",
        "term0.gain5",
        "term0.offset5",
        "term0.wave6",
        "term0.gain6",
        "term0.offset6",
        "term0.wave7",
        "term0.gain7",
        "term0.offset7",
        "term0.send_step",
        "term0.buf_use",
        "term0.fault ",
        "term0.con",
        "term0.jogvel",
        "term0.jogpos",
        "term0.jogging",
        "term0.tim2_rv",
    },

    (char *[])   {
        /*"vel",        */
        "vel0.rt_calc_time",
        "vel0.frt_calc_time",
        "vel0.rt_prio",
        "vel0.frt_prio",
        "vel0.pos_in ",
        "vel0.pos_out",
        "vel0.vel",
        "vel0.acc",
        "vel0.w",
        "vel0.d",
        "vel0.g",
        "vel0.h",
        "vel0.j",
        "vel0.lp",
        "vel0.torque",
        "vel0.vel_ff",
        "vel0.en ",
        "vel0.pos_error",
        "vel1.rt_calc_time",
        "vel1.frt_calc_time",
        "vel1.rt_prio",
        "vel1.frt_prio",
        "vel1.pos_in ",
        "vel1.pos_out",
        "vel1.vel",
        "vel1.acc",
        "vel1.w",
        "vel1.d",
        "vel1.g",
        "vel1.h",
        "vel1.j ",
        "vel1.lp",
        "vel1.torque ",
        "vel1.vel_ff",
        "vel1.en ",
        "vel1.pos_error"
    },

    (char *[])   {
        /*"vel_int",    */
        "vel_int0.rt_calc_time",
        "vel_int0.frt_calc_time",
        "vel_int0.rt_prio",
        "vel_int0.frt_prio",
        "vel_int0.pos_in",
        "vel_int0.pos_out",
        "vel_int0.vel_in",
        "vel_int0.vel_out",
        "vel_int0.wd",
        "vel_int0.error"
    },

//    (char *[])   {
//        /*"yaskawa"     */
//    }
};

/* match number with "hal_pins_list" */
int n_hal_pins[] = {
/*"adc",        */  14, 
/*"cauto",      */  17,
/*"curpid",     */  27,
/*"dq",         */  14,
/*"en",         */  6,
/*"enc_cmd",    */  9,
/*"enc_fb",     */  21,
/*"encm",       */  6,
///*"encs",       */  1,
/*"fault",      */  58,
/*"hv",         */  21,
/*"hyper",      */  8,
/*"idq",        */  10,
/*"io",         */  8,
/*"pid",        */  49,
/*"pmsm",       */  23,
/*"pmsm_limits",*/  23,
///*"pmsm_t2c",   */  1,
///*"probe",      */  1,
/*"res",        */  12,
/*"rev",        */  14,
/*"sim",        */  16,
/*"sserial",    */  22,
/*"stp",        */  14,
/*"term",       */  36,
/*"vel",        */  36,
/*"vel_int",    */  10 
///*"yaskawa"     */  
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

