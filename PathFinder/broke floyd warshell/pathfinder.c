#include "libmx.h"

typedef struct s_graph {
    char *file_str;
    // Matrix
    int **array;
    char **isld;
}t_graph;

typedef struct s_main {
    int fd; 
    int nmb_isld;
    char *str;
    char *delims;
    // warshell
    char *add_arow;
    char *add_plus;
    int cnt_loop;
    int rws;
    int clmn;
}t_main;

typedef struct s_warshell {
    int max_id;
    char **all_way;

    int cost;
    int path;    
}t_warshell;

t_main *create_vars(int fd, int nmb_isld) {
    t_main* util_vars = (t_main*)malloc(sizeof(t_main));
    util_vars->fd = fd;
    util_vars->nmb_isld = nmb_isld;
    util_vars->str = mx_strdup("");
    util_vars->delims = "-,\n";
    return util_vars;
}

t_graph *create_graph(t_main *vars) {
    t_graph *main_graph = (t_graph*)malloc(sizeof(t_graph));

    main_graph->file_str = NULL;

    main_graph->array = (int**)malloc(vars->nmb_isld * sizeof(int*));
    for (int i = 0; i < vars->nmb_isld; i++) 
        main_graph->array[i] = (int*)malloc(vars->nmb_isld * sizeof(int));		
    
    for (int i = 0; i < vars->nmb_isld; i++) 
        for (int j = 0; j < vars->nmb_isld; j++) 
            main_graph->array[i][j] = INT_MAX;

    return main_graph;
}

static bool mx_check_valid_1Line(const char *str) {	
    for (int i = 0; str[i] != '\0'; i++) 
        if (!(mx_isdigit(str[i]))) {
            return 0;
        }
    return 1;
}

static bool mx_check_isvalid_alpha(const char *str, int j) {
    if (j < 2) {
        for (int i = 0; str[i] != '\0'; i++) 
            if (!(mx_isalpha(str[i])))
                return 0;
        return 1;
    } 
    else {
        if (mx_check_valid_1Line(str)) { 
            return 1;
        }
        else return 0;
    }
}

static void mx_wrong_line(int i) {
    char *itoa = mx_itoa(i + 1);
    mx_printerr("error: line ");
    mx_printerr(itoa);
    mx_strdel(&itoa);
    mx_printerr(" isn't valid\n");
    exit(1);
}

static void mx_first_check(int *fd, int nmb_argc, char const *rl_argv) {
    if (nmb_argc != 2) {
        mx_printerr("usage: ./pathfinder [filename]\n");
        exit(1);
    } 
    if ((*fd = open(rl_argv, O_RDONLY)) == -1) {
        mx_printerr("error: file ");
        mx_printerr(rl_argv);
        mx_printerr(" doesn't exist\n");
        exit(1);
    } 	
}

static void mx_check_valid_file(int nmb_argc, 
char const *rl_argv, t_main *vars) {
    int str = -1;
    
    mx_first_check(&vars->fd, nmb_argc, rl_argv);
       
    if ((str = mx_read_line(&vars->str, 1, '\n', vars->fd)) < 0) {
        mx_printerr("error: file ");
        mx_printerr(rl_argv);
        mx_printerr(" is empty\n");
        exit(1);
    }	
    if ((mx_check_valid_1Line(vars->str)) == 0) {
        if (vars->str) mx_strdel(&vars->str);
        mx_printerr("error: line 1 isn't govna valid\n");
        if (vars->str) mx_strdel(&vars->str);
        exit(1);
    }
    else {
        vars->nmb_isld = mx_atoi(vars->str);		
        if (vars->str) mx_strdel(&vars->str);
    }
}

