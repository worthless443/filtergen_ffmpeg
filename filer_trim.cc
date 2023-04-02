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
		short o0 = 'a' + i;
		short o1 = 'a' + v_p.size() + i;
		auto vout = fmt::format("[v{}]",(char*)&o0);
		auto aout = fmt::format("[a{}]",(char*)&o1);
		con+=fmt::format(VTRIM,fmt::format("[v:{}]", vid), v_p[i].s, v_p[i].e,vout) + ";";
		con+=fmt::format(ATRIM,fmt::format("[a:{}]", vid), v_p[i].s, v_p[i].e,aout) + (((unsigned)i == v_p.size() - 1) ? "" : ";");
		out_streams.push_back(vout);
		out_streams.push_back(aout);
	}
	return con;
}

std::string gen_concat(std::vector<std::string> outstm) {
	return fmt::format(";{} concat=n={}:v=1:a=1 [v][a]", join(outstm), outstm.size()/2);
	
}

std::vector<struct Param> parse_param(std::ifstream &fs) {
	std::vector<struct Param> s_p;
	std::string line;
	while(std::getline(fs,line)) {
		std::vector<std::string> s_v;
		std::stringstream sstm(line);
		std::string v;
		struct Param p;
		for(int i=0;std::getline(sstm,v,',');++i)
			*((std::string*)&p + i) = v;

		s_p.push_back(p);
	}
	return s_p;
} 

int main(int argc, char **argv) {
	char *cfgfn = NULL;
	if(argc < 2)
		cfgfn = (char*)"nigconf";
	else 
		cfgfn = *(argv + 1);
	std::ifstream fs(cfgfn);
	std::vector<struct Param> v_p =  parse_param(fs);
	std::vector<std::string> outstm;
	std::cout << gen_trim(0,v_p,outstm) << gen_concat(outstm)  << "\n";
	return 0;
}
