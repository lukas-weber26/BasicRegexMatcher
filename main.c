#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//make all of these operators return the number of positions they move the string forward pls
int direct_match(char * text, char * regex) {
	if (strcmp(text,regex) == 0) {
		return 1;
	} else {
		return -1;
	}
}

int digit_match(char * text, char * regex) {
	char temp [2];
	temp[1] = '\0';
	for (int i = 0; i< strlen(text); i++) {
		temp[0] = text[i];
		if (!strstr("0123456789",temp)) {return -1;}
	}
	return 1;
}

int alpha_match(char * text, char * regex) {
	char temp [2];
	temp[1] = '\0';
	for (int i = 0; i< strlen(text); i++) {
		temp[0] = tolower(text[i]);
		if (!strstr("0123456789abcdefghijklmnopqrstuvwxyz_",temp)) {return -1;}
	}
	return 1;
}

int positive_match(char * text, char* group) {	
	char temp[2];
	temp[1] = '\0';
	temp[0] = text[0];
	if (!strstr(group, temp)) { return -1;}
	return 1; //this may have to be i+1
}

int negative_match(char * text, char* group) {	
	char temp[2];
	temp[1] = '\0';
	temp[0] = text[0];
	if (strstr(group, temp)) { return -1;}
	return 1; //this may have to be i+1
}

int match_wildcard(char * text, char * regex) {
	return 1;
}

enum regexType {DIRECT,DIGIT,ALPHA,POSITIVE,NEGATIVE,WILDCARD,STARTOF, ENDOF, OR};
enum regexMod {NONE, ONEPLUS, ZEROPLUS};

//this thing returns the appropriate increment 
char * read_regex(char * foreign_regex, int *type, int *mod, char * matchableText) {
	char * regex = foreign_regex;
	int increment = 0;

	//printf("Searching for correct regex type\n");
	if (strncmp("/d", regex, 2) == 0){
		*type = DIGIT;
		matchableText[0] = regex[0];
		matchableText[1] = '\0';
		increment += 2;
	} else if (strncmp("/w", regex, 2) == 0) {
		*type = ALPHA;
		matchableText[0] = regex[0];
		matchableText[1] = '\0';
		increment += 2;
	} else if (strncmp("*", regex,1) == 0) {
		*type = WILDCARD;
		matchableText[0] = regex[0];
		matchableText[1] = '\0';
		increment += 1;
	//the following two are almost certainly wrong!
	} else if (strncmp("[^", regex,2) == 0) {
		//printf("Negative Match\n");
		int i = 2; 	
		for (i; regex[i] != ']' ;i++) {if (regex[i] == '\0') {printf("Bad\n");exit(1);}};
		strncpy(matchableText, regex+2, i-2);
		matchableText[i] = '\0';
		*type = NEGATIVE;
		increment += i+1;
	} else if (strncmp("[", regex, 1) == 0) {
		//printf("Positive Match\n");
		int i = 1; 	
		for (i; regex[i] != ']' ;i++) {if (regex[i] == '\0') {printf("Bad\n");exit(1);}};
		strncpy(matchableText, regex+1, i-1);
		matchableText[i] = '\0';
		*type = POSITIVE;
		increment += i + 1;
	} else if (strncmp("^",regex,1) == 0) {
			*type = STARTOF;
			increment += 1;
	} else if (strncmp("?", regex, 1) == 0) {
			*type = ENDOF;
			increment += 1;
	} else if (strncmp("(", regex, 1) == 0) {
		printf("OR Match\n");
		int i = 1; 	
		for (i; regex[i] != ')' ;i++) {if (regex[i] == '\0') {printf("Bad\n");exit(1);}};
		strncpy(matchableText, regex+1, i-1);
		matchableText[i] = '\0';
		*type = OR;
		increment += i + 1;
	}else {
		*type = DIRECT;
		matchableText = malloc(sizeof(char)*2);
		matchableText[0] = regex[0];
		matchableText[1] = '\0';
		increment += 1;	
	}

	*mod = NONE;			

	if (strlen(regex) > increment) {
		regex = regex + increment;
		increment = 0;

		//step two, figure out if a modifyer is being used 
		if (strncmp(regex,"+",1) == 0) {
			*mod = ONEPLUS;
			increment += 1;
		} else if (strncmp(regex,"?",1) == 0) {
			*mod = ZEROPLUS;
			increment += 1;
		}
		regex = regex + increment;
	}	

	return regex;
}

