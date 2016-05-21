%{
#include "Barrista.h"
%}

%template(OperationList) std::vector<Barrista::operation_info>;

class Barrista 
{
	Barrista();
	~Barrista();
public:
	struct operation_info {
		unsigned line;		//0 for primary
		long long samples_num;
		long long samples_sum;
		std::string name;
	};

	std::vector<operation_info> Schedule_Generate();

	static Barrista* getInstance();
};
