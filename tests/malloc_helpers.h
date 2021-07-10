/* In dieser Datei sind nützliche Hilfsfunktionen zum testen der malloc
 * Implementierung. Zur Nutzung dieser Funktionen können die bereits
 * mitgelieferten Tests als Beispiel hergenommen werden
 * Auflistung der Funktionen:
 * void mem_print_blocks(void * mem)
 * unsigned int mem_block_count(void * mem)
 * size_t mem_size_total(void * mem)
 * size_t mem_size_used(void * mem)
 * size_t mem_size_free(void * mem)
 * char * mem_concat_data(void * mem)
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <os_malloc.h>

static const char * debug_fmt = ""
	"| Block at:     %2$*1$p |\n"
	"|  -  -  -  -  -  -  -  -  -  -  |\n"
	"| Next at:      %3$*1$p |\n"
	"| Availability: %4$*1$s |\n"
	"| Size:         %5$*1$zd |\n";

static const char * debug_fmt_data = ""
	"| Data (first 8 Bytes):          |\n";

#define pretty(X) (isprint(X) ? X : '_')
static const char * debug_fmt_data_ascii = ""
	"|   ASCII : %1$c.%2$c.%3$c.%4$c.%5$c.%6$c.%7$c.%8$c      |\n";

#define byte_hex(X) (X & 0xff)
static const char * debug_fmt_data_hex = ""
	"|   HEX   : 0x%1$02x 0x%2$02x 0x%3$02x 0x%4$02x  |\n"
	"|           0x%5$02x 0x%6$02x 0x%7$02x 0x%8$02x  |\n";

static const char * debug_fmt_line = ""
	"+--------------------------------+\n";

/* Iteriert über alle Blöcke und gibt deren Information in der
 * auftretenden Reihenfolge aus
 */
void mem_print_blocks(void * mem) {
	mem_block * curr = mem;

	printf("%s", "--- Beginning of memory layout ---\n");

	while(curr != NULL) {
		printf("%s", debug_fmt_line);
		printf(
			debug_fmt,
			16,
			curr,
			curr->next,
			curr->free ? "free" : "used",
			curr->size);
		if(!curr->free) {
			char * data = (char *)(curr + 1);
			printf("%s", debug_fmt_data);
			printf(debug_fmt_data_ascii,
				pretty(data[0]), pretty(data[1]),
				pretty(data[2]), pretty(data[3]),
				pretty(data[4]), pretty(data[5]),
				pretty(data[6]), pretty(data[7]));
			printf(debug_fmt_data_hex,
				byte_hex(data[0]), byte_hex(data[1]),
				byte_hex(data[2]), byte_hex(data[3]),
				byte_hex(data[4]), byte_hex(data[5]),
				byte_hex(data[6]), byte_hex(data[7]));
		}

		curr = curr->next;
	}
	printf("%s", debug_fmt_line);

	printf("%s", "------ End of memory layout ------\n");
}

/* Iteriert über alle Blöcke und gibt zurück wie viele gefunden wurden */
unsigned int mem_block_count(void * mem) {
	mem_block * curr = mem;
	unsigned int total = 0;
	while(curr != NULL) {
		total++;
		curr = curr->next;
	}
	return total;
}

/* Iteriert über alle Blöcke und summiert die verwaltet Größe von jedem Block
 * Dies resultierende Summe wird zurück gegeben. Diese sollte gleich der
 * ursprünglichen Größe sein die bei os_init(...) übergeben wurde
 */
size_t mem_size_total(void * mem) {
	mem_block * curr = mem;
	size_t total = 0;
	while(curr != NULL) {
		total += curr->size;
		curr = curr->next;
	}
	return total;
}


/* Iteriert über alle Blöcke und summiert die verwaltet Größe von allen
 * belegten Blöcken zusammen
 */
size_t mem_size_used(void * mem) {
	mem_block * curr = mem;
	size_t total = 0;
	while(curr != NULL) {
		if(!curr->free) {
			total += curr->size;
		}
		curr = curr->next;
	}
	return total;
}

/* Iteriert über alle Blöcke und summiert die verwaltet Größe von allen
 * freien Blöcken zusammen
 */
size_t mem_size_free(void * mem) {
	mem_block * curr = mem;
	size_t total = 0;
	while(curr != NULL) {
		if(curr->free) {
			total += curr->size;
		}
		curr = curr->next;
	}
	return total;
}

/*
* Diese Funktion sollte nur benutzt werden, wenn als Daten ausschließlich
* 0-terminierte Strings benutzt werden.
* Diese Funktion iteriert über all Blöcke und verkettet diese in der
* auftretenden Reihenfolge. Freie Blöcke werden ignoriert
* Beispiel:
* +-------------+  +-------------+  +-------------+
* | Block: 1    |->| Block: 2    |->| Block: 3    |
* | Daten: AAA  |  | Daten: CC   |  | Daten: BBBB |
* +-------------+  +-------------+  +-------------+
* Ergebnis: "AACCBBBB"
*/
char * mem_concat_data(void * mem) {
	mem_block * curr = mem;
	char * concat = malloc(sizeof(char));
	size_t concat_len = sizeof(char);

	if(concat == NULL) {
		fprintf(stderr, "Failed to alloc memory for test function\n");
		exit(1);
	}
	*concat = '\0';

	while(curr != NULL) {
		if(curr->free) {
			curr = curr->next;
			continue;
		}
		char * data = (char *)(curr + 1);
		size_t data_len = strlen(data);
		if(data_len) {
			concat_len += data_len;
			concat = realloc(concat, concat_len);
			if(concat == NULL) {
				fprintf(stderr, "Failed to realloc memory for test function\n");
				exit(1);
			}
			strcat(concat, data);
		}
		curr = curr->next;
	}

	return concat;
}
