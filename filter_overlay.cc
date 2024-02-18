// video triming filter_complex generator
#include<vector>
#include<string>
#include<sstream>
#include<fstream>
#include<iostream>

#include<cstdlib>
#include<cstring>

#include<fmt/core.h>

#include<gutils.h>
#include<errors.h>

#define MIN 60
#define HOUR 3200
#define SEC 1

#define OVERLAY "{} overlay={}:'enable=between({})' {}"
int n = -1,o_n=-1;

using vec_str = std::vector<std::string>;

static int nth_power(int x, int n) {
	int ret = 1;
	while(--n >= 0) 
		ret *= x;
	return ret;
}

std::string between_format(std::string lstream) {
	std::stringstream sstm(lstream);
	std::string line, s_final("t,");
	while(std::getline(sstm,line)) {
		const int a[] = {HOUR,MIN,SEC};
		int num=0;
		std::stringstream sstm0(line),sst;
		std::string snum = "";
		vec_str s_v;
		for(;std::getline(sstm0,snum,'.');) {
			s_v.push_back(snum);
		}
		for(int i = sizeof(a)/sizeof(int) - 1,j=s_v.size() - 1;j>=0;--i,--j) 
				num+=atoi(s_v[j].c_str()) * a[i];
		s_final+=fmt::format("{},", num);
	}
	char *tmp = (char*)s_final.c_str();
	memset(tmp + s_final.size() - 1, '\0',1);
	return std::string(tmp);
}

std::string get_format(int stm, std::string _fn) {
	const char *res = "100:60";
	std::string fn = between_format(_fn);
	std::string fn_fn = fmt::format(OVERLAY,std::string("[0:v]") + fmt::format("[{}:v]",stm),res,fn,fmt::format("[vo{}]",++o_n));
	return fn_fn;
}

extern std::vector<struct Param> exp_conv_parse_param(std::string s, int one=-1);

std::vector<std::pair<int,char*>> generate_keywords() {
	std::vector<std::pair<int,char*>> keywords;
	std::vector s = {"overlay", "trim","overfuck","gaylord"};
	std::vector in = {2,1,2,1};
	for(int i=0;i<s.size();++i)
		keywords.push_back(std::make_pair(in[i],(char*)s[i]));
	return keywords;
	
}

int is_integer(std::string str) {
	int isint = 0;
	const char *c_str = str.c_str();
	for(int i=0;i<str.size();++i)
	 	isint += (c_str[i] >= '0' && c_str[i] <= '9');
	return isint;
}

class KeywordCheck {
	private:
		std::vector<int> keywords;
		std::vector<std::pair<int,char*>> 
			accepted_keywords = generate_keywords();
	public:
	KeywordCheck() {
		for(int i=0;i<accepted_keywords.size() + 2;++i)
			keywords.push_back(0);
	}

	int End() {
		return keywords[keywords.size() - 2];
	}
	int End(int a) {
		keywords[keywords.size() - 2] = a;
		return End();
	}
	int o() {
		return keywords[keywords.size() - 1];
	}
	int o(int a) {
		keywords[keywords.size() - 1] = a;
		return o();
	}

	int end() {
		return keywords[0] && End();
	}

	void clear() {
		for(int i=0;i<accepted_keywords.size();++i)
			keywords[i] = 0;
	   	End(0);
	}

	int operator()(int idx, std::string keyword) {
		if(is_integer(keyword) > keyword.size()/2)  {
			return 2;
		}
		for(int i=0;i<accepted_keywords.size();++i) {
			if(idx == accepted_keywords[i].first
					&& keyword == accepted_keywords[i].second) {
				//std::cout << accepted_keywords[i].second << "\n";
				keywords[i] = 1;
				o(1); 
				return 1;
			}
			else if(idx == 1 && keyword == "end") {
				End(1);
				return 1;
			}
		}
		if(idx > 0 && is_integer(keyword) < keyword.size()/2) 
			return 0;	
		return 1;
	}

	int operator[](int idx) {
		if(!End() && keywords[idx]) 
			return 1;
		return 0;
	}

	std::vector<int> vec() {
		return keywords;
	}

};

void printvec(std::vector<int> v) {
	printf("[");
	for(auto a : v)
		std::cout << a << ",";
	printf("]\n");
}

