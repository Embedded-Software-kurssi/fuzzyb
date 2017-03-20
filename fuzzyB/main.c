/* Update to Greg Viot's fuzzy system -- DDJ, February 1993, page 94 */
/* By J. Tucker, P. Fraley, and L. Swanson, April 1993 */
/* Ported to AVR by JPP/ February 2017 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* NEW */
#define max(a, b) (a < b ? b : a) /* NEW */
#define min(a, b) (a > b ? b : a) /* NEW */
struct io_type *System_Inputs;    /* anchor inputs NEW */
struct io_type *System_Output;    /* anchor output NEW */
#define MAXNAME 10
#define UPPER_LIMIT 255
struct io_type
{
    char name[MAXNAME];
    int value;
    struct mf_type *membership_functions;
    struct io_type *next;
};
struct mf_type
{
    char name[MAXNAME];
    int value;
    int point1;
    int point2;
    float slope1;
    float slope2;
    struct mf_type *next;
};
struct rule_type
{
    struct rule_element_type *if_side;
    struct rule_element_type *then_side;
    struct rule_type *next;
};
struct rule_element_type
{
    int *value;
    struct rule_element_type *next;
};
struct rule_type *Rule_Base;
void exit(int x)
{
    while (1);
}

void initialize_system(void);
void get_system_inputs(int input1, int input2);
void fuzzification(void);
void rule_evaluation(void);
void defuzzification(void);
void put_system_outputs(void);
void compute_degree_of_membership(struct mf_type *mf, int input);
float compute_area_of_trapezoid(struct mf_type *mf);

int main()
{
    int input1, input2;		       /* NEW */
    input1 = 60;		       /* NEW */
    input2 = 125;		       /* NEW */
    initialize_system();	       /* Read input files, NEW */
    get_system_inputs(input1, input2); /* Get & put argv NEW */
    fuzzification();
    rule_evaluation();
    defuzzification();
    put_system_outputs(); /* print all data, NEW */
    while (1)
	;
} /* END MAIN */

void fuzzification()
{
    struct io_type *si;
    struct mf_type *mf;
    for (si = System_Inputs; si != NULL; si = si->next)
	for (mf = si->membership_functions; mf != NULL; mf = mf->next)
	    compute_degree_of_membership(mf, si->value);
} /* END FUZZIFICATION */

void rule_evaluation()
{
    struct rule_type *rule;
    struct rule_element_type *ip; /* if ptr */
    struct rule_element_type *tp; /* then ptr */
    int strength;
    int nomatch = 0; /* NEW, test some rules */
    for (rule = Rule_Base; rule != NULL; rule = rule->next)
    {
	strength = UPPER_LIMIT;
	for (ip = rule->if_side; ip != NULL; ip = ip->next)
	    strength = min(strength, *(ip->value));
	for (tp = rule->then_side; tp != NULL; tp = tp->next)
	{
	    *(tp->value) = max(strength, *(tp->value)); /* NEW */
	    if (strength > 0)
		nomatch = 1; /* NEW */
	}		     /* NEW */
    }
    if (nomatch == 0)
	printf("NO MATCHING RULES FOUND!\n"); /* NEW */
} /* END RULE EVALUATION */

void defuzzification()
{
    struct io_type *so;
    struct mf_type *mf;
    int sum_of_products;
    int sum_of_areas;
    int area, centroid;
    for (so = System_Output; so != NULL; so = so->next)
    {
	sum_of_products = 0;
	sum_of_areas = 0;
	for (mf = so->membership_functions; mf != NULL; mf = mf->next)
	{
	    area = compute_area_of_trapezoid(mf);
	    centroid = mf->point1 + (mf->point2 - mf->point1) / 2;
	    sum_of_products += area * centroid;
	    sum_of_areas += area;
	}
	if (sum_of_areas == 0) /* NEW */
	{
	    printf("Sum of Areas = 0, will cause div error\n"); /* NEW */
	    printf("Sum of Products= %d\n", sum_of_products);   /* NEW */
	    so->value = 0;					/* NEW */
	    return;						/* NEW */
	}							/* NEW */
	so->value = sum_of_products / sum_of_areas;
    }
} /* END DEFUZZIFICATION */

void compute_degree_of_membership(struct mf_type *mf, int input)
{
    int delta_1, delta_2;
    delta_1 = input - mf->point1;
    delta_2 = mf->point2 - input;
    if ((delta_1 <= 0) || (delta_2 <= 0))
	mf->value = 0;
    else
    {
	mf->value = min((mf->slope1 * delta_1), (mf->slope2 * delta_2));
	mf->value = min(mf->value, UPPER_LIMIT);
    }
} /* END DEGREE OF MEMBERSHIP */

