#pragma once
#include "messaging.hpp"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#define NANOVG_GL3_IMPLEMENTATION
#define nvgCreate nvgCreateGL3
#else
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#define NANOVG_GLES3_IMPLEMENTATION
#define nvgCreate nvgCreateGLES3
#endif

#include <atomic>
#include <map>
#include <string>
#include <sstream>

#include "nanovg.h"

#include "common/mat.h"
#include "common/visionipc.h"
#include "common/visionimg.h"
#include "common/framebuffer.h"
#include "common/modeldata.h"
#include "common/params.h"
#include "sound.hpp"

#define COLOR_BLACK nvgRGBA(0, 0, 0, 255)
#define COLOR_BLACK_ALPHA(x) nvgRGBA(0, 0, 0, x)
#define COLOR_WHITE nvgRGBA(255, 255, 255, 255)
#define COLOR_WHITE_ALPHA(x) nvgRGBA(255, 255, 255, x)
#define COLOR_OCHRE nvgRGBA(218, 111, 37, 255)
#define COLOR_OCHRE_ALPHA(x) nvgRGBA(218, 111, 37, x)
#define COLOR_GREEN nvgRGBA(0, 255, 0, 255)
#define COLOR_GREEN_ALPHA(x) nvgRGBA(0, 255, 0, x)
#define COLOR_ORANGE nvgRGBA(255, 165, 0, 255)
#define COLOR_ORANGE_ALPHA(x) nvgRGBA(255, 165, 0, x)
#define COLOR_RED nvgRGBA(255, 0, 0, 255)
#define COLOR_RED_ALPHA(x) nvgRGBA(255, 0, 0, x)
#define COLOR_YELLOW nvgRGBA(255, 255, 0, 255)
#define COLOR_YELLOW_ALPHA(x) nvgRGBA(255, 255, 0, x)
#define COLOR_ENGAGED nvgRGBA(23, 134, 68, 255)
#define COLOR_ENGAGEABLE nvgRGBA(23, 51, 73, 255)
#define COLOR_OPLONG nvgRGBA(105, 105, 105, 105)

#define UI_BUF_COUNT 4

typedef struct Rect {
  int x, y, w, h;
  int centerX() const { return x + w / 2; }
  int right() const { return x + w; }
  int bottom() const { return y + h; }
  bool ptInRect(int px, int py) const {
    return px >= x && px < (x + w) && py >= y && py < (y + h);
  }
} Rect;

const int sbr_w = 300;
const int bdr_is = 30;
const int bdr_s = 10;
const int vwp_h = 1080;
const int header_h = 420;
const int footer_h = 280;
const int footer_y = vwp_h-bdr_s-footer_h;
const Rect settings_btn = {50, 35, 200, 117};
const Rect home_btn = {60, 1080 - 180 - 40, 180, 180};

const int UI_FREQ = 20;   // Hz

const int MODEL_PATH_MAX_VERTICES_CNT = 98;
const int MODEL_LANE_PATH_CNT = 2;
const int TRACK_POINTS_MAX_CNT = 50 * 2;

const int SET_SPEED_NA = 255;

typedef enum NetStatus {
  NET_CONNECTED,
  NET_DISCONNECTED,
  NET_ERROR,
} NetStatus;

typedef enum UIStatus {
  STATUS_OFFROAD,
  STATUS_DISENGAGED,
  STATUS_ENGAGED,
  STATUS_ENGAGED_OPLONG,
  STATUS_WARNING,
  STATUS_ALERT,
} UIStatus;

static std::map<UIStatus, NVGcolor> bg_colors = {
  {STATUS_OFFROAD, nvgRGBA(0x07, 0x23, 0x39, 0xf1)},
  {STATUS_DISENGAGED, nvgRGBA(0x17, 0x33, 0x49, 0xc8)},
  {STATUS_ENGAGED, nvgRGBA(0x17, 0x86, 0x44, 0x0f)},
  {STATUS_WARNING, nvgRGBA(0xDA, 0x6F, 0x25, 0x0f)},
  {STATUS_ALERT, nvgRGBA(0xC9, 0x22, 0x31, 0xf1)},
  {STATUS_ENGAGED_OPLONG, nvgRGBA{0x69, 0x69, 0x69, 0x0f}},
};