struct VConfig parse_file2(std::ifstream &in) {
	vec_str s_v;
	std::string tmp,trim_s = "";
	while(std::getline(in,tmp)) 
		if(*tmp.c_str()) 
			s_v.push_back(tmp);
	struct VConfig vconf;
	vconf.err = 0;
	std::vector<struct Overlay> ov_v;
	std::vector<struct Param> p_v;
	struct Overlay ov;
	ov.n = 0;
	int t = 0,ovly=0,
		trim=0,end=0,x=-1,o=0,
		line_n = 1;

	KeywordCheck keywords;
	for(size_t i = 0;i<s_v.size();++i) {
		std::stringstream sstm(s_v[i]);	
		std::string tmp;
		// FIXME a better parse design for a shorter span of code
		for(int j = 0;std::getline(sstm,tmp,':');++j) {
			int ret = keywords(j,tmp);
			if(ret == 2) {
				keywords.o(0);
				keywords.End(0);
			}
			else if(ret == 0) {
				report_error_in_line(sstm.str(),j,line_n);
				vconf.err++;
			}
			// parsing the contents of the code 
			if(j == 1  && *tmp.c_str() >= '0' && *tmp.c_str() <= '9') 
				t = atoi(tmp.c_str());
			else t = 0;

			if(t) ov.n = t;
		}
		//printvec(keywords.vec());
		if(keywords.o()) 
			i+=1;
		if(keywords[0])
			*((std::string*)&ov + ++x) = s_v[i];
		else if(keywords[1]) {
			trim_s+=s_v[i] + "\n";
		}
		else if(keywords.End()) {
			keywords.clear();
			x = -1;
			ov_v.push_back(ov);
		}
	}

	p_v = exp_conv_parse_param(trim_s);
	vconf.o = ov_v.size() ? 1 : 0;
	vconf.p = p_v.size() ? 1 : 0;
	vconf.o_v = ov_v;
	vconf.p_v = p_v;
	return vconf;
}

struct VConfig parse_file(std::ifstream &in) {
	vec_str s_v;
	std::string tmp,trim_s = "";
	while(std::getline(in,tmp)) 
		if(*tmp.c_str()) 
			s_v.push_back(tmp);
	struct VConfig vconf;
	vconf.err = 0;
	std::vector<struct Overlay> ov_v;
	std::vector<struct Param> p_v;
	struct Overlay ov;
	ov.n = 0;
	int t = 0,ovly=0,
		trim=0,end=0,x=-1,o=0,
		line_n = 1;

	for(size_t i = 0;i<s_v.size();++i) {
		std::stringstream sstm(s_v[i]);	
		std::string tmp;
		// FIXME a better parse design for a shorter span of code
		for(int j = 0;std::getline(sstm,tmp,':');++j) {
	
			if( j == 2 && tmp == "overlay") {
					ovly = 1;
					o = 1;
					line_n +=1; 
					/* for each valid block of code 
					inc line_n just as you do when 
					"end" line is reached  */
			}
			else if(j == 1 && tmp == "trim") {
					trim = 1;
					o = 1;
					line_n+=1;
			}
			else if(j == 1 && tmp == "end") {
				line_n +=  1;
				end = 1;
			}

			else if(j > 0 && 
					(*tmp.c_str() < '0' || *tmp.c_str() > '9')) {

				//report_error_in_line(sstm.str(),j,line_n);
				vconf.err++;
			}
			else {
				o = 0;
				end = 0;
			}

			// parsing the contents of the code 
			if(j == 1  && *tmp.c_str() >= '0' && *tmp.c_str() <= '9') 
				t = atoi(tmp.c_str());
			else t = 0;

			if(t) ov.n = t;
		}

		 if(o) i+=1;
		 if(!end && ovly) 
		 	*((std::string*)&ov + ++x) = s_v[i];
		 else if(!end && trim)
		 	trim_s+=s_v[i] + "\n";
		 else if(end && ovly) {
		 	ovly = 0;
		 	end = 0;
		 	x = -1;
		 	ov_v.push_back(ov);
		 }
		line_n++;
	}
	std::cout << trim_s;
	return vconf;
	p_v = exp_conv_parse_param(trim_s);
	vconf.o = ov_v.size() ? 1 : 0;
	vconf.p = p_v.size() ? 1 : 0;
	vconf.o_v = ov_v;
	vconf.p_v = p_v;
	return vconf;
}

std::string gen_overlay(std::vector<struct Overlay> ov_v) {
	std::string overly = "";
	for(size_t i = 0;i<ov_v.size();++i) 
		overly+=get_format(ov_v[i].n, ov_v[i].b + "\n" + ov_v[i].e) + ((i < ov_v.size() - 1) ? ";": "");
	return overly;
}

