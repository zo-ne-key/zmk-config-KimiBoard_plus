#include <device.h>
#include <drivers/gpio.h>
#include <zmk/behavior.h>
#include <zmk/events/trackball_moved.h>
#include <zephyr.h>

static int rotation_mode = 0; // 0°, 90°, 180°, 270°

struct tb_rotate_behavior_config {
    uint8_t button_idx; // SW1
};

struct tb_rotate_behavior_data {};

// 初期化
static int tb_rotate_init(const struct device *dev) {
    ARG_UNUSED(dev);
    return 0;
}

// 回転計算
static void rotate_vector(int dx, int dy, int *out_x, int *out_y) {
    switch(rotation_mode) {
        case 0:   *out_x = dx;  *out_y = dy;  break;
        case 90:  *out_x = -dy; *out_y = dx;  break;
        case 180: *out_x = -dx; *out_y = -dy; break;
        case 270: *out_x = dy;  *out_y = -dx; break;
    }
}

// trackballイベントにフック
static int tb_rotate_trackball_moved(struct zmk_event *ev) {
    struct zmk_trackball_moved *tb_evt = as_zmk_trackball_moved(ev);

    int rx, ry;
    rotate_vector(tb_evt->dx, tb_evt->dy, &rx, &ry);

    tb_evt->dx = rx;
    tb_evt->dy = ry;

    return 0;
}

// ボタン押下で回転切替
static int tb_rotate_button_pressed(struct zmk_event *ev) {
    rotation_mode += 1;
    if (rotation_mode >= 4) rotation_mode = 0;
    return 0;
}

ZMK_LISTENER(tb_rotate, tb_rotate_trackball_moved);
ZMK_SUBSCRIPTION(tb_rotate, zmk_trackball_moved);

ZMK_BEHAVIOR_DEFINE(tb_rotate, tb_rotate_init, tb_rotate_button_pressed);
