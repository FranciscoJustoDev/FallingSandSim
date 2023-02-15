#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 10
#define COLS 10
#define WALL '#'
#define SAND 'o'
#define OPEN_SPACE '.'

typedef struct {
  int y;
  int x;
  int count;
} Spawner;

char **createMap() {
  // alloc memory
  char **map = malloc(sizeof(char *) * COLS);
  for (int i = 0; i < COLS; i++) {
    map[i] = malloc(sizeof(char) * ROWS);
  }
  // load level from file
  FILE *fp = fopen("test_level.txt", "r");
  if (fp == NULL) {
    printf("Error: File not found\n");
    exit(1);
  }
  int ch;
  int i = 0;
  int j = 0;
  while ((ch = fgetc(fp)) != EOF) {
    map[i][j] = ch;
    j++;
    if (ch == '\n') {
      i++;
      j = 0;
    }
  }
  free(fp);
  return map;
}

int drawMap(char **map) {
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      printf("%c", map[i][j]);
    }
    printf("\n");
  }
  return 0;
}

int spawn(char **map, Spawner *spawner) {
  map[spawner->y][spawner->x] = SAND;
  spawner->count++;
  return 0;
}

int logic(char **map, Spawner *spawner) {
  for (int i = 0; i < COLS; i++) {
    for (int j = 0; j < ROWS; j++) {
      if (map[i][j] == SAND) {
        if (map[i + 1][j] == OPEN_SPACE) {
          map[i][j] = OPEN_SPACE;
          map[i + 1][j] = SAND;
          return 0;
        } else if (map[i + 1][j - 1] == OPEN_SPACE) {
          map[i][j] = OPEN_SPACE;
          map[i + 1][j - 1] = SAND;
          return 0;
        } else if (map[i + 1][j + 1] == OPEN_SPACE) {
          map[i][j] = OPEN_SPACE;
          map[i + 1][j + 1] = SAND;
          return 0;
        }
      }
    }
  }
  if (map[spawner->y][spawner->x] == SAND) {
    return 1;
  }
  spawn(map, spawner);
  return 0;
}

int main() {
  char **map = createMap();
  Spawner spawner = {0, 4, 0};

  int run = 1;
  while (run) {
    drawMap(map);
    sleep(1);
    if (logic(map, &spawner) == 1)
      run = 0;
    system("clear");
  }
  printf("Sand grains spawned: %d.\n", spawner.count);
  return 0;
}
