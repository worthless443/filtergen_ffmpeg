#include<cstdio>
#include<cstdlib>
#include<cstring>

// TODO variadic arguments
int argparse(int argc, char **argv,char **cfgfn, int *still) {
	if(argc < 2) {
			fprintf(stderr, "genfilter: ./genfilter <filename>\n");
			return 1;
		}
		for(int i=1;i<argc;++i) {
			if(0 == strcmp("-in", argv[i])) 
				*cfgfn = argv[++i];
			else if(0 == strcmp("-n", argv[i])) 
				*still = atoi(argv[++i]);
			else {
				fprintf(stderr, "genfilter:\'%s\' not understood\n", argv[i]);
				return 1;
			}
		}
	return 0;
}
