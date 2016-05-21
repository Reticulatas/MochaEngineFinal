/*-----------------------------------------------------------------
  ----- © 2012 DigiPen Corporation (USA), All Rights Reserved -----
  -----------------------------------------------------------------
  -* Original Author: Nicholas Fuller
	-* See Footer for Revisions                                      */



#define DEF_MEM_VAR(var_name,var_type) \
	void Set##var_name(var_type const& var_name)\
	{\
		m##var_name = var_name; \
	}\
	var_type const& Get##var_name() const\
	{\
		return m##var_name; \
	}