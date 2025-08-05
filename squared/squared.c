#include "squared.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

const unsigned char NOOP = 0;
const unsigned char UP = 1;
const unsigned char DOWN = 2;
const unsigned char LEFT = 3;
const unsigned char RIGHT = 4;

const unsigned char EMPTY = 0;
const unsigned char AGENT = 1;
const unsigned char TARGET = 2;

void c_reset(Squared *env) {
  int N = env->size;

  // Clear observations
  memset(env->observations, 0, N * N * sizeof(char));

  // Put agent in the middle
  env->agent_x = N / 2;
  env->agent_y = N / 2;

  // Put target in a random position
  do {
    env->target_x = rand() % N;
    env->target_y = rand() % N;
  } while (env->target_x == env->agent_x && env->target_y == env->agent_y);

  // Place agent and target on observations
  env->observations[env->agent_y * N + env->agent_x] = AGENT;
  env->observations[env->target_y * N + env->target_x] = TARGET;
}

void c_step(Squared *env) {
  int N = env->size;
  int action = env->actions[0];

  // Clear agent from observations
  env->observations[env->agent_y * N + env->agent_x] = EMPTY;

  // Update agent
  if (action == UP)
    env->agent_y--;
  if (action == DOWN)
    env->agent_y++;
  if (action == LEFT)
    env->agent_x--;
  if (action == RIGHT)
    env->agent_x++;

  // Check if not in bounds
  if (env->agent_y < 0 || env->agent_y >= N || env->agent_x < 0 ||
      env->agent_x >= N) {
    env->rewards[0] = -1.0;
    env->terminals[0] = 1;
    c_reset(env);
    return;
  }

  // Check if agent == target
  if (env->agent_x == env->target_x && env->agent_y == env->target_y) {
    env->rewards[0] = 1.0;
    env->terminals[0] = 1;
    c_reset(env);
    return;
  }

  env->rewards[0] = 0;
  env->terminals[0] = 0;

  env->observations[env->agent_y * N + env->agent_x] = AGENT;
}

void c_render(Squared *env) {
  if (!IsWindowReady()) {
    InitWindow(64 * env->size, 64 * env->size, "PufferLib Squared");
    SetTargetFPS(5);
  }

  // Standard across our envs so exiting is always the same
  if (IsKeyDown(KEY_ESCAPE)) {
    exit(0);
  }

  BeginDrawing();
  ClearBackground((Color){6, 24, 24, 255});

  int px = 64;
  for (int i = 0; i < env->size; i++) {
    for (int j = 0; j < env->size; j++) {
      int tex = env->observations[i * env->size + j];
      if (tex == EMPTY) {
        continue;
      }
      Color color =
          (tex == AGENT) ? (Color){0, 187, 187, 255} : (Color){187, 0, 0, 255};
      DrawRectangle(j * px, i * px, px, px, color);
    }
  }

  EndDrawing();
}

// Required function. Should clean up anything you allocated
// Do not free env->observations, actions, rewards, terminals
void c_close(Squared *env) {
  if (IsWindowReady()) {
    CloseWindow();
  }
}

int main() {
  Squared env = {.size = 11};
  env.observations =
      (unsigned char *)calloc(env.size * env.size, sizeof(unsigned char));
  env.actions = (int *)calloc(1, sizeof(int));
  env.rewards = (float *)calloc(1, sizeof(float));
  env.terminals = (unsigned char *)calloc(1, sizeof(unsigned char));

  c_reset(&env);
  c_render(&env);
  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      env.actions[0] = 0;
      if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        env.actions[0] = UP;
      if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        env.actions[0] = DOWN;
      if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        env.actions[0] = LEFT;
      if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        env.actions[0] = RIGHT;
    } else {
      env.actions[0] = rand() % 5;
    }
    c_step(&env);
    c_render(&env);
  }
  free(env.observations);
  free(env.actions);
  free(env.rewards);
  free(env.terminals);
  c_close(&env);
}
