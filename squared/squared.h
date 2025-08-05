#ifndef SQUARED_H
#define SQUARED_H

typedef struct {
  int size;

  /* Shared-memory buffers */
  unsigned char *observations; // length size*size
  int *actions;
  float *rewards;
  unsigned char *terminals;

  int agent_x, agent_y;
  int target_x, target_y;
  unsigned int rng;
} Squared;

void c_reset(Squared *env);
void c_step(Squared *env);
void c_render(Squared *env);
void c_close(Squared *env);

#endif // SQUARED_H
