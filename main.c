#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SAVE 0
#define CHECK 1
#define WALL '#'
#define SAND 'o'
#define WATER '~'
#define OPEN_SPACE '.'
#define LEVEL_TEST "test.txt"
#define LEVEL_1 "/Users/codecadet/Work/C/FallingSandSim/heart.txt"
#define LEVEL_2 "hi.txt"

typedef struct {
  int y;
  int x;
  int count;
} Spawner;

typedef struct {
  char **map;
  char **second_map;
  int cols;
  int rows;
} Map;

Map *createMap(char *level) {
  Map *newMap = malloc(sizeof(Map));
  printf("allocated map\n");

  FILE *fp = fopen(level, "r");
  if (fp == NULL) {
    printf("Error: File not found\n");
    exit(1);
  }
  int ch;
  int i = 0;
  int j = 0;
  int rows_read = 0;

  while ((ch = fgetc(fp)) != EOF) {
    j++;
    if (ch == '\n') {
      if (rows_read == 0) {
        rows_read = 1;
        newMap->rows = j - 1;
      }
      i++;
      j = 0;
    }
  }
  newMap->cols = i + 1;

  printf("COMPLETE: reading rows and cols from file.\n");

  newMap->map = malloc(sizeof(char *) * newMap->cols);
  newMap->second_map = malloc(sizeof(char *) * newMap->cols);
  printf("COMPLETE: Allocated heights\n");

  for (int i = 0; i < newMap->cols; i++) {
    newMap->map[i] = malloc(sizeof(char) * newMap->rows);
  }
  for (int i = 0; i < newMap->cols; i++) {
    newMap->second_map[i] = malloc(sizeof(char) * newMap->rows);
  }
  printf("COMPLETE: Allocated widths\n");

  fseek(fp, 0, SEEK_SET);
  i = 0;
  j = 0;
  while ((ch = fgetc(fp)) != EOF) {
    newMap->map[i][j] = ch;
    j++;
    if (ch == '\n') {
      j = 0;
      i++;
    }
  }
  printf("COMPLETE: Reading operations.\n");

  fclose(fp);
  return newMap;
}

int drawMap(Map *map) {
  for (int i = 0; i < map->cols; i++) {
    for (int j = 0; j < map->rows; j++) {
      printf("%c", map->map[i][j]);
    }
    printf("\n");
  }
  return 0;
}

int spawn(Map *map, Spawner *spawner, char element) {
  map->map[spawner->y][spawner->x] = element;
  spawner->count++;
  return 0;
}

int check(int op, Map *map) {
  // save the map sand positions
  // check against old positions to verify movement
  int is_diff = 0;
  switch (op) {
  case SAVE:
    for (int i = 0; i < map->cols; i++) {
      for (int j = 0; j < map->rows; j++) {
        map->second_map[i][j] = map->map[i][j];
      }
    }
    break;
  case CHECK:
    for (int i = 0; i < map->cols; i++) {
      for (int j = 0; j < map->rows; j++) {
        if (map->map[i][j] != map->second_map[i][j]) {
          is_diff = 1;
        }
      }
    }
    break;
  default:
    break;
  }
  return is_diff;
}
int logic(Map *map, Spawner *spawner, char element, char *dir) {
  // save
  check(SAVE, map);
  // move sand
  for (int i = map->cols - 1; i >= 0; i--) {
    for (int j = 0; j < map->rows; j++) {
      if (map->map[i][j] == SAND) {
        if (map->map[i + 1][j] == OPEN_SPACE) {
          map->map[i][j] = OPEN_SPACE;
          map->map[i + 1][j] = SAND;
        } else if (map->map[i + 1][j - 1] == OPEN_SPACE) {
          map->map[i][j] = OPEN_SPACE;
          map->map[i + 1][j - 1] = SAND;
        } else if (map->map[i + 1][j + 1] == OPEN_SPACE) {
          map->map[i][j] = OPEN_SPACE;
          map->map[i + 1][j + 1] = SAND;
        }
      } else if (map->map[i][j] == WATER) {
        // down
        if (map->map[i + 1][j] == OPEN_SPACE) {
          map->map[i][j] = OPEN_SPACE;
          map->map[i + 1][j] = WATER;
        } // bottom left
        else if (map->map[i + 1][j - 1] == OPEN_SPACE) {
          map->map[i][j] = OPEN_SPACE;
          map->map[i + 1][j - 1] = WATER;
        } // bottom right
        else if (map->map[i + 1][j + 1] == OPEN_SPACE) {
          map->map[i][j] = OPEN_SPACE;
          map->map[i + 1][j + 1] = WATER;
        } else if (map->map[i][j - 1] == OPEN_SPACE && *dir == 'l') {
          map->map[i][j] = OPEN_SPACE;
          map->map[i][j - 1] = WATER;
        } else if (map->map[i][j + 1] == OPEN_SPACE) {
          *dir = 'r';
          map->map[i][j] = OPEN_SPACE;
          map->map[i][j + 1] = WATER;
        }
      }
    }
  }

  if (map->map[spawner->y][spawner->x] == element) {
    return 1;
  }
  // check and spawn
  if (check(CHECK, map) != 1) {
    *dir = 'l';
    spawn(map, spawner, element);
  }
  return 0;
}

int main(int argc, char **argv) {
  char lvl[15];
  if(argc > 1){
    strcpy(lvl, argv[1]);
  }
  Map *map = createMap(lvl);

  Spawner spawner = {0, 4, 0};
  int next_spawn = 0;
  char dir = 'l';

  // initial present level with delay before starting sim
  system("clear");
  drawMap(map);
  sleep(2);
  system("clear");

  int run = 1;
  while (run) {
    drawMap(map);
    usleep(125000);
    if (next_spawn == 0) {
      if (logic(map, &spawner, SAND, &dir) == 1) {
        spawner.x += 8;
        next_spawn = 1;
      }
    } else if (next_spawn == 1) {
      if (logic(map, &spawner, WATER, &dir) == 1) {
        run = 0;
      }
    }
    system("clear");
  }
  printf("Particles spawned: %d.\n", spawner.count);
  return 0;
}