// add a smb(s2) to the string(s1)
static char *strjoin_mod(char *s1, char s2) {
    char *result = NULL;
    if (!s1 && !s2) return result;
    else if (!s2) {
        result = mx_strdup(s1);
        free(s1);
        return result;
    }
    else {
        result = mx_strnew(mx_strlen(s1) + 1);
        result = mx_strcpy(result, s1);
        result[mx_strlen(s1)] = s2;
        free(s1);
    }
    return result;
}

// copy and delete src string
static char *mx_strjoin_mod2(char *str, char *arr){	
    char *temp = mx_strjoin(str, arr);
    if (str) mx_strdel(&str);
    return temp;
}

static char **crt_isld_matrix(t_main *vars, t_graph *graph){	
    char **isld = (char**)malloc(vars->nmb_isld * sizeof(char*));

    for (int k = 0, i = 0, rslt_rdln = 1; rslt_rdln > 0 ; i++) 
        for (int j = 0, flag = 1; j < 3; j++) {
            if ((rslt_rdln = mx_read_line(&vars->str, 1, vars->delims[j], vars->fd)) < 1) break;
            graph->file_str = mx_strjoin_mod2(graph->file_str, vars->str);
            graph->file_str = strjoin_mod(graph->file_str, vars->delims[j]);

            if ((mx_check_isvalid_alpha(vars->str, j)) == 1) {
                if (j < 2) {
                    for (int d = 0; d < k; d++)
                        if ((flag = mx_strcmp(vars->str, isld[d])) == 0) 
                            break;
                    if (flag != 0) 
                        isld[k++] = mx_strdup(vars->str);
                }
            }
            else mx_wrong_line(i);
            mx_strdel(&vars->str);
        }
    return isld;
}

// add node to graph
static void crt_isld_matrix_copy(t_graph *graph, t_main *vars){
    int arr[2] = {0};
    int index = 0;
    
    for (size_t j = 0, i = 0; graph->file_str[i] != '\0'; i++) 
        if (graph->file_str[i] != vars->delims[j]) 
            vars->str = strjoin_mod(vars->str, graph->file_str[i]);		
        else  
            if (j < 2) {
                for (int k = 0; k < vars->nmb_isld; k++) 
                    if (mx_strcmp(vars->str, graph->isld[k]) == 0) {
                        arr[index++] = k;	
                        free(vars->str);
                        vars->str = mx_strnew(0);
                    }
                j++;				
            } else {
                graph->array[arr[0]][arr[1]] = mx_atoi(vars->str);
                graph->array[arr[1]][arr[0]] = mx_atoi(vars->str);
                free(vars->str);
                vars->str = mx_strnew(0);
                j = 0;				
                index = 0;
            }		
    // if (vars->str) free(vars->str);
}

static void free_fn(t_main *vars, t_graph *graph) {    

    for (int i = 0; i < vars->nmb_isld; i++)
        free(graph->isld[i]);
    free(graph->isld);

    if (graph->file_str) mx_strdel(&graph->file_str);

    for (int i = 0; i < vars->nmb_isld; ++i)
        free(graph->array[i]);
    free(graph->array);
    
    free(graph);

    close(vars->fd);
    
    if (vars->str) mx_strdel(&vars->str);
    
    if (vars) free(vars);

    
}

static void mx_printDst(t_warshell **wrsh_mtrx, int i, int j, t_graph *graph) {
    if (wrsh_mtrx[i][j].path == i)
        return;

    mx_printDst(wrsh_mtrx, i, wrsh_mtrx[i][j].path, graph);

    
    
    if (wrsh_mtrx[i][j].cost == 0) {
        mx_printstr(" = ");
        mx_printint(wrsh_mtrx[i][j].cost - wrsh_mtrx[i][wrsh_mtrx[i][j].path].cost);
    } else {
        mx_printstr(" + ");
        mx_printint(wrsh_mtrx[i][j].cost - wrsh_mtrx[i][wrsh_mtrx[i][j].path].cost);
    }
    
}