int run_regex(int (*fun)(char *, char *),char * regex_target, char * regex_match, int mod) {
	
	int increment;
	char * target_string = regex_target;
	char * match_string = regex_match;

	switch (mod) {
		case (NONE):
			increment = fun(target_string, match_string);
			if (increment == -1) {return -1;} else {match_string += increment; return increment;}
			break;
		case (ONEPLUS):
			increment = fun(target_string, match_string);
			if (increment == -1) {return -1;} else {
				target_string += increment;
				int temp = 0;
				while ((strlen(target_string) > 0) && ((temp = fun(target_string, match_string)) != -1)) {
					increment += temp;
					target_string += temp;
				}
				return increment;
			}
			break;
		case (ZEROPLUS):
			increment = fun(target_string, match_string);
			if (increment == -1) {return 0;} else {
				target_string += increment;
				int temp = 0;
				while ((strlen(target_string) > 0) && ((temp = fun(target_string, match_string)) != -1)) {
					increment += temp;
					target_string += temp;
				}
				return increment;
			}
			break;
		default:
			printf("Mod error.\n");
			exit(0);
	}
}


int main(int argc, char * argv[]) {

	//next step: turn all of this into a function and add a bit of a wrapper. then you're pretty much done!

	//stores the actual string
	char * string_to_match = "abba1";
	char * string_to_match_mod = string_to_match;

	//regex string 	
	char * regex_string = "(a|b)"; //looks like ? doesn't work	
	char * regex_string_mod =  regex_string;

	//things to get infromation back from read_regex
	int type, mod; 
	char * matchable_regex; 
	matchable_regex = malloc(sizeof(char) * 512);
	
	//for checking first 
	int first = 0;
	char * matched;

	do {
		regex_string_mod = read_regex(regex_string_mod, &type, &mod, matchable_regex);
		matchable_regex = realloc(matchable_regex, strlen(matchable_regex) + 1);

	//	enum regexType {DIRECT,DIGIT,ALPHA,POSITIVE,NEGATIVE,WILDCARD,STARTOF, ENDOF};
	//	enum regexMod {NONE, ONEPLUS, ZEROPLUS};

		int num_matched;
		switch (type) {
			case(DIRECT):
				num_matched = run_regex(&direct_match, string_to_match_mod,matchable_regex , mod);
				break;
			case(ALPHA):
				num_matched = run_regex(&alpha_match, string_to_match_mod,matchable_regex , mod);
				break;
			case(DIGIT):
				num_matched = run_regex(&digit_match, string_to_match_mod,matchable_regex , mod);
				break;
			case(WILDCARD):
				num_matched = run_regex(&match_wildcard, string_to_match_mod,matchable_regex , mod);
				break;
			case(POSITIVE):
				num_matched = run_regex(&positive_match, string_to_match_mod,matchable_regex , mod);
				break;
			case(NEGATIVE):
				num_matched = run_regex(&negative_match, string_to_match_mod,matchable_regex , mod);
				break;
			case(STARTOF):
				if (first == 0) {
					num_matched = 1;
				} else {
					num_matched = -1;
				}		
				break;
			case(ENDOF):
				if ( strlen(string_to_match) == 1) {
					return 1;
				} else {
					num_matched = -1;
				}		
				break;
			case (OR):
				//matched = malloc(sizeof(char) * (strlen(matchable_regex) + 1));
				//strcpy(matched, matchable_regex);

				////split the string into two, work them seperately;
				//char * first;
				//char * second;
				//int j = 0;

				//for (j; matched[j] != '|';j++) {if (matched[j] == '\0') {printf("Bad OR\n"); exit(0);}};
			
				//first = malloc(sizeof(char) * (j));			
				//second = malloc(sizeof(char) * (strlen(matched) -j));
				//	
				//strncpy(first, matchable_regex, j);
				//strcpy(second,matchable_regex + j+1);

				////printf("First:%s,Second:%s.\n", first,second);
				////exit(0);

				//regex_string_mod = read_regex(first, &type, &mod, matchable_regex);
				//regex_string_mod = read_regex(second, &type, &mod, matchable_regex);

				////read the first regex, run the run_regex function on it 
				////if it works continue
				////if not, read the second regex and run the run_regex function on it
				////if not, move on.

				break;
			default:
				printf("Bad return value\n");
				exit(0);
		}

		if (num_matched == -1) {
			printf("Regex not matched\n");
			exit(0);
		} else {
			string_to_match_mod += num_matched;
				if (strlen(string_to_match_mod) == 0) {
					printf("Done regex.\n");
					exit(1);
				} else {
					printf("Regex Matched\n");
					printf("Next match:%s.\n", string_to_match_mod);
					printf("Remaining regex :%s.\n", regex_string_mod);
				}
		}

		free(matchable_regex);
		free(matched);
		first ++;

	} while (1);

	//question: do I want to bother with or?

}




