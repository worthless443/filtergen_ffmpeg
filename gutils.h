struct Overlay {
	std::string b,e;
	int n;
};

struct Param {
	std::string s,e;
	int genaud;
};

struct VConfig {
	std::vector<struct Overlay> o_v;
	std::vector<struct Param> p_v;
	int o,p;
};