typedef struct UIScene {

  float mpc_x[50];
  float mpc_y[50];

  mat4 extrinsic_matrix;      // Last row is 0 so we can use mat4.
  bool world_objects_visible;

  bool is_rhd;
  bool frontview;
  bool uilayout_sidebarcollapsed;
  // responsive layout
  Rect viz_rect;
  int ui_viz_ro;

  int lead_status;
  float lead_d_rel, lead_y_rel, lead_v_rel;

  int lead_status2;
  float lead_d_rel2, lead_y_rel2, lead_v_rel2;

  std::string alert_text1;
  std::string alert_text2;
  std::string alert_type;
  cereal::ControlsState::AlertSize alert_size;
  // ui add
  bool rightblindspot;
  bool leftblindspot;
  bool leftBlinker;
  bool rightBlinker;
  int blinker_blinkingrate;
  float angleSteers;
  float steerRatio;
  bool brakeLights;
  float angleSteersDes;
  bool steerOverride;
  float output_scale; 
  //float cpu0Temp;
  float maxCpuTemp;
  int batteryPercent;
  bool batteryCharging;
  char batteryStatus[64];
  // ip addr
  //char ipAddr[20];
  

  cereal::HealthData::HwType hwType;
  int satelliteCount;
  NetStatus athenaStatus;

  cereal::ThermalData::Reader thermal;
  cereal::RadarState::LeadData::Reader lead_data[2];
  cereal::ControlsState::Reader controls_state;
  cereal::DriverState::Reader driver_state;
  cereal::DMonitoringState::Reader dmonitoring_state;
  cereal::ModelData::Reader model;
  float left_lane_points[MODEL_PATH_DISTANCE];
  float path_points[MODEL_PATH_DISTANCE];
  float right_lane_points[MODEL_PATH_DISTANCE];
} UIScene;

typedef struct {
  float x, y;
} vertex_data;

typedef struct {
  vertex_data v[MODEL_PATH_MAX_VERTICES_CNT];
  int cnt;
} model_path_vertices_data;

typedef struct {
  vertex_data v[TRACK_POINTS_MAX_CNT];
  int cnt;
} track_vertices_data;


typedef struct UIState {
  // framebuffer
  FramebufferState *fb;
  int fb_w, fb_h;

  // NVG
  NVGcontext *vg;

  // fonts and images
  int font_sans_regular;
  int font_sans_semibold;
  int font_sans_bold;
  int img_wheel;
  int img_turn;
  int img_face;
  int img_brake;
  int img_button_settings;
  int img_button_home;
  int img_battery;
  int img_battery_charging;
  int img_network[6];

  SubMaster *sm;

  Sound *sound;
  UIStatus status;
  UIScene scene;
  cereal::UiLayoutState::App active_app;

  // vision state
  bool vision_connected;
  VisionStream stream;

  // graphics
  GLuint frame_program;
  GLuint frame_texs[UI_BUF_COUNT];
  EGLImageKHR khr[UI_BUF_COUNT];
  void *priv_hnds[UI_BUF_COUNT];

  GLint frame_pos_loc, frame_texcoord_loc;
  GLint frame_texture_loc, frame_transform_loc;
  GLuint frame_vao[2], frame_vbo[2], frame_ibo[2];
  mat4 rear_frame_mat, front_frame_mat;

  // device state
  bool awake;
  float light_sensor;

  bool started;
  bool ignition;
  bool is_metric;
  bool longitudinal_control;
  uint64_t last_athena_ping;
  uint64_t started_frame;

  bool alert_blinked;
  float alert_blinking_alpha;
  bool livempc_or_radarstate_changed;

  track_vertices_data track_vertices[2];
  model_path_vertices_data model_path_vertices[MODEL_LANE_PATH_CNT * 2];
} UIState;

void ui_init(UIState *s);
void ui_update(UIState *s);

int write_param_float(float param, const char* param_name, bool persistent_param = false);
template <class T>
int read_param(T* param, const char *param_name, bool persistent_param = false){
  T param_orig = *param;
  char *value;
  size_t sz;

  int result = read_db_value(param_name, &value, &sz, persistent_param);
  if (result == 0){
    std::string s = std::string(value, sz); // value is not null terminated
    free(value);

    // Parse result
    std::istringstream iss(s);
    iss >> *param;

    // Restore original value if parsing failed
    if (iss.fail()) {
      *param = param_orig;
      result = -1;
    }
  }
  return result;
}
