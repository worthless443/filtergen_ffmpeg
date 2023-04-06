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

#define MIN 60
#define HOUR 3200
#define SEC 1

#define OVERLAY "{} overlay={}:'enable=between({})' {}"
int n = -1,o_n=-1;

using vec_str = std::vector<std::string>;

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
	const char *res = "100:100";
	std::string fn = between_format(_fn);
	std::string fn_fn = fmt::format(OVERLAY,std::string("[0:v]") + fmt::format("[{}:v]",stm),res,fn,fmt::format("[vo{}]",++o_n));
	return fn_fn;
}


extern std::vector<struct Param> exp_conv_parse_param(std::string s, int one=-1);

struct VConfig parse_file(std::ifstream &in) {
	vec_str s_v;
	std::string tmp,trim_s = "";
	while(std::getline(in,tmp)) 
		if(*tmp.c_str()) 
			s_v.push_back(tmp);
	struct VConfig vconf;
	std::vector<struct Overlay> ov_v;
	std::vector<struct Param> p_v;
	struct Overlay ov;
	ov.n = 0;
	int t = 0,ovly=0,trim=0,end=0,x=-1,o=0;
	for(size_t i = 0;i<s_v.size();++i) {
		std::stringstream sstm(s_v[i]);	
		std::string tmp;
		// FIXME a better parse design for a shorter span of code
		for(int j = 0;std::getline(sstm,tmp,':');++j) {
			if( j == 2  && tmp == "overlay") {
				ovly = 1;
				o=1;
			}
			else if( j == 1  && tmp == "trim") {
				trim = 1;
				o = 1;
			}
			else o = 0;
			if( j == 1  && *tmp.c_str() >= '0' && *tmp.c_str() <= '9') t = atoi(tmp.c_str());
			else t = 0;
			if(tmp == "end") end = 1;
			else end = 0;
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
	}
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

