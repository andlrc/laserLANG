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

    if (map_x == 0 && c == '#') /* Skip shebang, and lines starting with # */
    {
      if (skip_comment(fh, map, map_y) != 0) break;
      map_x = 0;
      continue;
    }

    if (c == EOF) break;

    if (c == '\n')
    {
      map_x = 0;
      if (map_y == MAP_MAX_Y - 1) /* Cannot increase y */
	return 1;
      map_y++;
      continue;
    }

    map[map_y][map_x] = c;

    if (map_x == MAP_MAX_X - 1) /* Cannot increase x */
      return 1;
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
      case '>': /* Move memory cell right */
	if (move == left) /* Cannot come from left */
	  return 0;

	cellp++;
	move = right;
	break;

      case '<': /* Move memory cell left */
	if (move == right) /* Cannot come from right */
	  return 0;

	cellp--;
	move = left;
	break;

      case '-': /* Add one to memory cell */
	if (move != left && move != right) /* Can only access on x axis */
	  return 0;

	++*cellp;
	break;

      case '\'': /* Subtract one to memory cell */
	if (move != up && move != down) /* Can only access on y axis */
	  return 0;

	--*cellp;
	break;

      case '=': /* Output ascii character from current memory cell */
	if (move != left && move != right) /* Can only access on y axis */
	  return 0;

	printf("%c", *cellp);
	break;

      case '"': /* Output numeric value from current memory cell */
	if (move != up && move != down) /* Can only access on x axis */
	  return 0;

	printf("%d", *cellp);
	break;

      case '+': /* Turn 90 deg clockwise when current memory cell is positive,
		 * 90 deg counter clockwise when negative and go straight when
		 * zero
		 */
	if (*cellp > 0) /* Clockwise */
	{
	  move++;
	  if (move > 3) move = 0;
	}
	else if (*cellp < 0) /* Counter Clockwise */
	{
	  move--;
	  if (move < 0) move = 3;
	}
	break;

      case '#': /* Turn 90 deg counter clockwise when current memory cell is
		 * positive, * 90 deg clockwise when negative and go straight
		 * when zero
		 */
	if (*cellp > 0) /* Counter Clockwise */
	{
	  move--;
	  if (move < 0) move = 3;
	}
	else if (*cellp < 0) /* Clockwise */
	{
	  move++;
	  if (move > 3) move = 0;
	}
	break;

      case '\\': /* Turn 90 deg depending on input direction */
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

      case '/': /* Turn 90 deg depending on input direction */
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

      case 'v': /* Make left and right go down, but fail when going up */
	if (move == up) /* Cannot come from below */
	  return 0;
	move = down;
	break;

      case '^': /* Make left and right go up, but fail when going down */
	if (move == down) /* Cannot come from above */
	  return 0;
	move = up;
	break;

      case 'p': /* Debug */
	printf("d(%d) xy(%d, %d) near(%d, %d, %d, %d, %d) p(%d)\n",
	    move,
	    pos_x, pos_y,
	    *(cellp - 2), /* TODO: Can be out of bound */
	    *(cellp - 1), /* Can be out of bound */
	    *cellp,
	    *(cellp + 1), /* Can be out of bound */
	    *(cellp + 2), /* Can be out of bound */
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

  setbuffer(stdout, NULL, 0); /* Disable output buffering */

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