static void mx_printPath(t_warshell **wrsh_mtrx, int i, int j, t_graph *graph) {
    if (wrsh_mtrx[i][j].path == i)
        return;

    mx_printPath(wrsh_mtrx, i, wrsh_mtrx[i][j].path, graph);

    mx_printstr(" -> ");
    mx_printstr(graph->isld[wrsh_mtrx[i][j].path]);   
    
}

static void mx_print_shortests_path(t_warshell **wrsh_mtrx, t_main *vars, t_graph *graph) {
    int iter = 0;
    printf("path\n");
     for (int i = 0; i < vars->nmb_isld; i++) {
		for (int j = 0; j < vars->nmb_isld; j++) {
            printf("\t%d", wrsh_mtrx[i][j].path);
        }
        printf("\n");
    }
    printf("cost\n");
    for (int i = 0; i < vars->nmb_isld; i++) {
		for (int j = 0; j < vars->nmb_isld; j++) {
            if (wrsh_mtrx[i][j].cost == INT_MAX)
                printf("\tinf");
            else
                printf("\t%d", wrsh_mtrx[i][j].cost);
        }
        printf("\n");
    }


	for (int i = 0; i < vars->nmb_isld; i++) {
		for (int j = iter; j < vars->nmb_isld; j++) {
			if (j != i && wrsh_mtrx[i][j].path != -1) {
            mx_printstr("========================================\n");
                
            // Path
            mx_printstr("Path: ");
            mx_printstr(graph->isld[i]);
            mx_printstr(" -> ");
            mx_printstr(graph->isld[j]);
            mx_printstr("\n");
            // Route
            mx_printstr("Route: ");
            mx_printstr(graph->isld[i]);            
            mx_printPath(wrsh_mtrx, i, j, graph);
            mx_printstr(" -> ");
            mx_printstr(graph->isld[j]);
            mx_printstr("\n");

            // Distance
            if (wrsh_mtrx[i][wrsh_mtrx[i][j].path].cost != 0) {
                mx_printstr("Distance: ");
                mx_printint(wrsh_mtrx[i][wrsh_mtrx[i][j].path].cost); 
                mx_printDst(wrsh_mtrx, i, j, graph);
                mx_printstr(" = ");
                mx_printint(wrsh_mtrx[i][j].cost);
                mx_printstr("\n");
            } else {
                mx_printstr("Distance: ");
                mx_printint(wrsh_mtrx[i][j].cost); 
                mx_printstr("\n");
            }
            mx_printstr("========================================\n");
            }
		}
        iter++;
	}

    for (int i = 0; i < vars->nmb_isld; i++)
        free(wrsh_mtrx[i]);
    free(wrsh_mtrx);
}

//------------------------------------------------------------------------

int mx_check_all_top1(t_warshell **wrsh_mtrx, int x, int y, int nbr_of_path) {
	int temp;
	int content = wrsh_mtrx[y][x].cost;

	for (int i = nbr_of_path; i < wrsh_mtrx[0][0].max_id; i++) {
		if (wrsh_mtrx[y][i].path != 0 && wrsh_mtrx[y][i].path != INT_MAX) {
			temp = content - wrsh_mtrx[y][i].path;
			if (temp == wrsh_mtrx[i][x].cost)
				return i;
			}
		}
		return -2;
}

int mx_check_all_top(t_warshell **wrsh_mtrx, int x, int y) {
	int temp;
	int content = wrsh_mtrx[y][x].cost;

	for (int i = 0; i < wrsh_mtrx[0][0].max_id; i++) {
		if (wrsh_mtrx[y][i].path != 0 && wrsh_mtrx[y][i].path != INT_MAX) {
			temp = content - wrsh_mtrx[y][i].path;
			if (temp == wrsh_mtrx[i][x].cost)
				return i;
		}
	}

	return -2;
}

