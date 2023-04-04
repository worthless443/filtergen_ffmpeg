// video triming filter_complex generator
#include<string>
#include<random>
#include<sstream>
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<vector>

#include<fmt/core.h>

#define VTRIM "{} trim=start=\'{}\':end=\'{}\',setpts=PTS-STARTPTS {}"
#define ATRIM "{} atrim=start=\'{}\':end=\'{}\',asetpts=PTS-STARTPTS {}"

struct Param {
	std::string s,e;
	int genaud;
};

std::string join(std::vector<std::string> svec, std::string by = "") {
	std::string s("");
	for(size_t i=0;i<svec.size();++i)
		s+=svec[i] + (i == svec.size() -1 ? "" : by);
	return s;
}

// TODO support for multiple files
std::string gen_trim(int vid, std::vector<struct Param> v_p,std::vector<std::string> &out_streams ) {
	std::string con = "";
	for(unsigned int i=0;i<v_p.size();++i) {
		auto vout = fmt::format("[v{}]",i);
		auto aout = fmt::format("[a{}]",i);
		con+=fmt::format(VTRIM,fmt::format("[v:{}]", vid), v_p[i].s, v_p[i].e,vout) + ((v_p[i].genaud>=0) ? (((unsigned)i == v_p.size() - 1) ? "" : ";") : ";" );
		if(v_p[i].genaud<0)
			con+=fmt::format(ATRIM,fmt::format("[a:{}]", vid), v_p[i].s, v_p[i].e,aout) + (((unsigned)i == v_p.size() - 1) ? "" : ";");
		out_streams.push_back(vout);
		if(v_p[i].genaud<0)
			out_streams.push_back(aout);
	}
	return con;
}

std::string gen_concat(std::vector<std::string> outstm) {
	return fmt::format(";{} concat=n={}:v=1:a=1 [v][a]", join(outstm), outstm.size()/2);
	
}

std::string gen_split(std::vector<std::string> outstm) {
	std::string con(";");
	const char *format = "{} split [{}]";
	for(size_t i =0;i<outstm.size();++i) {
		char out = *(outstm[i].c_str() + 1);
		con+=fmt::format(format,outstm[i],std::string(&out)) + ((i < outstm.size() - 1) ? ";" : "");
	}
	return con;
}

std::vector<struct Param> parse_param(std::ifstream &fs, int one=-1) {
	int cur = 0;
	std::vector<struct Param> s_p;
	std::string line;
	while(std::getline(fs,line)) {
		std::vector<std::string> s_v;
		std::stringstream sstm(line);
		std::string v;
		struct Param p;
		p.genaud = one;
		for(int i=0;std::getline(sstm,v,',');++i)
			*((std::string*)&p + i) = v;
		if(one>=0) { 
			if(cur == one) { 
				s_p.clear();
				s_p.push_back(p);
				return s_p;
			}
		}
		s_p.push_back(p);
		cur+=1;
	}
	return s_p;
} 

std::vector<struct Param> exp_conv_parse_param(std::ifstream &fs, int one=-1) {
	int cur = 0;
	std::vector<struct Param> s_p;
	std::string line;
	while(std::getline(fs,line)) {
		std::vector<std::string> s_v;
		std::stringstream sstm(line);
		std::string v;
		struct Param p;
		p.genaud = one;
		for(int i=0;std::getline(sstm,v,',');++i) {
			int count = 0,j,ccount=0;
			for(unsigned int _=0;_<v.size();++_) {
				if(*(v.c_str() + _) == '.') ++count;
				if(*(v.c_str() + _) == ':') ++ccount;
			}
			if(ccount>0 || !count) {
				fprintf(stderr, "expected new experimental format!\n");
				return s_p;
			}
			std::stringstream sstm0(v);
			std::string raw = "",formatted(""),formatted_final("");
			for(j=0;std::getline(sstm0,raw,'.');++j) 
					formatted+=((raw.size() < 2) ? fmt::format("0{}", raw) : raw) + ((j>=count) ? "" : ((j == count - 1) ? "." : "\\:"));				
			for(int ii=0;ii<3 - count;++ii) formatted_final+="00\\:";
			formatted_final+=formatted;
			*((std::string*)&p + i) = formatted_final;
		}
		if(one>=0) { 
			if(cur == one) { 
				s_p.clear();
				s_p.push_back(p);
				return s_p;
			}
		}
		s_p.push_back(p);
		cur+=1;
	}
	return s_p;
}

extern int argparse(int argc, char **argv,char **cfgfn, int *still, int *expmtl);
int main(int argc, char **argv) {
	char *cfgfn = NULL;
	int still = -1,ret,expmtl=0;
	if((ret=argparse(argc,argv,&cfgfn, &still,&expmtl)>0)) return ret ;
	std::ifstream fs(cfgfn);
	std::vector<struct Param> v_p =  (expmtl) ? exp_conv_parse_param(fs,(still>=0) ? still : -1) : parse_param(fs,(still>=0) ? still : -1);
	if(v_p.size()==0) {
		fprintf(stderr, "nothing to generate\n");
		return 1;
	}
	std::vector<std::string> outstm;
	std::cout << gen_trim(0,v_p,outstm) << ((still >= 0) ? gen_split(outstm) : gen_concat(outstm)) << "\n";
	return 0;
}
