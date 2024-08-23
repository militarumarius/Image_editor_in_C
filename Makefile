build:
	gcc image_editor.c -lm -Wall -Wextra -std=c99 -o image_editor
clean:
	rm image_editor
