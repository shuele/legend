#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PERSON_FILE "persons.txt"
#define KUNGFU_FILE "kungfu.txt"
#define ITEM_FILE "item.txt"
#define SAVE_FILE "R1.GRP"
#define POS_NUM_PER_LINE 4
#define PERSON_MAX_COUT 100
#define KUNGFU_MAX_COUNT 100
#define ITEM_MAX_COUNT 2000

struct person_addr {
	short int id;
	char name[32];
	int offset;
};

struct kungfu {
	short int id;
	char name[32];
};

struct item {
	short int id;
	char name[32];
	char type;
};

struct person_parameters {
	short int id;
	short int tmp1[14];
	short int person_level;
	unsigned short int person_experience;
	short int life_value;
	short int life_max;
	short int tmp2[2];
	short int strength;
	short int tmp3;
	short int armed_weapon;
	short int armed_armor;
	short int tmp4[15];
	short int energy_type; /* 0 - negative; 1 - positive; 2 - mixed */
	short int energy_value;
	short int energy_max;
	short int attack_value;
	short int speed_cap;
	short int defence_value;
	short int medical_cap;
	short int poison_cap;
	short int depoison_cap;
	short int antitoxic_cap;
	short int box_cap;
	short int sword_cap;
	short int knife_cap;
	short int special_weapon_cap;
	short int hidden_weapon_cap;
	short int kungfu_knowledge;
	short int moral_value;
	short int kungfu_posion_effect;
	short int double_attack;
	short int prestige_value;
	short int aptitude;
	short int practice_item;
	short int practice_experience;
	short int kungfu_name[10];
	short int kungfu_level[10];
	short int tmp6[5];
};

static struct kungfu kungfu_map[KUNGFU_MAX_COUNT];
static int kungfu_count = 0;
static int max_kungfu_name_len = 0;

static struct item item_map[ITEM_MAX_COUNT];
static int item_count = 0;
static int max_item_name_len = 0;

static struct person_addr person_map[PERSON_MAX_COUT];
static int person_count = 0;
static int max_person_name_len = 0;
static int oper_person_index = -1;
struct person_parameters oper_person;

static int get_input_int(int min, int max)
{
    char buf[64];
    int value = -1;

input_value:
    memset(buf, 0, sizeof(buf));
    printf("Please input (%d ~ %d): ", min, max);
    while( !fgets(buf, sizeof(buf), stdin) || strlen(buf) < 2 )
        printf("Please input (%d ~ %d): ", min, max);

    sscanf(buf, "%d", &value);
    if(value < min || value > max) {
    	printf("Invalid input (%d), please input again\n", value);
    	goto input_value;
    }

    return value;
}