int **mx_negative_mass(t_warshell **wrsh_mtrx, int count, t_main *vars) {
	int **path_in_way = (int **)malloc(sizeof(int *) * count);

	for (int i = 0; i < count; i++) {
		path_in_way[i] = (int *)malloc(sizeof(int) * (vars->nmb_isld + 1));
		for (int j = 0; j < (vars->nmb_isld + 1); j++) {
			path_in_way[i][j] = -1;
		}
	}
	return path_in_way;
}

int **mx_path_from_to(t_warshell **wrsh_mtrx, int count, int x, int y, t_main *vars) {
    int **way = mx_negative_mass(wrsh_mtrx, count, vars);
	int end = 0;
	int temp = 0;

	for (int i = 0, index = 1; i < count; i++) {
		end = y;
		way[i][index - 1] = y;
		while(end != x) {
			way[i][index] = mx_check_all_top(wrsh_mtrx, x, end);
			if (i > 0 && way[i][index] == way[i - 1][index]) {
				temp = mx_check_all_top1(wrsh_mtrx, x, end, way[i][index] + 1);
				if (temp != -2)
					way[i][index] = temp;
			}
			end = way[i][index];
			index++;
		}
		index = 1;
	}
	return way;
}

int mx_count_other_way(t_warshell **wrsh_mtrx, int max, int x, int y) {
	int content = wrsh_mtrx[x][y].cost;
    int count = 0;
    int temp;

    for (int i = 0; i < max; i++) {
        if (wrsh_mtrx[y][i].cost != 0 && wrsh_mtrx[y][i].cost != INT_MAX) {
            temp = content - wrsh_mtrx[y][i].cost;
            if (temp == wrsh_mtrx[i][x].cost)
                count++;
        }
    }
    y--;
    if (x < y)
        count += (mx_count_other_way(wrsh_mtrx, max, x, y) - 1);

    return count;
}

void mx_add_top(t_warshell **wrsh_mtrx, int **path_in_way, int k, t_graph *graph) {
	wrsh_mtrx[0][0].all_way = (char **)malloc(sizeof(char *) * (wrsh_mtrx[0][0].max_id + 1));
	int index = 0;
	int j = 0;

	for (; path_in_way[k][index] != -1; index++);

	for (int i = index - 1; i >= 0; i--, j++) {
		wrsh_mtrx[0][0].all_way[j] = mx_strdup(graph->isld[path_in_way[k][i]]);
	}
	wrsh_mtrx[0][0].all_way[j] = NULL;
}

static void print_dist(t_warshell **wrsh_mtrx, int *ways, int x, int y) {
	int index = 0;

	while (ways[index] != -1) {
		index++;
	}
	for (int i = 0; i <= index; i++, index--) {
		mx_printint(wrsh_mtrx[ways[index - 1]][ways[index - 2]].path);
		if (i < index - 1)
			mx_printstr(" + ");
	}
	mx_printstr(" = ");
	mx_printint(wrsh_mtrx[x][y].path);
}

int mx_arr_size(char **str) {
	int size = 0;

	for (int i = 0; str[i]; i++)
		size++;

	return size;
}

static void print_ways(t_warshell **wrsh_mtrx, int *path_in_way, int i, int j, t_graph *graph) {
	mx_printstr("========================================\nPath: ");
	mx_printstr(graph->isld[i]);
	mx_printstr(" -> ");
	mx_printstr(graph->isld[j]);
	mx_printstr("\nRoute: ");
	mx_print_strarr(wrsh_mtrx[0][0].all_way, " -> ");
	mx_printstr("Distance: ");
	if (mx_arr_size(wrsh_mtrx[0][0].all_way) == 2)
		mx_printint(wrsh_mtrx[i][j].cost);
	else
		print_dist(wrsh_mtrx, path_in_way, i, j);
	mx_printstr("\n========================================\n");
}

void mx_del_int_arr(int **arr, int size) {
    int i = -1;

    while (++i < size) {
        free(arr[i]);
    }
    free(arr);
    arr = NULL;
}

