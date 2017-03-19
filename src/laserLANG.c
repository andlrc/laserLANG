#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdbool.h>

#define PROGRAM_NAME "laserLANG"
#define MAP_MAX_X 256
#define MAP_MAX_Y 256
#define CELLS_MAX 256

extern void setbuffer(FILE * stream, char *buf, size_t size);

enum MOVEMENT {
	up = 0,
	right,
	down,
	left
};

static int skip_comment(FILE * fh)
{
	char c = '0';
	while (c != '\n') {
		c = fgetc(fh);
		if (c == EOF)
			return 1;
	}

	return 0;
}

static int read_map(FILE * fh, char map[][MAP_MAX_X])
{
	uint8_t map_x = 0;
	uint8_t map_y = 0;

	for (;;) {
		char c = fgetc(fh);

		if (map_x == 0 && c == '#') {	/* Skip shebang, and lines
						 * starting with # */
			if (skip_comment(fh) != 0)
				break;
			map_x = 0;
			continue;
		}

		if (c == EOF)
			break;

		if (c == '\n') {
			map_x = 0;
			if (map_y == MAP_MAX_Y - 1)	/* Cannot increase y */
				return 1;
			map_y++;
			continue;
		}

		map[map_y][map_x] = c;

		if (map_x == MAP_MAX_X - 1)	/* Cannot increase x */
			return 1;
		map_x++;
	}

	return 0;
}

static int printd(enum MOVEMENT move,
		  uint8_t pos_y, uint8_t pos_x,
		  char map[][MAP_MAX_X], int32_t cells[CELLS_MAX])
{	/* Print debug information about  cells and laser position */
	char *out = malloc(2048);
	char *pout = out;

	uint8_t last_used = CELLS_MAX - 1;	/* Stupid way to trim off
						 * trailing zeros */
	while (last_used > 0 && cells[--last_used] == 0);

	for (int i = 0; i <= last_used; i++) {
		if (i > 0) {
			pout[0] = ',';
			pout++;
		}
		sprintf(pout, "%d", cells[i]);
		pout += strlen(pout);
	}

	fprintf(stderr, "move = %d, xy = {%d, %d}, char = %c, cells = {%s}\n",
		move, pos_x, pos_y, map[pos_y][pos_x], out);

	free(out);

	return 0;
}

static int shoot_laser(char map[][MAP_MAX_X])
{
	uint8_t pos_x = 0;
	uint8_t pos_y = 0;
	enum MOVEMENT move = right;

	int32_t cells[CELLS_MAX] = { 0 };
	int32_t *cellp = cells;

	for (;;) {
		char c = map[pos_y][pos_x];

		switch (c) {
		case '>':	/* Move memory cell right */
			if (move == left)	/* Cannot come from left */
				return 0;

			cellp++;
			move = right;
			break;

		case '<':	/* Move memory cell left */
			if (move == right)	/* Cannot come from right */
				return 0;

			cellp--;
			move = left;
			break;

		case '-':	/* Add one to memory cell */
			if (move != left && move != right)	/* Can only
								 * access on x
								 * axis */
				return 0;

			++*cellp;
			break;

		case '\'':	/* Subtract one to memory cell */
			if (move != up && move != down)	/* Can only access on y
							 * axis */
				return 0;

			--*cellp;
			break;

		case '=':	/* Output ascii character from current memory
				 * cell */
			if (move != left && move != right)	/* Can only
								 * access on y
								 * axis */
				return 0;

			printf("%c", *cellp);
			break;

		case '"':	/* Output numeric value from current cell */
			if (move != up && move != down)	/* Can only access on x
							 * axis */
				return 0;

			printf("%d", *cellp);
			break;

		case '+':	/* Turn 90 deg clockwise when current memory
				 * cell is positive, 90 deg counter clockwise
				 * when negative and go straight when zero
				 */
			if (*cellp > 0) {	/* Clockwise */
				move++;
				if (move > 3)
					move = 0;
			} else if (*cellp < 0) {	/* Counter Clockwise */
				move--;
				if (move < 0)
					move = 3;
			}
			break;

		case '#':	/* Turn 90 deg counter clockwise when current
				 * memory cell is positive, * 90 deg clockwise
				 * when negative and go straight when zero */
			if (*cellp > 0) {	/* Counter Clockwise */
				move--;
				if (move < 0)
					move = 3;
			} else if (*cellp < 0) {	/* Clockwise */
				move++;
				if (move > 3)
					move = 0;
			}
			break;

		case '\\':	/* Turn 90 deg depending on input direction */
			switch (move) {
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

		case '/':	/* Turn 90 deg depending on input direction */
			switch (move) {
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

		case 'v':	/* Make left and right go down, but fail when
				 * going up */
			if (move == up)	/* Cannot come from below */
				return 0;
			move = down;
			break;

		case '^':	/* Make left and right go up, but fail when
				 * going down */
			if (move == down)	/* Cannot come from above */
				return 0;
			move = up;
			break;

		case 'p':	/* Debug */
			printd(move, pos_x, pos_y, map, cells);
			break;

		case '~':	/* Read ASCII input character */
			if (move == up)	/* Turn left */
				move = left;
			else if (move == down)	/* Turn right */
				move = right;

			if ((*cellp = fgetc(stdin)) == EOF)
				return 1;
			break;

		case 's':	/* Read ASCII input character */
			if (move == left)	/* Turn up */
				move = up;
			else if (move == right)	/* Turn down */
				move = down;

			if ((*cellp = fgetc(stdin)) == EOF)
				return 1;
			break;
		}

		switch (move) {
		case up:
			if (pos_y == 0)
				return 0;
			pos_y--;
			break;
		case right:
			if (pos_x == MAP_MAX_X - 1)
				return 0;
			pos_x++;
			break;
		case down:
			if (pos_y == MAP_MAX_Y - 1)
				return 0;
			pos_y++;
			break;
		case left:
			if (pos_x == 0)
				return 0;
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

	setbuffer(stdout, NULL, 0);	/* Disable output buffering */

	if (argc > 1) {
		src_file = argv[1];
		fh = fopen(src_file, "r");
	} else {
		fh = stdin;
	}

	if (fh == NULL) {
		fprintf(stderr, "%s: cannot access '%s'\n", PROGRAM_NAME,
			src_file);
		return 1;
	}

	char map[MAP_MAX_Y][MAP_MAX_X];
	int rc = read_map(fh, map);
	fclose(fh);
	if (rc != 0) {
		fprintf(stderr, "%s: error in program\n", PROGRAM_NAME);
		return rc;
	}

	return shoot_laser(map);
}