static int setup_kungfu_map(void)
{
    FILE *fp;
    char buf[256];
    char *token = NULL;
    char *saveptr = NULL;
    char *endptr = NULL;
    int line = 0;
    int index = 0;
    struct kungfu *p = kungfu_map;
    int name_len;

    memset( p, 0, sizeof(kungfu_map) );
    fp = fopen(KUNGFU_FILE, "r");
    if (fp) {
        while( fgets(buf, sizeof(buf), fp) ) {
			token = strtok_r(buf, ",", &saveptr);
	        if(!token){
	            printf("%s - get Kungfu Id failed on line (%d):\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        p->id = strtol(token, NULL, 16);

	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get Kungfu Name failed at line (%d)\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        strncpy( p->name, token, sizeof(p->name) );
	        name_len = strlen(p->name);
	        while ( p->name[name_len - 1] == '\r' || p->name[name_len - 1] == '\n') {
	        	p->name[name_len - 1] = 0;
	        	name_len--;
	        }

	        if (name_len > max_kungfu_name_len)
	        	max_kungfu_name_len = name_len;

	        line++;
	        p++;
        }
		kungfu_count = line;
    }
    fclose(fp);

	return 0;
}

static int setup_item_map(void)
{
	FILE *fp;
    char buf[256];
    char *token = NULL;
    char *saveptr = NULL;
    char *endptr = NULL;
    int line = 0;
    int index = 0;
    struct item *p = item_map;
    int name_len;

    memset( p, 0, sizeof(item_map) );
    printf("cleared person_map of size(%lu)\n", sizeof(person_map));

    fp = fopen(ITEM_FILE, "r");
    if (fp) {
        while( fgets(buf, sizeof(buf), fp) ) {
			token = strtok_r(buf, ",", &saveptr);
	        if(!token){
	            printf("%s - get item id failed on line (%d):\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        p->id = strtol(token, NULL, 16);

	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get item name failed at line (%d)\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        strncpy( p->name, token, sizeof(p->name) );
	        name_len = strlen(p->name);
			while ( p->name[name_len - 1] == '\r' || p->name[name_len - 1] == '\n') {
	        	p->name[name_len - 1] = 0;
	        	name_len--;
	        }
	        if (name_len > max_item_name_len)
	        	max_item_name_len = name_len;

#if 0
	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get item type failed at line (%d): [%s]\n",
	                    __func__, line, saveptr);
                line++;
	            continue;
	        }
	        p->type = atoi(token);
#endif
	        line++;
	        p++;
        }
		item_count = line;
    }
    fclose(fp);

	return 0;
}

static int setup_person_map(void)
{
    FILE *fp;
    char buf[256];
    char *token = NULL;
    char *saveptr = NULL;
    char *endptr = NULL;
    int line = 0;
    int index = 0;
    struct person_addr *p = person_map;
    int name_len;

    memset( p, 0, sizeof(person_map) );
    printf("cleared person_map of size(%lu)\n", sizeof(person_map));

    fp = fopen(PERSON_FILE, "r");
    if (fp) {
        while( fgets(buf, sizeof(buf), fp) ) {
			token = strtok_r(buf, ",", &saveptr);
	        if(!token){
	            printf("%s - get person id failed on line (%d):\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        p->id = strtol(token, NULL, 16);

	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get person name failed at line (%d)\n",
	                    __func__, line);
                line++;
	            continue;
	        }
	        strncpy( p->name, token, sizeof(p->name) );
	        name_len = strlen(p->name);
	        if (name_len > max_person_name_len)
	        	max_person_name_len = name_len;

	        token = strtok_r(NULL, ",", &saveptr);
	        if(!token){
	            printf("%s - get address offset failed at line (%d): [%s]\n",
	                    __func__, line, saveptr);
                line++;
	            continue;
	        }
	        p->offset = atoi(token);

	        line++;
	        p++;
        }
		person_count = line;
    }
    fclose(fp);

	return 0;
}

static int print_person_list(void)
{
    struct person_addr *p = person_map;
    int i;
    int len;

    printf("Person list: (total %d)(max_person_name_len %d)\n", person_count, max_person_name_len);
    printf("------------------------------------------\n");
    for ( i = 0; i < person_count; i++) {
    	len = strlen(p->name);
    	printf("(%d) ID(0x%02X) Name[%s] Offset(%d) (0x%x), name_len(%d)\n",
    			 i,
    			 p->id,
    			 p->name,
    			 p->offset,
    			 p->offset,
    			 len);
    	p++;
    }
    printf("------------------------------------------\n");
    return 0;
}

static int print_person_menu(void)
{
struct person_addr *p = person_map;
    int i;

    printf("请选择人物：(总计 %d 人)\n", person_count);
    printf("Please select person: (total %d)\n", person_count);
    printf("------------------------------------------\n");
    for ( i = 0; i < person_count; i++) {
    	printf("(%d) [0x%02X] %-*s ",
    			 i,
    			 p->id,
    			 max_person_name_len, p->name);
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

static int select_person(void)
{
    char buf[64];
    int index = -1;

select_person:
    memset(buf, 0, sizeof(buf));
    printf("Please select: ");
    while( !fgets(buf, sizeof(buf), stdin) || strlen(buf) < 2 )
        printf("Please select: ");

    sscanf(buf, "%d", &index);
    if(index < person_count) {
        printf("You selected (%02d) %s\n", index, person_map[index].name);
    } else {
        printf("Wrong selection, please select again!\n");
        goto select_person;
    }

    return index;
}

static void print_item_name(short int item_id)
{
	int i = 0;
	for ( i = 0; i < item_count; i ++) {
		if (item_map[i].id == item_id) {
			printf("%-*s", max_item_name_len, item_map[i].name);
			return;
		}
		continue;
	}
	printf("Unknow Item\n");
	return;
}

static void print_kungfu_name(short int kungfu_id)
{
	int i = 0;
	for ( i = 0; i < kungfu_count; i ++) {
		if (kungfu_map[i].id == kungfu_id) {
			printf("%-*s", max_kungfu_name_len, kungfu_map[i].name);
			return;
		}
		continue;
	}
	printf("Unknow Kungfu\n");
	return;
}

static int get_person_param(void)
{
    FILE *fp;
    int len = 0;
    int ret = -1;

    fp = fopen(SAVE_FILE, "r");
    if (!fp) {
    	printf("Open file [%s] failed\n", SAVE_FILE);
    	return ret;
    }

    fseek(fp, person_map[oper_person_index].offset, SEEK_SET);
    len = fread(&oper_person, 1, sizeof(oper_person), fp);
    if (len <= 0) {
    	printf("read person param failed.\n");
    	ret = -1;
    } else {
    	ret = 0;
    }
    fclose(fp);
    return ret;
}

static int set_person_param(void)
{
    FILE *fp;
    int len = 0;
    int ret = -1;

    fp = fopen(SAVE_FILE, "r+");
    if (!fp) {
    	printf("Open file [%s] failed\n", SAVE_FILE);
    	return ret;
    }

    fseek(fp, person_map[oper_person_index].offset, SEEK_SET);
    len = fwrite(&oper_person, 1, sizeof(oper_person), fp);
    if (len <= 0) {
    	printf("write person param failed.\n");
    	ret = -1;
    } else {
    	ret = 0;
    }
    fclose(fp);

    return ret;
}

static int display_person(void)
{
    short int id = 0;
	int i = 0;

	if ( get_person_param() ) {
		return -1;
	}

    printf("-------------------------------------------------\n");
    printf("[%s]:\n", person_map[oper_person_index].name);
	printf("\tid: (%d)\n", oper_person.id);
	printf("\t[pl]级别 person_level: (%d)\n", oper_person.person_level);
	printf("\t[pe]经验值 person_experience: (%u)\n", oper_person.person_experience);
	printf("\t[lv]生命值 life_value: (%d)\n", oper_person.life_value);
	printf("\t[lm]生命值上限 life_max: (%d)\n", oper_person.life_max);
	printf("\t[st]体力 strength: (%d)\n", oper_person.strength);

	printf("\t[aw]装备武器 armed_weapon: ");
	if(oper_person.armed_weapon < 0)
		printf("NONE");
	else
		print_item_name(oper_person.armed_weapon);
	printf("\n");

	printf("\t[aa]装备防具 armed_armor: ");
	if(oper_person.armed_armor < 0)
		printf("NONE");
	else
		print_item_name(oper_person.armed_armor);
	printf("\n");

	printf("\t[et]内力属性: [%s]\n", (oper_person.energy_type == 0) ? "阴性" :
									 ( (oper_person.energy_type == 1) ? "阳性" : "阴阳调和" ) );

	printf("\t[ev]内力值 energy_value: (%d)\n", oper_person.energy_value);
	printf("\t[em]内力值上限 energy_max: (%d)\n", oper_person.energy_max);
	printf("\t[av]攻击力 attack_value: (%d)\n", oper_person.attack_value);
	printf("\t[sc]轻功 speed_cap: (%d)\n", oper_person.speed_cap);
	printf("\t[dv]防御力 defence_value: (%d)\n", oper_person.defence_value);
	printf("\t[mc]医疗能力 medical_cap: (%d)\n", oper_person.medical_cap);
	printf("\t[pc]用毒能力 poison_cap: (%d)\n", oper_person.poison_cap);
	printf("\t[dc]解毒能力 depoison_cap: (%d)\n", oper_person.depoison_cap);
	printf("\t[ac]抗毒能力 antitoxic_cap: (%d)\n", oper_person.antitoxic_cap);
	printf("\t[bc]拳掌功夫 box_cap: (%d)\n", oper_person.box_cap);
	printf("\t[sc]御剑能力 sword_cap: (%d)\n", oper_person.sword_cap);
	printf("\t[kc]耍刀技巧 knife_cap: (%d)\n", oper_person.knife_cap);
	printf("\t[sw]特殊兵器 special_weapon_cap: (%d)\n", oper_person.special_weapon_cap);
	printf("\t[hw]暗器技巧 hidden_weapon_cap: (%d)\n", oper_person.hidden_weapon_cap);
	printf("\t[kk]武学常识 kungfu_knowledge: (%d)\n", oper_person.kungfu_knowledge);
	printf("\t[mv]道德值 moral_value: (%d)\n", oper_person.moral_value);
	printf("\t[kp]武功带毒 kungfu_posion_effect: (%d)\n", oper_person.kungfu_posion_effect);
	printf("\t[da]左右互搏 double_attack: [%s]\n", oper_person.double_attack ? "有" : "无" );
	printf("\t[pv]声望 prestige_value: (%d)\n", oper_person.prestige_value);
	printf("\t[ap]资质 aptitude: (%d)\n", oper_person.aptitude);

	printf("\t[pi]修炼物品 practice_item: ");
	if (oper_person.practice_item > 0) {
		print_item_name(oper_person.practice_item);
	} else {
		printf("NONE\n");
	}
	printf("\t[pe]修炼经验值 practice_experience: (%d)\n", oper_person.practice_experience);

	printf("\t[kf]修炼武功：\n");
	for(i = 0; i < 10; i++) {
		if (oper_person.kungfu_name[i] > 0) {
			printf("\t\t[%d]", i);
			print_kungfu_name(oper_person.kungfu_name[i]);
			printf("\t: (%d)\n", oper_person.kungfu_level[i]/100 + 1);
		} else {
			break;
		}


	}
    printf("-------------------------------------------------\n");

    return 0;
}

static void print_action_menu(void)
{
	printf("Please select actioin for [%s]:\n", person_map[oper_person_index].name);
    printf("-------------------------------------------------\n");
    printf("1. dispaly person information.\n");
    printf("2. change person information.\n");
    printf("3. Select other person.\n");
    printf("4. Exit.\n");
    printf("-------------------------------------------------\n");
}

static int do_modify(char *param)
{
	int value = 0;
	int ret = -1;
	int index = -1;

	printf("do modify for [%c%c]\n", param[0], param[1]);
	if( strncmp(param, "pl", 2) == 0 ) {
		printf("Please enter new Person Level Value [pl]. ");
		value = get_input_int(0, 30);
		oper_person.person_level = value;
	} else if( strncmp(param, "pe", 2) == 0 ) {
		printf("Please enter new Person Experience value [pe]. ");
		value = get_input_int(0, 60000);
		oper_person.person_experience = value;
	} else if( strncmp(param, "lv", 2) == 0 ) {
		printf("Please enter new Life Value [lv]. ");
		value = get_input_int(0, 999);
		oper_person.life_value = value;
	} else if( strncmp(param, "lm", 2) == 0 ) {
		printf("Please enter new Life Max value [lv]. ");
		value = get_input_int(0, 999);
		oper_person.life_max = value;
	} else if( strncmp(param, "st", 2) == 0 ) {
		printf("Please enter new STrength value [st]. ");
		value = get_input_int(0, 100);
		oper_person.strength = value;
	} else if( strncmp(param, "aw", 2) == 0 ) {
		/* TODO: armed weapon. */

	} else if( strncmp(param, "aa", 2) == 0 ) {
		/* TODO: armed amor */

	} else if( strncmp(param, "et", 2) == 0 ) {
		printf("Please enter new Energy Type [lv] ( 0 - negative; 1 - positive; 2 - mixed). ");
		value = get_input_int(0, 2);
		oper_person.energy_type = value;
	} else if( strncmp(param, "em", 2) == 0 ) {
		printf("Please enter new Energy Max Value [em]. ");
		value = get_input_int(0, 999);
		oper_person.energy_max = value;
	} else if( strncmp(param, "av", 2) == 0 ) {
		printf("Please enter new Attack Value [av]. ");
		value = get_input_int(0, 100);
		oper_person.attack_value = value;
	} else if( strncmp(param, "sc", 2) == 0 ) {
		printf("Please enter new Speed Capability [sc]. ");
		value = get_input_int(0, 100);
		oper_person.speed_cap = value;
	} else if( strncmp(param, "dv", 2) == 0 ) {
		printf("Please enter new Defence Capability [dv]. ");
		value = get_input_int(0, 100);
		oper_person.defence_value = value;
	} else if( strncmp(param, "mc", 2) == 0 ) {
		printf("Please enter new Medical Capability [mc]. ");
		value = get_input_int(0, 100);
		oper_person.medical_cap = value;
	} else if( strncmp(param, "pc", 2) == 0 ) {
		printf("Please enter new Poison Capability [pc]. ");
		value = get_input_int(0, 100);
		oper_person.poison_cap = value;
	} else if( strncmp(param, "dc", 2) == 0 ) {
		printf("Please enter new Depoison Capability [dc]. ");
		value = get_input_int(0, 100);
		oper_person.depoison_cap = value;
	} else if( strncmp(param, "ac", 2) == 0 ) {
		printf("Please enter new Antitoxic Capability [ac]. ");
		value = get_input_int(0, 100);
		oper_person.antitoxic_cap  = value;
	} else if( strncmp(param, "bc", 2) == 0 ) {
		printf("Please enter new Box Capability [bc]. ");
		value = get_input_int(0, 100);
		oper_person.box_cap = value;
	} else if( strncmp(param, "sc", 2) == 0 ) {
		printf("Please enter new Sword Capability [sc]. ");
		value = get_input_int(0, 100);
		oper_person.sword_cap = value;
	} else if( strncmp(param, "kc", 2) == 0 ) {
		printf("Please enter new Knife Capability [kc]. ");
		value = get_input_int(0, 100);
		oper_person.knife_cap = value;
	} else if( strncmp(param, "sw", 2) == 0 ) {
		printf("Please enter new Special Weapon capability [sw]. ");
		value = get_input_int(0, 100);
		oper_person.special_weapon_cap = value;
	} else if( strncmp(param, "hw", 2) == 0 ) {
		printf("Please enter new Hidden Weapon Capability [hw]. ");
		value = get_input_int(0, 100);
		oper_person.hidden_weapon_cap = value;
	} else if( strncmp(param, "kk", 2) == 0 ) {
		printf("Please enter new Kungfu Knowledge [kk]. ");
		value = get_input_int(0, 100);
		oper_person.kungfu_knowledge = value;
	} else if( strncmp(param, "mv", 2) == 0 ) {
		printf("Please enter new Moral Value [mv]. ");
		value = get_input_int(0, 100);
		oper_person.moral_value = value;
	} else if( strncmp(param, "kp", 2) == 0 ) {
		printf("Please enter new Kungfu Poison effect [kp]. ");
		value = get_input_int(0, 100);
		oper_person.kungfu_posion_effect = value;
	} else if( strncmp(param, "da", 2) == 0 ) {
		printf("Please enter new Double Attack [da] ( 0: no double attack; 1: double attack). ");
		value = get_input_int(0, 1);
		oper_person.double_attack = value;
	} else if( strncmp(param, "pv", 2) == 0 ) {
		printf("Please enter new Prestige Value [pv]. ");
		value = get_input_int(0, 100);
		oper_person.prestige_value = value;
	} else if( strncmp(param, "ap", 2) == 0 ) {
		printf("Please enter new Aptitude [ap] ( 0 ~ 105 ): ");
		value = get_input_int(0, 105);
		oper_person.aptitude = value;
	} else if( strncmp(param, "pi", 2) == 0 ) {
		/* TODO: Practice item */
	} else if( strncmp(param, "pe", 2) == 0 ) {
		printf("Please enter new Kungfu Knowledge [pe] ( 0 ~ 15000 ): ");
		value = get_input_int(0, 15000);
		oper_person.practice_experience = value;
	} else if( strncmp(param, "kf", 2) == 0 ) {
		printf("Please select kungfu index: ");
		index = get_input_int(0, 9);
		printf("You select to modify: index(%d)", index);
		print_kungfu_name(oper_person.kungfu_name[index]);
		printf(". please enter level (1 - 10): ");
		value = get_input_int(1, 10);
		oper_person.kungfu_level[index] = value * 100 - 1;
    }

    ret = set_person_param();
    if(ret)
    	printf("set person param failed\n");
    else
    	printf("Successfully set person param\n");

	return 0;
}

static void print_modify_menu(void)
{
    char buf[64];
    int index = -1;
    char param[2];

    display_person();
select_param:
    memset(buf, 0, sizeof(buf));
	printf("Please select paramter to modify for [%s]: ", person_map[oper_person_index].name);

    while( !fgets(buf, sizeof(buf), stdin) || strlen(buf) < 2 )
        printf("Please select paramter to modify for [%s]: ", person_map[oper_person_index].name);

    sscanf(buf, "%2c", param);

    printf("You selected [%c%c]\n", param[0], param[1]);

    do_modify(param);

    return;
}

static void do_actions(void)
{
	char buf[64];
    int sel = -1;

	print_action_menu();
select_action:
    memset(buf, 0, sizeof(buf));
    printf("Please select: ");
    while( !fgets(buf, sizeof(buf), stdin) || strlen(buf) < 2 )
        printf("Please select: ");

    sscanf(buf, "%d", &sel);
    switch(sel) {
    	case 1:
    		display_person();
    		break;

    	case 2:
    		print_modify_menu();
    		break;

    	case 3:
			print_person_menu();
		    oper_person_index = select_person();
		    if(oper_person_index < 0)
		        printf("selection wrong, abort\n");
		    break;

		case 4:
			return;

		default:
		    printf("Wrong selection, please select again!\n");
        	goto select_action;
    }
    print_action_menu();
   	goto select_action;
}

int main()
{
	setup_kungfu_map();
	setup_item_map();
	setup_person_map();
#ifdef DEBUG
	print_person_list();
#endif


	/* main menu: select a person first. */
	print_person_menu();
    oper_person_index = select_person();
    if(oper_person_index < 0)
        printf("selection wrong, abort\n");

    /* action menu */
    do_actions();

	return 0;
}