float compute_area_of_trapezoid(struct mf_type *mf)
{
    float run_1, run_2, area, top;
    float base;
    base = mf->point2 - mf->point1;
    run_1 = mf->value / mf->slope1;
    run_2 = mf->value / mf->slope2;
    top = base - run_1 - run_2;
    area = mf->value * (base + top) / 2;
    return (area);
} /* END AREA OF TRAPEZOID */

struct mf_type *mfptr;
struct io_type *outptr;
struct mf_type *top_mf;
struct mf_type *mfptr;
struct io_type *ioptr;
struct rule_type *ruleptr;
struct rule_element_type *ifptr;
struct rule_element_type *thenptr;
void add_in_line(char *buff, int a, int b, int c, int d);
void add_rule_line(char *buff, char *buff1, char *buff2);

void add_in_line(char *buff, int a, int b, int c, int d)
{
    if (mfptr == NULL) /* first time thru only */
    {
	mfptr = (struct mf_type *)calloc(1, sizeof(struct mf_type));
	top_mf = mfptr;
	ioptr->membership_functions = mfptr;
    }
    else
    {
	for (mfptr = top_mf; mfptr->next; mfptr = mfptr->next)
	    ; /* spin to last */
	mfptr->next = (struct mf_type *)calloc(1, sizeof(struct mf_type));
	mfptr = mfptr->next;
    }
    sprintf(mfptr->name, "%s", buff); /* membership name, NL, ZE, etc */
    mfptr->point1 = a;		      /* left x axis value */
    mfptr->point2 = d;		      /* right x axis value */
    if (b - a > 0)
	mfptr->slope1 = UPPER_LIMIT / (b - a); /* left slope */
    else
    {
	printf("Error in input line, membership element %s.\n",
	       buff);
	exit(1);
    }
    if (d - c > 0)
	mfptr->slope2 = UPPER_LIMIT / (d - c); /* right slope */
    else
    {
	printf("Error in input line, membership element %s.\n",
	       buff);
	exit(1);
    }
}

void add_rule_line(char *buff, char *buff1, char *buff2)
{
    ioptr = System_Inputs; /* points to angle */
    for (mfptr = ioptr->membership_functions; mfptr != NULL; mfptr = mfptr->next)
    {
	if ((strcmp(mfptr->name, buff)) == 0)
	{
	    ifptr = (struct rule_element_type *)
		calloc(1, sizeof(struct rule_element_type));
	    ruleptr->if_side = ifptr;     /* points to angle */
	    ifptr->value = &mfptr->value; /* needs address here */
	    ifptr->next = (struct rule_element_type *)
		calloc(1, sizeof(struct rule_element_type));
	    ifptr = ifptr->next;
	    break; /* match found */
	}
    }
    ioptr = ioptr->next; /* points to velocity */
    for (mfptr = ioptr->membership_functions; mfptr != NULL; mfptr = mfptr->next)
    {
	if ((strcmp(mfptr->name, buff1)) == 0)
	{
	    ifptr->value = &mfptr->value; /* needs address here */
	    break;			  /* match found */
	}
    }
    if (outptr == NULL)
	outptr = System_Output; /* point then stuff to output */
    for (mfptr = outptr->membership_functions; mfptr != NULL; mfptr = mfptr->next)
    {
	if ((strcmp(mfptr->name, buff2)) == 0)
	{
	    thenptr = (struct rule_element_type *)
		calloc(1, sizeof(struct rule_element_type));
	    ruleptr->then_side = thenptr;
	    thenptr->value = &mfptr->value; /* needs address here */
	    break;			    /* match found */
	}
    }
    ruleptr->next = (struct rule_type *)calloc(1, sizeof(struct rule_type));
    ruleptr = ruleptr->next;
}

