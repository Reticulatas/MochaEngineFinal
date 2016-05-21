%{
#include "metadef.h"
%}

struct IMeta
{
private:
	IMeta();
	~IMeta();
public:
	const std::string mytoplevelname() const;
	unsigned mytopid() const;
	static unsigned gettopid();
	%csmethodmodifiers getCPtr(IMeta obj) "public";
};
