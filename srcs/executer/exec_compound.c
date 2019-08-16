#include "t_token.h"
#include "sh.h"
#include "sh_env.h"

//FOR
//NAME
//WORDLIST?
//DO

int		exec_compound_case(t_sh *p, t_token *tok)
{
	t_token	*case_elem;
	t_token	*word;

	dprintf(p->debug_fd, "treating CASE\n");
	case_elem = tok->sub;
	while (case_elem && !p->abort_cmd)
	{
		word = case_elem->sub;
		while (word)
		{
			if (!ft_strcmp(tok->content, word->content))
				return (exec_script(p, tok->sub->sub->sub, 0));
			word = word->next;
		}
		case_elem = case_elem->next;
	}
	return (0);
}

int		exec_compound_for(t_sh *p, t_token *tok)
{
	t_token		*ins;
	const char	*tmp;
	const char	*value;

	dprintf(p->debug_fd, "treating FOR\n");
	ins = tok->sub->sub;
	tmp = 0;
	if ((value = sh_getenv(tok->sub->content)))
		tmp = ft_strdup(value);
	while (ins && !p->abort_cmd)
	{
		if (ins->type == SH_WORD)
		{
			sh_setenv(tok->sub->content, ins->content);
			printf("%s\n", tok->sub->sub->sub->content);
			p->last_cmd_result = exec_script(p, tok->sub->sub->sub, 0);
		}
		ins = ins->next;
	}
	sh_unsetenv(tok->sub->content);
	if (tmp)
		sh_setenv(tok->sub->content, tmp);
	free(tmp);
	return (p->last_cmd_result);
}

int     exec_compound_while(t_sh *p, t_token *tok, t_toktype type)
{
	int ret;
	int tmp;

	dprintf(p->debug_fd, "treating WHILE\n");
	ret = 0;
	while (!p->abort_cmd && (((tmp = exec_script(p, tok->sub->sub, 0)) && type == SH_UNTIL) || (!tmp && type == SH_WHILE)) && !p->abort_cmd)
	{
		dprintf(p->debug_fd, "WHILE condition true\n");
		ret = exec_script(p, tok->sub->next, 0);
	}
	dprintf(p->debug_fd, "WHILE condition false\n");
	return (ret);
}

int     exec_compound_if(t_sh *p, t_token *tok)
{
	dprintf(p->debug_fd, "treating IF\n");
	if (!exec_script(p, tok->sub->sub, 0) && !p->abort_cmd)
	{
		dprintf(p->debug_fd, "IF true\n");
		return (p->last_cmd_result = exec_script(p, tok->sub->next->sub, 0));
	}
	dprintf(p->debug_fd, "IF false\n");
	if (tok->sub->next->next && !p->abort_cmd)
		return (p->last_cmd_result = exec_script(p, tok->sub->next->next, 0));
	return (0);
}