void initialize_system() /* NEW FUNCTION INITIALIZE */
{
    ioptr = NULL;
    ruleptr = NULL;
    ifptr = NULL;
    thenptr = NULL;
    /* READ THE FIRST FUZZY SET (ANTECEDENT); INITIALIZE STRUCTURES */
    ioptr = (struct io_type *)calloc(1, sizeof(struct io_type));
    System_Inputs = ioptr;		 /* Anchor to top of inputs */
    sprintf(ioptr->name, "%s", "Angle"); /* into struct io_type.name */
    mfptr = NULL;
    add_in_line("NL", 0, 31, 31, 63);
    add_in_line("NM", 31, 63, 63, 95);
    add_in_line("NS", 63, 95, 95, 127);
    add_in_line("ZE", 95, 127, 127, 159);
    add_in_line("PS", 127, 159, 159, 191);
    add_in_line("PM", 159, 191, 191, 223);
    add_in_line("PL", 191, 223, 223, 255);

    /* READ THE SECOND FUZZY SET (ANTECEDENT); INITIALIZE STRUCTURES */
    ioptr->next = (struct io_type *)calloc(1, sizeof(struct io_type));
    ioptr = ioptr->next;
    sprintf(ioptr->name, "%s", "Velocity"); /* into struct io_type.name */
    mfptr = NULL;
    add_in_line("NL", 0, 31, 31, 63);
    add_in_line("NM", 31, 63, 63, 95);
    add_in_line("NS", 63, 95, 95, 127);
    add_in_line("ZE", 95, 127, 127, 159);
    add_in_line("PS", 127, 159, 159, 191);
    add_in_line("PM", 159, 191, 191, 223);
    add_in_line("PL", 191, 223, 223, 255);

    /* READ THE THIRD FUZZY SET (CONSEQUENCE); INITIALIZE STRUCTURES */
    ioptr = (struct io_type *)calloc(1, sizeof(struct io_type));
    System_Output = ioptr;		 /* Anchor output structure */
    sprintf(ioptr->name, "%s", "Force"); /* into struct io_type.name */
    mfptr = NULL;
    add_in_line("NL", 0, 31, 31, 63);
    add_in_line("NM", 31, 63, 63, 95);
    add_in_line("NS", 63, 95, 95, 127);
    add_in_line("ZE", 95, 127, 127, 159);
    add_in_line("PS", 127, 159, 159, 191);
    add_in_line("PM", 159, 191, 191, 223);
    add_in_line("PL", 191, 223, 223, 255);

    /* READ RULES FILE; INITIALIZE STRUCTURES */
    ioptr = NULL;
    outptr = NULL;
    ruleptr = (struct rule_type *)calloc(1, sizeof(struct rule_type));
    if (ioptr == NULL)
	Rule_Base = ruleptr; /* first time thru, anchor */
    add_rule_line("NL", "ZE", "PL");
    //add_rule_line("ZE", "NL", "PL");
    add_rule_line("NM", "ZE", "PM");
    add_rule_line("ZE", "NM", "PM");
    add_rule_line("NS", "ZE", "PS");
    //add_rule_line("ZE", "NS", "PS");
    add_rule_line("NS", "PS", "PS");
    add_rule_line("ZE", "ZE", "PM");
    add_rule_line("ZE", "PS", "NS");
  //add_rule_line("PS", "ZE", "NS");
    add_rule_line("PS", "NS", "NS");
    add_rule_line("ZE", "PM", "NM");
    add_rule_line("NM", "ZE", "NM");
    //add_rule_line("ZE", "PL", "NL");
    add_rule_line("PL", "ZE", "NL");
} /* END INITIALIZE */

void put_system_outputs() /* NEW */
{
    int cnt = 1;
    for (ioptr = System_Inputs; ioptr != NULL; ioptr = ioptr->next)
    {
	printf("%s: Value= %d\n", ioptr->name, ioptr->value);
	for (mfptr = ioptr->membership_functions; mfptr != NULL; mfptr = mfptr->next)
	{
	    printf(" %s: Value %d Left %d Right %d\n",
		   mfptr->name, mfptr->value, mfptr->point1, mfptr->point2);
	}
	printf("\n");
    }
    for (ioptr = System_Output; ioptr != NULL; ioptr = ioptr->next)
    {
	printf("%s: Value= %d\n", ioptr->name, ioptr->value);
	for (mfptr = ioptr->membership_functions; mfptr != NULL; mfptr = mfptr->next)
	{
	    printf(" %s: Value %d Left %d Right %d\n",
		   mfptr->name, mfptr->value, mfptr->point1, mfptr->point2);
	}
    }
    /* print values pointed to by rule_type (if & then) */
    printf("\n");
    for (ruleptr = Rule_Base; ruleptr->next != NULL; ruleptr = ruleptr->next)
    {
	printf("Rule #%d:", cnt++);
	for (ifptr = ruleptr->if_side; ifptr != NULL; ifptr = ifptr->next)
	    printf(" %d", *(ifptr->value));
	for (thenptr = ruleptr->then_side; thenptr != NULL; thenptr = thenptr->next)
	    printf(" %d\n", *(thenptr->value));
    }
    printf("\n");
} /* END PUT SYSTEM OUTPUTS */
void get_system_inputs(int input1, int input2) /* NEW */
{
    struct io_type *ioptr;
    ioptr = System_Inputs;
    ioptr->value = input1;
    ioptr = ioptr->next;
    ioptr->value = input2;
}