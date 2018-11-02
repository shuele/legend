#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define H_POS_OFFSET 4
#define V_POS_OFFSET 6

#define POS_FILE "positions.txt"
#define SAVE_FILE "R1.GRP"
#define POS_NUM_PER_LINE 4

struct pos{
	char name[256];
	int horizontal;
	int vertical;
};

static struct pos pos_map[100];
static int pos_count = 0;
static int max_name_len = 0;

static int setup_pos_map(void)
{
    FILE *fp;
    char buf[256];
    char *token = NULL;
    char *saveptr = NULL;
    char *endptr = NULL;
    int line = 0;
    int index = 0;
    struct pos *p = pos_map;
    int name_len;

    memset( p, 0, sizeof(pos_map) );
    printf("cleared pos_map of size(%lu)\n", sizeof(pos_map));

    fp = fopen(POS_FILE, "r");
    if (fp) {
        while( fgets(buf, sizeof(buf), fp) ) {
			token = strtok_r(buf, ",", &saveptr);
	        if(!token){
	            printf("%s - get palce failed on line (%d):\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        strncpy( p->name, token, sizeof(p->name) );
	        name_len = strlen(p->name);
	        if (name_len > max_name_len)
	        	max_name_len = name_len;

	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get horizontal failed at line (%d)\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        p->horizontal = atoi(token);

	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get vertical failed at line (%d): [%s]\n",
	                    __func__, line, saveptr);
                line++;
	            continue;
	        }
	        p->vertical = atoi(token);

	        line++;
	        p++;
        }
		pos_count = line;
    }
    fclose(fp);

	return 0;	
}

static int print_pos_list(void)
{
    struct pos *p = pos_map;
    int i;
    int len;

    printf("Positions list: (total %d)(max_name_len %d)\n", pos_count, max_name_len);
    printf("------------------------------------------\n");
    for ( i = 0; i < pos_count; i++) {
    	len = strlen(p->name);
    	printf("[%d] [%s] (%d,%d) (0x%x, 0x%x), name_len(%d)\n",
    			 i,
    			 p->name,
    			 p->horizontal,
    			 p->vertical,
    			 p->horizontal,
    			 p->vertical,
    			 len);
    	p++;
    }
    printf("------------------------------------------\n");
    return 0;
}

static int print_pos_menu(void)
{
struct pos *p = pos_map;
    int i;

    printf("Please select postion to go: (total %d)\n", pos_count);
    printf("------------------------------------------\n");
    for ( i = 0; i < pos_count; i++) {
    	printf("[%02d] %-*s ",
    			 i,
    			 max_name_len, p->name);
    	if( i%POS_NUM_PER_LINE == (POS_NUM_PER_LINE - 1) )
    		printf("\n");
    	else
    		printf("\t");
    	p++;
    }
    printf("\n");
    printf("------------------------------------------\n");

    return 0;	
}

static int read_file_pos(void)
{
    FILE *fp;
    short int h_pos = 0;
    short int v_pos = 0;
    int len = 0;

    fp = fopen(SAVE_FILE, "r+");
    if (fp) {
        fseek(fp, H_POS_OFFSET, SEEK_SET);
        len = fread(&h_pos, 1, 2, fp);
        printf("get horizontal, read (%d) bytes: (0x%0x) (%d)\n", len, h_pos, h_pos);

        fseek(fp, V_POS_OFFSET, SEEK_SET);
        len  = fread(&v_pos, 1, 2, fp);
        printf("get vertical, read (%d) bytes: (0x%x) (%d)\n", len, v_pos, v_pos);
    }
    fclose(fp);

    return 0;
}

static int write_pos(short int h_pos, short int v_pos)
{
    FILE *fp;
    int len = 0;

    fp = fopen(SAVE_FILE, "r+");
    if (fp) {
        fseek(fp, H_POS_OFFSET, SEEK_SET);
        len = fwrite(&h_pos, 1, 2, fp);
        printf("write horizontal, write (%d) bytes: (0x%0x) (%d)\n", len, h_pos, h_pos);

        fseek(fp, V_POS_OFFSET, SEEK_SET);
        len  = fwrite(&v_pos, 1, 2, fp);
        printf("get vertical, read (%d) bytes: (0x%x) (%d)\n", len, v_pos, v_pos);
    }
    fclose(fp);

    return 0;
}

static int select_pos(void)
{
    char buf[64];
    int index = -1;

select_pos:
    memset(buf, 0, sizeof(buf));
    printf("Please select: ");
    while( !fgets(buf, sizeof(buf), stdin) || strlen(buf) < 2 )
        printf("Please select: ");

    sscanf(buf, "%d", &index);
    if(index < pos_count) {
        printf("You selected (%02d) %s\n", index, pos_map[index].name);    
    } else {
        printf("Wrong selection, please select again!\n");
        goto select_pos;
    }
    
    return index;
}

static int goto_pos(int index)
{
    write_pos(pos_map[index].horizontal, pos_map[index].vertical);
    return 0;
}

int main()
{
    int pos_index = -1;

	setup_pos_map();
#ifdef DEBUG
    print_pos_list();
#endif
    print_pos_menu();

    pos_index = select_pos();
    if(pos_index < 0)
        printf("selection wrong, abort\n");

    goto_pos(pos_index);

    return 0;
}
