#include <stdio.h>
#include <inttypes.h>
#include <fcntl.h>

#define PROGRAM_NAME "laserLANG"
#define MAP_MAX_X 256
#define MAP_MAX_Y 256
#define CELLS_MAX 256

enum MOVEMENT
{
  up = 0,
  right,
  down,
  left
};

int skip_comment(FILE *fh, char map[][MAP_MAX_X], uint8_t map_y)
{
  char c = '0';
  while (c != '\n')
  {
    c = fgetc(fh);
    if (c == EOF) return 1;
  }

  return 0;
}

int read_map(FILE *fh, char map[][MAP_MAX_X])
{
  uint8_t map_x = 0;
  uint8_t map_y = 0;

  for (;;)
  {
    char c = fgetc(fh);

    /* Skip shebang, and lines starting with # */
    if (map_x == 0 && c == '#')
    {
      if (skip_comment(fh, map, map_y) != 0) break;
      map_x = 0;
      continue;
    }

    if (c == EOF) break;

    if (c == '\n')
    {
      map_x = 0;
      /* Cannot increase y */
      if (map_y == MAP_MAX_Y - 1) return 1;
      map_y++;
      continue;
    }

    map[map_y][map_x] = c;

    /* Cannot increase x */
      if (map_x == MAP_MAX_X - 1) return 1;
    map_x++;
  }

  return 0;
}

int shoot_laser(char map[][MAP_MAX_X])
{
  uint8_t pos_x = 0;
  uint8_t pos_y = 0;
  enum MOVEMENT move = right;

  int8_t cells[CELLS_MAX] = {0};
  char *cellp = cells;

  for (;;)
  {
    char c = map[pos_y][pos_x];

    switch (c)
    {
      /* Move memory cell right */
      case '>':
	/* Cannot come from left */
	if (move == left) return 0;

	cellp++;
	move = right;
	break;

      /* Move memory cell left */
      case '<':
	/* Cannot come from right */
	if (move == right) return 0;

	cellp--;
	move = left;
	break;

      /* Add one to memory cell */
      case '-':
	/* Can only access on x axis */
	if (move != left && move != right) return 0;

	++*cellp;
	break;

      /* Subtract one to memory cell */
      case '\'':
	/* Can only access on y axis */
	if (move != up && move != down) return 0;

	--*cellp;
	break;

      /* Output ascii character from current memory cell */
      case '=':
	/* Can only access on y axis */
	if (move != left && move != right) return 0;

	printf("%c", *cellp);
	break;

      /* Output numeric value from current memory cell */
      case '"':
	/* Can only access on x axis */
	if (move != up && move != down) return 0;

	printf("%d", *cellp);
	break;

      /* Turn 90 deg clockwise when current memory cell is positive,
       * 90 deg counter clockwise when negative
       * and go straight when zero
       */
      case '+':
	/* Clockwise */
	if (*cellp > 0)
	{
	  move++;
	  if (move > 3) move = 0;
	}
	/* Counter Clockwise */
	else if (*cellp < 0)
	{
	  move--;
	  if (move < 0) move = 3;
	}
	break;

      /* Turn 90 deg counter clockwise when current memory cell is positive,
       * 90 deg clockwise when negative
       * and go straight when zero
       */
      case '#':
	/* Counter Clockwise */
	if (*cellp > 0)
	{
	  move--;
	  if (move < 0) move = 3;
	}
	/* Clockwise */
	else if (*cellp < 0)
	{
	  move++;
	  if (move > 3) move = 0;
	}
	break;

      /* Turn 90 deg depending on input direction */
      case '\\':
	switch (move)
	{
	  case up:
	    move = left;
	    break;
	  case right:
	    move = down;
	    break;
	  case down:
	    move = right;
	    break;
	  case left:
	    move = up;
	    break;
	}
	break;

      /* Turn 90 deg depending on input direction
       * Could be written as:
       * move += move & 1 ? -1 : 1;
       */
      case '/':
	switch (move)
	{
	  case up:
	    move = right;
	    break;
	  case right:
	    move = up;
	    break;
	  case down:
	    move = left;
	    break;
	  case left:
	    move = down;
	    break;
	}
	break;

      /* Make left and right go down, but fail when going up */
      case 'v':
	/* Cannot come from below */
	if (move == up) return 0;
	move = down;
	break;

      /* Make left and right go up, but fail when going down */
      case '^':
	/* Cannot come from above */
	if (move == down) return 0;
	move = up;
	break;

      /* Debug */
      case 'p':
	printf("d(%d) xy(%d, %d) near(%d, %d, %d, %d, %d) p(%d)\n",
	       move,
	       pos_x, pos_y,
	       *(cellp - 2),
	       *(cellp - 1),
	       *cellp,
	       *(cellp + 1),
	       *(cellp + 2),
	       cellp);
	break;
    }

    switch (move)
    {
      case up:
	if (pos_y == 0) return 0;
	pos_y--;
	break;
      case right:
	if (pos_x == MAP_MAX_X - 1) return 0;
	pos_x++;
	break;
      case down:
	if (pos_y == MAP_MAX_Y - 1) return 0;
	pos_y++;
	break;
      case left:
	if (pos_x == 0) return 0;
	pos_x--;
	break;
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  char *src_file;
  FILE *fh;

  /* Disable output buffering */
  setbuffer(stdout, NULL, 0);

  if (argc > 1)
  {
    src_file = argv[1];
    fh = fopen(src_file, "r");
  }
  else
  {
    fh = stdin;
  }

  if (fh == NULL) {
    fprintf(stderr, "%s: cannot access '%s'\n", PROGRAM_NAME, src_file);
    return 1;
  }

  /* Read map from input file */
  char map[MAP_MAX_Y][MAP_MAX_X];
  int rc = read_map(fh, map);
  fclose(fh);
  if (rc != 0)
  {
    fprintf(stderr, "%s: error in program\n", PROGRAM_NAME);
    return rc;
  }

  shoot_laser(map);
}