void mx_print_shortests_path2(t_warshell **wrsh_mtrx, t_main *vars, t_graph *graph) {
    int count = 0;
    int **path_in_way;

    for (int i = 0; i < vars->nmb_isld; i++) {
        for (int j = i + 1; j < vars->nmb_isld; j++) {
            count = mx_count_other_way(wrsh_mtrx, vars->nmb_isld, i, j);
            path_in_way = mx_path_from_to(wrsh_mtrx, count, i, j, vars);
            for (int k = 0; k < count; k++) {
				mx_add_top(wrsh_mtrx, path_in_way, k, graph);
				print_ways(wrsh_mtrx, path_in_way[k], i, j, graph);
				mx_del_strarr(&wrsh_mtrx[0][0].all_way);
			}
            mx_del_int_arr(path_in_way, count);
        }
    }
    
}

//------------------------------------------------------------------------

static void Warshell(t_graph *graph, t_main *vars) {
    // initialize matrix of structure
    t_warshell **wrsh_mtrx = (t_warshell**)malloc(vars->nmb_isld * sizeof(t_warshell*));    
    for (int i = 0; i < vars->nmb_isld; i++)        
        wrsh_mtrx[i] = (t_warshell*)malloc(vars->nmb_isld * sizeof(t_warshell));
    
    // fill matrix of costs and paths
    for (int i = 0; i < vars->nmb_isld; i++) 
        for (int j = 0 ; j < vars->nmb_isld; j++) {
            wrsh_mtrx[i][j].cost = graph->array[i][j];
            if (i == j) {
                wrsh_mtrx[i][j].path = 0;
                wrsh_mtrx[i][j].cost = 0;
            }
            else if (wrsh_mtrx[i][j].cost != INT_MAX)
                wrsh_mtrx[i][j].path = i;
        }  
    wrsh_mtrx[0][0].max_id = vars->nmb_isld;

    // run Floyd-Warshell    
    for (int k = 0; k < vars->nmb_isld; k++) {
        for (int i = 0; i < vars->nmb_isld; i++) {
            for (int j = 0 ; j < vars->nmb_isld; j++) {
                if (wrsh_mtrx[i][k].cost != INT_MAX && wrsh_mtrx[k][j].cost != INT_MAX
                && wrsh_mtrx[i][j].cost > wrsh_mtrx[i][k].cost + wrsh_mtrx[k][j].cost) {
                    wrsh_mtrx[i][j].cost = wrsh_mtrx[i][k].cost + wrsh_mtrx[k][j].cost;
                    wrsh_mtrx[i][j].path = wrsh_mtrx[k][j].path;
                }                
            }
        }
    }
    mx_print_shortests_path2(wrsh_mtrx, vars, graph);
    // mx_print_shortests_path(wrsh_mtrx, vars, graph);
}

int main(int argc, char const *argv[]) {
    t_main *vars = create_vars(0, 0);
    
    t_graph *graph;
    mx_check_valid_file(argc, argv[1], vars);	

    graph = create_graph(vars);
    
    graph->isld = crt_isld_matrix(vars, graph);

    crt_isld_matrix_copy(graph, vars);

    // output
    // for (int i = 0; i < vars->nmb_isld; i++)
    //     printf("%5c", graph->isld[i][0]);
    // printf("\n");
    // for (int i = 0; i < vars->nmb_isld; i++) {
    //     printf("%c", graph->isld[i][0]);
    //     for (int j = 0; j < vars->nmb_isld; j++) {
    //         printf("%4d ", graph->array[i][j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    
    Warshell(graph, vars);
    free_fn(vars, graph);
    system("leaks -q a.out");
    return 0;
}

// 5
// A-B,11
// A-C,10
// B-D,5
// C-D,6
// C-E,15
// D-E,4

// 4
// Greenland-Bananal,8
// Fraser-Greenland,10
// Bananal-Fraser,3
// Java-Fraser,5


