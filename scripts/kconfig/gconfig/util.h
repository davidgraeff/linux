 
/* Helping/Debugging Functions */

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
const char *dbg_sym_flags(int val)
{
	static char buf[256];

	bzero(buf, 256);

	if (val & SYMBOL_CONST)
		strcat(buf, "const/");
	if (val & SYMBOL_CHECK)
		strcat(buf, "check/");
	if (val & SYMBOL_CHOICE)
		strcat(buf, "choice/");
	if (val & SYMBOL_CHOICEVAL)
		strcat(buf, "choiceval/");
	if (val & SYMBOL_VALID)
		strcat(buf, "valid/");
	if (val & SYMBOL_OPTIONAL)
		strcat(buf, "optional/");
	if (val & SYMBOL_WRITE)
		strcat(buf, "write/");
	if (val & SYMBOL_CHANGED)
		strcat(buf, "changed/");
	if (val & SYMBOL_AUTO)
		strcat(buf, "auto/");

	buf[strlen(buf) - 1] = '\0';

	return buf;
}
