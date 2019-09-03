#ifndef SH_WORD_EXPANSION_H
# define SH_WORD_EXPANSION_H

# include <pwd.h>
# include <stdlib.h>
# include <unistd.h>
# include "t_token.h"
# include "sh_env.h"
# include "sh.h"
# include "libft.h"

# define COLON	1
# define ERROR	2
# define LEN	4

typedef struct	s_exp
{
	int		i;
	int		first_i;
	t_env	**env;
	t_env	*find;
	char	*value;
	char	*name;
	char	*content;
	short	opt;
}				t_exp;

typedef struct	s_split
{
	int			i;
	t_token		*tok;
	t_token		*sub;
	const char	*split;
}				t_split;

int		sh_word_expansion(t_token **tok, t_env **env);
void	sh_parameter_expansion(t_exp *exp);
int		sh_tilde_expansion(char **content, t_env *env);
void	sh_quote_removal(t_token *tok, const char *split);
void	sh_find_quote(t_split *splt, short quote);
int		sh_expansion_size(char *content, int i);

#endif